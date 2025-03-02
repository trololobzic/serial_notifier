#define LANGUAGE Spanish_Spain

namespace serial_notifier
{
namespace lang
{

template<>
    struct Translation<LANGUAGE, TranslationBase> : public TranslationBase
    {
        Translation() :
            TranslationBase(CString(TEXT("Español")), LANGUAGE)
        {
            app_name = TEXT("Monitoreo de puertos seriales");

            menu_items.languages = TEXT("Idioma");
            menu_items.devices = TEXT("Dispositivos");
            menu_items.autorun = TEXT("Inicio automático");
            menu_items.popup = TEXT("Mensajes emergentes");
            menu_items.exit = TEXT("Salida");
            menu_items.none = TEXT("(vacío)");

            popup_messages.plugged_singular = TEXT("Conectado");
            popup_messages.plugged_plural = TEXT("Conectado");
            popup_messages.unplugged_singular = TEXT("Desactivado");
            popup_messages.unplugged_plural = TEXT("Desactivado");

            question_enable_sys_popup = TEXT("Los mensajes emergentes están deshabilitados en el sistema. ¿Encender?");
            info_enable_sys_popup = TEXT("Se permiten mensajes emergentes"); 
            serial_info = TEXT("Puerto: %s\nNombre: %s\nDescripción: %s");
        }
    };

    const Translation<LANGUAGE, TranslationBase> spain;
}

}

#undef LANGUAGE
