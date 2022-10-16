// bom.hpp

// Copyright (c) Mateusz Jandura. All rights reserved.
// SPDX-License-Identifier: Apache-2.0

#pragma once
#ifndef _SDSDLL_ENCODING_BOM_HPP_
#define _SDSDLL_ENCODING_BOM_HPP_
#include <core/defs.hpp>
#if _SDSDLL_PREPROCESSOR_GUARD
#include <core/api.hpp>
#include <core/traits/string_traits.hpp>

_SDSDLL_BEGIN
// ENUM CLASS endian
enum class endian : unsigned char {
    none   = 0,
    big    = 1,
    little = 2,
    native = little
};

// ENUM CLASS encoding
enum class encoding : unsigned char {
    none,
    utf8,
    utf16
};

// STRUCT bom
struct bom {
    encoding encoding;
    endian endian;
};

// CONSTANT _Utf8_bom
_INLINE_VARIABLE constexpr char _Utf8_bom[] = "\xEF\xBB\xBF";

// CONSTANT _Utf16_big_endian_bom
_INLINE_VARIABLE constexpr char _Utf16_big_endian_bom[] = "\xFE\xFF";

// CONSTANT _Utf16_little_endian_bom
_INLINE_VARIABLE constexpr char _Utf16_little_endian_bom[] = "\xFF\xFE";

// FUNCTION fold_bom
_SDSDLL_API _NODISCARD const char* fold_bom(const bom& _Bom) noexcept;

// FUNCTION unfold_bom
_SDSDLL_API _NODISCARD bom unfold_bom(const char* const _Bom) noexcept;
_SDSDLL_END

#endif // _SDSDLL_PREPROCESSOR_GUARD
#endif // _SDSDLL_ENCODING_BOM_HPP_