#include "stdafx.h"
#include "settings.h"

#define CATCH_CONFIG_NO_CPP11
#include "catch.hpp"

/*
* Test helpers
*/
static CString make_app_path();

struct TestRegistry
{
    static const size_t max_value_length = 16383u;
};


struct TestRegistry1 : public TestRegistry
{
    static bool flag1;
    static bool flag2;
    static bool flag3;
    static bool flag4;

    inline static bool get_key_value(const HKEY section, const TCHAR * path, const TCHAR * key, serial_notifier::RegistryValue & value)
    {
        (void)section;
        (void)path;
        if (!::lstrcmp(key, TEXT("serial_notifier")))
        {
            value = serial_notifier::RegistryValue(CString(make_app_path()));
            return true;
        }
        else if (!::lstrcmp(key, TEXT("popup_enable")))
        {
            value = serial_notifier::RegistryValue(UINT32(1));
            return true;
        }
        else if (!::lstrcmp(key, TEXT("EnableBallonTip")))
        {
            value = serial_notifier::RegistryValue(UINT32(1));
            return true;
        }
        else if (!::lstrcmp(key, TEXT("lang")))
        {
            value = serial_notifier::RegistryValue(CString(TEXT("123a")));
            return true;
        }
        else if (!::lstrcmp(key, TEXT("InstallLanguage")))
        {
            value = serial_notifier::RegistryValue(CString(TEXT("567b")));
            return true;
        }
        return false;
    }

    inline static bool set_key_value(const HKEY section, const TCHAR * path, const TCHAR * key, const serial_notifier::RegistryValue & value)
    {
        (void)section;
        (void)path;
        if (!::lstrcmp(key, TEXT("popup_enable")))
        {
            if (value.get<UINT32>() == 0)
                flag2 = true;
            return true;
        }
        else if (!::lstrcmp(key, TEXT("EnableBallonTip")))
        {
            if (value.get<UINT32>() == 0)
                flag3 = true;
            return true;
        }
        else if (!::lstrcmp(key, TEXT("lang")))
        {
            CString a = value.get<CString>();
            CString b = CString(TEXT("a321"));
            if (a == b)
                flag4 = true;
            return true;
        }
        return false;
    }

    inline static bool is_path_valid(const HKEY section, const TCHAR * path)
    {
        (void)section;
        (void)path;
        return false;
    }

    inline static bool delete_key(const HKEY section, const TCHAR * path, const TCHAR * key)
    {
        (void)section;
        (void)path;

        if (!::lstrcmp(key, TEXT("serial_notifier")))
        {
            flag1 = true;
        }
        return true;
    }
};

bool TestRegistry1::flag1 = false;
bool TestRegistry1::flag2 = false;
bool TestRegistry1::flag3 = false;
bool TestRegistry1::flag4 = false;

struct TestRegistry2 : public TestRegistry
{
    static bool flag1;
    static bool flag2;
    static bool flag3;
    static bool flag4;

    inline static bool get_key_value(const HKEY section, const TCHAR * path, const TCHAR * key, serial_notifier::RegistryValue & value)
    {
        (void)section;
        (void)path;

        if (!::lstrcmp(key, TEXT("InstallLanguage")))
        {
            value = serial_notifier::RegistryValue(CString(TEXT("567b")));
            return true;
        }
        return false;
    }

    inline static bool set_key_value(const HKEY section, const TCHAR * path, const TCHAR * key, const serial_notifier::RegistryValue & value)
    {
        (void)section;
        (void)path;
        if (!::lstrcmp(key, TEXT("serial_notifier")))
        {
            if (value.get<CString>() == make_app_path())
                flag1 = true;
            return true;
        }
        else if (!::lstrcmp(key, TEXT("popup_enable")))
        {
            if (value.get<UINT32>() == 1)
                flag2 = true;
            return true;
        }
        else if (!::lstrcmp(key, TEXT("EnableBallonTip")))
        {
            if (value.get<UINT32>() == 1)
                flag3 = true;
            return true;
        }
        else if (!::lstrcmp(key, TEXT("lang")))
        {
            CString a = value.get<CString>();
            CString b = CString(TEXT("a321"));
            if (a == b)
                flag4 = true;
            return true;
        }
        return false;
    }

