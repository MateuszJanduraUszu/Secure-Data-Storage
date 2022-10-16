// scrypt.hpp

// Copyright (c) Mateusz Jandura. All rights reserved.
// SPDX-License-Identifier: Apache-2.0

#pragma once
#ifndef _SDSDLL_CRYPTOGRAPHY_HASH_PASSWORD_SCRYPT_HPP_
#define _SDSDLL_CRYPTOGRAPHY_HASH_PASSWORD_SCRYPT_HPP_
#include <core/defs.hpp>
#if _SDSDLL_PREPROCESSOR_GUARD
#include <botan/scrypt.h>
#include <core/api.hpp>
#include <core/memory/allocator.hpp>
#include <core/optimization/ebco.hpp>
#include <core/optimization/sbo.hpp>
#include <core/optimization/string_view.hpp>
#include <core/traits/concepts.hpp>
#include <core/traits/string_traits.hpp>
#include <cryptography/hash/hash_types.hpp>
#include <cstddef>
#include <encoding/utf8.hpp>
#include <string>

// STD types
using _STD basic_string;
using _STD string;
using _STD wstring;

_SDSDLL_BEGIN
// CLASS TEMPLATE _Scrypt_fn
template <class _Simple_types>
class _Scrypt_fn { // wrapper for the Botan::scrypt() function
public:
    using char_type       = typename _Simple_types::char_type;
    using byte_type       = typename _Simple_types::byte_type;
    using char_string     = typename _Simple_types::char_string;
    using byte_string     = typename _Simple_types::byte_string;
    using size_type       = typename _Simple_types::size_type;
    using difference_type = typename _Simple_types::difference_type;
    using engine          = typename _Simple_types::engine;

    constexpr _Scrypt_fn() noexcept     = default;
    _CONSTEXPR20 ~_Scrypt_fn() noexcept = default;

    _Scrypt_fn(const _Scrypt_fn&) = delete;
    _Scrypt_fn& operator=(const _Scrypt_fn&) = delete;

    // tries to invoke the Botan::scrypt() function
    _NODISCARD constexpr bool operator()(byte_type* const _Buf, const size_type _Buf_size,
        const char* const _Data, const size_type _Data_size, const byte_type* const _Salt,
        const size_type _Salt_size, const size_type _Cost, const size_type _Block_size,
        const size_type _Parallelism) const noexcept;
};

// CLASS TEMPLATE _Scrypt_default_engine
template <class _Elem>
class _Scrypt_default_engine { // default engine for the Scrypt algorithm
private:
    static_assert(is_any_of_v<_Elem, char, wchar_t>, "Requires UTF-8/UTF-16 element type.");

    using _Simple_types = _Simple_hash_types<_Elem, _Scrypt_default_engine>;
    using _Fn           = _Scrypt_fn<_Simple_types>;

public:
    using char_type       = typename _Simple_types::char_type;
    using byte_type       = typename _Simple_types::byte_type;
    using char_string     = typename _Simple_types::char_string;
    using byte_string     = typename _Simple_types::byte_string;
    using size_type       = typename _Simple_types::size_type;
    using difference_type = typename _Simple_types::difference_type;
    using engine          = typename _Simple_types::engine;

    static constexpr size_type size      = 64;
    static constexpr size_type salt_size = 16;

    constexpr _Scrypt_default_engine(const _Elem* const _Data, const size_type _Size,
        const byte_type* const _Salt, const size_type _Cost, const size_type _Block_size,
        const size_type _Parallelism) noexcept;
    _CONSTEXPR20 ~_Scrypt_default_engine() noexcept;

    _Scrypt_default_engine() = delete;
    _Scrypt_default_engine(const _Scrypt_default_engine&) = delete;
    _Scrypt_default_engine& operator=(const _Scrypt_default_engine&) = delete;

    // returns hashed data or an empty string
    _NODISCARD constexpr byte_string bytes() const;

private:
    _Ebco_pair<_Sbo_buffer<byte_type>, _Fn> _Mypair;
};

// CLASS TEMPLATE salt
template <class>
class salt;

// FUNCTION TEMPLATE scrypt
template <class _Elem, class _Eng = _Scrypt_default_engine<_Elem>>
_SDSDLL_API _NODISCARD constexpr byte_string scrypt(
    const _Elem* const _Data, const size_t _Size, const salt<_Eng>& _Salt,
    const size_t _Cost = 16384, const size_t _Block_size = 16, const size_t _Parallelism = 1);

template <class _Elem, class _Eng = _Scrypt_default_engine<_Elem>>
_SDSDLL_API _NODISCARD constexpr byte_string scrypt(
    const _Elem* const _Data, const salt<_Eng>& _Salt, const size_t _Cost = 16384,
    const size_t _Block_size = 16, const size_t _Parallelism = 1);

template <class _Elem, class _Eng = _Scrypt_default_engine<_Elem>>
_SDSDLL_API _NODISCARD constexpr byte_string scrypt(
    const basic_string_view<_Elem> _Data, const salt<_Eng>& _Salt, const size_t _Cost = 16384,
    const size_t _Block_size = 16, const size_t _Parallelism = 1);

template <class _Elem, class _Eng = _Scrypt_default_engine<_Elem>>
_SDSDLL_API _NODISCARD constexpr byte_string scrypt(
    const basic_string<_Elem>& _Data, const salt<_Eng>& _Salt, const size_t _Cost = 16384,
    const size_t _Block_size = 16, const size_t _Parallelism = 1);
_SDSDLL_END

#endif // _SDSDLL_PREPROCESSOR_GUARD
#endif // _SDSDLL_CRYPTOGRAPHY_HASH_PASSWORD_SCRYPT_HPP_