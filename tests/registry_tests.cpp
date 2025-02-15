#include "../src/stdafx.h"
#include "../src/registry/registry.h"

#define CATCH_CONFIG_NO_CPP11
#include "catch.hpp"

#include <ctime>

/*
* Test helpers
*/

CString make_test_reg_path()
{
	CString reg_test_path;
	time_t cur_time;
	::time(&cur_time);
	reg_test_path.Format(TEXT("SOFTWARE\\serial_notifier_tests_%lu"), UINT64(cur_time));
	return reg_test_path;
}

UINT win_thread_foo(LPVOID param)
{
	CString * path = static_cast<CString *>(param);
	if (serial_notifier::Registry::blocking_wait_for_changing(HKEY_CURRENT_USER, *path))
	{
		//something was changed by path
		return 2;
	}
	else
	{
		//error
		return 1;
	}
}

/*
* Test cases
*/

TEST_CASE("Checking registry value", "[registry]")
{
	{
		serial_notifier::RegistryValue reg_value;
		REQUIRE(reg_value.type() == REG_NONE);
		REQUIRE(reg_value.size() == 0);
		REQUIRE(reg_value.get<UINT32>() == 0);

		reg_value.resize(4);
		REQUIRE(reg_value.size() == 4);
		REQUIRE(reg_value.get<UINT32>() == 0);
	}

	{
		serial_notifier::RegistryValue reg_value(UINT32(123456));
		REQUIRE(reg_value.type() == REG_DWORD);
		REQUIRE(reg_value.size() == 4);
		REQUIRE(reg_value.get<UINT32>() == 123456);
	}

	{
		serial_notifier::RegistryValue reg_value(1234567890ULL);
		REQUIRE(reg_value.type() == REG_QWORD);
		REQUIRE(reg_value.size() == 8);
		REQUIRE(reg_value.get<UINT64>() == 1234567890ULL);
	}

	{
		CString str1, str2;
		str1.Format(TEXT("мама мыла раму"));

		serial_notifier::RegistryValue reg_value(str1);
		REQUIRE(reg_value.type() == REG_SZ);
		REQUIRE(reg_value.size() == (15 * sizeof(TCHAR)));

		str2 = reg_value.get<CString>();
		REQUIRE(str2 == str1);
	}

	{
		CString str1, str2;
		str1.Format(TEXT("abracadabra"));

		serial_notifier::RegistryValue reg_value(str1);
		REQUIRE(reg_value.type() == REG_SZ);
		REQUIRE(reg_value.size() == (12 * sizeof(TCHAR)));

		str2 = reg_value.get<CString>();
		REQUIRE(str2 == str1);
	}

	{
		serial_notifier::RegistryValue::BinaryData arr1, arr2;
		arr1.push_back(0x00);
		arr1.push_back(0x01);
		arr1.push_back(0x02);
		arr1.push_back(0x03);

		serial_notifier::RegistryValue reg_value(arr1);
		REQUIRE(reg_value.type() == REG_BINARY);
		REQUIRE(reg_value.size() == 4);

		arr2 = reg_value.get<std::vector<BYTE>>();
		REQUIRE(arr2 == arr1);
	}

	{
		serial_notifier::RegistryValue::MultiString multi_string1, multi_string2;
		CString str1, str2, str3;
		str1.Format(TEXT("строка1"));
		str2.Format(TEXT("string2"));
		str3.Format(TEXT("aaaaaa3"));
		multi_string1.push_back(str1);
		multi_string1.push_back(str2);
		multi_string1.push_back(str3);

		serial_notifier::RegistryValue reg_value(multi_string1);
		REQUIRE(reg_value.type() == REG_MULTI_SZ);
		REQUIRE(reg_value.size() == (25 * sizeof(TCHAR)));

		multi_string2 = reg_value.get<serial_notifier::RegistryValue::MultiString>();
		REQUIRE(multi_string1 == multi_string2);
	}

	{
		serial_notifier::RegistryValue::MultiString multi_string1, multi_string2;
		CString str1, str2, str3;
		str1.Format(TEXT(""));
		str2.Format(TEXT(""));
		str3.Format(TEXT(""));
		multi_string1.push_back(str1);
		multi_string1.push_back(str2);
		multi_string1.push_back(str3);

		serial_notifier::RegistryValue reg_value(multi_string1);
		REQUIRE(reg_value.type() == REG_MULTI_SZ);
		REQUIRE(reg_value.size() == (4 * sizeof(TCHAR)));

		multi_string2 = reg_value.get<serial_notifier::RegistryValue::MultiString>();
		REQUIRE(multi_string1 == multi_string2);
	}

	{
		serial_notifier::RegistryValue::MultiString multi_string1, multi_string2;

		serial_notifier::RegistryValue reg_value(multi_string1);
		REQUIRE(reg_value.type() == REG_MULTI_SZ);
		REQUIRE(reg_value.size() == sizeof(TCHAR));

		multi_string2 = reg_value.get<serial_notifier::RegistryValue::MultiString>();
		REQUIRE(multi_string1 == multi_string2);
	}
}

