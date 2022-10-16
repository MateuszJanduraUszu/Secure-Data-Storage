// sha512.hpp

// Copyright (c) Mateusz Jandura. All rights reserved.
// SPDX-License-Identifier: Apache-2.0

#pragma once
#ifndef _SDSDLL_CRYPTOGRAPHY_HASH_GENERIC_SHA512_HPP_
#define _SDSDLL_CRYPTOGRAPHY_HASH_GENERIC_SHA512_HPP_
#include <core/defs.hpp>
#if _SDSDLL_PREPROCESSOR_GUARD
#include <climits>
#include <core/api.hpp>
#include <core/optimization/ebco.hpp>
#include <core/optimization/sbo.hpp>
#include <core/optimization/string_view.hpp>
#include <core/traits/concepts.hpp>
#include <core/traits/string_traits.hpp>
#include <core/traits/type_traits.hpp>
#include <cryptography/hash/hash_types.hpp>
#include <filesystem/file.hpp>
#include <filesystem/path.hpp>
#include <openssl/evp.h>
#include <string>

// STD types
using _STD basic_string;
using _STD string;
using _STD wstring;

_SDSDLL_BEGIN
// STRUCT _Sha512_context_proxy
struct _Sha512_context_proxy {
    EVP_MD_CTX* _Ctx;

    _Sha512_context_proxy() noexcept;
    ~_Sha512_context_proxy() noexcept;

    _Sha512_context_proxy(const _Sha512_context_proxy&) = delete;
    _Sha512_context_proxy& operator=(const _Sha512_context_proxy&) = delete;
};

// CLASS TEMPLATE _Sha512_fn
template <class _Simple_types>
class _Sha512_fn { // wrapper for the EVP functions
public:
    using char_type       = typename _Simple_types::char_type;
    using byte_type       = typename _Simple_types::byte_type;
    using char_string     = typename _Simple_types::char_string;
    using byte_string     = typename _Simple_types::byte_string;
    using size_type       = typename _Simple_types::size_type;
    using difference_type = typename _Simple_types::difference_type;
    using engine          = typename _Simple_types::engine;

    constexpr _Sha512_fn() noexcept     = default;
    _CONSTEXPR20 ~_Sha512_fn() noexcept = default;

    _Sha512_fn(const _Sha512_fn&) = delete;
    _Sha512_fn& operator=(const _Sha512_fn&) = delete;

    // tries to invoke the EVP functions
    _NODISCARD constexpr bool operator()(byte_type* const _Buf, const size_type _Buf_size,
        const char_type* const _Data, const size_type _Data_size) const noexcept;
};

// CLASS TEMPLATE _Sha512_stream_fn
template <class _Simple_types>
class _Sha512_stream_fn { // wrapper for the EVP_DigestUpdate() function
public:
    using char_type       = typename _Simple_types::char_type;
    using byte_type       = typename _Simple_types::byte_type;
    using char_string     = typename _Simple_types::char_string;
    using byte_string     = typename _Simple_types::byte_string;
    using size_type       = typename _Simple_types::size_type;
    using difference_type = typename _Simple_types::difference_type;
    using engine          = typename _Simple_types::engine;

    constexpr _Sha512_stream_fn() noexcept     = default;
    _CONSTEXPR20 ~_Sha512_stream_fn() noexcept = default;

    _Sha512_stream_fn(const _Sha512_stream_fn&) = delete;
    _Sha512_stream_fn& operator=(const _Sha512_stream_fn&) = delete;

    // tries to invoke the EVP_DigestUpdate() function
    _NODISCARD constexpr bool operator()(
        EVP_MD_CTX* const _Ctx, byte_type* const _Buf, const size_type _Buf_size,
        const char_type* const _Data, const size_type _Data_size) const noexcept;
};

// CLASS TEMPLATE _Sha512_default_engine
template <class _Elem>
class _Sha512_default_engine { // default engine for the SHA-512 algorithm
private:
    static_assert(is_any_of_v<_Elem, char, wchar_t>, "Requires UTF-8/UTF-16 element type.");

