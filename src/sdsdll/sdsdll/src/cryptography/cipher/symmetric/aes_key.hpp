// aes_key.hpp

// Copyright (c) Mateusz Jandura. All rights reserved.
// SPDX-License-Identifier: Apache-2.0

#pragma once
#ifndef _SDSDLL_CRYPTOGRAPHY_CIPHER_SYMMETRIC_AES_KEY_HPP_
#define _SDSDLL_CRYPTOGRAPHY_CIPHER_SYMMETRIC_AES_KEY_HPP_
#include <core/defs.hpp>
#if _SDSDLL_PREPROCESSOR_GUARD
#include <climits>
#include <core/api.hpp>
#include <core/container/bytes.hpp>
#include <core/traits/concepts.hpp>
#include <core/traits/memory_traits.hpp>
#include <core/traits/type_traits.hpp>
#include <cstddef>
#include <openssl/rand.h>
#include <type_traits>

_SDSDLL_BEGIN
// STRUCT TEMPLATE _Is_standard_aes_key_size
template <size_t _Size>
struct _Is_standard_aes_key_size : bool_constant<_Size == 16 || _Size == 24 || _Size == 32> {};

// CLASS TEMPLATE aes_key
template <size_t _Size>
class _SDSDLL_API aes_key : public _Bytes_container<_Size> { // stores a 128/192/256-bit AES key
private:
    static_assert(_Is_standard_aes_key_size<_Size>::value, "Requires a 128/192/256-bit key.");

    using _Mybase = _Bytes_container<_Size>;

public:
    using value_type      = typename _Mybase::value_type;
    using size_type       = typename _Mybase::size_type;
    using difference_type = typename _Mybase::difference_type;
    using pointer         = typename _Mybase::pointer;
    using const_pointer   = typename _Mybase::const_pointer;

    constexpr aes_key() noexcept;
    constexpr aes_key(const aes_key& _Other) noexcept;
    constexpr aes_key(aes_key&& _Other) noexcept;
    _CONSTEXPR20 ~aes_key() noexcept;

    explicit constexpr aes_key(const value_type* const _Ptr) noexcept;

    constexpr aes_key& operator=(const aes_key& _Other) noexcept;
    constexpr aes_key& operator=(aes_key&& _Other) noexcept;
    constexpr aes_key& operator=(const value_type* const _Ptr) noexcept;
};

// FUNCTION TEMPLATE operator==
template <size_t _Size>
_SDSDLL_API _NODISCARD constexpr bool operator==(
    const aes_key<_Size>& _Left, const aes_key<_Size>& _Right) noexcept;

// FUNCTION TEMPLATE operator!=
template <size_t _Size>
_SDSDLL_API _NODISCARD constexpr bool operator!=(
    const aes_key<_Size>& _Left, const aes_key<_Size>& _Right) noexcept;

// FUNCTION TEMPLATE make_aes_key
template <size_t _Size>
_SDSDLL_API _NODISCARD constexpr aes_key<_Size> make_aes_key() noexcept;
_SDSDLL_END

#endif // _SDSDLL_PREPROCESSOR_GUARD
#endif // _SDSDLL_CRYPTOGRAPHY_CIPHER_SYMMETRIC_AES_KEY_HPP_