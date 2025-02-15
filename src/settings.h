#include "stdafx.h"
#include "registry/registry.h"

namespace serial_notifier
{

template<class TRegistry>
class Settings
{
    template<class T>
    struct Property
    {
        Property(const HKEY reg_section, const CString & reg_path, const CString & reg_key, const T & value) :
            reg_section(reg_section),
            reg_path(reg_path),
            reg_key(reg_key),
            value(value)
        {
        }

        HKEY reg_section;
        CString reg_path;
        CString reg_key;
        T value;
    };

public:

    inline Settings():
        _module_name(TEXT("serial_notifier")),
        _autorun(HKEY_CURRENT_USER, TEXT("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run"), _module_name, false),
        _popup(HKEY_CURRENT_USER, _make_app_reg_path(), TEXT("popup_enable"), false),
        _system_popup0(HKEY_CURRENT_USER, TEXT("Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Advanced"), TEXT("EnableBallonTip"), false)//,
        //_system_popup1(HKEY_CURRENT_USER, TEXT("Software\\Microsoft\\Windows\\CurrentVersion\\PushNotifications"), TEXT("ToastEnabled"), false),
        //_system_popup2(HKEY_CURRENT_USER, TEXT("Software\\Policies\\Microsoft\\Windows\\Explorer"), TEXT("DisableNotificationCenter"), false)
    {
        RegistryValue registry_value;
        _fill_property_from_registry(_autorun, REG_SZ, true, false, _is_value_containing_app_path);
        _fill_property_from_registry(_popup, REG_DWORD, true, false, _is_value_true);
        _fill_property_from_registry(_system_popup0, REG_DWORD, true, false, _is_value_true);
        //_fill_property_from_registry(_system_popup1, REG_DWORD, true, false, _is_value_true);
        //_fill_property_from_registry(_system_popup2, REG_DWORD, false, true, _is_value_true);
    }

    inline bool autorun() const
    {
        return _autorun.value;
    }

    inline void autorun(const bool value)
    {
        _autorun.value = value;
        if (value)
        {
            RegistryValue registry_value(_make_app_path());
            TRegistry::set_key_value(_autorun.reg_section, _autorun.reg_path, _autorun.reg_key, registry_value);
        }
        else
        {
            TRegistry::delete_key(_autorun.reg_section, _autorun.reg_path, _autorun.reg_key);
        }
    }

    inline bool popup() const
    {
        return _popup.value;
    }

    inline void popup(const bool value)
    {
        _popup.value = value;

        RegistryValue registry_value(UINT32(value ? 1 : 0)); //dummy conversion from bool to uint32 because of MSVC 1500 error C2664: 'TestRegistry1::set_key_value' :
                                                            //cannot convert parameter 4 from 'serial_notifier::RegistryValue (__cdecl *)(UINT32)' to
                                                            //'const serial_notifier::RegistryValue &'
        TRegistry::set_key_value(_popup.reg_section, _popup.reg_path, _popup.reg_key, registry_value);
    }

    inline bool system_popup() const
    {
        //return _system_popup0.value && _system_popup1.value && _system_popup2.value;
        return _system_popup0.value;
    }

    inline void system_popup(const bool value)
    {
        _system_popup0.value = value;

        RegistryValue registry_value(UINT32(value ? 1 : 0)); //dummy conversion from bool to uint32 because of MSVC 1500 error C2664: 'TestRegistry1::set_key_value' :
                                                            //cannot convert parameter 4 from 'serial_notifier::RegistryValue (__cdecl *)(UINT32)' to
                                                            //'const serial_notifier::RegistryValue &'
        TRegistry::set_key_value(_system_popup0.reg_section, _system_popup0.reg_path, _system_popup0.reg_key, registry_value);
    }

private:
    const CString _module_name;

    Property<bool> _autorun;
    Property<bool> _popup;
    Property<bool> _system_popup0;
    //Property<bool> _system_popup1;
    //Property<bool> _system_popup2;

    inline static CString _make_app_path()
    {
        TCHAR buffer[TRegistry::max_value_length] = {0};
        ::GetModuleFileName(::GetModuleHandle(0), buffer, TRegistry::max_value_length - 1);
        return CString(buffer);
    }

    inline CString _make_app_reg_path()
    {
        CString reg_path;
        reg_path.Format(TEXT("SOFTWARE\\%s"), _module_name);
        return reg_path;
    }

    template<class TProperty, class TValue, class TCompareMethod>
    inline static void _fill_property_from_registry(TProperty & prop, const DWORD expected_reg_type, const TValue & value_if_compare_ok, const TValue & value_if_compare_error, const TCompareMethod & compare_method)
    {

        RegistryValue registry_value;
        if (TRegistry::get_key_value(prop.reg_section, prop.reg_path, prop.reg_key, registry_value))
        {
            if (registry_value.type() == expected_reg_type)
            {
                prop.value = compare_method(registry_value) ? value_if_compare_ok : value_if_compare_error;
            }
        }
    }

    inline static bool _is_value_containing_app_path(const serial_notifier::RegistryValue & registry_value)
    {
        CString value = registry_value.get<CString>();
        CString app_path = _make_app_path();
        return value == app_path;
    }

    inline static bool _is_value_true(const serial_notifier::RegistryValue & registry_value)
    {
        UINT32 value = registry_value.get<UINT32>();
        return value != 0;
    }
};

}
