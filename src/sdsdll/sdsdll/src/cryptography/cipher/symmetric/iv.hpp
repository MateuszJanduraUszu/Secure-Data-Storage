// iv.hpp

// Copyright (c) Mateusz Jandura. All rights reserved.
// SPDX-License-Identifier: Apache-2.0

#pragma once
#ifndef _SDSDLL_CRYPTOGRAPHY_CIPHER_SYMMETRIC_IV_HPP_
#define _SDSDLL_CRYPTOGRAPHY_CIPHER_SYMMETRIC_IV_HPP_
#include <core/defs.hpp>
#if _SDSDLL_PREPROCESSOR_GUARD
#include <climits>
#include <core/api.hpp>
#include <core/container/bytes.hpp>
#include <core/traits/memory_traits.hpp>
#include <core/traits/type_traits.hpp>
#include <cstddef>
#include <openssl/rand.h>
#include <type_traits>

_SDSDLL_BEGIN
// CLASS TEMPLATE iv
template <size_t _Size>
class iv : public _Bytes_container<_Size> { // Initialization Vector (16 bytes by default)
private:
    using _Mybase = _Bytes_container<_Size>;

public:
    using value_type      = typename _Mybase::value_type;
    using size_type       = typename _Mybase::size_type;
    using difference_type = typename _Mybase::difference_type;
    using pointer         = typename _Mybase::pointer;
    using const_pointer   = typename _Mybase::const_pointer;

    constexpr iv() noexcept : _Mybase() {}

    constexpr iv(const iv& _Other) noexcept : _Mybase(_Other) {}
    
    constexpr iv(iv&& _Other) noexcept : _Mybase(_STD move(_Other)) {}

    _CONSTEXPR20 ~iv() noexcept {}

    explicit constexpr iv(const value_type* const _Ptr) noexcept : _Mybase(_Ptr) {}

    constexpr iv& operator=(const iv& _Other) noexcept {
        _Mybase::operator=(_Other);
        return *this;
    }

    constexpr iv& operator=(iv&& _Other) noexcept {
        _Mybase::operator=(_STD move(_Other));
        return *this;
    }

    constexpr iv& operator=(const value_type* const _Ptr) noexcept {
        _Mybase::operator=(_Ptr);
        return *this;
    }
};

// FUNCTION TEMPLATE operator==
template <size_t _Size>
_NODISCARD constexpr bool operator==(const iv<_Size>& _Left, const iv<_Size>& _Right) noexcept {
    using _Iv_t = iv<_Size>;
    return memory_traits::compare(
        _Left.get(), _Right.get(), _Iv_t::size * sizeof(typename _Iv_t::value_type)) == 0;
}

// FUNCTION TEMPLATE operator!=
template <size_t _Size>
_NODISCARD constexpr bool operator!=(const iv<_Size>& _Left, const iv<_Size>& _Right) noexcept {
    using _Iv_t = iv<_Size>;
    return memory_traits::compare(
        _Left.get(), _Right.get(), _Iv_t::size * sizeof(typename _Iv_t::value_type)) != 0;
}

// FUNCTION TEMPLATE make_iv
template <size_t _Size>
_NODISCARD constexpr iv<_Size> make_iv() noexcept {
    using _Iv_t = iv<_Size>;
    typename _Iv_t::value_type _Buf[_Iv_t::size];
    ::RAND_bytes(_Buf, static_cast<int>(_Iv_t::size));
    return _Iv_t{_Buf};
}
_SDSDLL_END

#endif // _SDSDLL_PREPROCESSOR_GUARD
#endif // _SDSDLL_CRYPTOGRAPHY_CIPHER_SYMMETRIC_IV_HPP_