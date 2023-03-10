#include <windows.h>
#include "resource.h"

#define WM_TRAY (WM_USER + 100)
#define WM_TASKBAR_CREATED RegisterWindowMessage(TEXT("TaskbarCreated"))

#define ID_SHOW		40001
#define ID_EXIT		40002

#define APP_NAME	TEXT("NeverLock程序")
#define APP_TIP		TEXT("防止自动锁屏程序")

NOTIFYICONDATA nid;		//托盘属性
HMENU hMenu;			//托盘菜单

//实例化托盘
void InitTray(HINSTANCE hInstance, HWND hWnd)
{
    nid.cbSize = sizeof(NOTIFYICONDATA);
    nid.hWnd = hWnd;
    nid.uID = IDI_ICON1;
    nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP | NIF_INFO;
    nid.uCallbackMessage = WM_TRAY;
    nid.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));
    lstrcpy(nid.szTip, APP_NAME);

    hMenu = CreatePopupMenu();//生成托盘菜单
    //为托盘菜单添加两个选项
    AppendMenu(hMenu, MF_STRING, ID_SHOW, TEXT("提示"));
    AppendMenu(hMenu, MF_STRING, ID_EXIT, TEXT("退出"));

    Shell_NotifyIcon(NIM_ADD, &nid);
}

//演示托盘气泡提醒
void ShowTrayMsg()
{
    lstrcpy(nid.szInfoTitle, APP_NAME);
    lstrcpy(nid.szInfo, TEXT("收到一条消息！"));
    nid.uTimeout = 1000;
    Shell_NotifyIcon(NIM_MODIFY, &nid);
}

//模拟按键
void ClickSimu()
{
    keybd_event(VK_SCROLL, 0, 0, 0);  // 按下按键
    keybd_event(VK_SCROLL, 0, KEYEVENTF_KEYUP, 0); // 弹起按键
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_TRAY:
        switch (lParam)
        {
        case WM_RBUTTONDOWN:
        {
            //获取鼠标坐标
            POINT pt; GetCursorPos(&pt);

            //解决在菜单外单击左键菜单不消失的问题
            SetForegroundWindow(hWnd);

            //使菜单某项变灰
            //EnableMenuItem(hMenu, ID_SHOW, MF_GRAYED);	

            //显示并获取选中的菜单
            int cmd = TrackPopupMenu(hMenu, TPM_RETURNCMD, pt.x, pt.y, NULL, hWnd,
                NULL);
            if (cmd == ID_SHOW)
                MessageBox(hWnd, APP_TIP, APP_NAME, MB_OK);
            if (cmd == ID_EXIT)
                PostMessage(hWnd, WM_DESTROY, NULL, NULL);
        }
        break;
        case WM_LBUTTONDOWN:
            MessageBox(hWnd, APP_TIP, APP_NAME, MB_OK);
            break;
        case WM_LBUTTONDBLCLK:
            break;
        }
        break;
    case WM_DESTROY:
        //窗口销毁时删除托盘
        Shell_NotifyIcon(NIM_DELETE, &nid);
        PostQuitMessage(0);
        break;
    case WM_TIMER:
        //ShowTrayMsg();
        //KillTimer(hWnd, wParam);
        ClickSimu();
        break;
    }
    if (uMsg == WM_TASKBAR_CREATED)
    {
        //系统Explorer崩溃重启时，重新加载托盘
        Shell_NotifyIcon(NIM_ADD, &nid);
    }
    return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/,
    LPSTR /*lpCmdLine*/, int iCmdShow)
{
    HWND hWnd;
    MSG msg;
    WNDCLASS wc = { 0 };
    wc.style = NULL;
    wc.hIcon = NULL;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = hInstance;
    wc.lpfnWndProc = WndProc;
    wc.hbrBackground = NULL;
    wc.lpszMenuName = NULL;
    wc.lpszClassName = APP_NAME;
    wc.hCursor = NULL;

    if (!RegisterClass(&wc)) return 0;

    hWnd = CreateWindowEx(WS_EX_TOOLWINDOW, APP_NAME, APP_NAME, WS_POPUP, CW_USEDEFAULT,
        CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, NULL, NULL, hInstance, NULL);

    ShowWindow(hWnd, iCmdShow);
    UpdateWindow(hWnd);

    InitTray(hInstance, hWnd);			        //实例化托盘
    SetTimer(hWnd, 3, 4 * 60 * 1000, NULL);		//定时发送按键消息

    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return msg.wParam;
}