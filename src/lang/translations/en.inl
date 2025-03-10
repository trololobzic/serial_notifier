﻿#define LANGUAGE English_UK

namespace serial_notifier
{
namespace lang
{

template<>
    struct Translation<LANGUAGE, TranslationBase> : public TranslationBase
    {
        Translation() :
            TranslationBase(CString(TEXT("English")), LANGUAGE, true)
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
            info_enable_sys_popup = TEXT("Pop-up messages are allowed");
            serial_info = TEXT("Port: %s\nFriendly name: %s\nDescription: %s");
        }
    };

    const Translation<LANGUAGE, TranslationBase> english;
}

}

#undef LANGUAGE