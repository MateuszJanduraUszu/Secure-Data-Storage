// generic_handle.hpp

// Copyright (c) Mateusz Jandura. All rights reserved.
// SPDX-License-Identifier: Apache-2.0

#pragma once
#ifndef _SDSDLL_SYSTEM_HANDLE_GENERIC_HANDLE_HPP_
#define _SDSDLL_SYSTEM_HANDLE_GENERIC_HANDLE_HPP_
#include <core/defs.hpp>
#if _SDSDLL_PREPROCESSOR_GUARD
#include <core/api.hpp>
#include <cstddef>
#include <handleapi.h>
#include <system/handle/handle_wrapper.hpp>

_SDSDLL_BEGIN
// STRUCT generic_handle_traits
struct _SDSDLL_API generic_handle_traits { // wraps a generic handle
    using value_type      = void*;
    using pointer         = void**;
    using const_pointer   = const void**;
    using reference       = void*&;
    using const_reference = const void*&;

    static constexpr value_type invalid_value = reinterpret_cast<value_type>(static_cast<intptr_t>(-1));

    // closes a valid handle (unchecked)
    static void close(const value_type _Handle) noexcept;
};

// ALIAS generic_handle_wrapper
using generic_handle_wrapper = handle_wrapper<generic_handle_traits>;
_SDSDLL_END

#endif // _SDSDLL_PREPROCESSOR_GUARD
#endif // _SDSDLL_SYSTEM_HANDLE_GENERIC_HANDLE_HPP_