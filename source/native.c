#include "win32_api.h"
__declspec(dllimport) void *memset(void*,int,unsigned long long);
int _fltused=0;
#define ROOM_STYLE (WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX)
static HWND rooms[2];
static POINT origin[2]={{100,180},{580,220}};
static int running=1;
static void fill(HDC dc, int x, int y, int w, int h, unsigned rgb) {
    RECT r={x,y,x+w,y+h};
    HBRUSH brush=CreateSolidBrush(((rgb&255)<<16)|(rgb&0xff00)|((rgb>>16)&255));
    FillRect(dc,&r,brush); DeleteObject(brush);
}
static float x=70, y=252;
static int actor_room=0;
static int system_move=0;
static long long frequency,last_tick;
static double debt=0;
static int in_frame=0;
static float vx=0;
static void frame(void);
static LRESULT WINAPI events(HWND hwnd,UINT msg,WPARAM wp,LPARAM lp) {
    int n=hwnd==rooms[1]?1:0;
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
    if(msg==WM_EXITSIZEMOVE){KillTimer(hwnd,77);system_move=0;return 0;}
    if (msg==WM_PAINT){
        PAINTSTRUCT ps;HDC dc=BeginPaint(hwnd,&ps);
        fill(dc,0,0,432,288,0xe8e5d7);
        fill(dc,0,266,432,22,0x47676c);
        SetBkMode(dc,1);SetTextColor(dc,0x413917);
        TextOutA(dc,18,20,"Move this window by its Windows title bar.",42);
        if(actor_room==n) {
            fill(dc,(int)x-10,(int)y-14,20,28,0xe66b4a);
            fill(dc,(int)x-6,(int)y-10,12,7,0x254b52);
        }
        TextOutA(dc,18,46,"A / D to roll.",14);
        EndPaint(hwnd,&ps);return 0;
    }
    return DefWindowProcA(hwnd,msg,wp,lp);
}
static void tick(float dt) {
    int left=(GetAsyncKeyState('A')&0x8000)!=0;
    int right=(GetAsyncKeyState('D')&0x8000)!=0;
    vx=(right-left)*210.0f;
    x+=vx*dt;
    if(x<10)x=10;
    if(x>422)x=422;
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
    for(i=0;i<2;i++)if(!IsIconic(rooms[i])){POINT p={0,0};ClientToScreen(rooms[i],&p);origin[i]=p;}
    while(debt>=1.0/120.0&&ticks<12){
        if(pid==GetCurrentProcessId())tick(1.0f/120.0f);
        debt-=1.0/120.0;ticks++;
    }
    for(i=0;i<2;i++)if(!IsIconic(rooms[i])){
        InvalidateRect(rooms[i],0,0);UpdateWindow(rooms[i]);
    }
    in_frame=0;
}
void mainCRTStartup(void){
    HINSTANCE instance=GetModuleHandleA(0);
    WNDCLASSEXA wc; MSG message;
    RECT size={0,0,432,288};
    int i;
    SetProcessDpiAwarenessContext((HANDLE)(long long)-4);
    memset(&wc,0,sizeof(wc));wc.size=sizeof(wc);wc.proc=events;
    wc.instance=instance;wc.cursor=LoadCursorA(0,(const char*)32512);
    wc.name="PatchworkNativeRoom";
    if(!RegisterClassExA(&wc))ExitProcess(1);
    AdjustWindowRectEx(&size,ROOM_STYLE,0,WS_EX_APPWINDOW);
    for(i=0;i<2;i++){
        rooms[i]=CreateWindowExA(WS_EX_APPWINDOW,wc.name,
            i==0?"PATCHWORK - Service bay":"PATCHWORK - Transfer deck", ROOM_STYLE,
            origin[i].x+size.left,origin[i].y+size.top,size.right-size.left,size.bottom-size.top,
            0,0,instance,0); /* No parent, no owner, no WS_CHILD. */
            if(!rooms[i])ExitProcess(1);
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
    for (i=0;i<2;i++)DestroyWindow(rooms[i]);
    ExitProcess(0);
}