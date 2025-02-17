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
    serial_notifier::SerialDevice device1(TEXT("COM1")), device2(TEXT("COM1"));

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
    serial_notifier::Serial<TestRegistrySerialList1>::SerialList serial_list = serial.get_list();

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

TEST_CASE("Checking setup_descriptions()", "[serial]")
{
    typedef serial_notifier::Serial<TestRegistrySerialList1> Serial;

    Serial::SerialList serial_list;
    serial_list.resize(1);
    serial_list.at(0).device_name = TEXT("COM4");
    serial_list.at(0).friendly_name = TEXT("\\Device\\Serial0");

    Serial::setup_descriptions(serial_list);

    //Result of setup_descriptions() depends of local machine language settings.
    //It can be "Serial port" or "Последовательный порт" for example.
    //So this test case check only description is not empty.
    CHECK(serial_list.at(0).description.GetLength());
}

TEST_CASE("Checking get_difference() - plugging devices", "[serial]")
{
    typedef serial_notifier::Serial<TestRegistrySerialList1> Serial;

    Serial::SerialList old_list, new_list;
    old_list.resize(2);
    new_list.resize(3);

    old_list[0].device_name = TEXT("COM9");
    old_list[1].device_name = TEXT("COM10");

    new_list[0].device_name = TEXT("COM9");
    new_list[1].device_name = TEXT("COM10");
    new_list[2].device_name = TEXT("COM20");

    Serial::SerialListDiff diff;
    Serial::get_difference(old_list, new_list, diff);

    REQUIRE(diff.plugged_devices.size() == 1);
    REQUIRE(diff.unplugged_devices.size() == 0);

    REQUIRE(diff.plugged_devices[0].device_name == CString(TEXT("COM20")));
}

TEST_CASE("Checking get_difference() - removing devices", "[serial]")
{
    typedef serial_notifier::Serial<TestRegistrySerialList1> Serial;

    Serial::SerialList old_list, new_list;
    old_list.resize(3);
    new_list.resize(2);

    old_list[0].device_name = TEXT("COM9");
    old_list[1].device_name = TEXT("COM10");
    old_list[2].device_name = TEXT("COM20");

    new_list[0].device_name = TEXT("COM9");
    new_list[1].device_name = TEXT("COM10");

    Serial::SerialListDiff diff;
    Serial::get_difference(old_list, new_list, diff);

    REQUIRE(diff.plugged_devices.size() == 0);
    REQUIRE(diff.unplugged_devices.size() == 1);

    REQUIRE(diff.unplugged_devices[0].device_name == CString(TEXT("COM20")));
}

TEST_CASE("Checking get_difference() - both plugging and unplugging devices", "[serial]")
{
    typedef serial_notifier::Serial<TestRegistrySerialList1> Serial;

    Serial::SerialList old_list, new_list;
    old_list.resize(4);
    new_list.resize(4);

    old_list[0].device_name = TEXT("COM9");
    old_list[1].device_name = TEXT("COM10");
    old_list[2].device_name = TEXT("COM20");
    old_list[3].device_name = TEXT("COM21");

    new_list[0].device_name = TEXT("COM9");
    new_list[1].device_name = TEXT("COM10");
    new_list[2].device_name = TEXT("COM18");
    new_list[3].device_name = TEXT("COM19");

    Serial::SerialListDiff diff;
    Serial::get_difference(old_list, new_list, diff);

    REQUIRE(diff.plugged_devices.size() == 2);
    REQUIRE(diff.unplugged_devices.size() == 2);

    REQUIRE(diff.unplugged_devices[0].device_name == CString(TEXT("COM20")));
    REQUIRE(diff.unplugged_devices[1].device_name == CString(TEXT("COM21")));
    REQUIRE(diff.plugged_devices[0].device_name == CString(TEXT("COM18")));
    REQUIRE(diff.plugged_devices[1].device_name == CString(TEXT("COM19")));
}