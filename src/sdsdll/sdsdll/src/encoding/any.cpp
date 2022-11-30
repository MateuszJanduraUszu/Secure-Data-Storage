// bom.cpp

// Copyright (c) Mateusz Jandura. All rights reserved.
// SPDX-License-Identifier: Apache-2.0

#include <build/sdsdll_pch.hpp>
#include <encoding/bom.hpp>
#if _SDSDLL_PREPROCESSOR_GUARD

_SDSDLL_BEGIN
// FUNCTION TEMPLATE narrow_string
template <class _Elem>
_NODISCARD constexpr string narrow_string(const _Elem* _Str, size_t _Count) {
    static_assert(is_any_of_v<_Elem, char, wchar_t>, "Requires a UTF-8/Unicode element type.");
    if _CONSTEXPR_IF (is_same_v<_Elem, char>) { // no conversion
        return string{_Str, _Count};
    } else { // convert a wide string to narrow
        string _Result;
        _Result.reserve(_Count);
        while (_Count-- > 0) {
            _Result.push_back(static_cast<char>(*_Str));
            ++_Str;
        }

        return _Result;
    }
}

template _SDSDLL_API _NODISCARD string narrow_string(const char*, size_t);
template _SDSDLL_API _NODISCARD string narrow_string(const wchar_t*, size_t);

template <class _Elem>
_NODISCARD constexpr string narrow_string(const _Elem* _Str) {
    using _Traits = string_traits<_Elem, size_t>;
    return _SDSDLL narrow_string(_Str, _Traits::length(_Str));
}

template _SDSDLL_API _NODISCARD string narrow_string(const char*);
template _SDSDLL_API _NODISCARD string narrow_string(const wchar_t*);

template <class _Elem>
_NODISCARD constexpr string narrow_string(const basic_string_view<_Elem> _Str) {
    return _SDSDLL narrow_string(_Str.data(), _Str.size());
}

template _SDSDLL_API _NODISCARD string narrow_string(const string_view);
template _SDSDLL_API _NODISCARD string narrow_string(const wstring_view);

template <class _Elem>
_NODISCARD constexpr string narrow_string(const basic_string<_Elem>& _Str) {
    return _SDSDLL narrow_string(_Str.c_str(), _Str.size());
}

template _SDSDLL_API _NODISCARD string narrow_string(const string&);
template _SDSDLL_API _NODISCARD string narrow_string(const wstring&);

// FUNCTION TEMPLATE widen_string
template <class _Elem>
_NODISCARD constexpr wstring widen_string(const _Elem* _Str, size_t _Count) {
    static_assert(is_any_of_v<_Elem, char, wchar_t>, "Requires a UTF-8/Unicode element type.");
    if _CONSTEXPR_IF (is_same_v<_Elem, char>) { // convert a narrow string to wide
        wstring _Result;
        _Result.reserve(_Count);
        while (_Count-- > 0) {
            _Result.push_back(static_cast<wchar_t>(*_Str));
            ++_Str;
        }

        return _Result;
    } else { // no conversion
        return wstring{_Str, _Count};
    }
}

template _SDSDLL_API _NODISCARD wstring widen_string(const char*, size_t);
template _SDSDLL_API _NODISCARD wstring widen_string(const wchar_t*, size_t);

template <class _Elem>
_NODISCARD constexpr wstring widen_string(const _Elem* _Str) {
    using _Traits = string_traits<_Elem, size_t>;
    return _SDSDLL widen_string(_Str, _Traits::length(_Str));
}

template _SDSDLL_API _NODISCARD wstring widen_string(const char*);
template _SDSDLL_API _NODISCARD wstring widen_string(const wchar_t*);

template <class _Elem>
_NODISCARD constexpr wstring widen_string(const basic_string_view<_Elem> _Str) {
    return _SDSDLL widen_string(_Str.data(), _Str.size());
}

template _SDSDLL_API _NODISCARD wstring widen_string(const string_view);
template _SDSDLL_API _NODISCARD wstring widen_string(const wstring_view);

template <class _Elem>
_NODISCARD constexpr wstring widen_string(const basic_string<_Elem>& _Str) {
    return _SDSDLL widen_string(_Str.c_str(), _Str.size());
}

template _SDSDLL_API _NODISCARD wstring widen_string(const string&);
template _SDSDLL_API _NODISCARD wstring widen_string(const wstring&);

// FUNCTION TEMPLATE auto_string
template <class _Elem1, class _Elem2>
_NODISCARD constexpr basic_string<_Elem1> auto_string(const _Elem2* _Str, size_t _Count) {
    static_assert(is_any_of_v<_Elem1, char, wchar_t> && is_any_of_v<_Elem2, char, wchar_t>,
        "Requires a UTF-8/Unicode element type.");
    if _CONSTEXPR_IF (is_same_v<_Elem1, _Elem2>) { // no conversion
        return basic_string<_Elem1>{_Str, _Count};
    } else { // convert a narrow string to wide or a wide string to narrow
        basic_string<_Elem1> _Result;
        _Result.reserve(_Count);
        while (_Count-- > 0) {
            _Result.push_back(static_cast<_Elem1>(*_Str));
            ++_Str;
        }

        return _Result;
    }
}

template _SDSDLL_API _NODISCARD string auto_string(const char*, size_t);
template _SDSDLL_API _NODISCARD string auto_string(const wchar_t*, size_t);
template _SDSDLL_API _NODISCARD wstring auto_string(const char*, size_t);
template _SDSDLL_API _NODISCARD wstring auto_string(const wchar_t*, size_t);

template <class _Elem1, class _Elem2>
_NODISCARD constexpr basic_string<_Elem1> auto_string(const _Elem2* _Str) {
    using _Traits = string_traits<_Elem2, size_t>;
    return _SDSDLL auto_string<_Elem1>(_Str, _Traits::length(_Str));
}

template _SDSDLL_API _NODISCARD string auto_string(const char*);
template _SDSDLL_API _NODISCARD string auto_string(const wchar_t*);
template _SDSDLL_API _NODISCARD wstring auto_string(const char*);
template _SDSDLL_API _NODISCARD wstring auto_string(const wchar_t*);

template <class _Elem1, class _Elem2>
_NODISCARD constexpr basic_string<_Elem1> auto_string(const basic_string_view<_Elem2> _Str) {
    return _SDSDLL auto_string<_Elem1>(_Str.data(), _Str.size());
}

template _SDSDLL_API _NODISCARD string auto_string(const string_view);
template _SDSDLL_API _NODISCARD string auto_string(const wstring_view);
template _SDSDLL_API _NODISCARD wstring auto_string(const string_view);
template _SDSDLL_API _NODISCARD wstring auto_string(const wstring_view);

template <class _Elem1, class _Elem2>
_NODISCARD constexpr basic_string<_Elem1> auto_string(const basic_string<_Elem2>& _Str) {
    return _SDSDLL auto_string<_Elem1>(_Str.c_str(), _Str.size());
}

template _SDSDLL_API _NODISCARD string auto_string(const string&);
template _SDSDLL_API _NODISCARD string auto_string(const wstring&);
template _SDSDLL_API _NODISCARD wstring auto_string(const string&);
template _SDSDLL_API _NODISCARD wstring auto_string(const wstring&);
_SDSDLL_END

#endif // _SDSDLL_PREPROCESSOR_GUARD