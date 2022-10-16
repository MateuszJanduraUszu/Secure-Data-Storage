// aes_key.cpp

// Copyright (c) Mateusz Jandura. All rights reserved.
// SPDX-License-Identifier: Apache-2.0

#include <build/sdsdll_pch.hpp>
#include <cryptography/cipher/symmetric/aes_key.hpp>
#if _SDSDLL_PREPROCESSOR_GUARD

_SDSDLL_BEGIN
// FUNCTION TEMPLATE aes_key constructors/destructor
template <size_t _Size>
constexpr aes_key<_Size>::aes_key() noexcept : _Mybase() {}

template <size_t _Size>
constexpr aes_key<_Size>::aes_key(const aes_key& _Other) noexcept : _Mybase(_Other) {}

template <size_t _Size>
constexpr aes_key<_Size>::aes_key(aes_key&& _Other) noexcept : _Mybase(_STD move(_Other)) {}

template <size_t _Size>
constexpr aes_key<_Size>::aes_key(const value_type* const _Ptr) noexcept : _Mybase(_Ptr) {}

template <size_t _Size>
_CONSTEXPR20 aes_key<_Size>::~aes_key() noexcept {}

// FUNCTION TEMPLATE aes_key::operator=
template <size_t _Size>
constexpr aes_key<_Size>& aes_key<_Size>::operator=(const aes_key& _Other) noexcept {
    _Mybase::operator=(_Other);
    return *this;
}

template <size_t _Size>
constexpr aes_key<_Size>& aes_key<_Size>::operator=(aes_key&& _Other) noexcept {
    _Mybase::operator=(_STD move(_Other));
    return *this;
}

template <size_t _Size>
constexpr aes_key<_Size>& aes_key<_Size>::operator=(const value_type* const _Ptr) noexcept {
    _Mybase::operator=(_Ptr);
    return *this;
}

template _SDSDLL_API class aes_key<16>;
template _SDSDLL_API class aes_key<24>;
template _SDSDLL_API class aes_key<32>;

// FUNCTION TEMPLATE operator==
template <size_t _Size>
_NODISCARD constexpr bool operator==(const aes_key<_Size>& _Left, const aes_key<_Size>& _Right) noexcept {
    using _Key_t = aes_key<_Size>;
    return memory_traits::compare(
        _Left.get(), _Right.get(), _Key_t::size * sizeof(typename _Key_t::value_type)) == 0;
}

template _SDSDLL_API _NODISCARD bool operator==(const aes_key<16>&, const aes_key<16>&) noexcept;
template _SDSDLL_API _NODISCARD bool operator==(const aes_key<24>&, const aes_key<24>&) noexcept;
template _SDSDLL_API _NODISCARD bool operator==(const aes_key<32>&, const aes_key<32>&) noexcept;

// FUNCTION TEMPLATE operator!=
template <size_t _Size>
_NODISCARD constexpr bool operator!=(const aes_key<_Size>& _Left, const aes_key<_Size>& _Right) noexcept {
    using _Key_t = aes_key<_Size>;
    return memory_traits::compare(
        _Left.get(), _Right.get(), _Key_t::size * sizeof(typename _Key_t::value_type)) != 0;
}

template _SDSDLL_API _NODISCARD bool operator!=(const aes_key<16>&, const aes_key<16>&) noexcept;
template _SDSDLL_API _NODISCARD bool operator!=(const aes_key<24>&, const aes_key<24>&) noexcept;
template _SDSDLL_API _NODISCARD bool operator!=(const aes_key<32>&, const aes_key<32>&) noexcept;

// FUNCTION TEMPLATE make_aes_key
template <size_t _Size>
_NODISCARD constexpr aes_key<_Size> make_aes_key() noexcept {
    using _Key_t   = aes_key<_Size>;
    using _Value_t = typename _Key_t::value_type;
    _Value_t _Buf[_Key_t::size];
    ::RAND_bytes(_Buf, static_cast<int>(_Key_t::size));
    return _Key_t{_Buf};
}

template _SDSDLL_API _NODISCARD aes_key<16> make_aes_key() noexcept;
template _SDSDLL_API _NODISCARD aes_key<24> make_aes_key() noexcept;
template _SDSDLL_API _NODISCARD aes_key<32> make_aes_key() noexcept;
_SDSDLL_END

#endif // _SDSDLL_PREPROCESSOR_GUARD