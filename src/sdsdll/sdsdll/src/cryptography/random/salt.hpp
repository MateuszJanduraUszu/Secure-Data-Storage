// salt.hpp

// Copyright (c) Mateusz Jandura. All rights reserved.
// SPDX-License-Identifier: Apache-2.0

#pragma once
#ifndef _SDSDLL_CRYPTOGRAPHY_SALT_HPP_
#define _SDSDLL_CRYPTOGRAPHY_SALT_HPP_
#include <core/defs.hpp>
#if _SDSDLL_PREPROCESSOR_GUARD
#include <core/api.hpp>
#include <core/container/bytes.hpp>
#include <core/traits/memory_traits.hpp>
#include <core/traits/type_traits.hpp>
#include <cryptography/hash/password/argon2d.hpp>
#include <cryptography/hash/password/argon2i.hpp>
#include <cryptography/hash/password/argon2id.hpp>
#include <cryptography/hash/password/scrypt.hpp>
#include <openssl/rand.h>
#include <type_traits>

_SDSDLL_BEGIN
// CLASS TEMPLATE salt
template <class _Eng>
class _SDSDLL_API salt
    : public _Bytes_container<_Eng::salt_size> { // random bytes for Argon2d, Argon2i, Argon2id and Scrypt
private:
    using _Mybase = _Bytes_container<_Eng::salt_size>;

public:
    using value_type      = typename _Mybase::value_type;
    using size_type       = typename _Mybase::size_type;
    using difference_type = typename _Mybase::difference_type;
    using pointer         = typename _Mybase::pointer;
    using const_pointer   = typename _Mybase::const_pointer;
    using engine          = _Eng;

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
template <class _Eng>
_NODISCARD constexpr bool operator==(const salt<_Eng>& _Left, const salt<_Eng>& _Right) noexcept;

// FUNCTION TEMPLATE operator!=
template <class _Eng>
_NODISCARD constexpr bool operator!=(const salt<_Eng>& _Left, const salt<_Eng>& _Right) noexcept;
_SDSDLL_END

#endif // _SDSDLL_PREPROCESSOR_GUARD
#endif // _SDSDLL_CRYPTOGRAPHY_SALT_HPP_