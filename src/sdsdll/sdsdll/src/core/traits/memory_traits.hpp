// memory_traits.hpp

// Copyright (c) Mateusz Jandura. All rights reserved.
// SPDX-License-Identifier: Apache-2.0

#pragma once
#ifndef _SDSDLL_CORE_TRAITS_MEMORY_TRAITS_HPP_
#define _SDSDLL_CORE_TRAITS_MEMORY_TRAITS_HPP_
#include <core/defs.hpp>
#if _SDSDLL_PREPROCESSOR_GUARD
#include <cstddef>
#include <cstring>

// prefer using intrinsic functions
#ifdef _MSC_VER
#pragma intrinsic(memcmp)
#pragma intrinsic(memcpy)
#pragma intrinsic(memmove)
#pragma intrinsic(memset)
#endif // _MSC_VER

_SDSDLL_BEGIN
// STRUCT memory_traits
struct memory_traits {
    using value_type = void;
    using int_type   = int;
    using size_type  = size_t;

    _NODISCARD static int_type compare(
        const void* const _Left, const void* const _Right, const size_type _Size) noexcept {
#if _HAS_BUILTIN(__builtin_memcmp)
        return __builtin_memcmp(_Left, _Right, _Size);
#else // ^^^ _HAS_BUILTIN(__builtin_memcmp) ^^^ / vvv !_HAS_BUILTIN(__builtin_memcmp) vvv
        return _CSTD memcmp(_Left, _Right, _Size);
#endif // _HAS_BUILTIN(__builtin_memcmp)
    }

    static void* copy(void* const _Dest, const void* const _Ptr, const size_type _Size) noexcept {
        if (!_Dest) {
            return nullptr;
        }

        if (!_Ptr) {
            return _Dest;
        }

#if _HAS_BUILTIN(__builtin_memcpy)
        return __builtin_memcpy(_Dest, _Ptr, _Size);
#else // ^^^ _HAS_BUILTIN(__builtin_memcpy) ^^^ / vvv !_HAS_BUILTIN(__builtin_memcpy) vvv
        return _CSTD memcpy(_Dest, _Ptr, _Size);
#endif // _HAS_BUILTIN(__builtin_memcpy)
    }

    static void* move(void* const _Dest, const void* const _Ptr, const size_type _Size) noexcept {
        if (!_Dest) {
            return nullptr;
        }

        if (!_Ptr) {
            return _Dest;
        }

#if _HAS_BUILTIN(__builtin_memmove)
        return __builtin_memmove(_Dest, _Ptr, _Size);
#else // ^^^ _HAS_BUILTIN(__builtin_memmove) ^^^ / vvv !_HAS_BUILTIN(__builtin_memmove) vvv
        return _CSTD memmove(_Dest, _Ptr, _Size);
#endif // _HAS_BUILTIN(__builtin_memmove)
    }

    static void* set(void* const _Dest, const int_type _Val, const size_type _Size) noexcept {
        if (!_Dest) {
            return nullptr;
        }

#if _HAS_BUILTIN(__builtin_memset)
        return __builtin_memset(_Dest, _Val, _Size);
#else // ^^^ _HAS_BUILTIN(__builtin_memset) ^^^ / vvv !_HAS_BUILTIN(__builtin_memset) vvv
        return _CSTD memset(_Dest, _Val, _Size);
#endif // _HAS_BUILTIN(__builtin_memset)
    }
};
_SDSDLL_END

#endif // _SDSDLL_PREPROCESSOR_GUARD
#endif // _SDSDLL_CORE_TRAITS_MEMORY_TRAITS_HPP_