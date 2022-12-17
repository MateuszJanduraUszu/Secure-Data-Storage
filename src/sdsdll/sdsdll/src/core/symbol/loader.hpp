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
template <class _Symbol>
_NODISCARD constexpr _Symbol _Load_symbol(const wchar_t* const _Lib, const char* const _Name) noexcept {
    library_handle_wrapper _Handle(::LoadLibraryW(_Lib));
    return reinterpret_cast<_Symbol>(::GetProcAddress(_Handle, _Name));
}
_SDSDLL_END

#endif // _SDSDLL_PREPROCESSOR_GUARD
#endif // _SDSDLL_CORE_SYMBOL_LOADER_HPP_