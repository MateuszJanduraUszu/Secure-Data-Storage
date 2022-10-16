// library_handle.hpp

// Copyright (c) Mateusz Jandura. All rights reserved.
// SPDX-License-Identifier: Apache-2.0

#pragma once
#ifndef _SDSDLL_SYSTEM_HANDLE_LIBRARY_HANDLE_HPP_
#define _SDSDLL_SYSTEM_HANDLE_LIBRARY_HANDLE_HPP_
#include <core/defs.hpp>
#if _SDSDLL_PREPROCESSOR_GUARD
#include <core/api.hpp>
#include <cstddef>
#include <libloaderapi.h>
#include <minwindef.h>
#include <system/handle/handle_wrapper.hpp>

_SDSDLL_BEGIN
// STRUCT library_handle_traits
struct _SDSDLL_API library_handle_traits { // wraps a handle to a library
    using value_type      = HMODULE;
    using pointer         = HMODULE*;
    using const_pointer   = const HMODULE*;
    using reference       = HMODULE&;
    using const_reference = const HMODULE&;

    static constexpr value_type invalid_value = nullptr;

    // closes a valid handle (unchecked)
    static void close(const value_type _Handle) noexcept;
};

// ALIAS library_handle_wrapper
using library_handle_wrapper = handle_wrapper<library_handle_traits>;
_SDSDLL_END

#endif // _SDSDLL_PREPROCESSOR_GUARD
#endif // _SDSDLL_SYSTEM_HANDLE_LIBRARY_HANDLE_HPP_