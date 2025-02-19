#pragma once
#include <winreg.h>
#include <vector>
#include "../exception.h"

namespace serial_notifier
{

class RegistrySection
{
public:

    enum AccessRights
    {
        AR_Read,
        AR_Write,
        AR_Notify
    };

    RegistrySection()
    {
    }

    RegistrySection(const HKEY section, const LPCWSTR path, AccessRights access_rights = AR_Read)
    {
        LSTATUS err_code = ::RegOpenKeyEx(section, path, 0, _make_rights_mask(access_rights), &_descriptor);
        if (err_code != ERROR_SUCCESS)
        {
            CString what;
            what.Format(TEXT("Can not open section [%u] path [%s] err_code [%u]"), section, path, err_code);
            throw Exception(what);
        }
    }

    ~RegistrySection()
    {
        ::RegCloseKey(_descriptor);
    }

    HKEY & operator()()
    {
        return _descriptor;
    }

protected:

    HKEY _descriptor;

    inline static DWORD _make_rights_mask(AccessRights access_rights)
    {
        switch(access_rights)
        {
        case AR_Read:
            return KEY_READ;
        case AR_Write:
            return KEY_WRITE;
        case AR_Notify:
            return KEY_NOTIFY;
        default:
            return 0;
        }
    }
};


class RegistrySectionForceCreate : public RegistrySection
{
public:

    RegistrySectionForceCreate(const HKEY section, const LPCWSTR path, AccessRights access_rights = AR_Read)
    {
        LSTATUS err_code = ::RegOpenKeyEx(section, path, 0, _make_rights_mask(access_rights), &_descriptor);
        if (err_code == ERROR_FILE_NOT_FOUND)
        {
            err_code = ::RegCreateKey(section, path, &_descriptor);
            if (err_code != ERROR_SUCCESS)
            {
                CString what;
                what.Format(TEXT("Can not create section [%u] path [%s] err_code [%u]"), section, path, err_code);
                throw Exception(what);
            }

            err_code = ::RegOpenKeyEx(section, path, 0, _make_rights_mask(access_rights), &_descriptor);
            if (err_code != ERROR_SUCCESS)
            {
                CString what;
                what.Format(TEXT("Can not open section [%u] path [%s] err_code [%u] after force create"), section, path, err_code);
                throw Exception(what);
            }
        }
        else if (err_code != ERROR_SUCCESS)
        {
            CString what;
            what.Format(TEXT("Can not open section [%u] path [%s] err_code [%u]"), section, path, err_code);
            throw Exception(what);
        }
    }
};


class RegistryValue
{
public:

    friend class Registry;
    typedef std::vector<BYTE> BinaryData;
    typedef std::vector<CString> MultiString;

    RegistryValue() :
        _data_type(REG_NONE)
    {
    }

    RegistryValue(UINT32 data) :
        _data_type(REG_DWORD)
    {
        _set_data_from_integer(data);
    }

    RegistryValue(UINT64 data) :
        _data_type(REG_QWORD)
    {
        _set_data_from_integer(data);
    }

    RegistryValue(const BinaryData & data) :
        _data_type(REG_BINARY)
    {
        _data = data;
    }

    RegistryValue(const CString & wstring) :
        _data_type(REG_SZ)
    {
        _data.resize((wstring.GetLength() + 1) * sizeof(CString::XCHAR));
        ::memcpy(&_data[0], wstring.GetString(), _data.size());
    }

    RegistryValue(const MultiString & wstrings) :
        _data_type(REG_MULTI_SZ)
    {
        size_t total_size = 0;
        for (std::vector<CString>::const_iterator it = wstrings.begin(); it != wstrings.end(); it++)
        {
            total_size += (it->GetLength() + 1) * sizeof(CString::XCHAR);
        }
        total_size += sizeof(CString::XCHAR);
        _data.resize(total_size);

        size_t pos = 0;
        for (std::vector<CString>::const_iterator it = wstrings.begin(); it != wstrings.end(); it++)
        {
            const size_t curr_string_size = (it->GetLength() + 1) * sizeof(CString::XCHAR);
            ::memcpy(&_data[pos], it->GetString(), curr_string_size);
            pos += curr_string_size;
        }
    }

    void type(UINT32 data_type)
    {
        _data_type = data_type;
    }

    UINT32 type() const
    {
        return _data_type;
    }

    void resize(size_t size)
    {
        _data.resize(size);
    }

    size_t size() const
    {
        return _data.size();
    }

    bool operator==(const RegistryValue & other) const
    {
        return (this->_data == other._data && this->_data_type == other._data_type);
    }

    bool operator!=(const RegistryValue & other) const
    {
        return !operator== (other);
    }

    bool operator<(const RegistryValue & other) const
    {
        if (this->_data_type < other._data_type)
            return true;
        else if (this->_data_type == other._data_type)
            return this->_data < other._data;
        else
            return false;
    }

    const BYTE * operator()() const
    {
        return &_data[0];
    }

    template<class T>
    T get() const
    {
        T ret_value = 0;

        for (unsigned int i = 0; i < (_data.size() < sizeof(T) ? _data.size() : sizeof(T)); i++)
            ret_value |= T(_data[i]) << (i * 8);

        return ret_value;
    }

    template<>
    BinaryData get<BinaryData>() const
    {
        return _data;
    }

    template<>
    CString get<CString>() const
    {
        CString return_value;
        if (_data.size())
            return_value.Format(TEXT("%s"), &_data[0]);
        return return_value;
    }

    template<>
    MultiString get<MultiString>() const
    {
        MultiString return_value;
        std::vector<BYTE> copy_of_data = _data;

        while(!copy_of_data.empty())
        {
            CString str;
            str.Format(TEXT("%s"), &copy_of_data[0]);
            return_value.push_back(str);
            copy_of_data.erase(copy_of_data.begin(), copy_of_data.begin() + (str.GetLength() + 1) * sizeof(CString::XCHAR));
        }

        if (return_value.size() && return_value.rbegin()->GetLength() == 0)
            return_value.pop_back();

        return return_value;
    }


private:

    template<class T>
    void _set_data_from_integer(T data)
    {
        for (int i = 0; i < sizeof(data); i++)
            _data.push_back((data >>(i * 8)) & 0xFF);
    }

    std::vector<BYTE> _data;
    UINT32 _data_type;
};

class RegistryEntry : public RegistryValue
{
public:

    RegistryEntry()
    {}

    template<class T>
    RegistryEntry(const CString & key, T value) :
        RegistryValue(value), _key(key)
    {
    }

    void key(const CString & key)
    {
        _key = key;
    }

    CString key() const
    {
        return _key;
    }

private:

    CString _key;
};

}