TEST_CASE("Checking path validation", "[registry]")
{
	REQUIRE_FALSE(serial_notifier::Registry::is_path_valid(HKEY_CURRENT_USER, TEXT("abracadabra_123")));
	REQUIRE_FALSE(serial_notifier::Registry::is_path_valid(HKEY_CURRENT_USER, TEXT("abracadabra_456")));
	REQUIRE(serial_notifier::Registry::is_path_valid(HKEY_CURRENT_USER, TEXT("Software")));
	REQUIRE(serial_notifier::Registry::is_path_valid(HKEY_LOCAL_MACHINE, TEXT("HARDWARE\\DEVICEMAP")));
}

TEST_CASE("Checking key validation", "[registry]")
{
	REQUIRE_FALSE(serial_notifier::Registry::is_key_present(HKEY_LOCAL_MACHINE, TEXT("SOFTWARE\\Microsoft\\Windows\\CurrentVersion"), TEXT("Abracadabra")));
	REQUIRE_FALSE(serial_notifier::Registry::is_key_present(HKEY_CURRENT_USER, TEXT("Control Panel\\Mouse"), TEXT("Abracadabra")));
	REQUIRE(serial_notifier::Registry::is_key_present(HKEY_LOCAL_MACHINE, TEXT("SOFTWARE\\Microsoft\\Windows\\CurrentVersion"), TEXT("ProductId")));
	REQUIRE(serial_notifier::Registry::is_key_present(HKEY_CURRENT_USER, TEXT("Control Panel\\Mouse"), TEXT("MouseSpeed")));
}

