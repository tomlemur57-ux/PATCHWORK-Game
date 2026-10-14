#include "win32_api.h"
__declspec(dllimport) void *memset(void*,int,unsigned long long);
int _fltused=0;
#define ROOM_STYLE(WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX)
static HWND rooms[2];
static POINT origin[2]={{100,180},{580,220}};