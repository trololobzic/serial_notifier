# How to support one more translation
1. Just add inc file in `Translations` directory with next content and fill strings.
2. Include your new inc file in the bottom of `lang.cpp`

```
#define LANGUAGE <item of SuppertedLanguegesEnum>

namespace serial_notifier
{
namespace lang
{

template<>
    struct Translation<LANGUAGE, TranslationBase> : public TranslationBase
    {
        Translation() :
            TranslationBase(CString(TEXT("")), LANGUAGE, true)
        {
            app_name = TEXT("");

            menu_items.languages = TEXT("");
            menu_items.devices = TEXT("");
            menu_items.autorun = TEXT("");
            menu_items.popup = TEXT("");
            menu_items.exit = TEXT("");
            menu_items.none = TEXT("");

            popup_messages.plugged_singular = TEXT("");
            popup_messages.plugged_plural = TEXT("");
            popup_messages.unplugged_singular = TEXT("");
            popup_messages.unplugged_plural = TEXT("");

            question_enable_sys_popup = TEXT("");
            info_enable_sys_popup = TEXT("");
            serial_info = TEXT(""); // "Port: %s\nFriendly name: %s\nDescription: %s"
        }
    };

    const Translation<LANGUAGE, TranslationBase> english;
}

}

#undef LANGUAGE
```