TEST_CASE("Ckeck read/write/delete from reg", "[registry]")
{
	CString reg_test_path = make_test_reg_path();

	serial_notifier::RegistryValue reg_value1(UINT32(123)), reg_value2(TEXT("123")), reg_value3, reg_value4;
	serial_notifier::RegistryValue ;

	REQUIRE(serial_notifier::Registry::set_key_value(HKEY_CURRENT_USER, reg_test_path, TEXT("test_key_1"), reg_value1));
	REQUIRE(serial_notifier::Registry::set_key_value(HKEY_CURRENT_USER, reg_test_path, TEXT("test_key_2"), reg_value2));
	REQUIRE(serial_notifier::Registry::set_key_value(HKEY_CURRENT_USER, reg_test_path + CString(TEXT("\\sub_key")), TEXT("test_key_1"), reg_value1));
	REQUIRE(serial_notifier::Registry::set_key_value(HKEY_CURRENT_USER, reg_test_path + CString(TEXT("\\sub_key_12\\sub_key_abracadabra")), TEXT("test_key_1"), reg_value1));

	REQUIRE(serial_notifier::Registry::get_key_value(HKEY_CURRENT_USER, reg_test_path, TEXT("test_key_1"), reg_value3));
	REQUIRE(serial_notifier::Registry::get_key_value(HKEY_CURRENT_USER, reg_test_path, TEXT("test_key_2"), reg_value4));

	REQUIRE(reg_value1 == reg_value3);
	REQUIRE(reg_value2 == reg_value4);

	serial_notifier::Registry::EntriesSet entries_set;
	REQUIRE(serial_notifier::Registry::get_key_value(HKEY_CURRENT_USER, reg_test_path, entries_set));
	REQUIRE(entries_set.size() == 2);
	REQUIRE(std::find(entries_set.begin(), entries_set.end(), reg_value1) != entries_set.end());
	REQUIRE(std::find(entries_set.begin(), entries_set.end(), reg_value2) != entries_set.end());

	REQUIRE(serial_notifier::Registry::delete_path(HKEY_CURRENT_USER, reg_test_path) == true);
	REQUIRE_FALSE(serial_notifier::Registry::is_path_valid(HKEY_LOCAL_MACHINE, reg_test_path));
}

TEST_CASE("Check blocking_wait_for_changing() for adding key", "[registry]")
{
	CString reg_test_path = make_test_reg_path();

	REQUIRE(serial_notifier::Registry::create_empty_path(HKEY_CURRENT_USER, reg_test_path));
	REQUIRE(serial_notifier::Registry::is_path_valid(HKEY_CURRENT_USER, reg_test_path));
	
	//create thread
	CWinThread* test_thread = ::AfxBeginThread(win_thread_foo, &reg_test_path);
	//test_thread->AssertValid();
	test_thread->m_bAutoDelete = false;

	//change something by the path
	Sleep(500);
	serial_notifier::RegistryValue reg_value1(UINT32(123));
	REQUIRE(serial_notifier::Registry::set_key_value(HKEY_CURRENT_USER, reg_test_path, TEXT("test_key_1"), reg_value1));

	//waiting for thread finished
	DWORD code;
	while ( test_thread->m_hThread && GetExitCodeThread ( test_thread->m_hThread, &code ) && code == STILL_ACTIVE )
	{
		//test_thread->AssertValid();
		Sleep(50);
	}

	REQUIRE(code == 2);
	REQUIRE(serial_notifier::Registry::delete_path(HKEY_CURRENT_USER, reg_test_path) == true);
	REQUIRE_FALSE(serial_notifier::Registry::is_path_valid(HKEY_LOCAL_MACHINE, reg_test_path));
}

TEST_CASE("Check blocking_wait_for_changing() for removing key", "[registry]")
{
	CString reg_test_path = make_test_reg_path();
	serial_notifier::RegistryValue reg_value1(UINT32(123));
	REQUIRE(serial_notifier::Registry::set_key_value(HKEY_CURRENT_USER, reg_test_path, TEXT("test_key_1"), reg_value1));
	
	//create thread
	CWinThread* test_thread = ::AfxBeginThread(win_thread_foo, &reg_test_path);
	//test_thread->AssertValid();
	test_thread->m_bAutoDelete = false;

	//change something by the path
	Sleep(500);
	REQUIRE(serial_notifier::Registry::delete_key(HKEY_CURRENT_USER, reg_test_path, TEXT("test_key_1")));

	//waiting for thread finished
	DWORD code;
	while (test_thread->m_hThread && GetExitCodeThread(test_thread->m_hThread, &code) && code == STILL_ACTIVE)
	{
		//test_thread->AssertValid();
		Sleep(50);
	}

	REQUIRE(code == 2);
	REQUIRE(serial_notifier::Registry::delete_path(HKEY_CURRENT_USER, reg_test_path) == true);
	REQUIRE_FALSE(serial_notifier::Registry::is_path_valid(HKEY_LOCAL_MACHINE, reg_test_path));
}