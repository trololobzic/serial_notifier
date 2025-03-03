#define LANGUAGE German

namespace serial_notifier
{
namespace lang
{

template<>
    struct Translation<LANGUAGE, TranslationBase> : public TranslationBase
    {
        Translation() :
            TranslationBase(CString(TEXT("Deutsch")), LANGUAGE)
        {
            app_name = TEXT("Überwachung serieller Schnittstellen");
 
            menu_items.languages = TEXT("Sprache");
            menu_items.devices = TEXT("Geräte");
            menu_items.autorun = TEXT("Autostart");
            menu_items.popup = TEXT("Popup-Nachrichten");
            menu_items.exit = TEXT("Ausfahrt");
            menu_items.none = TEXT("(leer)");

            popup_messages.plugged_singular = TEXT("Verbunden");
            popup_messages.plugged_plural = TEXT("Verbunden");
            popup_messages.unplugged_singular = TEXT("Deaktiviert");
            popup_messages.unplugged_plural = TEXT("Deaktiviert");

            question_enable_sys_popup = TEXT("Popup-Nachrichten sind im System deaktiviert. Anmachen?");
            info_enable_sys_popup = TEXT("Popup-Nachrichten sind erlaubt");            
            serial_info = TEXT("Hafen: %s\nName: %s\nBeschreibung: %s");
        }
    };

    const Translation<LANGUAGE, TranslationBase> german;
}

}

#undef LANGUAGE
