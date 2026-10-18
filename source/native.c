#include "game.h"
#include "win32_api.h"
int _fltused=0;
#define ROOM_STYLE (WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX)
static HWND rooms[PW_ROOMS];
static POINT origin[PW_ROOMS]={{100,180},{580,220},{1060,180}};
static int running=1;
static void fill(HDC dc, int x, int y, int w, int h, unsigned rgb) {
    RECT r={x,y,x+w,y+h};
    HBRUSH brush=CreateSolidBrush(((rgb&255)<<16)|(rgb&0xff00)|((rgb>>16)&255));
    FillRect(dc,&r,brush); DeleteObject(brush);
}
static Game game;
static Drawing draw;
static long long frequency,last_tick;
static double debt=0;
static int in_frame=0,system_move=0,previous[256];
static void frame(void);
static void paint_game(HDC dc, int n){
    int i; pw_render(&game,n,&draw);
    for(i=0;i<draw.count;i++){
        DrawOp*p=&draw.ops[i];
        if(p->kind==OP_RECT)fill(dc,(int)p->x,(int)p->y,(int)p->w,(int)p->h,p->color);
        else if(p->kind==OP_TEXT){
            SetBkMode(dc,1);SetTextColor(dc,((p->color&255)<<16)|(p->color&0xff00)|((p->color>>16)&255));
            TextOutA(dc,(int)p->x,(int)p->y,p->text,(int)strlen(p->text));
        }
    }
}
static LRESULT WINAPI events(HWND hwnd,UINT msg,WPARAM wp,LPARAM lp) {
    int n=(int)GetWindowLongPtrA(hwnd,-21);
    (void)n;
    if(msg==WM_CLOSE) {
        if(MessageBoxA(hwnd,"Quit PATCHWORK?","End this shift",0x24)==6)running=0;
        return 0;
    }
    if(msg==WM_ERASEBKGND)return 1;
    if(msg==WM_MOVE) {
        if(hwnd==rooms[n]) {POINT p={0,0};ClientToScreen(hwnd,&p);origin[n]=p;}
        return 0;
    }
    /*  windows enters modal loop whn title bar is dragged
    timer is there to keepo same frame frunction running during that loop*/
    if(msg==WM_ENTERSIZEMOVE){system_move=1;SetTimer(hwnd,77,8,0);return 0;}
    if(msg==WM_TIMER&&wp==77){frame();return 0;}
    if(msg==WM_EXITSIZEMOVE) {KillTimer(hwnd,77);system_move=0;return 0;}
    if(msg==WM_LBUTTONDOWN&&game.editing==n){
        POINT p; GetCursorPos(&p);
        int x=p.x-origin[n].x,y=p.y-origin[n].y;
        if(x>=121&&x<267&&y>=90&&y<236)pw_rotate(&game,n,((y-90)/50)*3+(x-121)/50);
        return 0;
    }
    if (msg==WM_PAINT){
        PAINTSTRUCT ps;HDC dc=BeginPaint(hwnd,&ps);
        paint_game(dc,n);
        EndPaint(hwnd,&ps);return 0;
    }
    return DefWindowProcA(hwnd,msg,wp,lp);
}
static void tick(float dt) {
    Input in; POINT mouse; int state[256],i;
    memset(&in,0,sizeof(in));GetCursorPos(&mouse);
    for(i=0;i<256;i++)state[i]=(GetAsyncKeyState(i)&0x8000)!=0;
    in.axis=(float)(state['D']-state['A']);
    in.jump=state[VK_SPACE]&&!previous[VK_SPACE];in.jump_held=state[VK_SPACE];
    in.dash=state[VK_SHIFT]&&!previous[VK_SHIFT];
    in.fire=state[VK_LBUTTON]&&!system_move;in.grapple=state[VK_RBUTTON]&&!system_move;
    in.interact=state['E']&&!previous['E'];in.up=state['W'];
    in.aim_x=(float)(mouse.x-origin[game.p.room].x);
    in.aim_y=(float)(mouse.y-origin[game.p.room].y-PW_TITLE);
    if(state['R']&&!previous['R'])pw_load(&game,0);
    if(state[VK_ESCAPE]&&!previous[VK_ESCAPE])game.paused=!game.paused;
    for(i=0;i<256;i++)previous[i]=state[i];
    pw_step(&game,in,dt);
}
static void frame(void){
    long long now;DWORD pid = 0; int i, ticks= 0;
    if(in_frame||!frequency)return;
    in_frame=1;
    QueryPerformanceCounter(&now);
    double elapsed=(double)(now-last_tick)/frequency;
    last_tick=now;
    if(elapsed>.1)elapsed=.1;
    debt+=elapsed;
    GetWindowThreadProcessId(GetForegroundWindow(),&pid);
    for(i=0;i<game.nrooms;i++)if(!IsIconic(rooms[i])){
        POINT p={0,0};ClientToScreen(rooms[i],&p);origin[i]=p;
        pw_move_room(&game,i,(float)p.x,(float)p.y+PW_TITLE,0);
    }
    while(debt>=1.0/120.0&&ticks<12){
        if(pid==GetCurrentProcessId())tick(1.0f/120.0f);
        debt-=1.0/120.0;ticks++;
    }
    for(i=0;i<game.nrooms;i++)if(!IsIconic(rooms[i])){
        InvalidateRect(rooms[i],0,0);UpdateWindow(rooms[i]);
    }
    in_frame=0;
}
void mainCRTStartup(void){
    HINSTANCE instance=GetModuleHandleA(0);
    WNDCLASSEXA wc; MSG message;
    RECT size={0,0,PW_W,PW_H+PW_TITLE};
    int i;
    pw_load(&game,0);
    SetProcessDpiAwarenessContext((HANDLE)(long long)-4);
    memset(&wc,0,sizeof(wc));wc.size=sizeof(wc);wc.proc=events;
    wc.instance=instance;wc.cursor=LoadCursorA(0,(const char*)32512);
    wc.name="PatchworkNativeRoom";
    if(!RegisterClassExA(&wc))ExitProcess(1);
    AdjustWindowRectEx(&size,ROOM_STYLE,0,WS_EX_APPWINDOW);
    for(i=0;i<game.nrooms;i++){
        rooms[i]=CreateWindowExA(WS_EX_APPWINDOW,wc.name,
            i==0?"PATCHWORK - Service bay":i==1?"PATCHWORK - Transfer deck":"PATCHWORK - Power room",ROOM_STYLE,
            origin[i].x+size.left,origin[i].y+size.top,size.right-size.left,size.bottom-size.top,
            0,0,instance,0); /*No parent, no owner, no WS_CHILD*/
            if(!rooms[i])ExitProcess(1);
            SetWindowLongPtrA(rooms[i],-21,i);
            {POINT p={0,0};ClientToScreen(rooms[i],&p);origin[i]=p;}ShowWindow(rooms[i],SW_SHOW);
    }
    if(!QueryPerformanceFrequency(&frequency)||frequency<=0)ExitProcess(1);
    QueryPerformanceCounter(&last_tick);
    while(running){
        while(PeekMessageA(&message,0,0,0,1)){
            if(message.message==WM_QUIT){running=0;break;}
            TranslateMessage(&message);DispatchMessageA(&message);
        }
        frame();
        Sleep(8);
    }
    for(i=0;i<game.nrooms;i++)DestroyWindow(rooms[i]);
    ExitProcess(0);
}