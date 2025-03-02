#include "stdafx.h"
#include "main_app.h"
#include "main_dlg.h"
#include "registry/registry.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#ifdef DEBUG
  #define TRACE_OUTPUT trace_output
#else
  #define TRACE_OUTPUT(...) ((void)0) //strip out PRINT instructions from code
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
    DDX_Control(pDX, IDC_TRACE_OUTPUT, _trace_cedit);
}

BEGIN_MESSAGE_MAP(CSerialNotifierDlg, CDialog)
    ON_WM_PAINT()
    ON_WM_DESTROY()
    ON_MESSAGE(WM_TRAYICON_EVENT, OnTrayIconEvent)
    ON_MESSAGE(WM_SERIAL_LIST_WAS_CHANGED, OnChangedSerialList)
    ON_MESSAGE(WM_POPUP_AUTORUN, OnChoiceMenuItemAutorun)
    ON_MESSAGE(WM_POPUP_POPUP_ENABLE, OnChoiceMenuItemPopup)
    ON_MESSAGE(WM_POPUP_SERIAL_LIST, OnChoiceMenuItemSerialList)
    //ON_WM_QUERYDRAGICON()
END_MESSAGE_MAP()



BOOL CSerialNotifierDlg::OnInitDialog()
{
    CDialog::OnInitDialog();

    try
    {
        _translation_ptr = serial_notifier::lang::Lang::get_translation(static_cast<serial_notifier::lang::SupportedLanguagesEnum>(_settings.lang()));
    }
    catch(serial_notifier::Exception & e)
    {
        ::MessageBox(NULL, e.what(), TEXT("Serial notifier"), MB_OK | MB_ICONERROR);
        DestroyWindow();
	    PostQuitMessage(0);
    }

    SetIcon(_h_icon, TRUE);            // Set big icon
    SetIcon(_h_icon, FALSE);        // Set small icon

    // Add extra initialization here
    CreateTrayIcon();
    SetTrayIconTipText(_translation_ptr->app_name);

    AfxBeginThread(SerialListChangingMonitor, &_serial);

    _trace_cedit.SetLimitText(MAXUINT);

    TRACE_OUTPUT(TEXT("Serial notifier start"));

    return FALSE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CSerialNotifierDlg::OnPaint()
{
    #ifndef DEBUG
    ShowWindow(SW_HIDE);
    #endif

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
    TRACE_OUTPUT(TEXT("%s start"), TEXT(__FUNCTION__));
    ::memset(&_notify_icon_data, 0 , sizeof(_notify_icon_data));
    _notify_icon_data.cbSize = NOTIFYICONDATA_V3_SIZE;

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

    _notify_icon_data.uTimeout = 1000u;

    if(!_notify_icon_data.hIcon)
    {
        TRACE_OUTPUT(TEXT("%s can not set notify icon data"), TEXT(__FUNCTION__));
        return FALSE;
    }

    _notify_icon_data.uVersion = NOTIFYICON_VERSION_4;

    if(!Shell_NotifyIcon(NIM_ADD, &_notify_icon_data))
    {
        TRACE_OUTPUT(TEXT("%s can not run NIM_ADD"), TEXT(__FUNCTION__));
        return FALSE;
    }

    //BOOL ret = Shell_NotifyIcon(NIM_SETVERSION, &_notify_icon_data);
    //TRACE_OUTPUT(TEXT("%s res [%u]"), TEXT(__FUNCTION__), ret);
    //return ret;
    return TRUE;
}

BOOL CSerialNotifierDlg::DestroyTrayIcon()
{
    return Shell_NotifyIcon(NIM_DELETE, &_notify_icon_data);
}

BOOL CSerialNotifierDlg::SetTrayIconTipText(const CString & text)
{
    if(StringCchCopy(_notify_icon_data.szTip, sizeof(_notify_icon_data.szTip), text) != S_OK)
    {
        TRACE_OUTPUT(TEXT("%s StringCchCopy return FALSE"), TEXT(__FUNCTION__));
        return FALSE;
    }

    _notify_icon_data.uFlags &= ~NIF_INFO;
    _notify_icon_data.uFlags |= NIF_TIP;
    BOOL ret =  Shell_NotifyIcon(NIM_MODIFY, &_notify_icon_data);
    TRACE_OUTPUT(TEXT("%s res [%u]"), TEXT(__FUNCTION__), ret);
    return ret;
}

/*
* Show notify ballooon message in tray.
* const CString & title  - title of balloon
* const CString & text   - messsage text
* const DWORD flags      - one of theese defines: NIIF_INFO, NIIF_WARNING, NIIF_ERROR, NIF_USER or 0
*/
BOOL CSerialNotifierDlg::ShowTrayIconBalloon(const CString & title, const CString & text, const DWORD flags)
{
    if (!_settings.popup())
    {
        TRACE_OUTPUT(TEXT("%s popup disabled"), TEXT(__FUNCTION__));
        return FALSE;
    }

    _notify_icon_data.uFlags |= NIF_INFO;
    _notify_icon_data.dwInfoFlags = flags;

    if(::StringCchCopy(_notify_icon_data.szInfoTitle, sizeof(_notify_icon_data.szInfoTitle), title) != S_OK)
    {
        TRACE_OUTPUT(TEXT("%s StringCchCopy return FALSE"), TEXT(__FUNCTION__));
        return FALSE;
    }

    if(::StringCchCopy(_notify_icon_data.szInfo, sizeof(_notify_icon_data.szInfo), text) != S_OK)
    {
        TRACE_OUTPUT(TEXT("%s StringCchCopy return FALSE"), TEXT(__FUNCTION__));
        return FALSE;
    }

    BOOL ret = Shell_NotifyIcon(NIM_MODIFY, &_notify_icon_data);
    TRACE_OUTPUT(TEXT("%s res [%u]"), TEXT(__FUNCTION__), ret);
    return ret;
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

VOID CSerialNotifierDlg::CreateMenu()
{
    _menu.CreatePopupMenu();

    _menu.AppendMenu(MF_SEPARATOR,  WM_POPUP_SEPARATOR,     TEXT(""));
    _menu.AppendMenu(MF_STRING,     WM_POPUP_POPUP_ENABLE,  _translation_ptr->menu_items.popup);
    _menu.AppendMenu(MF_STRING,     WM_POPUP_AUTORUN,       _translation_ptr->menu_items.autorun);
    _menu.AppendMenu(MF_SEPARATOR,  WM_POPUP_SEPARATOR,     TEXT(""));
    _menu.AppendMenu(MF_STRING,     WM_POPUP_EXIT,          _translation_ptr->menu_items.exit);

    _menu.CheckMenuItem(WM_POPUP_POPUP_ENABLE, _settings.popup() ? MF_CHECKED : MF_UNCHECKED);
    _menu.CheckMenuItem(WM_POPUP_AUTORUN,      _settings.autorun() ? MF_CHECKED : MF_UNCHECKED);

    typedef serial_notifier::lang::Lang Lang;
    const Lang::TranslationsList & translations_list = Lang::get_all_translations_list();
    TRACE_OUTPUT(TEXT("%s [%u] translations are available"), TEXT(__FUNCTION__), translations_list.size());
    _languages_submenu.CreatePopupMenu();
    for(Lang::TranslationsList::const_iterator it = translations_list.begin(); it != translations_list.end(); it++)
    {
        TRACE_OUTPUT(TEXT("%s append translation [%s]"), TEXT(__FUNCTION__), (*it)->lang_name.GetString());
        CString next_item_string = (*it)->lang_name;
        size_t idx = std::distance(translations_list.begin(), it);
        _languages_submenu.AppendMenu(MF_STRING , WM_POPUP_LANGS_LIST + idx, next_item_string);

        if ((*it) == _translation_ptr)
        {
            _languages_submenu.CheckMenuItem(WM_POPUP_LANGS_LIST + static_cast<DWORD>(idx), MF_CHECKED);
        }
    }
    _menu.InsertMenu(1, MF_POPUP | MF_STRING| MF_BYPOSITION, reinterpret_cast<UINT_PTR>(_languages_submenu.m_hMenu),  _translation_ptr->menu_items.languages);
}

void CSerialNotifierDlg::DestroyMenu()
{
    _languages_submenu.DestroyMenu();
    _menu.DeleteMenu(1, MF_BYPOSITION);
    _menu.DestroyMenu();
}

void CSerialNotifierDlg::CreateDevicesSubMenu()
{
    _devices_submenu.CreatePopupMenu();

    _serial.refresh_serial_list();
    Serial::SerialList serial_list = _serial.get_list();

    for(Serial::SerialList::iterator it = serial_list.begin(); it != serial_list.end(); it++)
    {
        CString next_item_string;
        next_item_string.Format(TEXT("%-7s  "), it->device_name.GetString());
        next_item_string += it->description.GetLength() ? it->description : CString(TEXT("\"")) + it->friendly_name + CString(TEXT("\""));
        _devices_submenu.AppendMenu(MF_STRING , WM_POPUP_SERIAL_LIST + std::distance(serial_list.begin(), it), next_item_string.GetString());
    }

    if (serial_list.empty())
    {
        _devices_submenu.AppendMenu(MF_STRING | MF_GRAYED, WM_POPUP_SERIAL_LIST , _translation_ptr->menu_items.none);
    }

    _menu.InsertMenu(0, MF_POPUP | MF_STRING| MF_BYPOSITION, reinterpret_cast<UINT_PTR>(_devices_submenu.m_hMenu),  _translation_ptr->menu_items.devices);
}

void CSerialNotifierDlg::DestroyDevicesSubMenu()
{
    _devices_submenu.DestroyMenu();
	_menu.DeleteMenu(0, MF_BYPOSITION);
}

LRESULT CSerialNotifierDlg::OnTrayIconEvent(WPARAM wp, LPARAM lp)
{
    (void)wp;

    UINT16 event_idx = static_cast<UINT16>(lp);
    if (event_idx != WM_LBUTTONDOWN && event_idx != WM_RBUTTONDOWN)
        return NULL;

    POINT cp;
    GetCursorPos(&cp);

    TRACE_OUTPUT(TEXT("%s show popup menu at [%lu,%lu]"), TEXT(__FUNCTION__), cp.x, cp.y);

    CreateMenu();
    CreateDevicesSubMenu();
    SetForegroundWindow();

    INT32 menu_choice = _menu.TrackPopupMenu(TPM_RIGHTALIGN |TPM_RETURNCMD, cp.x, cp.y, this);
    TRACE_OUTPUT(TEXT("%s menu choice is [%d]"), TEXT(__FUNCTION__), menu_choice);
    if (menu_choice == WM_POPUP_AUTORUN)
    {
        SendMessage(WM_POPUP_AUTORUN, NULL, NULL); //handle message immediately
    }
    else if (menu_choice == WM_POPUP_POPUP_ENABLE)
    {
        SendMessage(WM_POPUP_POPUP_ENABLE, NULL, NULL); //handle message immediately
    }
    else if (menu_choice == WM_POPUP_EXIT)
    {
        DestroyWindow();
	    PostQuitMessage(0);
    }
    else if (menu_choice >= WM_POPUP_SERIAL_LIST && menu_choice < WM_POPUP_SERIAL_LIST + static_cast<INT32>(_serial.get_list_size()))
    {
        size_t device_idx = menu_choice - WM_POPUP_SERIAL_LIST;
        SendMessage(WM_POPUP_SERIAL_LIST, static_cast<WPARAM>(device_idx), NULL); //handle message immediately
    }
    else if (menu_choice >= WM_POPUP_LANGS_LIST && menu_choice < WM_POPUP_LANGS_LIST + static_cast<INT32>(serial_notifier::lang::Lang::get_all_translations_list().size()))
    {
        size_t lang_idx = menu_choice - WM_POPUP_LANGS_LIST;

        const serial_notifier::lang::TranslationBase * const new_translation_ptr = serial_notifier::lang::Lang::get_all_translations_list()[lang_idx];
        if (new_translation_ptr != _translation_ptr)
        {
            _settings.lang(new_translation_ptr->get_cur_kang_idx());
            _translation_ptr = new_translation_ptr;

            SetTrayIconTipText(_translation_ptr->app_name);
        }
    }

    DestroyDevicesSubMenu();
    DestroyMenu();
    return NULL;
}

LRESULT CSerialNotifierDlg::OnChoiceMenuItemAutorun(WPARAM wp, LPARAM lp)
{
    (void)wp;
    (void)lp;

    if (_menu.GetMenuState(WM_POPUP_AUTORUN, MF_BYCOMMAND) & MF_CHECKED)
    {
        _settings.autorun(false);
        _menu.CheckMenuItem(WM_POPUP_AUTORUN, MF_UNCHECKED );
    }
    else
    {
        _settings.autorun(true);
        _menu.CheckMenuItem(WM_POPUP_AUTORUN, MF_CHECKED );
    }

    return NULL;
}

LRESULT CSerialNotifierDlg::OnChoiceMenuItemPopup(WPARAM wp, LPARAM lp)
{
    (void)wp;
    (void)lp;

    if (_menu.GetMenuState(WM_POPUP_POPUP_ENABLE, MF_BYCOMMAND) & MF_CHECKED)
    {
        _settings.popup(false);
        _menu.CheckMenuItem(WM_POPUP_POPUP_ENABLE, MF_UNCHECKED );
    }
    else
    {
        if (_settings.system_popup())
        {
            _settings.popup(true);
            _menu.CheckMenuItem(WM_POPUP_POPUP_ENABLE, MF_CHECKED );
        }
        else
        {
            if (MessageBox(_translation_ptr->question_enable_sys_popup, _translation_ptr->app_name, MB_YESNO | MB_ICONQUESTION) == IDYES)

            {
                _settings.system_popup(true);
                _settings.popup(true);
                _menu.CheckMenuItem(WM_POPUP_POPUP_ENABLE, MF_CHECKED );
            }
        }
    }

    return NULL;
}

LRESULT CSerialNotifierDlg::OnChoiceMenuItemSerialList(WPARAM wp, LPARAM lp)
{
    (void)lp;

    serial_notifier::SerialDevice device = _serial.get_list()[static_cast<size_t>(wp)];

    MessageBoxData * mesage_box_data = new MessageBoxData;
    mesage_box_data->title.Format(_translation_ptr->app_name);
    mesage_box_data->text.Format(_translation_ptr->serial_info, device.device_name.GetString(), device.friendly_name.GetString(), device.description.GetString());
    mesage_box_data->flags = MB_OK | MB_ICONINFORMATION;

    AfxBeginThread(ShowMessageBox, mesage_box_data);
    return NULL;
}

LRESULT CSerialNotifierDlg::OnChangedSerialList(WPARAM wp, LPARAM lp)
{
    (void)wp;
    (void)lp;

    Sleep(200);

    Serial::SerialListDiff diff = _serial.refresh_serial_list();

    if (diff.plugged_devices.empty() && diff.unplugged_devices.empty())
        return NULL;

    CString plugged_message = MakeBalloonMessage(diff.plugged_devices, _translation_ptr->popup_messages.plugged_singular, _translation_ptr->popup_messages.plugged_plural);
    CString unplugged_message = MakeBalloonMessage(diff.unplugged_devices, _translation_ptr->popup_messages.unplugged_singular, _translation_ptr->popup_messages.unplugged_plural);
    if (plugged_message.GetLength() && unplugged_message.GetLength())
    {
        plugged_message += CString(TEXT("\n"));
    }

    ShowTrayIconBalloon(_translation_ptr->app_name, plugged_message + unplugged_message, NIIF_USER);
    return NULL;
}

UINT CSerialNotifierDlg::SerialListChangingMonitor(LPVOID param)
{
    const Serial * serial_ptr = static_cast<Serial*>(param);

    while(1)
    {
        if (serial_notifier::Registry::blocking_wait_for_changing(serial_ptr->serials_reg_section, serial_ptr->serials_reg_path))
        {
            AfxGetMainWnd()->PostMessage(WM_SERIAL_LIST_WAS_CHANGED, NULL, NULL); //push message to message queue
        }
        else
        {
            ::Sleep(200);
        }
    }
}

UINT CSerialNotifierDlg::ShowMessageBox(LPVOID param)
{
    MessageBoxData * mesage_box_data = static_cast<MessageBoxData *>(param);
    ::MessageBox(NULL, mesage_box_data->text, mesage_box_data->title, mesage_box_data->flags);
    delete mesage_box_data;
    return 0;
}

CString CSerialNotifierDlg::MakeBalloonMessage(const Serial::SerialList & serial_list, const CString & singular_prefix, const CString & plural_prefix)
{
    CString message;
    switch (serial_list.size())
    {
        case 0:
            break;

        case 1:
            message += singular_prefix + CString(TEXT(" ")) + serial_list[0].device_name;
            break;

        default:
        {
            message += plural_prefix + CString(TEXT(":\n"));
            for (Serial::SerialList::const_iterator it = serial_list.begin(); it != serial_list.end(); it++)
            {
                message += it->device_name + CString(TEXT("\n"));
            }
            message.Delete(message.GetLength() - 1);
            break;
        }
    }
    return message;
}

#ifdef DEBUG
void CSerialNotifierDlg::trace_output(LPCTSTR format_str, ...)
{
    int length = _trace_cedit.GetWindowTextLength() * sizeof(TCHAR);
    CString msg, full_msg;
    va_list argList;
    va_start(argList, format_str);
    msg.FormatV(format_str, argList);
    va_end(argList);

    time_t curr_time = std::time(NULL);
    std::tm ptm;
    ::localtime_s(&ptm, &curr_time);
    full_msg.Format(TEXT("[%04d.%02d.%02d %02d:%02d:%02d] %s\r\n"), ptm.tm_year + 1900, ptm.tm_mon + 1, ptm.tm_mday, ptm.tm_hour, ptm.tm_min, ptm.tm_sec, msg.GetString());
    //_trace_cedit.SetFocus();
    _trace_cedit.SetSel(length, length);
    _trace_cedit.ReplaceSel(full_msg.GetString());
}
#endif
