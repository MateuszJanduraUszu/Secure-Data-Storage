// iv.hpp

// Copyright (c) Mateusz Jandura. All rights reserved.
// SPDX-License-Identifier: Apache-2.0

#pragma once
#ifndef _SDSDLL_CRYPTOGRAPHY_CIPHER_SYMMETRIC_IV_HPP_
#define _SDSDLL_CRYPTOGRAPHY_CIPHER_SYMMETRIC_IV_HPP_
#include <core/defs.hpp>
#if _SDSDLL_PREPROCESSOR_GUARD
#include <core/api.hpp>
#include <core/container/bytes.hpp>
#include <core/traits/string_traits.hpp>
#include <core/traits/type_traits.hpp>
#include <cstddef>
#include <openssl/rand.h>
#include <type_traits>

_SDSDLL_BEGIN
// CLASS TEMPLATE iv
template <size_t _Size>
class _SDSDLL_API iv : public _Bytes_container<_Size> { // Initialization Vector
private:
    using _Mybase = _Bytes_container<_Size>;

public:
    using value_type      = typename _Mybase::value_type;
    using size_type       = typename _Mybase::size_type;
    using difference_type = typename _Mybase::difference_type;
    using pointer         = typename _Mybase::pointer;
    using const_pointer   = typename _Mybase::const_pointer;

    constexpr iv() noexcept;
    constexpr iv(const iv& _Other) noexcept;
    constexpr iv(iv&& _Other) noexcept;
    _CONSTEXPR20 ~iv() noexcept;

    explicit constexpr iv(const value_type* const _Ptr) noexcept;

    constexpr iv& operator=(const iv& _Other) noexcept;
    constexpr iv& operator=(iv&& _Other) noexcept;
    constexpr iv& operator=(const value_type* const _Ptr) noexcept;
};

// FUNCTION TEMPLATE operator==
template <size_t _Size>
_NODISCARD constexpr bool operator==(const iv<_Size>& _Left, const iv<_Size>& _Right) noexcept;

// FUNCTION TEMPLATE operator!=
template <size_t _Size>
_NODISCARD constexpr bool operator!=(const iv<_Size>& _Left, const iv<_Size>& _Right) noexcept;

// FUNCTION TEMPLATE make_iv
template <size_t _Size>
_SDSDLL_API _NODISCARD constexpr iv<_Size> make_iv() noexcept;
_SDSDLL_END

#endif // _SDSDLL_PREPROCESSOR_GUARD
#endif // _SDSDLL_CRYPTOGRAPHY_CIPHER_SYMMETRIC_IV_HPP_