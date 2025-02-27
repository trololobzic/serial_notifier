#include "stdafx.h"
#include "lang.h"

#define LANG English_UK

namespace serial_notifier
{
namespace lang
{

template<>
    struct Translation<LANG, TranslationBase> : public TranslationBase
    {
        Translation() :
            TranslationBase(CString(TEXT("English")), LANG, true)

        {
            app_name = TEXT("Serial notifier");

            menu_items.languages = TEXT("Language");
            menu_items.devices = TEXT("Devices");
            menu_items.autorun = TEXT("Autorun");
            menu_items.popup = TEXT("Popup");
            menu_items.exit = TEXT("Exit");
            menu_items.none = TEXT("(none)");

            popup_messages.plugged_singular = TEXT("Plugged");
            popup_messages.plugged_plural = TEXT("Plugged");
            popup_messages.unplugged_singular = TEXT("Unplugged");
            popup_messages.unplugged_plural = TEXT("Unplugged");

            question_enable_sys_popup = TEXT("Popup messages are disable in this system. Enable?");
            serial_info = TEXT("Port: %s\nFriendly name: %s\nDescription: %s");
        }
    };

    const Translation<LANG, TranslationBase> english;
}

}