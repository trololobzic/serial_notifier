#include "stdafx.h"
#include "main_app.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


BEGIN_MESSAGE_MAP(CSerialNotifierApp, CWinAppEx)
    ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


CSerialNotifierApp::CSerialNotifierApp() :
    _settings(true)
{
}

CSerialNotifierApp serial_notifier_app;

BOOL CSerialNotifierApp::InitInstance()
{
    CWinAppEx::InitInstance();
    CSerialNotifierDlg dlg(_settings, _serial);
    m_pMainWnd = &dlg;
    dlg.DoModal();

    // Since the dialog has been closed, return FALSE so that we exit the
    //  application, rather than start the application's message pump.
    return FALSE;
}
