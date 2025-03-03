#pragma once
#include <vector>
#include <algorithm>
#include "supported_langs.h"
#include "exception.h"

namespace serial_notifier
{
namespace lang
{
    struct TranslationBase;

    class Lang
    {
        //typedef std::map<UINT32, const TranslationBase*> TranslationsMap;
        //static TranslationsMap _translations;
    public:
        typedef std::vector<const TranslationBase*> TranslationsList;
        static void register_translation(const TranslationBase * lang_ptr);
        static void unregister_translation(const TranslationBase * lang_ptr);
        static const TranslationBase * get_translation(const SupportedLanguagesEnum supported_lang_idx);
        static const TranslationsList & get_all_translations_list();


    private:
        static TranslationsList _translations;

        static TranslationsList::iterator _find_translation_by_supported_lang_idx(const SupportedLanguagesEnum supported_lang_idx);
        static TranslationsList::iterator _find_related_translation(const SupportedLanguagesEnum supported_lang_idx);
        static TranslationsList::iterator _find_default_translation();
        static bool compare_translation_ptrs(const TranslationBase *, const TranslationBase *);
    };

    struct TranslationBase
    {
        TranslationBase(const CString & lang_name, const SupportedLanguagesEnum supported_lang_idx, bool default_lang = false) :
            lang_name(lang_name),
            _supported_lang_idx(supported_lang_idx),
            _default_lang(default_lang)

        {
            Lang::register_translation(this);
        }

        ~TranslationBase()
        {
            Lang::unregister_translation(this);
        }

        CString app_name;
        CString lang_name;

        struct MenuItems
        {
            CString languages;
            CString devices;
            CString autorun;
            CString popup;
            CString exit;
            CString none;
        };
        MenuItems menu_items;

        struct PopupMessages
        {
            CString plugged_singular;
            CString plugged_plural;
            CString unplugged_singular;
            CString unplugged_plural;
        };
        PopupMessages popup_messages;

        CString question_enable_sys_popup;
        CString info_enable_sys_popup;

        CString serial_info;

        bool operator==(const TranslationBase & other) const
        {
            return this->lang_name == other.lang_name;
        }

        bool operator!=(const TranslationBase & other) const
        {
            return !this->operator==(other);
        }

        bool operator<(const TranslationBase & other) const
        {
            return this->lang_name < other.lang_name;
        }

        UINT32 get_cur_kang_idx() const
        {
            return static_cast<UINT32>(_supported_lang_idx);
        }

    private:
        friend class Lang;
        const SupportedLanguagesEnum _supported_lang_idx;
        const bool _default_lang;

        TranslationBase(const TranslationBase & other) :
            _supported_lang_idx(Unknown),
            _default_lang(false)
        {
            (void)other;
        }

        void operator=(const TranslationBase & other)
        {
            (void)other;
        }
    };

    template<UINT32 supported_lang_idx, class TBase>
    struct Translation;
}
}
