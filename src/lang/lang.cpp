#include "../stdafx.h"
#include "lang.h"

namespace serial_notifier
{
namespace lang
{
    Lang::TranslationsList Lang::_translations;

    void Lang::register_translation(const TranslationBase * lang_ptr)
    {
        _translations.push_back(lang_ptr);
        std::sort(_translations.begin(), _translations.end(), compare_translation_ptrs);
    }

    void Lang::unregister_translation(const TranslationBase * lang_ptr)
    {
        Lang::TranslationsList::iterator it = std::find(_translations.begin(), _translations.end(), lang_ptr);
        if (it != _translations.end())
            _translations.erase(it);
    }

    const TranslationBase * Lang::get_translation(const SupportedLanguagesEnum supported_lang_idx)
    {
        TranslationsList::iterator it = _find_translation_by_supported_lang_idx(supported_lang_idx);
        if (it == _translations.end())
        {
            it = _find_related_translation(supported_lang_idx);
        }

        if (it == _translations.end())
        {
            it = _find_default_translation();
        }

        if (it == _translations.end())
        {
            throw serial_notifier::Exception(CString(TEXT("No valid translation was found")));
        }

        return *it;
    }

    Lang::TranslationsList::iterator Lang::_find_translation_by_supported_lang_idx(const SupportedLanguagesEnum supported_lang_idx)
    {
        for (TranslationsList::iterator it = _translations.begin(); it != _translations.end(); it++)
        {
            if ((*it)->_supported_lang_idx == supported_lang_idx)
                return it;
        }
        return _translations.end();
    }

    Lang::TranslationsList::iterator Lang::_find_related_translation(const SupportedLanguagesEnum supported_lang_idx)
    {
        for (TranslationsList::iterator it = _translations.begin(); it != _translations.end(); it++)
        {
            UINT8 translation_lang_code = UINT8((*it)->_supported_lang_idx & 0xFF);
            UINT8 lang_code_for_find = UINT8(supported_lang_idx & 0xFF);

            if (translation_lang_code == lang_code_for_find)
                return it;
        }
        return _translations.end();
    }

    Lang::TranslationsList::iterator Lang::_find_default_translation()
    {
        for (TranslationsList::iterator it = _translations.begin(); it != _translations.end(); it++)
        {
            if ((*it)->_default_lang)
                return it;
        }
        return _translations.end();
    }

    const Lang::TranslationsList & Lang::get_all_translations_list()
    {
        return _translations;
    }

    bool Lang::compare_translation_ptrs(const TranslationBase * a, const TranslationBase * b)
    {
        return (*a) < (*b);
    }

}
}