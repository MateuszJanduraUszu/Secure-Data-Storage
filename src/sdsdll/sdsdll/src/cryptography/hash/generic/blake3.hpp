// blake3.hpp

// Copyright (c) Mateusz Jandura. All rights reserved.
// SPDX-License-Identifier: Apache-2.0

#pragma once
#ifndef _SDSDLL_CRYPTOGRAPHY_HASH_GENERIC_BLAKE3_HPP_
#define _SDSDLL_CRYPTOGRAPHY_HASH_GENERIC_BLAKE3_HPP_
#include <core/defs.hpp>
#if _SDSDLL_PREPROCESSOR_GUARD
#include <blake3.h>
#include <climits>
#include <core/api.hpp>
#include <core/memory/allocator.hpp>
#include <core/optimization/ebco.hpp>
#include <core/optimization/sbo.hpp>
#include <core/optimization/string_view.hpp>
#include <core/traits/concepts.hpp>
#include <core/traits/string_traits.hpp>
#include <core/traits/type_traits.hpp>
#include <cryptography/hash/hash_types.hpp>
#include <cstddef>
#include <filesystem/file.hpp>
#include <filesystem/path.hpp>
#include <string>

// STD types
using _STD basic_string;
using _STD string;
using _STD wstring;

_SDSDLL_BEGIN
// CLASS TEMPLATE _Blake3_fn
template <class _Simple_types>
class _Blake3_fn { // wrapper for the blake3_hasher functions
public:
    using char_type       = typename _Simple_types::char_type;
    using byte_type       = typename _Simple_types::byte_type;
    using char_string     = typename _Simple_types::char_string;
    using byte_string     = typename _Simple_types::byte_string;
    using size_type       = typename _Simple_types::size_type;
    using difference_type = typename _Simple_types::difference_type;
    using engine          = typename _Simple_types::engine;

    constexpr _Blake3_fn() noexcept     = default;
    _CONSTEXPR20 ~_Blake3_fn() noexcept = default;

    _Blake3_fn(const _Blake3_fn&) = delete;
    _Blake3_fn& operator=(const _Blake3_fn&) = delete;

    // tries to invoke the blake3_hasher functions
    _NODISCARD constexpr bool operator()(byte_type* const _Buf, const size_type _Buf_size,
        const char_type* const _Data, const size_type _Data_size) const noexcept;
};

// CLASS TEMPLATE _Blake3_stream_fn
template <class _Simple_types>
class _Blake3_stream_fn { // wrapper for the blake3_hasher_update() function
public:
    using char_type       = typename _Simple_types::char_type;
    using byte_type       = typename _Simple_types::byte_type;
    using char_string     = typename _Simple_types::char_string;
    using byte_string     = typename _Simple_types::byte_string;
    using size_type       = typename _Simple_types::size_type;
    using difference_type = typename _Simple_types::difference_type;
    using engine          = typename _Simple_types::engine;

    constexpr _Blake3_stream_fn() noexcept     = default;
    _CONSTEXPR20 ~_Blake3_stream_fn() noexcept = default;

    _Blake3_stream_fn(const _Blake3_stream_fn&) = delete;
    _Blake3_stream_fn& operator=(const _Blake3_stream_fn&) = delete;

    // tries to invoke the blake3_hasher_update() function
    _NODISCARD constexpr bool operator()(
        blake3_hasher* const _Hasher, byte_type* const _Buf, const size_type _Buf_size,
        const char_type* const _Data, const size_type _Data_size) const noexcept;
};

// CLASS TEMPLATE _Blake3_default_engine
template <class _Elem>
class _Blake3_default_engine { // default engine for the BLAKE3 algorithm
private:
    static_assert(is_any_of_v<_Elem, char, wchar_t>, "Requires UTF-8/UTF-16 element type.");

