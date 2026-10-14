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