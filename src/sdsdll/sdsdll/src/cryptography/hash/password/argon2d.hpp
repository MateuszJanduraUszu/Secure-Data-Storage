// argon2d.hpp

// Copyright (c) Mateusz Jandura. All rights reserved.
// SPDX-License-Identifier: Apache-2.0

#pragma once
#ifndef _SDSDLL_CRYPTOGRAPHY_HASH_PASSWORD_ARGON2D_HPP_
#define _SDSDLL_CRYPTOGRAPHY_HASH_PASSWORD_ARGON2D_HPP_
#include <core/defs.hpp>
#if _SDSDLL_PREPROCESSOR_GUARD
#include <botan/argon2.h>
#include <core/api.hpp>
#include <core/optimization/ebco.hpp>
#include <core/optimization/sbo.hpp>
#include <core/optimization/string_view.hpp>
#include <core/traits/concepts.hpp>
#include <core/traits/string_traits.hpp>
#include <cryptography/hash/hash_types.hpp>
#include <cstddef>
#include <cstdint>
#include <encoding/utf8.hpp>
#include <string>

// STD types
using _STD basic_string;
using _STD string;
using _STD wstring;

_SDSDLL_BEGIN
// CLASS TEMPLATE _Argon2d_fn
template <class _Simple_types>
class _Argon2d_fn { // wrapper for the Botan::argon2() function
public:
    using char_type       = typename _Simple_types::char_type;
    using byte_type       = typename _Simple_types::byte_type;
    using char_string     = typename _Simple_types::char_string;
    using byte_string     = typename _Simple_types::byte_string;
    using size_type       = typename _Simple_types::size_type;
    using difference_type = typename _Simple_types::difference_type;
    using engine          = typename _Simple_types::engine;

    constexpr _Argon2d_fn() noexcept     = default;
    _CONSTEXPR20 ~_Argon2d_fn() noexcept = default;

    _Argon2d_fn(const _Argon2d_fn&) = delete;
    _Argon2d_fn& operator=(const _Argon2d_fn&) = delete;

    // tries to invoke the Botan::argon2() function
    _NODISCARD constexpr bool operator()(byte_type* const _Buf, const size_type _Buf_size,
        const char* const _Data, const size_type _Data_size, const byte_type* const _Salt,
        const size_type _Salt_size, const size_type _Memory_amount,
        const size_type _Iterations, const size_type _Parallelism) const noexcept;
};

// CLASS TEMPLATE _Argon2d_default_engine
template <class _Elem>
class _Argon2d_default_engine { // default engine for the Argon2d algorithm
private:
    static_assert(is_any_of_v<_Elem, char, wchar_t>, "Required UTF-8/UTF-16 element type.");

    using _Simple_types = _Simple_hash_types<_Elem, _Argon2d_default_engine<_Elem>>;
    using _Fn           = _Argon2d_fn<_Simple_types>;

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

    constexpr _Argon2d_default_engine(const _Elem* const _Data, const size_type _Size,
        const byte_type* const _Salt, const size_type _Memory_amount,
        const size_type _Iterations, const size_type _Parallelism) noexcept;
    _CONSTEXPR20 ~_Argon2d_default_engine() noexcept;

    _Argon2d_default_engine() = delete;
    _Argon2d_default_engine(const _Argon2d_default_engine&) = delete;
    _Argon2d_default_engine& operator=(const _Argon2d_default_engine&) = delete;

    // returns hashed data or an empty string
    _NODISCARD constexpr byte_string bytes() const;

private:
    _Ebco_pair<_Sbo_buffer<byte_type>, _Fn> _Mypair;
};

// CLASS TEMPLATE salt
template <class>
class salt;

// FUNCTION TEMPLATE argon2d
template <class _Elem, class _Eng = _Argon2d_default_engine<_Elem>>
_SDSDLL_API _NODISCARD constexpr byte_string argon2d(const _Elem* const _Data, const size_t _Size,
    const salt<_Eng>& _Salt, const size_t _Memory_amount = 16384, const size_t _Iterations = 8,
    const size_t _Parallelism = 1);

template <class _Elem, class _Eng = _Argon2d_default_engine<_Elem>>
_SDSDLL_API _NODISCARD constexpr byte_string argon2d(const _Elem* const _Data, const salt<_Eng>& _Salt,
    const size_t _Memory_amount = 16384, const size_t _Iterations = 8, const size_t _Parallelism = 1);

template <class _Elem, class _Eng = _Argon2d_default_engine<_Elem>>
_SDSDLL_API _NODISCARD constexpr byte_string argon2d(
    const basic_string_view<_Elem> _Data, const salt<_Eng>& _Salt, const size_t _Memory_amount = 16384,
    const size_t _Iterations = 8, const size_t _Parallelism = 1);

template <class _Elem, class _Eng = _Argon2d_default_engine<_Elem>>
_SDSDLL_API _NODISCARD constexpr byte_string argon2d(
    const basic_string<_Elem>& _Data, const salt<_Eng>& _Salt, const size_t _Memory_amount = 16384,
    const size_t _Iterations = 8, const size_t _Parallelism = 1);
_SDSDLL_END

#endif // _SDSDLL_PREPROCESSOR_GUARD
#endif // _SDSDLL_CRYPTOGRAPHY_HASH_PASSWORD_ARGON2D_HPP_