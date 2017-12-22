// s_tray.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// Cs_trayApp:
// See s_tray.cpp for the implementation of this class
//

class Cs_trayApp : public CWinApp
{
public:
	Cs_trayApp();

// Overrides
	public:
	virtual BOOL InitInstance();

// Implementation

	DECLARE_MESSAGE_MAP()
};

extern Cs_trayApp theApp;