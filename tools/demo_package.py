#!/usr/bin/env python3
"""Create and verify a portable Windows demo from the checked-out saved source."""
from __future__ import annotations
import argparse, base64, hashlib, io, json, os, pathlib, re, struct, subprocess, time, urllib.request, zipfile
REPOSITORY = 'tomlemur57-ux/PATCHWORK-Game'
PUBLIC_TEXT = 'Move the desktop windows, line up their doors and guide the robot to its charging dock.'
ROOT = pathlib.Path(__file__).resolve().parents[1]
BLOCKED = re.compile(r'\b(?:'+'|'.join(bytes.fromhex(h).decode() for h in (
 '6169','6172746966696369616c20696e74656c6c6967656e6365','6368617074657273','63686170746572',
 '7475746f7269616c73','7475746f7269616c','6c6573736f6e73','6c6573736f6e','63686174677074','636f646578','6b61706974656c'))+r')\b', re.I)
C_PROSE = re.compile(r'"(?:\\[\s\S]|[^"\\])*"|\'(?:\\[\s\S]|[^\'\\])*\'|/\*[\s\S]*?\*/|//[^\n]*')
RUNTIME_DIRS = ('assets','art','audio','levels','worlds','data')
def check_text(text: str, name: str) -> None:
    if pathlib.Path(name).suffix.lower() in ('.c','.h'):
        text='\n'.join(C_PROSE.findall(text))
    if BLOCKED.search(text):
        raise ValueError('Internal development wording in '+name+'; publication stopped.')
def audit(root: pathlib.Path) -> list[str]:
    names=subprocess.check_output(['git','ls-files','-z'],cwd=root).decode().split('\0')
    checked=[]
    for name in filter(None,names):
        p=root/name
        if p.is_symlink(): raise ValueError('Linked tracked path: '+name)
        if not p.is_file(): continue
        if p.suffix.lower() in ('.md','.txt','.c','.h','.py','.yml','.yaml','.json','.html','.svg','.js','.cjs'):
            check_text(p.read_text(encoding='utf-8'),name);checked.append(name)
    return checked
def digest(data: bytes) -> str:
    return hashlib.sha256(data).hexdigest()
def api(route: str, method='GET', body=None):
    args=['gh','api','repos/'+REPOSITORY+route,'--method',method,'-H','Cache-Control: no-cache']
    if body is not None: args+=['--input','-']
    p=subprocess.run(args,input=None if body is None else json.dumps(body).encode(),capture_output=True)
    if p.returncode: raise RuntimeError(p.stderr.decode(errors='replace')[-1200:])
    return json.loads(p.stdout or b'null')
def pe_check(exe: bytes) -> None:
    if len(exe)<128 or exe[:2]!=b'MZ': raise ValueError('Missing Windows executable.')
    offset=struct.unpack_from('<I',exe,60)[0]
    if offset+6>len(exe) or exe[offset:offset+4]!=b'PE\0\0' or struct.unpack_from('<H',exe,offset+4)[0]!=0x8664:
        raise ValueError('Expected a Windows x64 executable.')
def package(root: pathlib.Path, commit: str, tag: str, operation: str) -> dict:
    if not re.fullmatch(r'[a-f0-9]{40}',commit): raise ValueError('An exact commit is required.')
    if not re.fullmatch(r'(?:v\d+\.\d+\.\d+|demo-\d{8}-\d{6}-[a-f0-9]{6})',tag): raise ValueError('Invalid version.')
    if operation not in ('demo','preview'): raise ValueError('Invalid build operation.')
    exe=(root/'PATCHWORK.exe').read_bytes();pe_check(exe)
    url='https://github.com/'+REPOSITORY+'/releases/tag/'+tag if operation=='demo' else None
    files={'PATCHWORK.exe':exe,'README.txt':('PATCHWORK\n\n'+PUBLIC_TEXT+'\n\nWindows x64. Extract all files and run PATCHWORK.exe.\nUse the controls shown in the game.\n'+(('\n'+url+'\n') if url else '')).encode(),
           'BUILD-INFO.txt':('Commit: '+commit+'\nWindows x64.\n').encode()}
    if url: files['demo_link.txt']=(url+'\n').encode()
    for name in ('LICENSE','LICENSE.txt','LICENSE.md','COPYING','NOTICE'):
        p=root/name
        if p.is_file(): files[name]=p.read_bytes()
    for name in RUNTIME_DIRS:
        folder=root/name
        if not folder.is_dir(): continue
        for p in sorted(folder.rglob('*')):
            if p.is_symlink(): raise ValueError('Linked runtime file.')
            if p.is_file():
                rel=p.relative_to(root).as_posix()
                if any(part.startswith('.') for part in pathlib.PurePosixPath(rel).parts): continue
                if p.stat().st_size>16*1024*1024: raise ValueError('Runtime file too large: '+rel)
                files[rel]=p.read_bytes()
    manifest={p.relative_to(root).as_posix():digest(p.read_bytes()) for p in sorted((root/'source').rglob('*')) if p.is_file()}
    files['SOURCE-SHA256.json']=json.dumps(manifest,indent=2).encode()
    for name,data in files.items():
        if pathlib.Path(name).suffix.lower() in ('.txt','.md','.json','.svg'): check_text(data.decode('utf-8'),name)
    if sum(map(len,files.values()))>48*1024*1024: raise ValueError('Demo exceeds 48 MB.')
    files['SHA256SUMS.txt']=''.join(digest(data)+'  '+name+'\n' for name,data in sorted(files.items())).encode()
    out=root/'dist';out.mkdir(exist_ok=True);dest=out/'PATCHWORK-Windows.zip'
    with zipfile.ZipFile(dest,'w',zipfile.ZIP_DEFLATED,compresslevel=9) as z:
        for name,data in sorted(files.items()):
            info=zipfile.ZipInfo(name,(2026,1,1,0,0,0));info.compress_type=zipfile.ZIP_DEFLATED;info.external_attr=0o100644<<16;z.writestr(info,data)
    with zipfile.ZipFile(dest) as z:
        if {n:z.read(n) for n in z.namelist()}!=files: raise ValueError('ZIP readback mismatch.')
    return {'commit':commit,'tag':tag,'operation':operation,'url':url,'zipSha256':digest(dest.read_bytes()),'published':False}
