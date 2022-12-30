// salt.hpp

// Copyright (c) Mateusz Jandura. All rights reserved.
// SPDX-License-Identifier: Apache-2.0

#pragma once
#ifndef _SDSDLL_CRYPTOGRAPHY_RANDOM_SALT_HPP_
#define _SDSDLL_CRYPTOGRAPHY_RANDOM_SALT_HPP_
#include <core/defs.hpp>
#if _SDSDLL_PREPROCESSOR_GUARD
#include <core/api.hpp>
#include <core/container/bytes.hpp>
#include <core/traits/memory_traits.hpp>
#include <core/traits/type_traits.hpp>
#include <openssl/rand.h>
#include <type_traits>

_SDSDLL_BEGIN
// CLASS TEMPLATE salt
template <size_t _Size>
class _SDSDLL_API salt
    : public _Bytes_container<_Size> { // random bytes for Argon2d, Argon2i, Argon2id and Scrypt
private:
    using _Mybase = _Bytes_container<_Size>;

public:
    using value_type      = typename _Mybase::value_type;
    using size_type       = typename _Mybase::size_type;
    using difference_type = typename _Mybase::difference_type;
    using pointer         = typename _Mybase::pointer;
    using const_pointer   = typename _Mybase::const_pointer;

    constexpr salt() noexcept;
    constexpr salt(const salt& _Other) noexcept;
    constexpr salt(salt&& _Other) noexcept;
    _CONSTEXPR20 ~salt() noexcept;

    explicit constexpr salt(const value_type* const _Ptr) noexcept;

    constexpr salt& operator=(const salt& _Other) noexcept;
    constexpr salt& operator=(salt&& _Other) noexcept;
    constexpr salt& operator=(const value_type* const _Ptr) noexcept;
};

// FUNCTION TEMPLATE operator==
template <size_t _Size>
_NODISCARD constexpr bool operator==(const salt<_Size>& _Left, const salt<_Size>& _Right) noexcept;

// FUNCTION TEMPLATE operator!=
template <size_t _Size>
_NODISCARD constexpr bool operator!=(const salt<_Size>& _Left, const salt<_Size>& _Right) noexcept;

// FUNCTION TEMPLATE make_salt
template <size_t _Size>
_SDSDLL_API _NODISCARD constexpr salt<_Size> make_salt() noexcept;
_SDSDLL_END

#endif // _SDSDLL_PREPROCESSOR_GUARD
#endif // _SDSDLL_CRYPTOGRAPHY_RANDOM_SALT_HPP_