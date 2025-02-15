#include "../src/stdafx.h"
#include "../src/serial.h"

#define CATCH_CONFIG_NO_CPP11
#include "catch.hpp"

/*
* Test helpers
*/
struct TestRegistrySerialList1
{
    typedef std::vector<serial_notifier::RegistryEntry> EntriesSet;
    inline static bool get_key_value(const HKEY section, const TCHAR * path, EntriesSet & result)
    {
        (void)section;
        (void)path;

        result.resize(5);
        result.at(0) = serial_notifier::RegistryEntry(CString(TEXT("/dev/ttyS9")), CString(TEXT("COM9")));
        result.at(1) = serial_notifier::RegistryEntry(CString(TEXT("/dev/ttyS2")), CString(TEXT("COM2")));
        result.at(2) = serial_notifier::RegistryEntry(CString(TEXT("/dev/ttyS10")), CString(TEXT("COM10")));
        result.at(3) = serial_notifier::RegistryEntry(CString(TEXT("/dev/ttyS1")), CString(TEXT("COM1")));
        result.at(4) = serial_notifier::RegistryEntry(CString(TEXT("/dev/ttyS12")), CString(TEXT("COM12")));

        return true;
    }
};


/*
* Test cases
*/

TEST_CASE("Checking SerialDevice compare operators", "[serial]")
{
    serial_notifier::SerialDevice device1, device2;

    device1.device_name = TEXT("COM1");
    device2.device_name = TEXT("COM1");

    REQUIRE(device1 == device2);
    REQUIRE_FALSE(device1 != device2);
    REQUIRE_FALSE(device1 < device2);

    device1.device_name = TEXT("COM2");
    device2.device_name = TEXT("COM10");

    REQUIRE_FALSE(device1 == device2);
    REQUIRE(device1 != device2);
    REQUIRE(device1 < device2);
}

TEST_CASE("Checking SerialDevice _get_device_idx()", "[serial]")
{
    std::vector<serial_notifier::SerialDevice> vec;
    vec.resize(7);
    vec.at(0).device_name = TEXT("ABRACADABRA");
    vec.at(1).device_name = TEXT("COM10");
    vec.at(2).device_name = TEXT("COM1_");
    vec.at(3).device_name = TEXT("COM12");
    vec.at(4).device_name = TEXT("COM11");
    vec.at(5).device_name = TEXT("COM6");
    vec.at(6).device_name = TEXT("COM2");

    std::sort(vec.begin(), vec.end());

    REQUIRE(::lstrcmp(vec.at(0).device_name, TEXT("ABRACADABRA")) == 0);
    REQUIRE(::lstrcmp(vec.at(1).device_name, TEXT("COM1_")) == 0);
    REQUIRE(::lstrcmp(vec.at(2).device_name, TEXT("COM2")) == 0);
    REQUIRE(::lstrcmp(vec.at(3).device_name, TEXT("COM6")) == 0);
    REQUIRE(::lstrcmp(vec.at(4).device_name, TEXT("COM10")) == 0);
    REQUIRE(::lstrcmp(vec.at(5).device_name, TEXT("COM11")) == 0);
    REQUIRE(::lstrcmp(vec.at(6).device_name, TEXT("COM12")) == 0);
}

TEST_CASE("Checking read_serial_list()", "[serial]")
{
    serial_notifier::Serial<TestRegistrySerialList1> serial;
    serial_notifier::Serial<TestRegistrySerialList1>::SerialList serial_list = serial.read_serial_list();

    REQUIRE(serial_list.size() == 5);

    REQUIRE(::lstrcmp(serial_list.at(0).device_name, TEXT("COM1")) == 0);
    REQUIRE(::lstrcmp(serial_list.at(0).friendly_name, TEXT("/dev/ttyS1")) == 0);

    REQUIRE(::lstrcmp(serial_list.at(1).device_name, TEXT("COM2")) == 0);
    REQUIRE(::lstrcmp(serial_list.at(1).friendly_name, TEXT("/dev/ttyS2")) == 0);

    REQUIRE(::lstrcmp(serial_list.at(2).device_name, TEXT("COM9")) == 0);
    REQUIRE(::lstrcmp(serial_list.at(2).friendly_name, TEXT("/dev/ttyS9")) == 0);

    REQUIRE(::lstrcmp(serial_list.at(3).device_name, TEXT("COM10")) == 0);
    REQUIRE(::lstrcmp(serial_list.at(3).friendly_name, TEXT("/dev/ttyS10")) == 0);

    REQUIRE(::lstrcmp(serial_list.at(4).device_name, TEXT("COM12")) == 0);
    REQUIRE(::lstrcmp(serial_list.at(4).friendly_name, TEXT("/dev/ttyS12")) == 0);
}