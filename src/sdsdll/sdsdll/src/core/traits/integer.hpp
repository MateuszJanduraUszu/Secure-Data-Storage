// integer.hpp

// Copyright (c) Mateusz Jandura. All rights reserved.
// SPDX-License-Identifier: Apache-2.0

#pragma once
#ifndef _SDSDLL_CORE_INTEGER_HPP_
#define _SDSDLL_CORE_INTEGER_HPP_
#include <core/defs.hpp>
#if _SDSDLL_PREPROCESSOR_GUARD
#include <array>
#include <core/traits/concepts.hpp>
#include <core/traits/memory_traits.hpp>
#include <cstdint>

// STD types
using _STD array;

_SDSDLL_BEGIN
// FUNCTION TEMPLATE pack_integer
template <class _Integer>
_NODISCARD constexpr _Integer pack_integer(const uint8_t (&_Bytes)[sizeof(_Integer)]) noexcept {
    static_assert(is_standard_integer_v<_Integer>, "Requires 1/2/4/8-byte standard integer.");
    if constexpr (sizeof(_Integer) == 1) { // pack 1-byte integer
        return static_cast<_Integer>(_Bytes[0]);
    } else { // pack 2/4/8-byte integer
        _Integer _Result;
        memory_traits::copy(&_Result, _Bytes, sizeof(_Integer));
        return _Result;
    }
}

template <class _Integer>
_NODISCARD constexpr _Integer pack_integer(const array<uint8_t, sizeof(_Integer)>& _Bytes) noexcept {
    static_assert(is_standard_integer_v<_Integer>, "Requires 1/2/4/8-byte standard integer.");
    if constexpr (sizeof(_Integer) == 1) { // pack 1-byte integer
        return static_cast<_Integer>(_Bytes[0]);
    } else { // pack 2/4/8-byte integer
        _Integer _Result;
        memory_traits::copy(&_Result, _Bytes.data(), sizeof(_Integer));
        return _Result;
    }
}

// FUNCTION TEMPLATE unpack_integer
template <class _Integer>
_NODISCARD constexpr array<uint8_t, sizeof(_Integer)> unpack_integer(const _Integer _Val) noexcept {
    static_assert(is_standard_integer_v<_Integer>, "Requires 1/2/4/8-byte standard integer.");
    if constexpr (sizeof(_Integer) == 1) { // unpack 1-byte integer
        return {static_cast<uint8_t>(_Val)};
    } else { // unpack 2/4/8-byte integer
        array<uint8_t, sizeof(_Integer)> _Result;
        memory_traits::copy(_Result.data(), &_Val, sizeof(_Integer));
        return _Result;
    }
}
_SDSDLL_END

#endif // _SDSDLL_PREPROCESSOR_GUARD
#endif // _SDSDLL_CORE_INTEGER_HPP_