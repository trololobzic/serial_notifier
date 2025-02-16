#pragma once
#pragma comment (lib, "Setupapi.lib")

#include <vector>
#include <algorithm>
#include <setupapi.h>
#include <initguid.h>
#include <devguid.h>
#include "registry/registry.h"

namespace serial_notifier
{

struct SerialDevice
{
    CString device_name;
    CString friendly_name;
    CString description;

    SerialDevice()
    {
    }

    SerialDevice(const CString & device_name) :
        device_name(device_name)
    {
    }

    SerialDevice(const CString & device_name, const CString & friendly_name) :
        device_name(device_name),
        friendly_name(friendly_name)
    {
    }

    SerialDevice(const CString & device_name, const CString & friendly_name, const CString & description) :
        device_name(device_name),
        friendly_name(friendly_name),
        description(description)
    {
    }

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

    struct SerialListDiff
    {
        SerialList plugged_devices;
        SerialList unplugged_devices;
    };

    Serial() :
        _serials_reg_section(HKEY_LOCAL_MACHINE),
        _serials_reg_path(TEXT("HARDWARE\\DEVICEMAP\\SERIALCOMM"))
    {
        SerialList serial_list;
        setup_descriptions(serial_list);
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

    inline static void setup_descriptions(SerialList & serial_list)
    {
        (void)serial_list;
        _SP_DEVINFO_DATA dev_info_data = {};
        dev_info_data.cbSize = sizeof(dev_info_data);

        // get the tree containing the info for the ports
        HDEVINFO h_device_info = ::SetupDiGetClassDevs(&GUID_DEVCLASS_PORTS, 0, NULL, DIGCF_PRESENT);
        if (h_device_info == INVALID_HANDLE_VALUE)
        {
            return;
        }

        DWORD device_idx = 0;
        while (::SetupDiEnumDeviceInfo(h_device_info, device_idx++, &dev_info_data))
        {
            DWORD reg_data_type;
            const size_t property_buffer_size = 1024;
            TCHAR property_buffer[property_buffer_size] = {0};
            if (::SetupDiGetDeviceRegistryProperty(h_device_info, &dev_info_data, SPDRP_FRIENDLYNAME, &reg_data_type, (PBYTE)property_buffer, (property_buffer_size - 1) * sizeof(TCHAR), NULL) &&
                ::_tcslen(property_buffer))
            {
                 SerialList::iterator serial_list_iterator = std::find(serial_list.begin(), serial_list.end(), SerialDevice(TEXT(""), property_buffer));
                ::memset(property_buffer, 0 , property_buffer_size * sizeof(TCHAR));

                if (serial_list_iterator != serial_list.end() &&
                    ::SetupDiGetDeviceRegistryProperty(h_device_info, &dev_info_data, SPDRP_DEVICEDESC, &reg_data_type, (PBYTE)property_buffer, (property_buffer_size - 1) * sizeof(TCHAR), NULL) &&
                    ::_tcslen(property_buffer))
                {
                    serial_list_iterator->description = property_buffer;
                }
            }
        }
    }

    inline static void get_difference(const SerialList & old_list, const SerialList & new_list, SerialListDiff & diff)
    {
        std::set_difference(old_list.begin(), old_list.end(), new_list.begin(), new_list.end(), std::back_inserter(diff.unplugged_devices));
        std::set_difference(new_list.begin(), new_list.end(), old_list.begin(), old_list.end(), std::back_inserter(diff.plugged_devices));
    }

private:
    SerialList _serial_list;
    const HKEY _serials_reg_section;
    const CString _serials_reg_path;
};

}
