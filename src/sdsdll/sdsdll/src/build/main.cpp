// main.cpp

// Copyright (c) Mateusz Jandura. All rights reserved.
// SPDX-License-Identifier: Apache-2.0

#include <build/sdsdll_pch.hpp>

int __STDCALL_OR_CDECL DllMain(HMODULE, unsigned long, void*) {
    return 1; // nothing happens when loading/unloading library
}