#pragma once

class SerialInfo
{
public:
	SerialInfo(void);
	~SerialInfo(void);
	
	CString name;
	CString port;
	CString friendlyName;


	const BOOL operator == (SerialInfo const & s)
	{
		return ( (this->name == s.name) && (this->port == s.port));
	}

	const BOOL operator != (SerialInfo const & s)
	{
		return !( (this->name == s.name) && (this->port == s.port));
	}

	const BOOL operator < (SerialInfo const & s)
	{
		CString str_this = this->port, str_s = s.port;
		str_this.Delete(0,3);
		str_s.Delete(0,3);
		int a_s = atoi(str_s);
		int a_this = atoi(str_this);
		
		//return ( s.port < this->port );
		return (a_s < a_this);
	}
};
