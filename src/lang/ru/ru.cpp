#include "stdafx.h"
#include "lang.h"

#define LANG Russian

namespace serial_notifier
{
namespace lang
{

template<>
    struct Translation<LANG, TranslationBase> : public TranslationBase
    {
        Translation() :
            TranslationBase(LANG)

        {
            app_name = TEXT("Менеджер уведомлений COM-портов");
            lang_name = TEXT("Русский");

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
            serial_info = TEXT("Порт: %s\nНаименование: %s\nОписание: %s");
        }
    };

    const Translation<LANG, TranslationBase> russian;
}

}