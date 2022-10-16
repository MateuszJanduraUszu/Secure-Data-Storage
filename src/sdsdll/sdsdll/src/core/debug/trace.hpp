// trace.hpp

// Copyright (c) Mateusz Jandura. All rights reserved.
// SPDX-License-Identifier: Apache-2.0

#pragma once
#ifndef _SDSDLL_CORE_DEBUG_TRACE_HPP_
#define _SDSDLL_CORE_DEBUG_TRACE_HPP_
#include <core/defs.hpp>
#if _SDSDLL_PREPROCESSOR_GUARD
#include <core/memory/allocator.hpp>
#include <core/optimization/ebco.hpp>
#include <core/pattern/singleton.hpp>
#include <core/symbol/loader.hpp>
#include <cstddef>
#include <cstdio>
#include <debugapi.h>
#include <minwindef.h>

_SDSDLL_BEGIN
// CLASS _Dbg_print_loader
class _Dbg_print_loader { // tries to load the DbgPrint() from the ntdll.dll
private:
    friend singleton<_Dbg_print_loader>;
    
    using _Func_t = ULONG(__cdecl*)(const char*, ...);

public:
    _Dbg_print_loader(const _Dbg_print_loader&) = delete;
    _Dbg_print_loader& operator=(const _Dbg_print_loader&) = delete;

    // checks if the DbgPrint() has been loaded
    _NODISCARD bool _Ok() const noexcept;

    template <class... _Types>
    constexpr void operator()(const char* const _Fmt, const _Types&... _Args) {
        if (_Myfunc) {
            _Myfunc(_Fmt, _Args...);
        }
    }

private:
    _Dbg_print_loader() noexcept;
    ~_Dbg_print_loader() noexcept;

    _Func_t _Myfunc; // pointer to the DbgPrint()
};

// CLASS _Debug_trace_entry_formatter
class _Debug_trace_entry_formatter {
public:
    _CONSTEXPR20 _Debug_trace_entry_formatter() noexcept;
    _CONSTEXPR20 ~_Debug_trace_entry_formatter() noexcept;

    _Debug_trace_entry_formatter(const _Debug_trace_entry_formatter&) = delete;
    _Debug_trace_entry_formatter& operator=(const _Debug_trace_entry_formatter&) = delete;

    template <class... _Types>
    _NODISCARD _CONSTEXPR_DYNAMIC_ALLOC char* _Format_entry(const char* const _Fmt, const _Types&... _Args) {
        _Allocated_buffer& _Data = _Mypair._Val1;
        _Data._Size              = _Estimate_buffer_size(_Fmt, _Args...) + 1; // null-terminated
        _Data._Ptr               = _Mypair._Get_val2().allocate(_Data._Size);
        if (!_Data._Ptr) { // failed to allocate a buffer
            _Data._Size = 0;
            return nullptr;
        }

        _CSTD snprintf(_Data._Ptr, _Data._Size, _Fmt, _Args...);
        return _Data._Ptr;
    }

private:
    template <class... _Types>
    _NODISCARD constexpr size_t _Estimate_buffer_size(const char* const _Fmt, const _Types&... _Args) {
        return _CSTD snprintf(nullptr, 0, _Fmt, _Args...);
    }

    struct _Allocated_buffer {
        char* _Ptr;
        size_t _Size;
    };

#ifdef _MSC_VER
#pragma warning(push, 1)
#pragma warning(disable : 4251) // C4251: _Ebco_pair requires dll-interface
#endif // _MSC_VER
    _Ebco_pair<_Allocated_buffer, allocator<char>> _Mypair;
#ifdef _MSC_VER
#pragma warning(pop)
#endif // _MSC_VER
};

// FUNCTION TEMPLATE debug_trace
template <class... _Types>
_CONSTEXPR20 void debug_trace(const char* const _Fmt, const _Types&... _Args) {
    _Dbg_print_loader& _Loader = _SDSDLL instanceof<_Dbg_print_loader>();
    if (_Loader._Ok()) { // use DbgPrint() instead of OutputDebugStringA()
        _Loader(_Fmt, _Args...);
    } else { // failed to load DbgPrint(), use OutputDebugStringA()
        _Debug_trace_entry_formatter _Formatter;
        const char* const _Ptr = _Formatter._Format_entry(_Fmt, _Args...);
        if (_Ptr) { // buffer ok, report a trace
            OutputDebugStringA(_Ptr);
        }
    }
}

// Base macro for _SDSDLL_DEBUG_TRACE/_SDSDLL_DEBUG_WARNING/_SDSDLL_DEBUG_ERROR
// that formats common part of the entry.
#define _SDSDLL_REPORT_DEBUG_TRACE(_Level, _Fmt, ...)                           \
    _SDSDLL debug_trace("sdsdll.dll!" __FILE__ " (" _SDSDLL_STRINGIZE(__LINE__) \
    ") [" _Level "]: " _Fmt "\n", __VA_ARGS__)

// Available only in debug mode.
#if _ENABLE_DEBUG_TRACE
#define _SDSDLL_DEBUG_TRACE(_Fmt, ...)   _SDSDLL_REPORT_DEBUG_TRACE("TRACE", _Fmt, __VA_ARGS__)
#define _SDSDLL_DEBUG_WARNING(_Fmt, ...) _SDSDLL_REPORT_DEBUG_TRACE("WARNING", _Fmt, __VA_ARGS__)
#define _SDSDLL_DEBUG_ERROR(_Fmt, ...)   _SDSDLL_REPORT_DEBUG_TRACE("ERROR", _Fmt, __VA_ARGS__)
#else // ^^^ _ENABLE_DEBUG_TRACE ^^^ / vvv !_ENABLE_DEBUG_TRACE vvv
#define _SDSDLL_DEBUG_TRACE(_Fmt, ...)
#define _SDSDLL_DEBUG_WARNING(_Fmt, ...)
#define _SDSDLL_DEBUG_ERROR(_Fmt, ...)
#endif // _ENABLE_DEBUG_TRACE
_SDSDLL_END

#endif // _SDSDLL_PREPROCESSOR_GUARD
#endif // _SDSDLL_CORE_DEBUG_TRACE_HPP_