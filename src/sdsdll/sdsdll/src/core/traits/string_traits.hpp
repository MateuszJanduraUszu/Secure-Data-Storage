// string_traits.hpp

// Copyright (c) Mateusz Jandura. All rights reserved.
// SPDX-License-Identifier: Apache-2.0

#pragma once
#ifndef _SDSDLL_CORE_TRAITS_STRING_TRAITS_HPP_
#define _SDSDLL_CORE_TRAITS_STRING_TRAITS_HPP_
#include <core/defs.hpp>
#if _SDSDLL_PREPROCESSOR_GUARD
#include <core/traits/concepts.hpp>
#include <core/traits/memory_traits.hpp>
#include <cstddef>
#include <cstring>
#include <string>

// prefer using intrinsic functions
#ifdef _MSC_VER
#pragma intrinsic(strcmp)
#pragma intrinsic(strlen)
#endif // _MSC_VER

// STD types
using _STD basic_string;

_SDSDLL_BEGIN
// ALIAS byte_string
using byte_string = basic_string<unsigned char>;

// STRUCT TEMPLATE _Char_string_traits
template <class _Elem, class _Integer>
struct _Char_string_traits { // traits for char-like (1 byte) elements
private:
    static_assert(sizeof(_Elem) == 1, "Requires a 1-byte element type.");
    static_assert(is_standard_integer_v<_Integer>, "Requires a standard integer.");

public:
    using char_type = _Elem;
    using int_type  = _Integer;
    using size_type = size_t;

    _NODISCARD static constexpr int_type compare(
        const _Elem* const _Left, const _Elem* const _Right) noexcept {
#if _HAS_BUILTIN(__builtin_strcmp)
        return static_cast<_Integer>(__builtin_strcmp(
            reinterpret_cast<const char*>(_Left), reinterpret_cast<const char*>(_Right)));
#else // ^^^ _HAS_BUILTIN(__builtin_strcmp) ^^^ / vvv !_HAS_BUILTIN(__builtin_strcmp) vvv
        return static_cast<_Integer>(_CSTD strcmp(
            reinterpret_cast<const char*>(_Left), reinterpret_cast<const char*>(_Right)));
#endif // _HAS_BUILTIN(__builtin_strcmp)
    }

    _NODISCARD static constexpr int_type compare(
        const _Elem* const _Left, const _Elem* const _Right, size_type _Count) noexcept {
#if _HAS_BUILTIN(__builtin_strncmp)
        return static_cast<_Integer>(__builtin_strncmp(
            reinterpret_cast<const char*>(_Left), reinterpret_cast<const char*>(_Right), _Count));
#else // ^^^ _HAS_BUILTIN(__builtin_strncmp) ^^^ / vvv !_HAS_BUILTIN(__builtin_strncmp) vvv
        return static_cast<_Integer>(_CSTD strncmp(
            reinterpret_cast<const char*>(_Left), reinterpret_cast<const char*>(_Right), _Count));
#endif // _HAS_BUILTIN(__builtin_strncmp)
    }

    _NODISCARD static constexpr int_type length(const _Elem* _Str) noexcept {
#if _HAS_BUILTIN(__builtin_strlen)
#ifdef __cpp_char8_t
        if _CONSTEXPR_IF (is_same_v<_Elem, char8_t>) {
            return static_cast<_Integer>(__builtin_u8strlen(_Str));
        } else
#endif // __cpp_char8_t
        {
            return static_cast<_Integer>(__builtin_strlen(reinterpret_cast<const char*>(_Str)));
        }
#else // ^^^ _HAS_BUILTIN(__builtin_strlen) ^^^ / vvv !_HAS_BUILTIN(__builtin_strlen) vvv
        return static_cast<_Integer>(_CSTD strlen(reinterpret_cast<const char*>(_Str)));
#endif // _HAS_BUILTIN(__builtin_strlen)
    }

