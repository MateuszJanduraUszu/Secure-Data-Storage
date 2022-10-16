// com.cpp

// Copyright (c) Mateusz Jandura. All rights reserved.
// SPDX-License-Identifier: Apache-2.0

#include <build/sdsdll_pch.hpp>
#include <filesystem/com.hpp>
#if _SDSDLL_PREPROCESSOR_GUARD

_SDSDLL_BEGIN
// FUNCTION _Com_instance copy constructor/destructor
_Com_instance::_Com_instance(const COINIT _Options) noexcept
    : _Mystate(_Com_state{SUCCEEDED(CoInitializeEx(nullptr, _Options))}) {}

_Com_instance::~_Com_instance() noexcept {
    if (_Mystate == _Com_state::_Initialized) {
        CoUninitialize();
    }
}

// FUNCTION _Com_instance::_Ok
_NODISCARD bool _Com_instance::_Ok() const noexcept {
    return _Mystate == _Com_state::_Initialized;
}
_SDSDLL_END

#endif // _SDSDLL_PREPROCESSOR_GUARD