def publish(root: pathlib.Path, info: dict) -> None:
    dest=root/'dist/PATCHWORK-Windows.zip';tag=info['tag']
    notes=PUBLIC_TEXT+'\n\n**Windows x64.** Download `PATCHWORK-Windows.zip`, extract it and run `PATCHWORK.exe`. Use the controls shown in the game.\n\nEarly prototype.\n'
    check_text(notes,'release description')
    # A repeat of the exact request can complete the same draft, never another version.
    p=subprocess.run(['gh','api','repos/'+REPOSITORY+'/releases/tags/'+tag],capture_output=True)
    if p.returncode:
        if b'HTTP 404' not in p.stderr: raise RuntimeError(p.stderr.decode(errors='replace')[-800:])
        release=api('/releases','POST',{'tag_name':tag,'target_commitish':info['commit'],'name':'PATCHWORK '+tag+' - Windows demo','body':notes,'draft':True,'prerelease':True})
    else:
        release=json.loads(p.stdout)
        if release['target_commitish']!=info['commit']: raise ValueError('Version already belongs to different source.')
        if not release['draft']:
            asset=next((x for x in release['assets'] if x['name']==dest.name),None)
            if not asset or asset.get('digest')!='sha256:'+info['zipSha256']: raise ValueError('Published package differs; refusing to overwrite it.')
    if release['draft']:
        subprocess.run(['gh','release','upload',tag,str(dest),'--repo',REPOSITORY,'--clobber'],check=True)
        fresh=api('/releases/'+str(release['id']))
        asset=next(x for x in fresh['assets'] if x['name']==dest.name)
        if asset.get('digest')!='sha256:'+info['zipSha256']: raise ValueError('Uploaded ZIP digest mismatch.')
        release=api('/releases/'+str(release['id']),'PATCH',{'draft':False,'body':notes})
    download='https://github.com/'+REPOSITORY+'/releases/download/'+tag+'/'+dest.name
    for attempt in range(10):
        try:
            with urllib.request.urlopen(download,timeout=30) as response: data=response.read(64*1024*1024+1)
            if digest(data)!=info['zipSha256']: raise ValueError('Public download differs from build.')
            break
        except Exception:
            if attempt==9: raise
            time.sleep(3)
    wanted=(info['url']+'\n').encode()
    for attempt in range(5):
        row=api('/contents/demo_link.txt?ref=main')
        if base64.b64decode(row['content'])==wanted: break
        try:
            api('/contents/demo_link.txt','PUT',{'branch':'main','sha':row['sha'],'message':'Update demo link [skip ci]','content':base64.b64encode(wanted).decode()})
            break
        except RuntimeError:
            if attempt==4: raise
            time.sleep(2)
    if base64.b64decode(api('/contents/demo_link.txt?ref=main')['content'])!=wanted: raise ValueError('Demo link readback failed.')
    info['published']=True
if __name__=='__main__':
    p=argparse.ArgumentParser();p.add_argument('--audit',action='store_true');p.add_argument('--operation',choices=['demo','preview']);p.add_argument('--version');args=p.parse_args()
    checked=audit(ROOT);print('Public copy checked:',len(checked),'tracked text files')
    if args.audit: raise SystemExit(0)
    if os.environ.get('GITHUB_REPOSITORY')!=REPOSITORY: raise SystemExit('Unexpected repository.')
    commit=subprocess.check_output(['git','rev-parse','HEAD'],cwd=ROOT,text=True).strip()
    result=package(ROOT,commit,args.version,args.operation)
    if args.operation=='demo': publish(ROOT,result)
    (ROOT/'dist/demo-result.json').write_text(json.dumps(result,indent=2),encoding='utf-8')
    print('Verified Windows package:',result['zipSha256'])