    inline static bool delete_key(const HKEY section, const TCHAR * path, const TCHAR * key)
    {
        (void)section;
        (void)path;
        (void)key;

        return false;
    }

    inline static bool is_path_valid(const HKEY section, const TCHAR * path)
    {
        (void)section;
        (void)path;
        return false;
    }
};

bool TestRegistry2::flag1 = false;
bool TestRegistry2::flag2 = false;
bool TestRegistry2::flag3 = false;
bool TestRegistry2::flag4 = false;

struct TestRegistry3 : public TestRegistry
{
    inline static bool get_key_value(const HKEY section, const TCHAR * path, const TCHAR * key, serial_notifier::RegistryValue & value)
    {
        (void)section;
        (void)path;
        if (!::lstrcmp(key, TEXT("serial_notifier")))
        {
            return false;
        }
        else if (!::lstrcmp(key, TEXT("popup_enable")))
        {
            return false;
        }
        else if (!::lstrcmp(key, TEXT("EnableBallonTip")))
        {
            value = serial_notifier::RegistryValue(UINT32(1));
            return true;
        }
        return false;
    }

    inline static bool set_key_value(const HKEY section, const TCHAR * path, const TCHAR * key, const serial_notifier::RegistryValue & value)
    {
        (void)section;
        (void)path;
        (void)key;
        (void)value;

        return false;
    }

    inline static bool delete_key(const HKEY section, const TCHAR * path, const TCHAR * key)
    {
        (void)section;
        (void)path;
        (void)key;

        return false;
    }

    inline static bool is_path_valid(const HKEY section, const TCHAR * path)
    {
        (void)section;
        (void)path;

        return false;
    }
};

static CString make_app_path()
{
    TCHAR buffer[TestRegistry::max_value_length] = {0};
    ::GetModuleFileName(::GetModuleHandle(0), buffer, TestRegistry::max_value_length - 1);
    return CString(buffer);
}

/*
* Test cases
*/

TEST_CASE("Testing_settings_for_all_flags_are_true", "[settings]")
{
    serial_notifier::Settings<TestRegistry1> settings;

    REQUIRE(settings.popup());
    REQUIRE(settings.autorun());
    REQUIRE(settings.system_popup());
    REQUIRE(settings.lang() == 0x123a);

    settings.autorun(false);
    settings.popup(false);
    settings.system_popup(false);
    settings.lang(0xa321);

    REQUIRE(TestRegistry1::flag1);
    REQUIRE(TestRegistry1::flag2);
    REQUIRE(TestRegistry1::flag3);
    REQUIRE(TestRegistry1::flag4);
}

TEST_CASE("Testing_settings_for_all_flags_are_false", "[settings]")
{
    serial_notifier::Settings<TestRegistry2> settings;

    REQUIRE_FALSE(settings.popup());
    REQUIRE_FALSE(settings.autorun());
    REQUIRE_FALSE(settings.system_popup());
    REQUIRE(settings.lang() == 0x567b);

    settings.autorun(true);
    settings.popup(true);
    settings.system_popup(true);
    settings.lang(0xa321);

    REQUIRE(TestRegistry2::flag1);
    REQUIRE(TestRegistry2::flag2);
    REQUIRE(TestRegistry2::flag3);
    REQUIRE(TestRegistry2::flag4);
}

TEST_CASE("Testing_settings_force_popup_setting_up", "[settings]")
{
    serial_notifier::Settings<TestRegistry3> settings(true);

    REQUIRE(settings.popup());
    REQUIRE_FALSE(settings.autorun());
    REQUIRE(settings.system_popup());
}
