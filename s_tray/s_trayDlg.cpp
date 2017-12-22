// s_trayDlg.cpp : implementation file
//




#include "stdafx.h"
#include "s_tray.h"
#include "s_trayDlg.h"
#include "SerialInfo.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// Cs_trayDlg dialog




Cs_trayDlg::Cs_trayDlg(CWnd* pParent /*=NULL*/)
	: CDialog(Cs_trayDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDI_ICON1);	
}

void Cs_trayDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(Cs_trayDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	//ON_BN_CLICKED(IDC_BUTTON1, &Cs_trayDlg::OnBnClickedButton1)
	//ON_BN_CLICKED(IDC_BUTTON2, &Cs_trayDlg::OnBnClickedButton2)
	ON_MESSAGE(WM_MYICONNOTIFY,OnIcon)
	ON_COMMAND(WM_POPUP_EXIT, &Cs_trayDlg::OnPopupExit)
	ON_COMMAND(WM_POPUP_AUTORUN, &Cs_trayDlg::OnPopupAutorun)
END_MESSAGE_MAP()


// Cs_trayDlg message handlers

BOOL Cs_trayDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
	
	this->trayIconWndId = m_hWnd;
	this->TrayIcon_Start(_T("Serial Monitor"));

	this->GetSerialList(this->serialList);
	std::sort(this->serialList.rbegin(), this->serialList.rend());

	this->menu.CreatePopupMenu();
	this->menu.AppendMenu(MF_STRING, WM_POPUP_AUTORUN, _T("Автозапуск"));
	this->menu.AppendMenu(MF_SEPARATOR, WM_POPUP_SEPARATOR, _T(""));
	this->menu.AppendMenu(MF_STRING, WM_POPUP_EXIT, _T("Выход"));
	
	if ( this->GetAutorunSetting() )
		this->menu.CheckMenuItem(WM_POPUP_AUTORUN, MF_CHECKED);

	AfxBeginThread(DoWaitNotificationThread, this );	

	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void Cs_trayDlg::OnPaint()
{
	ShowWindow(SW_HIDE);
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
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{		
		CDialog::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR Cs_trayDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



//tray icon part
void Cs_trayDlg::TrayIcon_Start(char * tip)
{
	NOTIFYICONDATA nf = {0};

	nf.cbSize = NOTIFYICONDATA_V3_SIZE ;
	nf.hWnd = trayIconWndId;
	nf.hIcon = m_hIcon;
	nf.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP ;
	nf.uCallbackMessage = WM_MYICONNOTIFY;	
	lstrcpyn(nf.szTip, tip, sizeofarr(nf.szTip) - 1 );
	nf.uTimeout = 11000;
	Shell_NotifyIcon(NIM_ADD,&nf);	
	return;
}
void Cs_trayDlg::TrayIcon_Stop()
{
	NOTIFYICONDATA nf = {0};

	nf.cbSize = NOTIFYICONDATA_V3_SIZE ;
	
	nf.hWnd = trayIconWndId;
	nf.uID = NULL;
	nf.uFlags = NIF_ICON;
	nf.uCallbackMessage = NULL;
	nf.hIcon = NULL;

	Shell_NotifyIcon(NIM_DELETE,&nf);	
	return;
}
void Cs_trayDlg::TrayIcon_Ballon(const char * message, char * title, DWORD infoFlag = NIIF_NONE)
{
	NOTIFYICONDATA nf = {0};
	nf.cbSize = NOTIFYICONDATA_V3_SIZE ;
	nf.hWnd = trayIconWndId;
	nf.hIcon=m_hIcon;	
	nf.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP | NIF_INFO /*| NIF_REALTIME*/;	
	nf.uCallbackMessage = WM_MYICONNOTIFY;
    nf.dwInfoFlags = infoFlag;    
    nf.uTimeout = 12000;
	lstrcpyn ( nf.szTip, message, sizeofarr(nf.szTip) - 1 );
    lstrcpyn ( nf.szInfo, message, sizeofarr(nf.szInfo) - 1 );
    lstrcpyn ( nf.szInfoTitle, title, sizeofarr(nf.szInfoTitle) - 1);
    Shell_NotifyIcon ( NIM_MODIFY, &nf );
}

LRESULT Cs_trayDlg::OnIcon(WPARAM wp, LPARAM lp)
{
	char msg[512] = {0};
	int cmdId = 0, portId = 0 ;
	//_snprintf(msg, 512 , "%d", lp );
	//AfxMessageBox(msg);

	switch(lp)
	{
	case WM_LBUTTONDOWN:
	case WM_RBUTTONDOWN:
	case WM_LBUTTONDBLCLK:
	case WM_RBUTTONDBLCLK:
		{
			//draw popup menu
			CMenu submenu;
			POINT cp;
			GetCursorPos(&cp);			
			submenu.CreatePopupMenu();

			if (serialList.size())
			{
				for (UINT i = 0 ; i < serialList.size(); i++)
				{
					CString str;
					if ( serialList[i].friendlyName.GetLength() )
					{
						//str += serialList[i].friendlyName;
						str.Format(_T("%-7s  %s"), serialList[i].port, serialList[i].friendlyName);
					}
					else
					{
						str.Format(_T("%-7s  \"%s\""), serialList[i].port, serialList[i].name);
					}
					
					
					
					submenu.AppendMenu(MF_STRING , WM_POPUP_SERIAL_LIST + i, str);
				}
			}
			else
			{
				submenu.AppendMenu(MF_STRING | MF_GRAYED, WM_POPUP_SERIAL_LIST , _T("(нет)"));
			}


			
			menu.InsertMenu(0, MF_POPUP|MF_STRING| MF_BYPOSITION, (UINT)submenu.m_hMenu,  _T("Устройства"));
			
			SetForegroundWindow();
			cmdId = menu.TrackPopupMenu(TPM_RIGHTALIGN | TPM_RETURNCMD, cp.x, cp.y, this); 
			

			submenu.DestroyMenu();
			menu.DeleteMenu(0, MF_BYPOSITION);

			switch(cmdId)
			{
			case 0:
				//do nothing
				break;

			case WM_POPUP_AUTORUN:
				this->OnPopupAutorun();
				break;

			case WM_POPUP_EXIT:
				this->OnPopupExit();
				break;

			default:
				{
					portId = cmdId - WM_POPUP_SERIAL_LIST;
					if ( (portId >= 0) && (portId < serialList.size()) )
					{	
						HANDLE Port;
						COMMCONFIG comm;
						
						CString portNameStr = _T("");				
						portNameStr+= _T("\\\\.\\") + serialList[portId].port;
						
						Port = CreateFile(portNameStr, 0, 0, NULL, OPEN_EXISTING, 0, NULL);
						if (Port == INVALID_HANDLE_VALUE)
						{
							CString msg;
							msg.Format(_T("%s занят"), serialList[portId].port);
							MessageBox(msg, "Error", MB_OK | MB_ICONERROR);				 
						}
						else
						{
							unsigned long new_size = 0;
							if (GetCommConfig(Port, &comm,&new_size))
							{
								CommConfigDialog( serialList[portId].port, NULL,&comm);
							}
							CloseHandle(Port);
							
						}
					}
				}
				break;

			}

			
		}
		break;

	default:
		break;
	
	}
	return 0;
}

void Cs_trayDlg::OnPopupExit()
{
	DestroyWindow();
	PostQuitMessage(0);
}

void Cs_trayDlg::OnPopupAutorun()
{	
	BOOL autorunEnable = TRUE;
	UINT checkBoxChecked = MF_CHECKED;
	UINT flags = menu.GetMenuState(WM_POPUP_AUTORUN, MF_BYCOMMAND);


	if (flags & MF_CHECKED)
	{
		autorunEnable = FALSE;
		checkBoxChecked = MF_UNCHECKED ;
	}	
	
	if ( SetAutorunSetting(autorunEnable) )
			menu.CheckMenuItem(WM_POPUP_AUTORUN, checkBoxChecked );
}

BOOL Cs_trayDlg::GetAutorunSetting()
{
	BOOL res = FALSE;
	HKEY hKey;	
	if( ::RegOpenKeyEx( HKEY_CURRENT_USER, _T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run"),0, 
		KEY_QUERY_VALUE, &hKey) == ERROR_SUCCESS)
	{
		TCHAR szData[256];
		DWORD dwKeyDataType;
		DWORD dwDataBufSize = 256;
		if (::RegQueryValueEx(hKey, REG_AUTORUN_ID, NULL, &dwKeyDataType, // /"ProxyServer"
				(LPBYTE) &szData, &dwDataBufSize) == ERROR_SUCCESS)
		{
			res = TRUE;
			/*
			switch ( dwKeyDataType )
			{
				case REG_SZ:
					cSvar = CString(szData);
					break;
			}
			*/
		}

	}
	::RegCloseKey( hKey );

	return res;
}

BOOL Cs_trayDlg::SetAutorunSetting(BOOL autorunEnable)
{
	BOOL res = TRUE;
	HKEY hKey;	
	char szPath[1024] = {0};
	LSTATUS errorCode = ERROR_SUCCESS ;
	CString errorStr = _T("");

	errorCode = ::RegOpenKeyEx( HKEY_CURRENT_USER, 
		_T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run"),0, 
		KEY_WRITE, &hKey);

	if( errorCode != ERROR_SUCCESS)
	{
		errorStr.Format(_T("RegOpenKeyEx() ошибка %d"), errorCode );
		AfxMessageBox(errorStr, MB_ICONSTOP | MB_OK);
		return FALSE;
	}
	
	if (autorunEnable)
	{
		GetModuleFileName(GetModuleHandle(0),szPath, sizeofarr(szPath)-1);

		errorCode = ::RegSetValueEx(hKey,
			REG_AUTORUN_ID,
			0,
			REG_SZ,			
			(LPBYTE)szPath,			
			strlen(szPath)*sizeof(char));
		if ( errorCode != ERROR_SUCCESS)
		{
			errorStr.Format(_T("RegSetValueEx() ошибка %d"), errorCode );
			AfxMessageBox(errorStr, MB_ICONSTOP | MB_OK);
			res = FALSE ;
		}
	}
	else
	{
		errorCode = ::RegDeleteValue( hKey, REG_AUTORUN_ID );
		if ( errorCode != ERROR_SUCCESS )
		{
			errorStr.Format(_T("RegDeleteValue() ошибка %d"), errorCode );
			AfxMessageBox(errorStr, MB_ICONSTOP | MB_OK);
			res = FALSE ;
		}
	}

	errorCode = ::RegCloseKey( hKey );
	if ( errorCode != ERROR_SUCCESS )
	{
		errorStr.Format(_T("RegCloseKey() ошибка %d"), errorCode );
		AfxMessageBox(errorStr, MB_ICONSTOP | MB_OK);
		res = FALSE ;
	}
	return res;
}

void Cs_trayDlg::GetSerialList(std::vector<SerialInfo> & serialList)
{
	HKEY hKey;
	LSTATUS errorCode = ERROR_SUCCESS ;

	//RegQueryInfoKey() definitions
	const int MAX_KEY_LENGTH = 255;
	const int MAX_VALUE_NAME = 16383;
	//TCHAR    achKey[MAX_KEY_LENGTH];   // buffer for subkey name
	//DWORD    cbName;                   // size of name string 
	TCHAR    achClass[MAX_PATH] = _T("");  // buffer for class name 
	TCHAR    value[MAX_VALUE_NAME] = _T("");  // buffer for value 
	DWORD    cchClassName = MAX_PATH;  // size of class string 
	DWORD    cSubKeys=0;               // number of subkeys 
	DWORD    cbMaxSubKey;              // longest subkey size 
	DWORD    cchMaxClass;              // longest class string 
	DWORD    cValues;              // number of values for key 
	DWORD    cchMaxValue;          // longest value name 
	DWORD    cbMaxValueData;       // longest value data 
	DWORD    cbSecurityDescriptor; // size of security descriptor 
	FILETIME ftLastWriteTime;      // last write time 
	//DWORD i, retCode; 
	TCHAR  achValue[MAX_VALUE_NAME]; 
	DWORD cchValue = MAX_VALUE_NAME;

	serialList.clear();

	errorCode = ::RegOpenKeyEx( HKEY_LOCAL_MACHINE, 
		REGKEY_SERIAL ,0, 
		KEY_READ, &hKey);

	switch(errorCode)
	{
	case ERROR_SUCCESS:
		break;

	case ERROR_FILE_NOT_FOUND:
		{
			//threris no HARDWARE\\DEVICEMAP\\SERIALCOMM. Create it?
			if ( ::RegCreateKey(HKEY_LOCAL_MACHINE, REGKEY_SERIAL, &hKey ) == ERROR_SUCCESS )
			{
				::RegCloseKey( hKey );
				TrayIcon_Ballon(_T("COM-порты отсутствуют в системе"), _T("Serial Monitor"), NIIF_INFO );
				return;
			}
		}

	default:
		{			
			CString errorStr;
			errorStr.Format(_T("Ошибка %d"), errorCode);
			TrayIcon_Ballon(errorStr.GetString(), _T("Serial Monitor"), NIIF_FILTER_INTERFACE );

			::RegCloseKey( hKey );
		}
		return;

	} //switch(errorCode)

	errorCode = ::RegQueryInfoKey(
		hKey,                    // key handle 
		achClass,                // buffer for class name 
		&cchClassName,           // size of class string 
		NULL,                    // reserved 
		&cSubKeys,               // number of subkeys 
		&cbMaxSubKey,            // longest subkey size 
		&cchMaxClass,            // longest class string 
		&cValues,                // number of values for this key 
		&cchMaxValue,            // longest value name 
		&cbMaxValueData,         // longest value data 
		&cbSecurityDescriptor,   // security descriptor 
		&ftLastWriteTime);       // last write time 

	if (errorCode != ERROR_SUCCESS)
	{
		CString errorStr;
		errorStr.Format(_T("Ошибка %d"), errorCode);
		TrayIcon_Ballon(errorStr.GetString(), _T("Serial Monitor"), NIIF_FILTER_INTERFACE );

		::RegCloseKey( hKey );
		return ;
	}

	if (cValues)
	{
		for (int i = 0 ; i < (int)cValues; i++)
		{
			SerialInfo serialInfo;
			cchValue = MAX_VALUE_NAME;
			achValue[0] = '\0';
			errorCode = ::RegEnumValue(hKey, i, 
				achValue, 
				&cchValue, 
				NULL, 
				NULL,
				NULL,
				NULL);

			if ( errorCode == ERROR_SUCCESS )
			{
				DWORD lpData = cbMaxValueData;
				value[0] = '\0';
				RegQueryValueEx(hKey, achValue, 0, NULL, (LPBYTE)value, &lpData);

				serialInfo.name = achValue;
				serialInfo.port = value;
				serialList.push_back(serialInfo);

			}
/*
			cchValue = MAX_VALUE_NAME; 
			achValue[0] = '\0'; 
			retCode = RegEnumValue(hKey, i, 
				achValue, 
				&cchValue, 
				NULL, 
				NULL,
				NULL,
				NULL);

			if (retCode == ERROR_SUCCESS ) 
				{ 
				
				DWORD lpData = cbMaxValueData;
				buffer[0] = '\0';
				LONG dwRes = RegQueryValueEx(hKey, achValue, 0, NULL, buffer, &lpData);
				_tprintf(TEXT("(%d) %s : %s\n"), i+1, achValue, buffer); 
				} 
			}
		*/

		}

		GetSerialFriendlyName(serialList);

	}
	else
	{
		TrayIcon_Ballon(_T("COM-порты отсутствуют в системе"), _T("Serial Monitor"), NIIF_INFO );
	}

	::RegCloseKey( hKey );
	return;

}

void Cs_trayDlg::HandleRegKeyChanging()
{
	CString out;
	HKEY hKey;	
	std::vector<SerialInfo> newSerialList;
	std::vector<SerialInfo> addedSerialList;
	std::vector<SerialInfo> removedSerialList;

	Sleep(100);
	this->GetSerialList(newSerialList);


	std::sort(newSerialList.rbegin(), newSerialList.rend());	
#if 0
		
	std::set_difference(serialList.begin(), 
		serialList.end(), 
		newSerialList.begin(), 
		newSerialList.end(), 
		std::back_inserter(removedSerialList),
		SerialInfo::Compare);
    
    std::set_difference(newSerialList.begin(), 
		newSerialList.end(), 
		serialList.begin(), 
		serialList.end(), 
		std::back_inserter(addedSerialList),
		SerialInfo::Compare);

#else
	for (int i = 0 ; i < newSerialList.size() ; i++ )
	{
		BOOL itIsNewElement = TRUE ;
		for (int j = 0 ; j < serialList.size(); j++ )
		{			
			if ( newSerialList[i] == serialList[j] )			
			{
				itIsNewElement = FALSE ;
				break;
			}
		}

		if (itIsNewElement)
			addedSerialList.push_back( newSerialList[i] );
	}

	for (int i = 0 ; i < serialList.size() ; i++ )
	{
		BOOL itIsRemovedElement = TRUE ;
		for (int j = 0 ; j < newSerialList.size(); j++ )
		{
			//if ( !memcmp(&serialList[i], &newSerialList[j], sizeof(SerialInfo)) )
			if ( serialList[i] == newSerialList[j])		
			{
				itIsRemovedElement = FALSE ;
				break;
			}
		}

		if (itIsRemovedElement)
			removedSerialList.push_back( serialList[i] );
	}

#endif

	if ( addedSerialList.size() )
	{
		BOOL multilineOutput = FALSE ;
		if (addedSerialList.size() > 1)
			multilineOutput = TRUE ;

		//only added elements
		out += _T("Подключен");
		if ( multilineOutput )
			out += _T("ы:\n");
		else
			out += _T(" ");

		for (int i = 0 ; i < addedSerialList.size(); i++)
		{
			CString serialName;
			serialName.Format(_T("%s"), addedSerialList[i].port );
			if ( multilineOutput && i )
				out += _T("\n");
			out += serialName ;
		}
	}
	if ( removedSerialList.size() )
	{
		BOOL multilineOutput = FALSE ;
		if (removedSerialList.size() > 1)
			multilineOutput = TRUE ;

		if (out.GetLength())
			out += _T("\n");

		//only removed elements
		out += _T("Удален");
		if ( multilineOutput )
			out += _T("ы:\n");
		else
			out += _T(" ");

		for (int i = 0 ; i < removedSerialList.size(); i++)
		{
			CString serialName;
			serialName.Format(_T("%s"), removedSerialList[i].port );
			if ( multilineOutput && i )
				out += _T("\n");
			out += serialName ;	
		}
	}

	std::sort(newSerialList.rbegin(), newSerialList.rend());
	serialList = newSerialList;
	TrayIcon_Ballon( out, _T("Serial Monitor") );
}

void Cs_trayDlg::GetSerialFriendlyName(std::vector<SerialInfo> & serialList)
{

	_SP_DEVINFO_DATA devInfoData = {};
    devInfoData.cbSize = sizeof(devInfoData);

    // get the tree containing the info for the ports
    HDEVINFO hDeviceInfo = SetupDiGetClassDevs(&GUID_DEVCLASS_PORTS,
                                               0,
                                               NULL,
                                               DIGCF_PRESENT
                                               );
    if (hDeviceInfo == INVALID_HANDLE_VALUE)
    {
        return;
    }

    // iterate over all the devices in the tree
    int nDevice = 0;
    while (SetupDiEnumDeviceInfo(hDeviceInfo,            // Our device tree
                                 nDevice++,            // The member to look for
                                 &devInfoData))
    {
        DWORD regDataType;
        DWORD reqSize = 0;

	
		BYTE buf[1024] = {0};
		SetupDiGetDeviceRegistryProperty(hDeviceInfo, &devInfoData, SPDRP_FRIENDLYNAME, &regDataType, buf, sizeof(buf), &reqSize);

		CString friendlyName(buf);
		for (int i = 0 ; i < serialList.size(); i++)
		{
			CString searchBuf;
			searchBuf.Format("(%s)", serialList[i].port );
			if ( friendlyName.Find( searchBuf ) != -1 )
			{
				SetupDiGetDeviceRegistryProperty(hDeviceInfo, &devInfoData, SPDRP_DEVICEDESC, &regDataType, buf, sizeof(buf), &reqSize);
				serialList[i].friendlyName.Format(_T("%s"), buf);
			}
		}

    }
}



UINT DoWaitNotificationThread(LPVOID param)
{
	Cs_trayDlg * dlg = (Cs_trayDlg *)param;
	HKEY hKey;
	HANDLE hEvent;
	
	for(;;)
	{
		if ( ::RegOpenKeyEx(HKEY_LOCAL_MACHINE, REGKEY_SERIAL, 0, KEY_NOTIFY, &hKey) != ERROR_SUCCESS )
		{
			return 0;
		}

		hEvent = ::CreateEvent(NULL, TRUE, FALSE, NULL);
		if ( !hEvent )
		{
			return 0;
		}

		if ( ::RegNotifyChangeKeyValue(hKey, 
										TRUE, 
										REG_NOTIFY_CHANGE_LAST_SET, 
										hEvent, 
										TRUE)!= ERROR_SUCCESS )
		{
			::RegCloseKey(hKey);
			return 0;
		}

	
		if (WaitForSingleObject(hEvent, INFINITE) == WAIT_FAILED)
		{			
			return 0;
		}

		dlg->HandleRegKeyChanging();		
		
		::RegCloseKey(hKey);
		::CloseHandle(hEvent);
	}

	return 0 ;
}