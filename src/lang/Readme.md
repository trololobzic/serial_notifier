# How to support one more translation
Just add cpp-file with next content and fill strings:

```
#include "stdafx.h"
#include "lang.h"

#define LANG <item of SupportedLanguagesEnum>

namespace serial_notifier
{
namespace lang
{

template<>
    struct Translation<LANG, TranslationBase> : public TranslationBase
    {
        Translation() :
            TranslationBase(LANG, true)

        {
            app_name = TEXT("");
            lang_name = TEXT("");

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
            serial_info = TEXT("");
        }
    };

    const Translation<LANG, TranslationBase> english;
}

}
```