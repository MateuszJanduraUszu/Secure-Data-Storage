// defs.hpp

// Copyright (c) Mateusz Jandura. All rights reserved.
// SPDX-License-Identifier: Apache-2.0

#pragma once
#ifndef _SDSDLL_CORE_DEFS_HPP_
#define _SDSDLL_CORE_DEFS_HPP_

// Each header should be protected against C++/CLI and unsupported platforms
#ifndef _SDSDLL_PREPROCESSOR_GUARD
#if defined(_M_CEE) || !defined(_WIN32)
#define _SDSDLL_PREPROCESSOR_GUARD 0
#else // ^^^ defined(_M_CEE) || !defined(_WIN32) ^^^ / vvv !defined(_M_CEE) && defined(_WIN32) vvv
#define _SDSDLL_PREPROCESSOR_GUARD 1
#endif // defined(_M_CEE) || !defined(_WIN32)
#endif // _SDSDLL_PREPROCESSOR_GUARD

#if _SDSDLL_PREPROCESSOR_GUARD
// Only C++ compilers are supported
#ifndef __cplusplus
#error Expected C++ compiler.
#endif // __cplusplus

// Requires at least C++14
#if __cplusplus < 201402L
#error Expected C++14 or later.
#endif // __cplusplus

// Only 32/64-bit non-ARM platforms are supported
#if (defined(_M_ARM) || defined(_M_ARM64)) || (!defined(_M_IX86) && !defined(_M_X64))
#error Expected non-ARM 32/64-bit platform.
#endif // (defined(_M_ARM) || defined(_M_ARM64)) || (!defined(_M_IX86) && !defined(_M_X64))

// Use the __cdecl for 32-bit platforms and the __stdcall for 64-bit platforms
#ifndef __STDCALL_OR_CDECL
#ifdef _M_X64
#define __STDCALL_OR_CDECL __stdcall
#else // ^^^ _M_X64 ^^^ / vvv _M_IX86 vvv
#define __STDCALL_OR_CDECL __cdecl
#endif // _M_X64
#endif // __STDCALL_OR_CDECL

// Microsoft's STL stores some stuff in <xlib>/<xlib.h> (non-standard).
// Prefer using <xlib>/<xlib.h> over <lib> if possible.
#ifndef _USE_INTERNAL_MICROSOFT_STL_HEADERS
#ifdef _MSC_VER
#define _USE_INTERNAL_MICROSOFT_STL_HEADERS 1
#else // ^^^ _MSC_VER ^^^ / vvv !_MSC_VER vvv
#define _USE_INTERNAL_MICROSOFT_STL_HEADERS 0
#endif // _MSC_VER
#endif // _USE_INTERNAL_MICROSOFT_STL_HEADERS

// The __has_cpp_attribute macro may be undefined
#ifndef _HAS_CXX_ATTRIBUTE
#ifdef __has_cpp_attribute
#define _HAS_CXX_ATTRIBUTE(_Attr) __has_cpp_attribute(_Attr)
#else // ^^^ __has_cpp_attribute ^^^ / vvv !__has_cpp_attribute vvv
#define _HAS_CXX_ATTRIBUTE(_Attr) 0
#endif // __has_cpp_attribute
#endif // _HAS_CXX_ATTRIBUTE

// The __has_builtin macro may be undefined
#ifndef _HAS_BUILTIN
#ifdef __has_builtin
#define _HAS_BUILTIN(_Builtin) __has_builtin(_Builtin)
#else // ^^^ __has_builtin ^^^ / vvv !__has_builtin vvv
#define _HAS_BUILTIN(_Builtin) 0
#endif // __has_builtin
#endif // _HAS_BUILTIN

// The _NODISCARD macro is already defined in <vcruntime.h>
#ifdef _MSC_VER
#include <vcruntime.h>
#else // ^^^ _MSC_VER ^^^ / vvv !_MSC_VER vvv
#if _HAS_CXX_ATTRIBUTE(nodiscard)
#define _NODISCARD [[nodiscard]]
#else // ^^^ _HAS_CXX_ATTRIBUTE(nodiscard) ^^^ / vvv !_HAS_CXX_ATTRIBUTE(nodiscard) vvv
#define _NODISCARD
#endif // _HAS_CXX_ATTRIBUTE(nodiscard)
#endif // _MSC_VER

