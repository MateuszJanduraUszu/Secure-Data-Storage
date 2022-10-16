// hash_types.hpp

// Copyright (c) Mateusz Jandura. All rights reserved.
// SPDX-License-Identifier: Apache-2.0

#pragma once
#ifndef _SDSDLL_CRYPTOGRAPHY_HASH_HASH_TYPES_HPP_
#define _SDSDLL_CRYPTOGRAPHY_HASH_HASH_TYPES_HPP_
#include <core/defs.hpp>
#if _SDSDLL_PREPROCESSOR_GUARD
#include <core/api.hpp>
#include <core/traits/concepts.hpp>
#include <core/traits/string_traits.hpp>
#include <cstddef>
#include <filesystem/file.hpp>
#include <filesystem/path.hpp>
#include <string>

// STD types
using _STD basic_string;

_SDSDLL_BEGIN
// STRUCT TEMPLATE _Simple_hash_types
template <class _Elem, class _Eng>
struct _Simple_hash_types {
    using char_type       = _Elem;
    using byte_type       = unsigned char;
    using char_string     = basic_string<char_type>;
    using byte_string     = basic_string<byte_type>;
    using size_type       = size_t;
    using difference_type = ptrdiff_t;
    using engine          = _Eng;
};

// FUNCTION TEMPLATE _Hash_data
template <class _Elem, class _Eng>
_NODISCARD constexpr byte_string _Hash_data(const _Elem* const _Data, const size_t _Count) {
    static_assert(is_same_v<_Elem, typename _Eng::char_type>, "Incompatible type.");
    static_assert(is_same_v<size_t, typename _Eng::size_type>, "Incompatible type.");
    return _Eng(_Data, _Count).bytes();
}

// CLASS TEMPLATE salt
template <class>
class salt;

// FUNCTION TEMPLATE _Hash_password
template <class _Elem, class _Eng>
_NODISCARD constexpr byte_string _Hash_password(
    const _Elem* const _Data, const size_t _Count, const salt<_Eng>& _Salt,
    const size_t _Memory_amount_or_cost, const size_t _Iterations_or_block_size, const size_t _Paralleism) {
    static_assert(is_same_v<_Elem, typename _Eng::char_type>, "Incompatible type.");
    static_assert(is_same_v<size_t, typename _Eng::size_type>, "Incompatible type.");
    return _Eng(
        _Data, _Count, _Salt.get(), _Memory_amount_or_cost, _Iterations_or_block_size, _Paralleism).bytes();
}

// FUNCTION TEMPLATE _Hash_file
template <class _Eng>
_NODISCARD constexpr byte_string _Hash_file(file& _File, const size_t _Off = 0) {
    if (!_File.is_open()) { // no file is open
        return byte_string{};
    }

    if (!_File.seek(_Off)) { // failed to seek the file
        return byte_string{};
    }

    using _Byte_t               = typename _Eng::byte_type;
    using _Size_t               = typename _Eng::size_type;
    constexpr _Size_t _Buf_size = 1024; // 1024-byte chunk
    char _Buf[_Buf_size];
    _Size_t _Read = 0; // read bytes, must be initialized
    _Eng _Engine;
    for (;;) {
        if (!_File.read(_Buf, _Buf_size, _Buf_size, &_Read)) { // failed to read the next chunk
            return byte_string{};
        }

        if (_Read == 0) { // no more data
            break;
        }

        if (!_Engine.next_chunk(_Buf, _Read)) { // failed to hash the next chunk
            return byte_string{};
        }
    }

    return _Engine.bytes();
}
_SDSDLL_END

#endif // _SDSDLL_PREPROCESSOR_GUARD
#endif // _SDSDLL_CRYPTOGRAPHY_HASH_HASH_TYPES_HPP_