    static constexpr _Elem* copy(
        _Elem* const _Dest, const _Elem* const _Str, const size_type _Count) noexcept {
        return _Dest ? static_cast<_Elem*>(
            memory_traits::copy(_Dest, _Str, _Count * sizeof(_Elem))) : nullptr;
    }

    static constexpr _Elem* move(
        _Elem* const _Dest, const _Elem* const _Str, const size_type _Count) noexcept {
        return _Dest ? static_cast<_Elem*>(
            memory_traits::move(_Dest, _Str, _Count * sizeof(_Elem))) : nullptr;
    }

    _NODISCARD static constexpr const _Elem* find(const _Elem* _Str, const _Elem _Ch) noexcept {
        if (!_Str) {
            return nullptr;
        }

#if _HAS_BUILTIN(__builtin_strchr)
        return reinterpret_cast<const _Elem*>(__builtin_strchr(
            reinterpret_cast<const char*>(_Str), static_cast<char>(_Ch)));
#else // ^^^ _HAS_BUILTIN(__builtin_strchr) ^^^ / vvv !_HAS_BUILTIN(__builtin_strchr) vvv
        return reinterpret_cast<const _Elem*>(_CSTD strchr(
            reinterpret_cast<const char*>(_Str), static_cast<char>(_Ch)));
#endif // _HAS_BUILTIN(__builtin_strchr)
    }

    _NODISCARD static constexpr const _Elem* find(const _Elem* _Str, const _Elem* const _Substr) noexcept {
        if (!_Str) {
            return nullptr;
        }

        if (!_Substr) {
            return _Str;
        }

#if _HAS_BUILTIN(__builtin_strstr)
        return reinterpret_cast<const _Elem*>(__builtin_strstr(
            reinterpret_cast<const char*>(_Str), reinterpret_cast<const char*>(_Substr)));
#else // ^^^ _HAS_BUILTIN(__builtin_strstr) ^^^ / vvv !_HAS_BUILTIN(__builtin_strstr) vvv
        return reinterpret_cast<const _Elem*>(_CSTD strstr(
            reinterpret_cast<const char*>(_Str), reinterpret_cast<const char*>(_Substr)));
#endif // _HAS_BUILTIN(__builtin_strstr)
    }
};

// STRUCT TEMPLATE _Wchar_string_traits
template <class _Elem, class _Integer>
struct _Wchar_string_traits { // traits for wchar_t-like (2 bytes) elements
private:
    static_assert(sizeof(_Elem) == 2, "Requires a 2-byte element type.");
    static_assert(is_standard_integer_v<_Integer>, "Requires a standard integer.");

public:
    using char_type = _Elem;
    using int_type  = _Integer;
    using size_type = size_t;

    _NODISCARD static constexpr int_type compare(const _Elem* _Left, const _Elem* _Right) noexcept {
#if _HAS_BUILTIN(__builtin_wcscmp)
        return static_cast<_Integer>(__builtin_wcscmp(
            reinterpret_cast<const wchar_t*>(_Left), reinterpret_cast<const wchar_t*>(_Right)));
#else // ^^^ _HAS_BUILTIN(__builtin_wcscmp) ^^^ / vvv !_HAS_BUILTIN(__builtin_wcscmp) vvv
        return static_cast<_Integer>(_CSTD wcscmp(
            reinterpret_cast<const wchar_t*>(_Left), reinterpret_cast<const wchar_t*>(_Right)));
#endif // _HAS_BUILTIN(__builtin_wcscmp)
    }

    _NODISCARD static constexpr int_type compare(
        const _Elem* _Left, const _Elem* _Right, size_type _Count) noexcept {
#if _HAS_BUILTIN(__builtin_wcsncmp)
        return static_cast<_Integer>(__builtin_wcsncmp(
            reinterpret_cast<const wchar_t*>(_Left), reinterpret_cast<const wchar_t*>(_Right), _Count));
#else // ^^^ _HAS_BUILTIN(__builtin_wcsncmp) ^^^ / vvv !_HAS_BUILTIN(__builtin_wcsncmp) vvv
        return static_cast<_Integer>(_CSTD wcsncmp(
            reinterpret_cast<const wchar_t*>(_Left), reinterpret_cast<const wchar_t*>(_Right), _Count));
#endif // _HAS_BUILTIN(__builtin_wcsncmp)
    }

