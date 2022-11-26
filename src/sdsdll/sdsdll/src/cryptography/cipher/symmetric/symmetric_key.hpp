// symmetric_key.hpp

// Copyright (c) Mateusz Jandura. All rights reserved.
// SPDX-License-Identifier: Apache-2.0

#pragma once
#ifndef _SDSDLL_CRYPTOGRAPHY_CIPHER_SYMMETRIC_KEY_HPP_
#define _SDSDLL_CRYPTOGRAPHY_CIPHER_SYMMETRIC_KEY_HPP_
#include <core/defs.hpp>
#if _SDSDLL_PREPROCESSOR_GUARD
#include <core/api.hpp>
#include <core/traits/string_traits.hpp>
#include <cstddef>
#include <openssl/rand.h>
#include <type_traits>

_SDSDLL_BEGIN
// CLASS TEMPLATE symmetric_key
template <size_t _Size>
class _SDSDLL_API symmetric_key : public _Bytes_container<_Size> { // stores a symmetric key
private:
    using _Mybase = _Bytes_container<_Size>;

public:
    using value_type      = typename _Mybase::value_type;
    using size_type       = typename _Mybase::size_type;
    using difference_type = typename _Mybase::difference_type;
    using pointer         = typename _Mybase::pointer;
    using const_pointer   = typename _Mybase::const_pointer;

    constexpr symmetric_key() noexcept;
    constexpr symmetric_key(const symmetric_key& _Other) noexcept;
    constexpr symmetric_key(symmetric_key&& _Other) noexcept;
    _CONSTEXPR20 ~symmetric_key() noexcept;

    explicit constexpr symmetric_key(const value_type* const _Ptr) noexcept;

    constexpr symmetric_key& operator=(const symmetric_key& _Other) noexcept;
    constexpr symmetric_key& operator=(symmetric_key&& _Other) noexcept;
    constexpr symmetric_key& operator=(const value_type* const _Ptr) noexcept;
};

// FUNCTION TEMPLATE operator==
template <size_t _Size>
_SDSDLL_API _NODISCARD constexpr bool operator==(
    const symmetric_key<_Size>& _Left, const symmetric_key<_Size>& _Right) noexcept;

// FUNCTION TEMPLATE operator!=
template <size_t _Size>
_SDSDLL_API _NODISCARD constexpr bool operator!=(
    const symmetric_key<_Size>& _Left, const symmetric_key<_Size>& _Right) noexcept;

// FUNCTION TEMPLATE make_symmetric_key
template <size_t _Size>
_SDSDLL_API _NODISCARD constexpr symmetric_key<_Size> make_symmetric_key() noexcept;
_SDSDLL_END

#endif // _SDSDLL_PREPROCESSOR_GUARD
#endif // _SDSDLL_CRYPTOGRAPHY_CIPHER_SYMMETRIC_KEY_HPP_