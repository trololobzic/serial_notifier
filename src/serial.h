#pragma once
#include <vector>
#include "registry/registry.h"

namespace serial_notifier
{

struct SerialDevice
{
    CString device_name;
    CString friendly_name;

    bool operator==(const SerialDevice & other) const
    {
        return this->device_name == other.device_name;
    }

    bool operator!=(const SerialDevice & other) const
    {
        return !this->operator==(other);
    }

    bool operator<(const SerialDevice & other) const
    {
        UINT32 this_idx = this->_get_device_idx();
        UINT32 other_idx = other._get_device_idx();
        return this_idx < other_idx;
    }

private:
    UINT32 _get_device_idx() const
    {
        CString copy_of_device_name = device_name;

        while (!copy_of_device_name.IsEmpty() && !::isdigit(copy_of_device_name[0]))
            copy_of_device_name.Delete(0);

        return ::_ttoi(copy_of_device_name);
    }
};

template<class TRegistry>
class Serial
{
public:
    typedef std::vector<SerialDevice> SerialList;


    Serial() :
        _serials_reg_section(HKEY_LOCAL_MACHINE),
        _serials_reg_path(TEXT("HARDWARE\\DEVICEMAP\\SERIALCOMM"))
    {
        //TODO
    }

    SerialList read_serial_list()
    {
        SerialList serial_list;
        TRegistry::EntriesSet reg_entries;
        if (!TRegistry::get_key_value(_serials_reg_section, _serials_reg_path, reg_entries))
        {
            return serial_list;
        }

        for(TRegistry::EntriesSet::iterator it = reg_entries.begin(); it != reg_entries.end(); it++)
        {
            SerialDevice serial_device;
            serial_device.friendly_name = it->key();
            serial_device.device_name = it->get<CString>();

            serial_list.push_back(serial_device);
        }

        std::sort(serial_list.begin(), serial_list.end());
        return serial_list;
    }

private:
    SerialList _serial_list;
    const HKEY _serials_reg_section;
    const CString _serials_reg_path;
};

}