    using _Simple_types = _Simple_hash_types<_Elem, _Sha512_default_engine<_Elem>>;
    using _Fn           = _Sha512_fn<_Simple_types>;

public:
    using char_type       = typename _Simple_types::char_type;
    using byte_type       = typename _Simple_types::byte_type;
    using char_string     = typename _Simple_types::char_string;
    using byte_string     = typename _Simple_types::byte_string;
    using size_type       = typename _Simple_types::size_type;
    using difference_type = typename _Simple_types::difference_type;
    using engine          = typename _Simple_types::engine;

    // always a 512-bit (64-byte) ASCII string
    static constexpr size_type size = 512 / CHAR_BIT;

    constexpr _Sha512_default_engine(const _Elem* const _Data, const size_type _Size) noexcept;
    _CONSTEXPR20 ~_Sha512_default_engine() noexcept;

    _Sha512_default_engine() = delete;
    _Sha512_default_engine(const _Sha512_default_engine&) = delete;
    _Sha512_default_engine& operator=(const _Sha512_default_engine&) = delete;

    // returns hashed data or an empty string
    _NODISCARD constexpr byte_string bytes() const;

private:
    _Ebco_pair<_Sbo_buffer<byte_type>, _Fn> _Mypair;
};

// CLASS _Sha512_stream_default_engine
class _Sha512_stream_default_engine { // default engine for the stream SHA-512 algorithm
private:
    using _Simple_types = _Simple_hash_types<char, _Sha512_stream_default_engine>;
    using _Fn           = _Sha512_stream_fn<_Simple_types>;

public:
    using char_type       = typename _Simple_types::char_type;
    using byte_type       = typename _Simple_types::byte_type;
    using char_string     = typename _Simple_types::char_string;
    using byte_string     = typename _Simple_types::byte_string;
    using size_type       = typename _Simple_types::size_type;
    using difference_type = typename _Simple_types::difference_type;
    using engine          = typename _Simple_types::engine;

    // always a 512-bit (64-byte) ASCII string
    static constexpr size_type size = 512 / CHAR_BIT;

    _Sha512_stream_default_engine() noexcept;
    ~_Sha512_stream_default_engine() noexcept;

    _Sha512_stream_default_engine(const _Sha512_stream_default_engine&) = delete;
    _Sha512_stream_default_engine& operator=(const _Sha512_stream_default_engine&) = delete;

    // hashes the next chunk
    _NODISCARD bool next_chunk(const char_type* const _Chunk, const size_type _Count) noexcept;

    // returns hashed data or an empty string
    _NODISCARD byte_string bytes();

private:
    _Ebco_pair<_Sbo_buffer<byte_type>, _Fn> _Mypair;
    _Sha512_context_proxy _Myproxy;
};

// FUNCTION TEMPLATE sha512
template <class _Elem, class _Eng = _Sha512_default_engine<_Elem>>
_SDSDLL_API _NODISCARD constexpr byte_string sha512(const _Elem* const _Data, const size_t _Count);

template <class _Elem, class _Eng = _Sha512_default_engine<_Elem>>
_SDSDLL_API _NODISCARD constexpr byte_string sha512(const _Elem* const _Data);

template <class _Elem, class _Eng = _Sha512_default_engine<_Elem>>
_SDSDLL_API _NODISCARD constexpr byte_string sha512(const basic_string_view<_Elem> _Data);

template <class _Elem, class _Eng = _Sha512_default_engine<_Elem>>
_SDSDLL_API _NODISCARD constexpr byte_string sha512(const basic_string<_Elem>& _Data);

// FUNCTION TEMPLATE sha512_file
template <class _Eng = _Sha512_stream_default_engine>
_SDSDLL_API _NODISCARD constexpr byte_string sha512_file(file& _File, const size_t _Off = 0);

template <class _Eng = _Sha512_stream_default_engine>
_SDSDLL_API _NODISCARD constexpr byte_string sha512_file(const path& _Target, const size_t _Off = 0);
_SDSDLL_END

#endif // _SDSDLL_PREPROCESSOR_GUARD
#endif // _SDSDLL_CRYPTOGRAPHY_HASH_GENERIC_SHA512_HPP_