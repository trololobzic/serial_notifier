#pragma once



class UTray
{
static const int szInfoSize = 256;
static bool traypresence; 

public:
	UTray(void);
	~UTray(void);



	HICON icon;
	HWND wndId;
	CHAR tip[szInfoSize];



};
