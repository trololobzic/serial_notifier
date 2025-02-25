#include "stdafx.h"
#include "lang.h"

#define LANG Italian

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
            app_name = TEXT("Monitoraggio delle porte seriali");
            lang_name = TEXT("Italiano");

            menu_items.languages = TEXT("Lingua");
            menu_items.devices = TEXT("Dispositivi");
            menu_items.autorun = TEXT("Avvio automatico");
            menu_items.popup = TEXT("Messaggi pop-up");
            menu_items.exit = TEXT("Uscita");
            menu_items.none = TEXT("(vuoto)");

            popup_messages.plugged_singular = TEXT("Collegato");
            popup_messages.plugged_plural = TEXT("Collegato");
            popup_messages.unplugged_singular = TEXT("Disabilitato");
            popup_messages.unplugged_plural = TEXT("Disabilitato");

            question_enable_sys_popup = TEXT("I messaggi pop-up sono disattivati ​​nel sistema. Accendere?");
            serial_info = TEXT("Porta: %s\nNome: %s\nDescrizione: %s");
        }
    };

    const Translation<LANG, TranslationBase> english;
}

}