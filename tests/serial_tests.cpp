#include "../src/stdafx.h"
#include "../src/serial.h"

#define CATCH_CONFIG_NO_CPP11
#include "catch.hpp"

TEST_CASE("Checking SerialDevice->_get_device_idx()", "[serial]")
{
    struct TestSerialDevice : public serial_notifier::SerialDevice
    {
        UINT32 test_get_device_idx() const
        {
            return _get_device_idx();
        }
    };

    TestSerialDevice serial_device;

    serial_device.device_name = TEXT("COM1");
    REQUIRE(serial_device.test_get_device_idx() == 1);

    serial_device.device_name = TEXT("COM53");
    REQUIRE(serial_device.test_get_device_idx() == 53);

    serial_device.device_name = TEXT("_SMTH67A");
    REQUIRE(serial_device.test_get_device_idx() == 67);

    serial_device.device_name = TEXT("ABRACADABRA");
    REQUIRE(serial_device.test_get_device_idx() == 0);
}

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
