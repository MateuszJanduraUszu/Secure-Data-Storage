// trace.cpp

// Copyright (c) Mateusz Jandura. All rights reserved.
// SPDX-License-Identifier: Apache-2.0

#include <build/sdsdll_pch.hpp>
#include <core/debug/trace.hpp>
#if _SDSDLL_PREPROCESSOR_GUARD

_SDSDLL_BEGIN
// FUNCTION _Dbg_print_loader constructor/destructor
_Dbg_print_loader::_Dbg_print_loader() noexcept : _Myfunc(_Load_symbol<_Func_t>("ntdll", "DbgPrint")) {}

_Dbg_print_loader::~_Dbg_print_loader() noexcept {}

// FUNCTION _Dbg_print_loader::_Ok
_NODISCARD bool _Dbg_print_loader::_Ok() const noexcept {
    return _Myfunc != nullptr;
}

// FUNCTION _Debug_trace_entry_formatter constructor/destructor
_CONSTEXPR20 _Debug_trace_entry_formatter::_Debug_trace_entry_formatter() noexcept
    : _Mypair(_Ebco_default_init{}) {}

_CONSTEXPR20 _Debug_trace_entry_formatter::~_Debug_trace_entry_formatter() noexcept {
    _Allocated_buffer& _Data = _Mypair._Val1;
    if (_Data._Ptr && _Data._Size != 0) {
        _Mypair._Get_val2().deallocate(_Data._Ptr, _Data._Size);
    }
}
_SDSDLL_END

#endif // _SDSDLL_PREPROCESSOR_GUARD