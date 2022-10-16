// process_handle.hpp

// Copyright (c) Mateusz Jandura. All rights reserved.
// SPDX-License-Identifier: Apache-2.0

#pragma once
#ifndef _SDSDLL_SYSTEM_HANDLE_PROCESS_HANDLE_HPP_
#define _SDSDLL_SYSTEM_HANDLE_PROCESS_HANDLE_HPP_
#include <core/defs.hpp>
#if _SDSDLL_PREPROCESSOR_GUARD
#include <core/api.hpp>
#include <cstddef>
#include <handleapi.h>
#include <system/handle/handle_wrapper.hpp>

_SDSDLL_BEGIN
// STRUCT process_handle_traits
struct _SDSDLL_API process_handle_traits { // wraps a handle to a process
    using value_type      = void*;
    using pointer         = void**;
    using const_pointer   = const void**;
    using reference       = void*&;
    using const_reference = const void*&;

    static constexpr value_type invalid_value = nullptr;

    // closes a valid handle (unchecked)
    static void close(const value_type _Handle) noexcept;
};

// ALIAS process_handle_wrapper
using process_handle_wrapper = handle_wrapper<process_handle_traits>;
_SDSDLL_END

#endif // _SDSDLL_PREPROCESSOR_GUARD
#endif // _SDSDLL_SYSTEM_HANDLE_PROCESS_HANDLE_HPP_