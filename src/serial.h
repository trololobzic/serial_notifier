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

protected:
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


private:
    SerialList _serial_list;
};

}
