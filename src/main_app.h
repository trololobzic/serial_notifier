#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols
#include "settings.h"
#include "registry/registry.h"
#include "serial.h"
#include "main_dlg.h"

class CSerialNotifierApp : public CWinAppEx
{
public:
	CSerialNotifierApp();

	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()

private:
	Settings _settings;
	Serial _serial;
};

extern CSerialNotifierApp serial_notifier_app;