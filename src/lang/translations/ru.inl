#define LANGUAGE Russian

namespace serial_notifier
{
namespace lang
{

template<>
    struct Translation<LANGUAGE, TranslationBase> : public TranslationBase
    {
        Translation() :
            TranslationBase(CString(TEXT("Русский")), LANGUAGE)
        {
            app_name = TEXT("Менеджер уведомлений COM-портов");

            menu_items.languages = TEXT("Язык");
            menu_items.devices = TEXT("Устройства");
            menu_items.autorun = TEXT("Автозапуск");
            menu_items.popup = TEXT("Всплывающие сообщения");
            menu_items.exit = TEXT("Выход");
            menu_items.none = TEXT("(нет)");

            popup_messages.plugged_singular = TEXT("Подключен");
            popup_messages.plugged_plural = TEXT("Подключены");
            popup_messages.unplugged_singular = TEXT("Отключен");
            popup_messages.unplugged_plural = TEXT("Отключены");

            question_enable_sys_popup = TEXT("Всплывающие сообщения отключены в системе. Включить?");
            info_enable_sys_popup = TEXT("Всплывающие сообщения разрешены");
            serial_info = TEXT("Порт: %s\nНаименование: %s\nОписание: %s");
        }
    };

    const Translation<LANGUAGE, TranslationBase> russian;
}

}

#undef LANGUAGE