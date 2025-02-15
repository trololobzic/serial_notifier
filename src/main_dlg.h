#pragma once
#include <windows.h>
#include <initguid.h>
#include <devguid.h>
#include <setupapi.h>
#include <vector>
#include <algorithm>
#include <strsafe.h>

//#define WM_MYICONNOTIFY                    (WM_USER+1)
#define WM_TRAYICON_EVENT (WM_APP + 1)

#define WM_MYICONNOTIFY                    (WM_USER+1)
#define WM_POPUP_AUTORUN                (WM_USER+2)
#define WM_POPUP_POPUP_ENABLE            (WM_USER+3)
#define WM_POPUP_SEPARATOR                (WM_USER+4)
#define WM_POPUP_EXIT                    (WM_USER+5)
#define WM_POPUP_SERIAL_LIST            (WM_USER+6)

#define ID_SYSTEMTRAY 0x1000

//#define sizeofarr(__a__) sizeof(__a__)/sizeof(__a__[0])
typedef serial_notifier::Settings<serial_notifier::Registry> Settings;
typedef serial_notifier::Serial<serial_notifier::Registry> Serial;

// Cmfc1Dlg dialog
class CSerialNotifierDlg : public CDialog
{
// Construction
public:
    CSerialNotifierDlg(Settings & settings, Serial & serial, CWnd* pParent = NULL);    // standard constructor
    ~CSerialNotifierDlg();

// Dialog Data
    enum { IDD = IDD_DIALOG };

    protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

    //static const int WM_MYICONNOTIFY = WM_USER + 1;

// Implementation
protected:
    HICON          _h_icon;
    NOTIFYICONDATA _notify_icon_data;
    CMenu          _menu;
    Settings&      _settings;
    Serial&        _serial;

    // Generated message map functions
    virtual BOOL OnInitDialog();
    afx_msg void OnDestroy();
    afx_msg void OnPaint();
    //afx_msg HCURSOR OnQueryDragIcon();


    BOOL CreateTrayIcon();
    BOOL SetTrayIconTipText(LPCTSTR pszText);
    BOOL ShowTrayIconBalloon(LPCTSTR pszTitle, LPCTSTR pszText, UINT unTimeout, DWORD dwInfoFlags);
    BOOL SetTrayIcon(HICON hIcon);
    BOOL SetTrayIcon(WORD wIconID);
    BOOL DestroyTrayIcon();

    BOOL CreateMenu();
    LRESULT OnTrayIconEvent(WPARAM wp, LPARAM lp);



    DECLARE_MESSAGE_MAP()
};
