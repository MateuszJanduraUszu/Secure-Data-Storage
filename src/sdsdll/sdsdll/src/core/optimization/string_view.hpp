// string_view.hpp

// Copyright (c) Mateusz Jandura. All rights reserved.
// SPDX-License-Identifier: Apache-2.0

#pragma once
#ifndef _SDSDLL_CORE_OPTIMIZATION_STRING_VIEW_HPP_
#define _SDSDLL_CORE_OPTIMIZATION_STRING_VIEW_HPP_
#include <core/defs.hpp>
#if _SDSDLL_PREPROCESSOR_GUARD
#include <string_view>

_SDSDLL_BEGIN
using _STD basic_string_view;
using _STD string_view;
#ifdef __cpp_char8_t
using _STD u8string_view;
#endif // __cpp_char8_t
using _STD u16string_view;
using _STD u32string_view;
using _STD wstring_view;

// ALIAS byte_string_view
using byte_string_view = basic_string_view<unsigned char>;
_SDSDLL_END

#endif // _SDSDLL_PREPROCESSOR_GUARD
#endif // _SDSDLL_CORE_OPTIMIZATION_STRING_VIEW_HPP_