    _NODISCARD static constexpr int_type length(const _Elem* _Str) noexcept {
#if _HAS_BUILTIN(__builtin_wcslen)
        return static_cast<_Integer>(__builtin_wcslen(reinterpret_cast<const wchar_t*>(_Str)));
#else // ^^^ _HAS_BUILTIN(__builtin_wcslen) ^^^ / vvv !_HAS_BUILTIN(__builtin_wcslen) vvv
        return static_cast<_Integer>(_CSTD wcslen(reinterpret_cast<const wchar_t*>(_Str)));
#endif // _HAS_BUILTIN(__builtin_wcslen)
    }

    static constexpr _Elem* copy(
        _Elem* const _Dest, const _Elem* const _Str, const size_type _Count) noexcept {
        return _Dest ? static_cast<_Elem*>(
            memory_traits::copy(_Dest, _Str, _Count * sizeof(_Elem))) : nullptr;
    }

    static constexpr _Elem* move(
        _Elem* const _Dest, const _Elem* const _Str, const size_type _Count) noexcept {
        return _Dest ? static_cast<_Elem*>(
            memory_traits::move(_Dest, _Str, _Count * sizeof(_Elem))) : nullptr;
    }

    _NODISCARD static constexpr const _Elem* find(const _Elem* _Str, const _Elem _Ch) noexcept {
        if (!_Str) {
            return nullptr;
        }

#if _HAS_BUILTIN(__builtin_wcschr)
        return reinterpret_cast<const _Elem*>(__builtin_wcschr(
            reinterpret_cast<const wchar_t*>(_Str), static_cast<wchar_t>(_Ch)));
#else // ^^^ _HAS_BUILTIN(__builtin_wcschr) ^^^ / vvv !_HAS_BUILTIN(__builtin_wcschr) vvv
        return reinterpret_cast<const _Elem*>(_CSTD wcschr(
            reinterpret_cast<const char*>(_Str), static_cast<wchar_t>(_Ch)));
#endif // _HAS_BUILTIN(__builtin_wcschr)
    }

    _NODISCARD static constexpr const _Elem* find(const _Elem* _Str, const _Elem* const _Substr) noexcept {
        if (!_Str) {
            return nullptr;
        }

        if (!_Substr) {
            return _Str;
        }

#if _HAS_BUILTIN(__builtin_wcsstr)
        return reinterpret_cast<const _Elem*>(__builtin_wcsstr(
            reinterpret_cast<const wchar_t*>(_Str), reinterpret_cast<const wchar_t*>(_Substr)));
#else // ^^^ _HAS_BUILTIN(__builtin_wcsstr) ^^^ / vvv !_HAS_BUILTIN(__builtin_wcsstr) vvv
        return reinterpret_cast<const _Elem*>(_CSTD wcsstr(
            reinterpret_cast<const wchar_t*>(_Str), reinterpret_cast<const wchar_t*>(_Substr)));
#endif // _HAS_BUILTIN(__builtin_wcsstr)
    }
};

// STRUCT TEMPLATE _Char32_string_traits
template <class _Elem, class _Integer>
struct _Char32_string_traits { // traits for char32_t-like (4 bytes) elements
private:
    static_assert(sizeof(_Elem) == 4, "Requires a 4-byte element type.");
    static_assert(is_standard_integer_v<_Integer>, "Requires a standard integer.");
    
public:
    using char_type = _Elem;
    using int_type  = _Integer;
    using size_type = size_t;

