#include "stdafx.h"
#include "lang.h"

#define CATCH_CONFIG_NO_CPP11
#include "catch.hpp"

TEST_CASE("Checking TranslationBase comparing", "[lang]")
{
    {
        serial_notifier::lang::TranslationBase a(CString(TEXT("abracadabra")), serial_notifier::lang::Swedish);
        serial_notifier::lang::TranslationBase b(CString(TEXT("abracadabra")), serial_notifier::lang::Turkish);
        REQUIRE(a == b);
    }

    {
        serial_notifier::lang::TranslationBase a(CString(TEXT("abracadabra_1")), serial_notifier::lang::Norwegian);
        serial_notifier::lang::TranslationBase b(CString(TEXT("abracadabra_2")), serial_notifier::lang::Norwegian);
        REQUIRE_FALSE(a == b);
    }

    {
        serial_notifier::lang::TranslationBase a(CString(TEXT("aaaaaaaaaaaa")), serial_notifier::lang::Hungarian);
        serial_notifier::lang::TranslationBase b(CString(TEXT("zzzzzzzzzzzz")), serial_notifier::lang::Hungarian);
        REQUIRE(a < b);
    }
}

TEST_CASE("Checking Lang 1", "[lang]")
{    
    {
        serial_notifier::lang::TranslationBase a(CString(TEXT("")), serial_notifier::lang::Galician);
        const serial_notifier::lang::TranslationBase * ptr = serial_notifier::lang::Lang::get_translation(serial_notifier::lang::Galician);

        REQUIRE(ptr == &a);
    }

    REQUIRE_THROWS(serial_notifier::lang::Lang::get_translation(serial_notifier::lang::Galician));
}

TEST_CASE("Checking Lang 2", "[lang]")
{
    REQUIRE(serial_notifier::lang::Lang::get_all_translations_list().empty());
    serial_notifier::lang::TranslationBase a(CString(TEXT("C")), serial_notifier::lang::Danish),
                                           b(CString(TEXT("B")), serial_notifier::lang::German),
                                           c(CString(TEXT("A")), serial_notifier::lang::Russian),
                                           d(CString(TEXT("D")), serial_notifier::lang::Russian),
                                           e(CString(TEXT("E")), serial_notifier::lang::Russian);

    serial_notifier::lang::Lang::TranslationsList list = serial_notifier::lang::Lang::get_all_translations_list();
    REQUIRE(list.size() == 5);
    REQUIRE(list[0] == &c);
    REQUIRE(list[1] == &b);
    REQUIRE(list[2] == &a);
    REQUIRE(list[3] == &d);
    REQUIRE(list[4] == &e);
}

TEST_CASE("Checking Lang 3", "[lang]")
{
    serial_notifier::lang::TranslationBase a(CString(TEXT("")), serial_notifier::lang::Swedish);
    serial_notifier::lang::TranslationBase b(CString(TEXT("")), serial_notifier::lang::Turkish, true);
    serial_notifier::lang::TranslationBase c(CString(TEXT("")), serial_notifier::lang::Norwegian);
    serial_notifier::lang::TranslationBase d(CString(TEXT("")), serial_notifier::lang::English_UK);

    REQUIRE(serial_notifier::lang::Lang::get_translation(static_cast<serial_notifier::lang::SupportedLanguagesEnum>(0)) == &b);
    REQUIRE(serial_notifier::lang::Lang::get_translation(serial_notifier::lang::Norwegian) == &c);
    REQUIRE(serial_notifier::lang::Lang::get_translation(serial_notifier::lang::English_USA) == &d);
}
