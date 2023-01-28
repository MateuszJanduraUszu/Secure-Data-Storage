// com.cpp

// Copyright (c) Mateusz Jandura. All rights reserved.
// SPDX-License-Identifier: Apache-2.0

#include <build/sdsdll_pch.hpp>
#include <system/handle/com.hpp>
#if _SDSDLL_PREPROCESSOR_GUARD

_SDSDLL_BEGIN
// FUNCTION com_instance::initialize
void com_instance::initialize() noexcept {
    (void) ::CoInitializeEx(nullptr, COINIT_MULTITHREADED);
}

// FUNCTION com_instance::uninitialize
void com_instance::uninitialize() noexcept {
    ::CoUninitialize();
}

// FUNCTION com_instance_guard constructor/destructor
com_instance_guard::com_instance_guard() noexcept {
    com_instance::initialize();
}

com_instance_guard::~com_instance_guard() noexcept {
    com_instance::uninitialize();
}
_SDSDLL_END

#endif // _SDSDLL_PREPROCESSOR_GUARD