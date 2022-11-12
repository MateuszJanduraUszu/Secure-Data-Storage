// generic_handle.cpp

// Copyright (c) Mateusz Jandura. All rights reserved.
// SPDX-License-Identifier: Apache-2.0

#include <build/sdsdll_pch.hpp>
#include <system/handle/generic_handle.hpp>
#if _SDSDLL_PREPROCESSOR_GUARD

_SDSDLL_BEGIN
// FUNCTION generic_handle_traits::close
void generic_handle_traits::close(const value_type _Handle) noexcept {
    ::CloseHandle(_Handle);
}
_SDSDLL_END

#endif // _SDSDLL_PREPROCESSOR_GUARD