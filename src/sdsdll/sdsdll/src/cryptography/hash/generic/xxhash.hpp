// xxhash.hpp

// Copyright (c) Mateusz Jandura. All rights reserved.
// SPDX-License-Identifier: Apache-2.0

#pragma once
#ifndef _SDSDLL_CRYPTOGRAPHY_HASH_GENERIC_XXHASH_HPP_
#define _SDSDLL_CRYPTOGRAPHY_HASH_GENERIC_XXHASH_HPP_
#include <core/defs.hpp>
#if _SDSDLL_PREPROCESSOR_GUARD
#include <climits>
#include <core/api.hpp>
#include <core/memory/allocator.hpp>
#include <core/optimization/ebco.hpp>
#include <core/optimization/sbo.hpp>
#include <core/optimization/string_view.hpp>
#include <core/traits/concepts.hpp>
#include <core/traits/integer.hpp>
#include <core/traits/memory_traits.hpp>
#include <core/traits/string_traits.hpp>
#include <core/traits/type_traits.hpp>
#include <cryptography/hash/hash_types.hpp>
#include <cstddef>
#include <cstdint>
#include <filesystem/file.hpp>
#include <filesystem/path.hpp>
#include <string>
#include <xxhash.h>

// STD types
using _STD basic_string;
using _STD string;
using _STD wstring;

_SDSDLL_BEGIN
// CLASS TEMPLATE _Xxhash_fn
template <class _Simple_types>
class _Xxhash_fn { // wrapper for the XXH3_64bits() function
public:
    using char_type       = typename _Simple_types::char_type;
    using byte_type       = typename _Simple_types::byte_type;
    using char_string     = typename _Simple_types::char_string;
    using byte_string     = typename _Simple_types::byte_string;
    using size_type       = typename _Simple_types::size_type;
    using difference_type = typename _Simple_types::difference_type;
    using engine          = typename _Simple_types::engine;

    constexpr _Xxhash_fn() noexcept     = default;
    _CONSTEXPR20 ~_Xxhash_fn() noexcept = default;

    _Xxhash_fn(const _Xxhash_fn&) = delete;
    _Xxhash_fn& operator=(const _Xxhash_fn&) = delete;

    // tries to invoke the XXH3_64bits() function
    _NODISCARD constexpr bool operator()(byte_type* const _Buf, const size_type _Buf_size,
        const char_type* const _Data, const size_type _Data_size) const noexcept;
};

// CLASS TEMPLATE _Xxhash_stream_fn
template <class _Simple_types>
class _Xxhash_stream_fn { // wrapper for the XXH3_64bits_update() function
public:
    using char_type       = typename _Simple_types::char_type;
    using byte_type       = typename _Simple_types::byte_type;
    using char_string     = typename _Simple_types::char_string;
    using byte_string     = typename _Simple_types::byte_string;
    using size_type       = typename _Simple_types::size_type;
    using difference_type = typename _Simple_types::difference_type;
    using engine          = typename _Simple_types::engine;

    constexpr _Xxhash_stream_fn() noexcept     = default;
    _CONSTEXPR20 ~_Xxhash_stream_fn() noexcept = default;

    _Xxhash_stream_fn(const _Xxhash_stream_fn&) = delete;
    _Xxhash_stream_fn& operator=(const _Xxhash_stream_fn&) = delete;

    // tries to invoke the XXH3_64bits_update() function
    _NODISCARD constexpr bool operator()(XXH3_state_t* const _State,
        const char_type* const _Data, const size_type _Data_size) const noexcept;
};

// CLASS TEMPLATE _Xxhash_default_engine
template <class _Elem>
class _Xxhash_default_engine { // default engine for the xxHash algorithm
private:
    static_assert(is_any_of_v<_Elem, char, wchar_t>, "Requires UTF-8/UTF-16 element type.");