    using _Simple_types = _Simple_hash_types<_Elem, _Blake3_default_engine<_Elem>>;
    using _Fn           = _Blake3_fn<_Simple_types>;

public:
    using char_type       = typename _Simple_types::char_type;
    using byte_type       = typename _Simple_types::byte_type;
    using char_string     = typename _Simple_types::char_string;
    using byte_string     = typename _Simple_types::byte_string;
    using size_type       = typename _Simple_types::size_type;
    using difference_type = typename _Simple_types::difference_type;
    using engine          = typename _Simple_types::engine;

    // always a 256-bit (32-byte) ASCII string
    static constexpr size_type size = 256 / CHAR_BIT;

    constexpr _Blake3_default_engine(const _Elem* const _Data, const size_type _Size) noexcept;
    _CONSTEXPR20 ~_Blake3_default_engine() noexcept;

    _Blake3_default_engine() = delete;
    _Blake3_default_engine(const _Blake3_default_engine&) = delete;
    _Blake3_default_engine& operator=(const _Blake3_default_engine&) = delete;

    // returns hashed data or an empty string
    _NODISCARD constexpr byte_string bytes() const;

private:
    _Ebco_pair<_Sbo_buffer<byte_type>, _Fn> _Mypair;
};

// CLASS _Blake3_stream_default_engine
class _Blake3_stream_default_engine { // default engine for the stream BLAKE3 algorithm
private:
    using _Simple_types = _Simple_hash_types<char, _Blake3_stream_default_engine>;
    using _Fn           = _Blake3_stream_fn<_Simple_types>;

public:
    using char_type       = typename _Simple_types::char_type;
    using byte_type       = typename _Simple_types::byte_type;
    using char_string     = typename _Simple_types::char_string;
    using byte_string     = typename _Simple_types::byte_string;
    using size_type       = typename _Simple_types::size_type;
    using difference_type = typename _Simple_types::difference_type;
    using engine          = typename _Simple_types::engine;

    // always a 256-bit (32-byte) ASCII string
    static constexpr size_type size = 256 / CHAR_BIT;

    _Blake3_stream_default_engine() noexcept;
    ~_Blake3_stream_default_engine() noexcept;

    _Blake3_stream_default_engine(const _Blake3_stream_default_engine&) = delete;
    _Blake3_stream_default_engine& operator=(const _Blake3_stream_default_engine&) = delete;

    // hashes the next chunk
    _NODISCARD bool next_chunk(const char_type* const _Chunk, const size_type _Count) noexcept;

    // returns hashed data or an empty string
    _NODISCARD byte_string bytes();

private:
    _Ebco_pair<_Sbo_buffer<byte_type>, _Fn> _Mypair;
    blake3_hasher* _Myhasher;
};

// FUNCTION TEMPLATE blake3
template <class _Elem, class _Eng = _Blake3_default_engine<_Elem>>
_SDSDLL_API _NODISCARD constexpr byte_string blake3(const _Elem* const _Data, const size_t _Count);

template <class _Elem, class _Eng = _Blake3_default_engine<_Elem>>
_SDSDLL_API _NODISCARD constexpr byte_string blake3(const _Elem* const _Data);

template <class _Elem, class _Eng = _Blake3_default_engine<_Elem>>
_SDSDLL_API _NODISCARD constexpr byte_string blake3(const basic_string_view<_Elem> _Data);

template <class _Elem, class _Eng = _Blake3_default_engine<_Elem>>
_SDSDLL_API _NODISCARD constexpr byte_string blake3(const basic_string<_Elem>& _Data);

// FUNCTION TEMPLATE blake3_file
template <class _Eng = _Blake3_stream_default_engine>
_SDSDLL_API _NODISCARD constexpr byte_string blake3_file(file& _File, const size_t _Off = 0);

template <class _Eng = _Blake3_stream_default_engine>
_SDSDLL_API _NODISCARD constexpr byte_string blake3_file(const path& _Target, const size_t _Off = 0);
_SDSDLL_END

#endif // _SDSDLL_PREPROCESSOR_GUARD
#endif // _SDSDLL_CRYPTOGRAPHY_HASH_GENERIC_BLAKE3_HPP_