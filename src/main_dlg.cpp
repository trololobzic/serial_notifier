#include "stdafx.h"
#include "main_app.h"
#include "main_dlg.h"
#include "registry/registry.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CSerialNotifierDlg::CSerialNotifierDlg(Settings & settings, Serial & serial, CWnd* pParent):
    _settings(settings),
    _serial(serial),
    CDialog(CSerialNotifierDlg::IDD, pParent)
{
    _h_icon = AfxGetApp()->LoadIcon(IDR_MAINFRAME1);
}

CSerialNotifierDlg::~CSerialNotifierDlg()
{
}

void CSerialNotifierDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CSerialNotifierDlg, CDialog)
    ON_WM_PAINT()
    ON_WM_DESTROY()
    ON_MESSAGE(WM_TRAYICON_EVENT, OnTrayIconEvent)
    ON_MESSAGE(WM_SERIAL_LIST_WAS_CHANGED, OnChangedSerialList)

    //ON_WM_QUERYDRAGICON()
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()


// Cmfc1Dlg message handlers

BOOL CSerialNotifierDlg::OnInitDialog()
{
    CDialog::OnInitDialog();

    // Set the icon for this dialog.  The framework does this automatically
    //  when the application's main window is not a dialog
    SetIcon(_h_icon, TRUE);            // Set big icon
    SetIcon(_h_icon, FALSE);        // Set small icon


    // TODO: Add extra initialization here
    //this->TrayIcon_Start(_T("Serial Monitor"));

    CreateTrayIcon();
    SetTrayIconTipText(TEXT("Serial monitor"));

    //MessageBox(TEXT("qwerty мама мыла раму"), TEXT("123"), MB_OK | MB_ICONINFORMATION);

    PrepareMenu();

    //UINT tread_id = ::GetCurrentThreadId();

    ;
    AfxBeginThread(SerialListChangingMonitor, this->m_hWnd );
    return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CSerialNotifierDlg::OnPaint()
{
    //ShowWindow(SW_HIDE);
    if (IsIconic())
    {
        CPaintDC dc(this); // device context for painting

        SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

        // Center icon in client rectangle
        int cxIcon = GetSystemMetrics(SM_CXICON);
        int cyIcon = GetSystemMetrics(SM_CYICON);
        CRect rect;
        GetClientRect(&rect);
        int x = (rect.Width() - cxIcon + 1) / 2;
        int y = (rect.Height() - cyIcon + 1) / 2;

        // Draw the icon
        dc.DrawIcon(x, y, _h_icon);
    }
    else
    {
        CDialog::OnPaint();
    }
}

void CSerialNotifierDlg::OnDestroy()
{
    DestroyTrayIcon();
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
//HCURSOR CSTrayDlg::OnQueryDragIcon()
//{
//    return static_cast<HCURSOR>(m_hIcon);
//}

BOOL CSerialNotifierDlg::CreateTrayIcon()
{
    memset(&_notify_icon_data, 0 , sizeof(_notify_icon_data));
    _notify_icon_data.cbSize = sizeof(_notify_icon_data);

    // set tray icon ID
    _notify_icon_data.uID = IDD_SYSTEMTRAY;

    // set handle to the window that receives tray icon notifications
    ASSERT(::IsWindow(GetSafeHwnd()));
    _notify_icon_data.hWnd = GetSafeHwnd();

    // set message that will be sent from tray icon to the window
    _notify_icon_data.uCallbackMessage = WM_TRAYICON_EVENT;

    // fields that are being set when adding tray icon
    _notify_icon_data.uFlags = NIF_MESSAGE|NIF_ICON;

    // set image
    _notify_icon_data.hIcon = LoadIcon(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDR_MAINFRAME1));

    if(!_notify_icon_data.hIcon)
        return FALSE;

    _notify_icon_data.uVersion = NOTIFYICON_VERSION_4;

    if(!Shell_NotifyIcon(NIM_ADD, &_notify_icon_data))
        return FALSE;

    return Shell_NotifyIcon(NIM_SETVERSION, &_notify_icon_data);
}

BOOL CSerialNotifierDlg::DestroyTrayIcon()
{
    return Shell_NotifyIcon(NIM_DELETE, &_notify_icon_data);
}

BOOL CSerialNotifierDlg::SetTrayIconTipText(LPCTSTR pszText)
{
    if(StringCchCopy(_notify_icon_data.szTip, sizeof(_notify_icon_data.szTip), pszText) != S_OK)
        return FALSE;

    _notify_icon_data.uFlags |= NIF_TIP;
    return Shell_NotifyIcon(NIM_MODIFY, &_notify_icon_data);
}

BOOL CSerialNotifierDlg::ShowTrayIconBalloon(LPCTSTR pszTitle, LPCTSTR pszText, UINT unTimeout, DWORD dwInfoFlags)
{
    _notify_icon_data.uFlags |= NIF_INFO;
    _notify_icon_data.uTimeout = unTimeout;
    _notify_icon_data.dwInfoFlags = dwInfoFlags;

    if(StringCchCopy(_notify_icon_data.szInfoTitle, sizeof(_notify_icon_data.szInfoTitle), pszTitle) != S_OK)
        return FALSE;

    if(StringCchCopy(_notify_icon_data.szInfo, sizeof(_notify_icon_data.szInfo), pszText) != S_OK)
        return FALSE;

    return Shell_NotifyIcon(NIM_MODIFY, &_notify_icon_data);
}

BOOL CSerialNotifierDlg::SetTrayIcon(HICON hIcon)
{
    _notify_icon_data.hIcon = hIcon;
    _notify_icon_data.uFlags |= NIF_ICON;

    return Shell_NotifyIcon(NIM_MODIFY, &_notify_icon_data);
}

BOOL CSerialNotifierDlg::SetTrayIcon(WORD wIconID)
{
    HICON hIcon = LoadIcon(AfxGetInstanceHandle(), MAKEINTRESOURCE(wIconID));

    if(!hIcon)
        return FALSE;

    return SetTrayIcon(hIcon);
}

VOID CSerialNotifierDlg::PrepareMenu()
{
    _menu.CreatePopupMenu();

    _menu.AppendMenu(MF_SEPARATOR,  WM_POPUP_SEPARATOR,     TEXT(""));
    _menu.AppendMenu(MF_STRING,     WM_POPUP_POPUP_ENABLE,  TEXT("Popup"));
    _menu.AppendMenu(MF_STRING,     WM_POPUP_AUTORUN,       TEXT("Autorun"));
    _menu.AppendMenu(MF_SEPARATOR,  WM_POPUP_SEPARATOR,     TEXT(""));
    _menu.AppendMenu(MF_STRING,     WM_POPUP_EXIT,          TEXT("Exit"));

    _menu.CheckMenuItem(WM_POPUP_POPUP_ENABLE, _settings.popup() ? MF_CHECKED : MF_UNCHECKED);
    _menu.CheckMenuItem(WM_POPUP_AUTORUN,      _settings.autorun() ? MF_CHECKED : MF_UNCHECKED);
}

void CSerialNotifierDlg::CreateDevicesSubMenu()
{
    _devices_submenu.CreatePopupMenu();

    _serial.refresh_serial_list();
    Serial::SerialList serial_list = _serial.get_list();

    for(Serial::SerialList::iterator it = serial_list.begin(); it != serial_list.end(); it++)
    {
        CString next_item_string;
        next_item_string.Format(TEXT("%-7s  "), it->device_name);
        next_item_string += it->description.GetLength() ? it->description : CString(TEXT("\"")) + it->friendly_name + CString(TEXT("\""));
        _devices_submenu.AppendMenu(MF_STRING , WM_POPUP_SERIAL_LIST + std::distance(serial_list.begin(), it), next_item_string.GetString());
    }

    if (serial_list.empty())
    {
        _devices_submenu.AppendMenu(MF_STRING | MF_GRAYED, WM_POPUP_SERIAL_LIST , TEXT("(none)"));
    }

    _menu.InsertMenu(0, MF_POPUP | MF_STRING| MF_BYPOSITION, (UINT)_devices_submenu.m_hMenu,  TEXT("Devices"));
}

void CSerialNotifierDlg::DestroyDevicesSubMenu()
{
    _devices_submenu.DestroyMenu();
	_menu.DeleteMenu(0, MF_BYPOSITION);
}

LRESULT CSerialNotifierDlg::OnTrayIconEvent(WPARAM wp, LPARAM lp)
{
    (void)wp;

    switch(lp)
    {
        case WM_LBUTTONDOWN:
        case WM_RBUTTONDOWN:
        case WM_LBUTTONDBLCLK:
        case WM_RBUTTONDBLCLK:
        {
            POINT cp;
            GetCursorPos(&cp);

            CreateDevicesSubMenu();
            SetForegroundWindow();
           _menu.TrackPopupMenu(TPM_RIGHTALIGN |TPM_RETURNCMD, cp.x, cp.y, this);

           DestroyDevicesSubMenu();

           break;
        }

        default:
            break;
    }
    return NULL;
}

LRESULT CSerialNotifierDlg::OnChangedSerialList(WPARAM wp, LPARAM lp)
{
    (void)wp;
    (void)lp;

    Serial::SerialListDiff diff = _serial.refresh_serial_list();
    //MessageBox(TEXT("qwerty мама мыла раму"), TEXT("123"), MB_OK | MB_ICONINFORMATION);


    return NULL;
}

UINT CSerialNotifierDlg::SerialListChangingMonitor(LPVOID param)
{
    (void)param;

    while(1)
    {
        ::Sleep(5*1000);
        //PostMessage(hwnd, WM_MYICONNOTIFY, NULL, WM_LBUTTONDOWN);
        AfxGetMainWnd()->PostMessage(WM_SERIAL_LIST_WAS_CHANGED, NULL, NULL);

        //TODO monitor the registry
    }
}