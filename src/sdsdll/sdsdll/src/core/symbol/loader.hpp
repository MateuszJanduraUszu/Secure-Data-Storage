// loader.hpp

// Copyright (c) Mateusz Jandura. All rights reserved.
// SPDX-License-Identifier: Apache-2.0

#pragma once
#ifndef _SDSDLL_CORE_SYMBOL_LOADER_HPP_
#define _SDSDLL_CORE_SYMBOL_LOADER_HPP_
#include <core/defs.hpp>
#if _SDSDLL_PREPROCESSOR_GUARD
#include <core/api.hpp>
#include <core/traits/concepts.hpp>
#include <libloaderapi.h>
#include <minwindef.h>
#include <system/handle/library_handle.hpp>

_SDSDLL_BEGIN
// FUNCTION TEMPLATE _Load_symbol
template <class _Symbol, class _Elem>
_NODISCARD constexpr _Symbol _Load_symbol(const _Elem* const _Lib, const char* const _Name) noexcept {
    library_handle_wrapper _Handle;
    if _CONSTEXPR_IF (is_same_v<_Elem, char>) {
        _Handle = ::LoadLibraryA(_Lib);
    } else if _CONSTEXPR_IF (is_same_v<_Elem, wchar_t>) {
        _Handle = ::LoadLibraryW(_Lib);
    } else { // unsupported element type
        return _Symbol{};
    }

    return reinterpret_cast<_Symbol>(::GetProcAddress(_Handle, _Name));
}
_SDSDLL_END

#endif // _SDSDLL_PREPROCESSOR_GUARD
#endif // _SDSDLL_CORE_SYMBOL_LOADER_HPP_