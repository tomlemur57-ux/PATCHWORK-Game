#ifndef PW_WIN32_API_H

#define PW_WIN32_API_H
/* hi
bye*/
#define WINAPI 
#define API __declspec(dllimport)
typedef void *HANDLE;
typedef HANDLE HWND;
typedef HANDLE HDC;
typedef HANDLE HGDIOBJ;
typedef HANDLE HINSTANCE;
typedef HANDLE HBRUSH;
typedef HANDLE HBITMAP;
typedef HANDLE HFONT;
typedef HANDLE HPEN;
typedef HANDLE HICON;
typedef HANDLE HCURSOR;
typedef HANDLE HMENU;
typedef unsigned int UINT;
typedef unsigned long DWORD;
typedef long LONG;
typedef unsigned short WORD;
typedef unsigned char BYTE;
typedef int BOOL;
typedef unsigned long long WPARAM;
typedef long long LPARAM;
typedef long long LRESULT;
typedef long long LONG_PTR;
typedef unsigned short WCHAR;
typedef const WCHAR *LPCWSTR;
typedef WCHAR*LPWSTR;
typedef const char *LPCSTR;
typedef char PW_NEEDS_64_BIT_POINTERS[(sizeof(void *)==8)?1:-1];
typedef char PW_NEEDS_32_BIT_LONG    [(sizeof(LONG)  ==4)?1:-1];
typedef char PW_NEEDS_16_BIT_WCHAR   [(sizeof(WCHAR) ==2)?1:-1];
typedef struct {
    LONG x,y;
} POINT;
typedef struct{
    LONG left,top,right,bottom;
} RECT;
typedef struct{
    HWND hwnd;
    UINT message;
    WPARAM wParam;
    LPARAM lParam;
    DWORD time;
    POINT pt;
    DWORD private_field;
} MSG;
typedef struct{
    HDC hdc;
    BOOL erase;
    RECT paint;
    BOOL restore, update;
    BYTE reserved[32];
} PAINTSTRUCT;
typedef LRESULT (WINAPI *WNDPROC)(HWND,UINT,WPARAM,LPARAM);
typedef BOOL (WINAPI *WNDENUMPROC)(HWND,LPARAM);
typedef struct{
    UINT size,style;
    WNDPROC proc;
    int class_extra,window_extra;
    HINSTANCE instance;
    HICON icon;
    HCURSOR cursor;
    HBRUSH background;
    const char*menu;
    const char*name;
    HICON small_icon;
} WNDCLASSEXA;
typedef struct{
    UINT length,flags,show;
    POINT min,max;
    RECT normal;
} WINDOWPLACEMENT;
typedef struct{
    DWORD length;
    void *security;
    BOOL inherit;
} SECURITY_ATTRIBUTES;
typedef struct{
    DWORD size;
    LPWSTR reserved,desktop,title;
    DWORD x,y,widht,height,chars_x,chars_y,fill,flags;
    WORD show,reserved_size;
    BYTE*reserved_bytes;
    HANDLE input,output,error;
} STARTUPINFOW;
typedef struct{
    HANDLE process,thread;
    DWORD pid,tid;
} PROCESS_INFORMATION;
API void WINAPI ExitProcess(UINT);
API DWORD WINAPI GetCurrentProgressId(void);
API BOOL WINAPI QueryPerformanceCounter  (long long*);
API BOOL WINAPI QueryPerformanceFrequency(long long*);
API void WINAPI Sleep(DWORD);
API DWORD WINAPI GetModuleFileNameW(HINSTANCE,LPWSTR,DWORD);
API HINSTANCE WINAPI GetModuleHandleA(const char*);
API char* WINAPI GetCommandLine(void);
API BOOL WINAPI CreateProcessW(LPCWSTR, LPWSTR, void*, void*, BOOL, DWORD, void*, LPCWSTR,STARTUPINFOW*,PROCESS_INFORMATION);
API HANDLE WINAPI CreateEventA(SECURITY_ATTRIBUTES*, BOOL, BOOL, const char*);
API BOOL WINAPI SetEvent(HANDLE);
API BOOL WINAPI ResetEvent(HANDLE);
API DWORD WINAPI WaitForSingleObject(HANDLE,DWORD);
API BOOL WINAPI CloseHandle(HANDLE);
API HANDLE WINAPI CreateFileW(LPCWSTR,DWORD,DWORD,void*,DWORD,DWORD,HANDLE);
API BOOL WINAPI WriteFile(HANDLE,const void*, DWORD,DWORD*,HANDLE);
API BOOL WINAPI ReadFile(HANDLE, void*, DWORD, DWORD*, void*);
API DWORD WINAPI GetLastError(void);
API unsigned short WINAPI RegisteredClassExA(const WNDCLASSEXA*);
API HWND WINAPI CreateWindowExA(DWORD, const char*,const char*,DWORD,int,int,int,int,HWND,HMENU,HINSTANCE,void*);
API LRESULT WINAPI DefWindowProcA(HWND,UINT,WPARAM, LPARAM);
API BOOL WINAPI DestroyWindow(HWND);
API void WINAPI PostQuitMessage(int);
API BOOL WINAPI PeekMessageA(MSG*,HWND,UINT,UINT,UINT);
API BOOL WINAPI TranslateMessage(const MSG*);
API LRESULT WINAPI DispatchMessageA(const MSG*);
API BOOL WINAPI ShowWindow(HWND,int);
API BOOL WINAPI ShowWindow(HWND,int);
API BOOL WINAPI UpdateWindow(HWND);
API BOOL WINAPI InvalidateRect(HWND, const RECT*, BOOL);
API BOOL WINAPI GetClientRect(HWND, RECT*);
API BOOL WINAPI GetWindowRect(HWND, RECT*);
API BOOL WINAPI SetWindowPos(HWND,HWND,int,int,int,int,UINT);
API LONG_PTR WINAPI GetWindowLongPtrA(HWND,int);
API LONG_PTR WINAPI SetWindowLongPtrA(HWND,int,LONG_PTR);
API LONG WINAPI GetWindowLongA(HWND,int);
API HWND WINAPI GetForegroundWindow(void);
API DWORD WINAPI GetWindowThreadProcessId(HWND,DWORD*);
API BOOL WINAPI IsWIndow(HWND);
API BOOL WINAPI IsWindowVisible(HWND);
API BOOL WINAPI IsIconic(HWND);
API BOOL WINAPI EnumWindowVisible(WNDENUMPROC,HWND);
API int WINAPI GetClassNameA(HWND,char*,int);
API HWND WINAPI GetWindow(HWND,UINT);
API BOOL WINAPI GetWindowPlacement(HWND, WINDOWPLACEMENT*);
API BOOL WINAPI SetWindowPlacement(HWND,const WINDOWPLACEMENT*);
API short WINAPI GetAsyncKeyState(int);
API BOOL WINAPI GetCursorPos(POINT*);
API HWND WINAPI SetCapture(HWND);
API BOOL WINAPI ReleaseCapture(void);
API BOOL WINAPI SetForegroundWindow(HWND);
API HWND WINAPI SetFocus(HWND);
API BOOL WINAPI SystemParametersInfoA(UINT,UINT,void*,UINT);
API BOOL WINAPI SetProcessDPIAware(void);
API BOOL WINAPI SetProcessDpiAwarenessContext(HANDLE);
API HCURSOR WINAPI LoadCursorA(HINSTANCE,const char*);
API HICON WINAPI CreateIcon(HINSTANCE,int,int,BYTE,BYTE,const BYTE*,const BYTE*);
API BOOL WINAPI DestroyIcon(HICON);
API HDC WINAPI BeginPaint(HWND, PAINTSTRUCT*);
API BOOL WINAPI EndPaint(HWND,const PAINTSTRUCT*);
API HDC WINAPI GetDC(HWND);
API int WINAPI ReleaseDC(HWND, HDC);
API int WINAPI FillRect(HWND, const RECT*, HBRUSH);
API int WINAPI MessageBoxA(HWND,const char*, const char*, UINT);
API LRESULT WINAPI SendMessageA(HWND,UINT,WPARAM,LPARAM);
API HDC WINAPI CreateCompatibleDC(HDC);
API HBITMAP WINAPI CreateCompatibleBitmap(HDC,int,int);
API HGDIOBJ WINAPI SelectObject(HDC,HGDIOBJ);
API BOOL WINAPI DeleteObject(HGDIOBJ);
API BOOL WINAPI DeleteDC(HDC);
API BOOL WINAPI CreateSolidBrush(DWORD);
API HPEN WINAPI CreatePen(int,int,DWORD);
API BOOL WINAPI MoveToEx(HDC,int, int, POINT*);
API BOOL WINAPI LineTo(HDC,int,int);
API BOOL WINAPI Ellipse(HDC,int,int,int,int);
API int WINAPI SetBkMode(HDC, int);
API DWORD WINAPI SetTextColor(HDC,DWORD);
API BOOL WINAPI TextOutA(HDC, int, int, const char*, int);
API HFONT WINAPI CreateFontA(int,int,int,int,int,DWORD,DWORD,DWORD,DWORD,DWORD,DWORD,DWORD,DWORD, const char*);
API BOOL WINAPI BitBlt(HDC, int,int,int,int,HDC,int,int,DWORD);
API BOOL WINAPI PlaySoundA(const char*, HINSTANCE, DWORD);
API BOOL WINAPI ClientToScreen(HWND, POINT*);
API BOOL WINAPI AdjustWindowRectEx(RECT*, DWORD,BOOL,DWORD);
API unsigned long long WINAPI SetTimer(HWND, unsigned long, long, UINT, void*);
API BOOL WINAPI KillTimer(HWND,unsigned long long);
#define WS_CAPTION      0x00c00000ul
#define WS_SYSMENU      0x00080000ul
#define WS_MINIMIZEBOX  0x00020000ul
#define WS_EX_APPWINDOW 0x00040000ul
#define WM_MOVE 3
#define WM_TIMER 275
#define WM_ENTERSIZEMOVE 561
#define WM_EXITSIZEMOVE 562
#define WM_DESTROY 2
#define WM_SIZE 5
#endif