    using _Simple_types = _Simple_hash_types<_Elem, _Xxhash_default_engine<_Elem>>;
    using _Fn           = _Xxhash_fn<_Simple_types>;

public:
    using char_type       = typename _Simple_types::char_type;
    using byte_type       = typename _Simple_types::byte_type;
    using char_string     = typename _Simple_types::char_string;
    using byte_string     = typename _Simple_types::byte_string;
    using size_type       = typename _Simple_types::size_type;
    using difference_type = typename _Simple_types::difference_type;
    using engine          = typename _Simple_types::engine;

    // always a 64-bit (8-byte) ASCII string
    static constexpr size_type size = 64 / CHAR_BIT;

    constexpr _Xxhash_default_engine(const _Elem* const _Data, const size_type _Size) noexcept;
    _CONSTEXPR20 ~_Xxhash_default_engine() noexcept;

    _Xxhash_default_engine() = delete;
    _Xxhash_default_engine(const _Xxhash_default_engine&) = delete;
    _Xxhash_default_engine& operator=(const _Xxhash_default_engine&) = delete;
    
    // returns hashed data or an empty string
    _NODISCARD constexpr byte_string bytes() const;

private:
    _Ebco_pair<_Sbo_buffer<byte_type>, _Fn> _Mypair;
};

// CLASS _Xxhash_stream_default_engine
class _Xxhash_stream_default_engine { // default engine for the stream xxHash algorithm
private:
    using _Simple_types = _Simple_hash_types<char, _Xxhash_stream_default_engine>;
    using _Fn           = _Xxhash_stream_fn<_Simple_types>;

public:
    using char_type       = typename _Simple_types::char_type;
    using byte_type       = typename _Simple_types::byte_type;
    using char_string     = typename _Simple_types::char_string;
    using byte_string     = typename _Simple_types::byte_string;
    using size_type       = typename _Simple_types::size_type;
    using difference_type = typename _Simple_types::difference_type;
    using engine          = typename _Simple_types::engine;

    // always a 64-bit (8-byte) ASCII string
    static constexpr size_type size = 64 / CHAR_BIT;

    _Xxhash_stream_default_engine() noexcept;
    ~_Xxhash_stream_default_engine() noexcept;

    _Xxhash_stream_default_engine(const _Xxhash_stream_default_engine&) = delete;
    _Xxhash_stream_default_engine& operator=(const _Xxhash_stream_default_engine&) = delete;

    // hashes the next chunk
    _NODISCARD bool next_chunk(const char_type* const _Data, const size_type _Count) noexcept;

    // returns hashed data or an empty string
    _NODISCARD byte_string bytes();

private:
    _Fn _Myfunc;
    XXH3_state_t* _Mystate;
};

// FUNCTION TEMPLATE xxhash
template <class _Elem, class _Eng = _Xxhash_default_engine<_Elem>>
_SDSDLL_API _NODISCARD constexpr byte_string xxhash(const _Elem* const _Data, const size_t _Count);

template <class _Elem, class _Eng = _Xxhash_default_engine<_Elem>>
_SDSDLL_API _NODISCARD constexpr byte_string xxhash(const _Elem* const _Data);

template <class _Elem, class _Eng = _Xxhash_default_engine<_Elem>>
_SDSDLL_API _NODISCARD constexpr byte_string xxhash(const basic_string_view<_Elem> _Data);

template <class _Elem, class _Eng = _Xxhash_default_engine<_Elem>>
_SDSDLL_API _NODISCARD constexpr byte_string xxhash(const basic_string<_Elem>& _Data);

// FUNCTION TEMPLATE xxhash_file
template <class _Eng = _Xxhash_stream_default_engine>
_SDSDLL_API _NODISCARD constexpr byte_string xxhash_file(file& _File, const size_t _Off = 0);

template <class _Eng = _Xxhash_stream_default_engine>
_SDSDLL_API _NODISCARD constexpr byte_string xxhash_file(const path& _Target, const size_t _Off = 0);
_SDSDLL_END

#endif // _SDSDLL_PREPROCESSOR_GUARD
#endif // _SDSDLL_CRYPTOGRAPHY_HASH_GENERIC_XXHASH_HPP_