    _NODISCARD static constexpr int_type compare(const _Elem* _Left, const _Elem* _Right) noexcept {
        while (*_Left == *_Right) { // no intrinsic function
            if (*_Left == _Elem{0}) { // end of the string
                return 0;
            }
        }

        return *_Left > *_Right ? int_type{-1} : int_type{1};
    }

    _NODISCARD static constexpr int_type compare(
        const _Elem* _Left, const _Elem* _Right, size_type _Count) noexcept {
        while (*_Left == *_Right && _Count != static_cast<size_type>(-1)) { // no intrinsic function
            if (*_Left == _Elem{0}) { // end of the string
                return 0;
            }

            --_Count;
        }

        return *_Left > *_Right ? int_type{-1} : int_type{1};
    }

    _NODISCARD static constexpr int_type length(const _Elem* _Str) noexcept {
        int_type _Count = 0;
        while (*_Str != _Elem{0}) { // no intrinsic function
            ++_Str;
            ++_Count;
        }

        return _Count;
    }

    static constexpr _Elem* copy(
        _Elem* const _Dest, const _Elem* const _Str, const size_type _Count) noexcept {
        return _Dest ? static_cast<_Elem*>(
            memory_traits::copy(_Dest, _Str, _Count * sizeof(_Elem))) : nullptr;
    }

    static constexpr _Elem* move(
        _Elem* const _Dest, const _Elem* const _Str, const size_type _Count) noexcept {
        return _Dest ? static_cast<_Elem*>(
            memory_traits::move(_Dest, _Str, _Count * sizeof(_Elem))) : nullptr;
    }

    _NODISCARD static constexpr const _Elem* find(const _Elem* _Str, const _Elem _Ch) noexcept {
        if (!_Str) {
            return nullptr;
        }

        while (*_Str != _Elem{0}) { // no intrinsic function
            if (*_Str == _Ch) {
                return _Str;
            }
        }

        return nullptr;
    }

    _NODISCARD static constexpr const _Elem* find(const _Elem* _Str, const _Elem* const _Substr) noexcept {
        if (!_Str) {
            return nullptr;
        }

        if (!_Substr) {
            return _Str;
        }

        while (*_Str != _Elem{0}) { // no intrinsic function
            if (*_Str == *_Substr) {
                // Note: We must use copies of the pointers, otherwise we will get a bad result.
                const _Elem* _Temp_str    = _Str;
                const _Elem* _Temp_substr = _Substr;
                bool _Match;
                while (*_Temp_substr != _Elem{0}) {
                    if (*_Temp_str != *_Temp_substr || *_Temp_str == _Elem{0}) {
                        _Match = false;
                        break;
                    }

                    _Match = true;
                    ++_Temp_str;
                    ++_Temp_substr;
                }

                if (_Match) { // found substring
                    return _Str;
                }
            }

            ++_Str;
        }

        return nullptr;
    }
};

// STRUCT TEMPLATE string_traits
template <class _Elem, class _Integer>
struct string_traits {
    static_assert(_Always_false<_Elem, _Integer>, "Requires a 1/2/4-byte element type.");
};

template <class _Integer>
struct string_traits<char, _Integer> : _Char_string_traits<char, _Integer> {};

template <class _Integer>
struct string_traits<unsigned char, _Integer> : _Char_string_traits<unsigned char, _Integer> {};

#ifdef __cpp_char8_t
template <class _Integer>
struct string_traits<char8_t, _Integer> : _Char_string_traits<char8_t, _Integer> {};
#endif // __cpp_char8_t

template <class _Integer>
struct string_traits<wchar_t, _Integer> : _Wchar_string_traits<wchar_t, _Integer> {};

template <class _Integer>
struct string_traits<char16_t, _Integer> : _Wchar_string_traits<char16_t, _Integer> {};

template <class _Integer>
struct string_traits<char32_t, _Integer> : _Char32_string_traits<char32_t, _Integer> {};
_SDSDLL_END

#endif // _SDSDLL_PREPROCESSOR_GUARD
#endif // _SDSDLL_CORE_TRAITS_STRING_TRAITS_HPP_