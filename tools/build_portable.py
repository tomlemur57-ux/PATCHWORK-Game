#!/usr/bin/env python3
"""Reproducible cross-build with Clang + LLD, no downloaded SDK or runtime.
The import libraries are generated from named Windows exports. The temporary
stub DLLs are NOT distributed. Windows supplies the actual DLLs at runtime.
"""
from pathlib import Path
import subprocess, re, shutil, sys
ROOT=Path(__file__).resolve().parents[1]
BUILD=ROOT/'build'; BUILD.mkdir(exist_ok=True)
CLANG=shutil.which('clang'); LINK=shutil.which('lld-link')
if sys.platform=='win32':
 import os
 llvm=Path(os.environ.get('ProgramFiles',r'C:\Program Files'))/'LLVM'/'bin'
 CLANG=CLANG or (str(llvm/'clang.exe') if (llvm/'clang.exe').exists() else None)
 LINK=LINK or (str(llvm/'lld-link.exe') if (llvm/'lld-link.exe').exists() else None)
if not CLANG or not LINK: sys.exit('No existing compiler found. Use the supplied GitHub Actions workflow to build remotely; no local compiler install is required.')
if not (ROOT/'source/native.c').exists(): sys.exit('Missing source/native.c. Open the complete game folder before building.')
imports={
'kernel32':['ExitProcess','GetCurrentProcessId','QueryPerformanceCounter','QueryPerformanceFrequency','Sleep','GetModuleFileNameW','GetModuleHandleA','GetCommandLineA','CreateProcessW','CreateEventA','SetEvent','ResetEvent','WaitForSingleObject','CloseHandle','CreateFileW','WriteFile','ReadFile','GetLastError','GetFileSizeEx','FlushFileBuffers','MoveFileExW','DeleteFileW'],
'user32':['RegisterClassExA','CreateWindowExA','DefWindowProcA','DestroyWindow','PostQuitMessage','PeekMessageA','TranslateMessage','DispatchMessageA','ShowWindow','ShowWindowAsync','UpdateWindow','InvalidateRect','GetClientRect','GetWindowRect','SetWindowPos','GetWindowLongPtrA','SetWindowLongPtrA','GetWindowLongA','GetForegroundWindow','GetWindowThreadProcessId','IsWindow','IsWindowVisible','IsIconic','EnumWindows','GetClassNameA','GetWindow','GetWindowPlacement','SetWindowPlacement','GetAsyncKeyState','GetCursorPos','SetCapture','ReleaseCapture','SetForegroundWindow','SetFocus','SystemParametersInfoA','SetProcessDPIAware','SetProcessDpiAwarenessContext','LoadCursorA','CreateIcon','DestroyIcon','BeginPaint','EndPaint','GetDC','ReleaseDC','FillRect','MessageBoxA','SendMessageA','ClientToScreen','AdjustWindowRectEx','SetTimer','KillTimer','SetWindowTextA'],
'gdi32':['CreateCompatibleDC','CreateCompatibleBitmap','SelectObject','DeleteObject','DeleteDC','CreateSolidBrush','CreatePen','MoveToEx','LineTo','Ellipse','SetBkMode','SetTextColor','TextOutA','CreateFontA','BitBlt'],
'winmm':['PlaySoundA'],
'comdlg32':['GetOpenFileNameW','GetSaveFileNameW','CommDlgExtendedError'],
'msvcrt':['memset','memcpy','strlen','sqrt','sin','cos','_snprintf']}
for dll,symbols in imports.items():
 d=BUILD/(dll+'.def'); d.write_text('LIBRARY '+dll+'.dll\nEXPORTS\n'+'\n'.join(symbols)+'\n')
 subprocess.run([LINK,'/dll','/noentry','/machine:x64','/def:'+str(d),'/out:'+str(BUILD/(dll+'.dll')),'/implib:'+str(BUILD/(dll+'.lib'))],check=True,capture_output=True)
abi=ROOT/'tests/abi_checks.c'
if abi.exists():
 subprocess.run([CLANG,'--target=x86_64-pc-windows-msvc','-std=c99','-ffreestanding','-Wall','-Wextra','-Werror','-fsyntax-only',str(abi)],check=True)
objs=[]
for filename in sorted(p.name for p in (ROOT/'source').glob('*.c')):
 out=BUILD/(filename+'.obj');objs.append(str(out))
 subprocess.run([CLANG,'--target=x86_64-pc-windows-msvc','-std=c99','-O2','-ffreestanding','-fno-stack-protector','-mno-stack-arg-probe','-Wall','-Wextra','-Werror','-Wno-unused-function','-c',str(ROOT/'source'/filename),'-o',str(out)],check=True)
subprocess.run([LINK,'/entry:mainCRTStartup','/subsystem:windows,6.0','/machine:x64','/nodefaultlib','/dynamicbase','/nxcompat','/highentropyva','/stack:8388608,1048576','/timestamp:0','/out:'+str(ROOT/'PATCHWORK.exe')]+objs+[str(BUILD/(dll+'.lib')) for dll in imports],check=True)
print('Built',ROOT/'PATCHWORK.exe')
