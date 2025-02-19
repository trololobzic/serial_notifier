﻿#pragma once
#include <windows.h>
//#include <initguid.h>
//#include <devguid.h>
//#include <setupapi.h>
#include <vector>
#include <algorithm>
#include <strsafe.h>

typedef serial_notifier::Settings<serial_notifier::Registry> Settings;
typedef serial_notifier::Serial<serial_notifier::Registry> Serial;

// CSerialNotifierDlg dialog
class CSerialNotifierDlg : public CDialog
{
// Construction
public:
    CSerialNotifierDlg(Settings & settings, Serial & serial, CWnd* pParent = NULL);    // standard constructor
    ~CSerialNotifierDlg();

// Dialog Data
    enum { IDD = IDD_DIALOG };

    enum InterfaceItemsAndEvents
    {
        WM_TRAYICON_EVENT = (WM_USER+1),
        WM_SERIAL_LIST_WAS_CHANGED,

        WM_POPUP_AUTORUN,
        WM_POPUP_POPUP_ENABLE,
        WM_POPUP_SEPARATOR,
        WM_POPUP_EXIT,
        WM_POPUP_SERIAL_LIST,
    };

// Implementation
protected:
    HICON          _h_icon;
    NOTIFYICONDATA _notify_icon_data;
    CMenu          _menu;
    CMenu          _devices_submenu;
    Settings&      _settings;
    Serial&        _serial;

    struct MessageBoxData
    {
        CString title;
        CString text;
        UINT flags;
    };

protected:
    //
    static UINT SerialListChangingMonitor(LPVOID);
    static UINT ShowMessageBox(LPVOID);

    // Tray icon handle methods
    BOOL CreateTrayIcon();
    BOOL SetTrayIconTipText(const CString & text);
    BOOL ShowTrayIconBalloon(const CString & title, const CString & text, const DWORD dwInfoFlags);
    BOOL SetTrayIcon(HICON hIcon);
    BOOL SetTrayIcon(WORD wIconID);
    BOOL DestroyTrayIcon();

    //popup menu methods
    void PrepareMenu();
    void CreateDevicesSubMenu();
    void DestroyDevicesSubMenu();


    LRESULT OnTrayIconEvent(WPARAM wp, LPARAM lp);
    LRESULT OnChoiceMenuItemAutorun(WPARAM wp, LPARAM lp);
    LRESULT OnChoiceMenuItemPopup(WPARAM wp, LPARAM lp);
    LRESULT OnChoiceMenuItemSerialList(WPARAM wp, LPARAM lp);

    LRESULT OnChangedSerialList(WPARAM wp, LPARAM lp);

    CString MakeBalloonMessage(const Serial::SerialList & serial_list, const CString & singular_prefix, const CString & plural_prefix);


protected:
    // Generated message map functions
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    virtual BOOL OnInitDialog();
    //afx_msg HCURSOR OnQueryDragIcon();
    afx_msg void OnDestroy();
    afx_msg void OnPaint();
    DECLARE_MESSAGE_MAP()
};
