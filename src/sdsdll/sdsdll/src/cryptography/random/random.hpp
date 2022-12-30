// random.hpp

// Copyright (c) Mateusz Jandura. All rights reserved.
// SPDX-License-Identifier: Apache-2.0

#pragma once
#ifndef _SDSDLL_CRYPTOGRAPHY_RANDOM_RANDOM_HPP_
#define _SDSDLL_CRYPTOGRAPHY_RANDOM_RANDOM_HPP_
#include <core/defs.hpp>
#if _SDSDLL_PREPROCESSOR_GUARD
#include <core/api.hpp>
#include <core/traits/string_traits.hpp>
#include <core/traits/type_traits.hpp>
#include <cstddef>
#include <cstdint>
#include <random>
#include <string>

// STD types
using _STD mt19937;
using _STD random_device;
using _STD string;
using _STD uniform_int_distribution;

_SDSDLL_BEGIN
// ENUM CLASS random_format
enum class random_format : unsigned char {
    number    = 0b0001,
    lowercase = 0b0010,
    uppercase = 0b0100,
    general   = number | lowercase | uppercase
};

_BIT_OPS(random_format)

// STRUCT TEMPLATE _Random_elements
template <class _Elem>
struct _Random_elements {
    _Elem _Number;
    _Elem _Lowercase;
    _Elem _Uppercase;
};

// FUNCTION _Generate_random_byte
extern _NODISCARD uint8_t _Generate_random_byte(const uint8_t _Min = 0, const uint8_t _Max = 0xFF);

// FUNCTION TEMPLATE _Generate_random_elements
template <class _Elem>
constexpr void _Generate_random_elements(_Random_elements<_Elem>& _Elems, const random_format _Fmt) {
    static_assert(is_any_of_v<_Elem, char, uint8_t>, "Requires 1-byte element type.");
    if (_Has_bits(_Fmt, random_format::number)) { // random number [0, 9] taken from the ASCII table
        _Elems._Number = static_cast<_Elem>(_Generate_random_byte(48, 57));
    }

    if (_Has_bits(_Fmt, random_format::lowercase)) { // random lowercase [a, z] taken from the ASCII table
        _Elems._Lowercase = static_cast<_Elem>(_Generate_random_byte(97, 122));
    }

    if (_Has_bits(_Fmt, random_format::uppercase)) { // random uppercase [A, Z] taken from the ASCII table
        _Elems._Uppercase = static_cast<_Elem>(_Generate_random_byte(65, 90));
    }
}

// FUNCTION TEMPLATE _Choose_random_element
template <class _Elem>
_NODISCARD constexpr _Elem _Choose_random_element(
    const _Random_elements<_Elem>& _Elems, const random_format _Fmt) noexcept {
    _Elem _Buf[3]  = {0}; // number, lowercase and uppercase
    uint8_t _Count = 0;
    if (_Has_bits(_Fmt, random_format::number)) {
        _Buf[_Count++] = _Elems._Number;
    }

    if (_Has_bits(_Fmt, random_format::lowercase)) {
        _Buf[_Count++] = _Elems._Lowercase;
    }

    if (_Has_bits(_Fmt, random_format::uppercase)) {
        _Buf[_Count++] = _Elems._Uppercase;
    }

    return _Buf[_Generate_random_byte(0, _Count - 1)];
}

// FUNCTION random_string
_SDSDLL_API _NODISCARD string random_string(const size_t _Size, const random_format _Fmt);

// FUNCTION random_bytes
_SDSDLL_API _NODISCARD byte_string random_bytes(const size_t _Size, const random_format _Fmt);
_SDSDLL_END

#endif // _SDSDLL_PREPROCESSOR_GUARD
#endif // _SDSDLL_CRYPTOGRAPHY_RANDOM_RANDOM_HPP_