// The __declspec keyword is specific for VC++, however Clang supports it as well.
// Note: Set -fdeclspec or -fms-extensions in the Clang options to enable support for __declspec.
#if defined(_MSC_VER) || defined(__clang__)
#define _CRT_ALLOCATOR __declspec(allocator)
#else // ^^^ defined(_MSC_VER) || defined(__clang__) ^^^ / vvv !defined(_MSC_VER) && !defined(__clang__) vvv
#define _CRT_ALLOCATOR
#endif // defined(_MSC_VER) || defined(__clang__)

// Microsoft's STL uses _HAS_CXX17/_HAS_CXX20/_HAS_CXX23 to check the C++ version.
// Note: Set /Zc:__cplusplus in the VC++ options to update the __cplusplus macro.
#ifndef _MSC_VER
#if __cplusplus == 201402L
#define _HAS_CXX17 0
#define _HAS_CXX20 0
#define _HAS_CXX23 0
#elif __cplusplus == 201703L
#define _HAS_CXX17 1
#define _HAS_CXX20 0
#define _HAS_CXX23 0
#elif __cplusplus == 202002L
#define _HAS_CXX17 1
#define _HAS_CXX20 1
#define _HAS_CXX23 0
#else // ^^^ __cplusplus == 202002L ^^^ / vvv __cplusplus > 202002L vvv
#define _HAS_CXX17 1
#define _HAS_CXX20 1
#define _HAS_CXX23 1
#endif // __cplusplus == 201402L
#endif // _MSC_VER

// Microsoft's STL uses _CONSTEXPR17/_CONSTEXPR20/_CONSTEXPR23 to check if constexpr is available.
// Define these macros if not defined.
#ifndef _MSC_VER
// Functions that became constexpr in C++17
#if _HAS_CXX17
#define _CONSTEXPR17 constexpr
#else // ^^^ _HAS_CXX17 ^^^ / vvv !_HAS_CXX17 vvv
#define _CONSTEXPR17 inline
#endif // _HAS_CXX17

// Functions that became constexpr in C++20
#if _HAS_CXX20
#define _CONSTEXPR20 constexpr
#else // ^^^ _HAS_CXX20 ^^^ / vvv !_HAS_CXX20 vvv
#define _CONSTEXPR20 inline
#endif // _HAS_CXX20

// Functions that became constexpr in C++23
#if _HAS_CXX23
#define _CONSTEXPR23 constexpr
#else // ^^^ _HAS_CXX23 ^^^ / vvv !_HAS_CXX23 vvv
#define _CONSTEXPR23 inline
#endif // _HAS_CXX23
#endif // _MSC_VER

// Functions that became consteval in C++20
#ifdef __cpp_consteval
#define _CONSTEVAL20 consteval
#else // ^^^ __cpp_consteval ^^^ / vvv !__cpp_consteval vvv
#define _CONSTEVAL20 constexpr
#endif // __cpp_consteval

