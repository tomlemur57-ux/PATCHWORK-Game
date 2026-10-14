#include "win32_api.h"
__declspec(dllimport) void *memset(void*,int,unsigned long long);
int _fltused=0;
#define ROOM_STYLE (WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX)
static HWND rooms[2];
static POINT origin[2]={{100,180},{580,220}};
static int running=1;
static void fill(HDC dc, int x, int y, int w, int h, unsigned rgb) {
    RECT r={x,y,x+w,y,w};
    HBRUSH brush=CreateSolidBrush(((rgb&255)<<16|(rgb&0xff00)|(rgb>>16)&255));
    
}