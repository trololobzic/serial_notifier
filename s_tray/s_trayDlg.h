// s_trayDlg.h : header file
//

#pragma once
#pragma comment (lib, "Setupapi.lib")

#include <windows.h>
#include <initguid.h>
#include <devguid.h>
#include <setupapi.h>
#include <vector>
#include <algorithm>
#include "SerialInfo.h"

#define sizeofarr(__a__) sizeof(__a__)/sizeof(__a__[0]) 

#define REG_AUTORUN_ID _T("s_tray")
#define REGKEY_SERIAL _T("HARDWARE\\DEVICEMAP\\SERIALCOMM")

#define WM_MYICONNOTIFY					(WM_USER+1)
#define WM_POPUP_AUTORUN				(WM_USER+2)
#define WM_POPUP_SEPARATOR				(WM_USER+3)
#define WM_POPUP_EXIT					(WM_USER+4)
#define WM_POPUP_SERIAL_LIST			(WM_USER+5)


// Cs_trayDlg dialog
class Cs_trayDlg : public CDialog
{
// Construction
public:
	Cs_trayDlg(CWnd* pParent = NULL);	// standard constructor
	~Cs_trayDlg()
	{
		TrayIcon_Stop();
	}

// Dialog Data
	enum { IDD = IDD_S_TRAY_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()


public:
	//registry part	
	std::vector<SerialInfo> serialList;
	BOOL SetAutorunSetting(BOOL autorunEnable);
	BOOL GetAutorunSetting();
	void GetSerialList(std::vector<SerialInfo> & serialList);
	void HandleRegKeyChanging();
	void GetSerialFriendlyName(std::vector<SerialInfo> & serialList);

	//tray icon part
	HWND trayIconWndId;
	void TrayIcon_Start(char * tip);
	void TrayIcon_Stop();
	void TrayIcon_Ballon(const char * message, char * title, DWORD infoFlag );

	afx_msg LRESULT OnIcon(WPARAM wp, LPARAM lp);	

	CMenu menu;
	afx_msg void OnPopupExit();
	afx_msg void OnPopupAutorun();
};


UINT DoWaitNotificationThread(LPVOID param);