// Bit operations for enumerations
#ifndef _BIT_OPS
#define _BIT_OPS(_Ty)                                                                      \
    constexpr _Ty operator&(const _Ty _Left, const _Ty _Right) noexcept {                  \
        using _Int_t = underlying_type_t<_Ty>;                                             \
        return static_cast<_Ty>(static_cast<_Int_t>(_Left) & static_cast<_Int_t>(_Right)); \
    }                                                                                      \
                                                                                           \
    constexpr _Ty operator|(const _Ty _Left, const _Ty _Right) noexcept {                  \
        using _Int_t = underlying_type_t<_Ty>;                                             \
        return static_cast<_Ty>(static_cast<_Int_t>(_Left) | static_cast<_Int_t>(_Right)); \
    }                                                                                      \
                                                                                           \
    constexpr _Ty operator^(const _Ty _Left, const _Ty _Right) noexcept {                  \
        using _Int_t = underlying_type_t<_Ty>;                                             \
        return static_cast<_Ty>(static_cast<_Int_t>(_Left) ^ static_cast<_Int_t>(_Right)); \
    }                                                                                      \
                                                                                           \
    constexpr _Ty operator~(const _Ty _Val) noexcept {                                     \
        using _Int_t = underlying_type_t<_Ty>;                                             \
        return static_cast<_Ty>(~static_cast<_Int_t>(_Val));                               \
    }                                                                                      \
                                                                                           \
    constexpr _Ty operator&=(_Ty& _Dest, const _Ty _Val) noexcept {                        \
        _Dest = _Dest & _Val;                                                              \
        return _Dest;                                                                      \
    }                                                                                      \
                                                                                           \
    constexpr _Ty operator|=(_Ty& _Dest, const _Ty _Val) noexcept {                        \
        _Dest = _Dest | _Val;                                                              \
        return _Dest;                                                                      \
    }                                                                                      \
                                                                                           \
    constexpr _Ty operator^=(_Ty& _Dest, const _Ty _Val) noexcept {                        \
        _Dest = _Dest ^ _Val;                                                              \
        return _Dest;                                                                      \
    }
#endif // _BIT_OPS

// Enable debug trace (only in debug mode)
#ifdef _DEBUG_TRACE_ENABLED
#define _ENABLE_DEBUG_TRACE 1
#else // ^^^ _DEBUG_TRACE_ENABLED ^^^ / vvv !_DEBUG_TRACE_ENABLED vvv
#define _ENABLE_DEBUG_TRACE 0
#endif  // _DEBUG_TRACE_ENABLED

// Verify _Cond and report an exception if failed
#ifdef _DEBUG
#define _SDSDLL_VERIFY(_Cond, _Msg)
#else // ^^^ _DEBUG ^^^ / vvv NDEBUG vvv
#define _SDSDLL_VERIFY(_Cond, _Msg)    \
    do {                               \
        if (_Cond) {                   \
            /* no error, do nothing */ \
        } else {                       \
            throw;                     \
        }                              \
    } while (false)
#endif // _DEBUG

// SDSDLL namespace
#define _SDSDLL_BEGIN namespace sdsdll {
#define _SDSDLL_END   }
#define _SDSDLL       ::sdsdll::

// C/C++ Standard namespace
#ifndef _MSC_VER
#define _CSTD ::
#define _STD  ::std::
#endif // _MSC_VER

// For empty argument declaration
#define _EMPTY_ARG

// Converts _Val to C-style string
#define _SDSDLL_STRINGIZE_BASE(_Val) #_Val
#define _SDSDLL_STRINGIZE(_Val)      _SDSDLL_STRINGIZE_BASE(_Val)

// C++17 constexpr if, see P0292R1
#ifdef __cpp_if_constexpr
#define _CONSTEXPR_IF constexpr
#else // ^^^ __cpp_if_constexpr ^^^ / vvv !__cpp_if_constexpr vvv
#define _CONSTEXPR_IF
#endif // __cpp_if_constexpr

// C++17 inline variables, see P0386R1
#ifdef __cpp_inline_variables
#define _INLINE_VARIABLE inline
#else // ^^^ __cpp_inline_variables ^^^ / vvv !__cpp_inline_variables vvv
#define _INLINE_VARIABLE
#endif // __cpp_inline_variables

// C++20 constexpr dynamic memory allocation, see P0784R7
#ifdef __cpp_constexpr_dynamic_alloc
#define _CONSTEXPR_DYNAMIC_ALLOC constexpr
#else // ^^^ __cpp_constexpr_dynamic_alloc ^^^ / vvv !__cpp_constexpr_dynamic_alloc vvv
#define _CONSTEXPR_DYNAMIC_ALLOC
#endif // __cpp_constexpr_dynamic_alloc
#endif // _SDSDLL_PREPROCESSOR_GUARD
#endif // _SDSDLL_CORE_DEFS_HPP_