#include "stdafx.h"
#include "lang.h"

#define LANGUAGE French_France

namespace serial_notifier
{
namespace lang
{

template<>
    struct Translation<LANGUAGE, TranslationBase> : public TranslationBase
    {
        Translation() :
            TranslationBase(CString(TEXT("Français")), LANGUAGE, true)
        {
            app_name = TEXT("Surveillance des ports série");

            menu_items.languages = TEXT("Langue");
            menu_items.devices = TEXT("Appareils");
            menu_items.autorun = TEXT("Démarrage automatique");
            menu_items.popup = TEXT("Messages contextuels");
            menu_items.exit = TEXT("Finition");
            menu_items.none = TEXT("(non)");

            popup_messages.plugged_singular = TEXT("Connecté");
            popup_messages.plugged_plural = TEXT("Connecté");
            popup_messages.unplugged_singular = TEXT("Désactivé");
            popup_messages.unplugged_plural = TEXT("Désactivé");

            question_enable_sys_popup = TEXT("Les messages contextuels sont désactivés dans le système. Allumer?");
            info_enable_sys_popup = TEXT("Les messages contextuels sont autorisés");
            serial_info = TEXT("Port: %s\nFriendly nom %s\nDescription: %s");
        }
    };

    const Translation<LANGUAGE, TranslationBase> french;
}

}

#undef LANGUAGE