// com.hpp

// Copyright (c) Mateusz Jandura. All rights reserved.
// SPDX-License-Identifier: Apache-2.0

#pragma once
#ifndef _SDSDLL_FILESYSTEM_COM_HPP_
#define _SDSDLL_FILESYSTEM_COM_HPP_
#include <core/defs.hpp>
#if _SDSDLL_PREPROCESSOR_GUARD
#include <core/api.hpp>
#include <combaseapi.h>
#include <objbase.h>
#include <winerror.h>

_SDSDLL_BEGIN
// ENUM CLASS _Com_state
enum class _Com_state : bool {
    _Uninitialized,
    _Initialized
};

// CLASS _Com_instance
class _Com_instance { // initializes and uninitializes a COM instance for the current thread
public:
    _Com_instance(const COINIT _Options) noexcept;
    ~_Com_instance() noexcept;

    _Com_instance() = delete;
    _Com_instance(const _Com_instance&) = delete;
    _Com_instance& operator=(const _Com_instance&) = delete;

    // checks if a COM instance has been initialized
    _NODISCARD bool _Ok() const noexcept;

private:
    _Com_state _Mystate;
};
_SDSDLL_END

#endif // _SDSDLL_PREPROCESSOR_GUARD
#endif // _SDSDLL_FILESYSTEM_COM_HPP_