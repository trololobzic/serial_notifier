#include "stdafx.h"
#include "lang.h"

#define CATCH_CONFIG_NO_CPP11
#include "catch.hpp"

TEST_CASE("Checking TranslationBase comparing", "[lang]")
{
    {
        serial_notifier::lang::TranslationBase a(serial_notifier::lang::Swedish);
        serial_notifier::lang::TranslationBase b(serial_notifier::lang::Turkish);

        a.lang_name = TEXT("abracadabra");
        b.lang_name = TEXT("abracadabra");

        REQUIRE(a == b);
    }

    {
        serial_notifier::lang::TranslationBase a(serial_notifier::lang::Norwegian);
        serial_notifier::lang::TranslationBase b(serial_notifier::lang::Norwegian);

        a.lang_name = TEXT("abracadabra_1");
        b.lang_name = TEXT("abracadabra_2");

        REQUIRE_FALSE(a == b);
    }

    {
        serial_notifier::lang::TranslationBase a(serial_notifier::lang::Hungarian);
        serial_notifier::lang::TranslationBase b(serial_notifier::lang::Hungarian);

        a.lang_name = TEXT("aaaaaaaaaaaa");
        b.lang_name = TEXT("zzzzzzzzzzzz");

        REQUIRE(a < b);
    }
}

TEST_CASE("Checking Lang 1", "[lang]")
{    
    {
        serial_notifier::lang::TranslationBase a(serial_notifier::lang::Galician);
        const serial_notifier::lang::TranslationBase * ptr = serial_notifier::lang::Lang::get_translation(serial_notifier::lang::Galician);

        REQUIRE(ptr == &a);
    }

    REQUIRE_THROWS(serial_notifier::lang::Lang::get_translation(serial_notifier::lang::Galician));
}

TEST_CASE("Checking Lang 2", "[lang]")
{
    REQUIRE(serial_notifier::lang::Lang::get_all_translations_list().empty());
    serial_notifier::lang::TranslationBase a(serial_notifier::lang::Danish), b(serial_notifier::lang::German), c(serial_notifier::lang::Russian);

    REQUIRE(serial_notifier::lang::Lang::get_all_translations_list().size() == 3);
}

TEST_CASE("Checking Lang 3", "[lang]")
{
    serial_notifier::lang::TranslationBase a(serial_notifier::lang::Swedish);
    serial_notifier::lang::TranslationBase b(serial_notifier::lang::Turkish, true);
    serial_notifier::lang::TranslationBase c(serial_notifier::lang::Norwegian);
    serial_notifier::lang::TranslationBase d(serial_notifier::lang::English_UK);

    REQUIRE(serial_notifier::lang::Lang::get_translation(static_cast<serial_notifier::lang::SupportedLanguagesEnum>(0)) == &b);
    REQUIRE(serial_notifier::lang::Lang::get_translation(serial_notifier::lang::Norwegian) == &c);
    REQUIRE(serial_notifier::lang::Lang::get_translation(serial_notifier::lang::English_USA) == &d);
}
