#pragma once
#include "registry_helpers.h"

namespace serial_notifier
{

class Registry
{
public:

	static const size_t max_value_length = 16383u;
	static const size_t max_key_length = 255u;

	/*
	* Get value by the path and key
	*/
	inline static bool get_key_value(const HKEY section, const TCHAR * path, const TCHAR * key, RegistryValue & value)
	{
		try
		{		
			RegistrySection registry_section(section, path);

			DWORD value_type, value_size;
			//get value type and value size. Not read value yet
			LSTATUS err_code = ::RegQueryValueEx(registry_section(), key, NULL, &value_type, NULL, &value_size);
			if (err_code != ERROR_SUCCESS)
			{
				throw Exception();
			}

			value.type(value_type);
			value.resize(value_size);

			//get value
			err_code = ::RegQueryValueEx(registry_section(), key, NULL, NULL, &value._data[0], &value_size);
			if (err_code != ERROR_SUCCESS)
			{
				throw Exception();
			}

			return true;
		}
		catch(...)
		{
			return false;
		}		
	}

	/*
	* Get all pairs key-value by the path
	*/
	typedef std::vector<RegistryEntry> EntriesSet;
	inline static bool get_key_value(const HKEY section, const TCHAR * path, EntriesSet & result)
	{
		try
		{		
			RegistrySection registry_section(section, path);

			DWORD keys_number = 0;
			LSTATUS err_code = RegQueryInfoKey(registry_section(), NULL, NULL, NULL, NULL, NULL, NULL, &keys_number, NULL, NULL, NULL, NULL);
			if (err_code != ERROR_SUCCESS)
			{
				throw Exception();
			}

			for(UINT32 key_idx = 0; key_idx < keys_number; key_idx++)
			{				
				TCHAR key[max_value_length] = {0};
				DWORD key_length = max_value_length;
				

				err_code = ::RegEnumValue(registry_section(), key_idx, key, &key_length, NULL, NULL, NULL, NULL);
				if (err_code != ERROR_SUCCESS)
				{
					throw Exception();
				}

				RegistryEntry entry;

				//get value type and value size. Not read value yet
				DWORD value_type, value_size;
				LSTATUS err_code = ::RegQueryValueEx(registry_section(), key, NULL, &value_type, NULL, &value_size);
				if (err_code != ERROR_SUCCESS)
				{
					throw Exception();
				}

				entry.key(key);
				entry.type(value_type);
				entry.resize(value_size);

				//get value
				err_code = ::RegQueryValueEx(registry_section(), key, NULL, NULL, &entry._data[0], &value_size);
				if (err_code != ERROR_SUCCESS)
				{
					throw Exception();
				}
				result.push_back(entry);
			}
			
			return true;
		}
		catch(...)
		{
			return false;
		}		
	}

	inline static bool set_key_value(const HKEY section, const TCHAR * path, const TCHAR * key, const RegistryValue & value)
	{
		try
		{
			RegistrySectionForceCreate registry_section(section, path, RegistrySectionForceCreate::AR_Write);
			LSTATUS err_code = ::RegSetValueEx(registry_section(), key, 0, value.type(), value(), value.size());
			if (err_code != ERROR_SUCCESS)
				throw Exception();

			return true;
		}
		catch(...)
		{
			return false;
		}
	}

	inline static bool is_path_valid(const HKEY section, const TCHAR * path)
	{
		try
		{
			RegistrySection registry_section(section, path);
		}
		catch(...)
		{
			return false;
		}
		return true;
	}

	inline static bool is_key_present(const HKEY section, const TCHAR * path, const TCHAR * key)
	{		
		try
		{
			RegistrySection registry_section(section, path);
			DWORD value_type, value_size;
			LSTATUS err_code = ::RegQueryValueEx(registry_section(), key, NULL, &value_type, NULL, &value_size);
			if (err_code != ERROR_SUCCESS)
				throw Exception();
			
			return true;
		}
		catch(...)
		{
			return false;
		}		
	}

	inline static bool create_empty_path(const HKEY section, const TCHAR * path)
	{
		try
		{
			RegistrySectionForceCreate registry_section(section, path);
			return true;
		}
		catch(...)
		{
			return false;
		}
	}

	inline static bool delete_key(const HKEY section, const TCHAR * path, const TCHAR * key)
	{
		try
		{
			RegistrySection registry_section(section, path, RegistrySection::AR_Write);
			if (::RegDeleteValue(registry_section(), key) != ERROR_SUCCESS)
				throw Exception();

			return true;
		}
		catch(...)
		{
			return false;
		}
	}

	inline static bool delete_path(const HKEY section, const TCHAR * path)
	{
		LSTATUS err_code = ERROR_SUCCESS;
		try
		{
			RegistrySection registry_section(section, path);

			DWORD sub_keys_total;
			err_code = RegQueryInfoKey(registry_section(), NULL, NULL, NULL, &sub_keys_total, NULL, NULL, NULL, NULL, NULL, NULL, NULL);

			if (err_code != ERROR_SUCCESS)
				throw Exception();

			if (sub_keys_total)
			{
				//delete recur
				DWORD sub_key_idx = 0;
				TCHAR sub_key_name[max_key_length] = {0};
				DWORD sub_key_name_length = max_key_length;
				while(::RegEnumKeyEx(registry_section(), sub_key_idx, sub_key_name, &sub_key_name_length, NULL, NULL, NULL, NULL) == ERROR_SUCCESS)
				{
					CString sub_key_path;
					sub_key_path.Format(TEXT("%s\\%s"), path, sub_key_name);

					if ( !delete_path(section, sub_key_path))
						return false;

					::memset(sub_key_name, 0, max_key_length * sizeof(TCHAR));
					sub_key_name_length = max_key_length;
				}
			}

			err_code = ::RegDeleteKey(section, path);
			if (err_code != ERROR_SUCCESS)
				throw Exception();

			return true;
		}
		catch(...)
		{
			return false;
		}
	}

	/*
	* Return true if any keys by path were changed
	* Return false if something went wrong
	*/
	inline static bool blocking_wait_for_changing(const HKEY section, const TCHAR * path, DWORD timeout_ms = INFINITE)
	{
		try
		{
			RegistrySection registry_section(section, path, RegistrySection::AR_Notify);

			HANDLE event_handler = ::CreateEvent(NULL, TRUE, FALSE, NULL);
			if (event_handler == NULL)
				throw Exception();

			if ( ::RegNotifyChangeKeyValue(registry_section(), TRUE, REG_NOTIFY_CHANGE_LAST_SET, event_handler, TRUE) != ERROR_SUCCESS )
				throw Exception();

			if (WaitForSingleObject(event_handler, timeout_ms) == WAIT_FAILED)
				throw Exception();

			::CloseHandle(event_handler);
			return true;
		}
		catch(...)
		{
			return false;
		}		
	}
};
}