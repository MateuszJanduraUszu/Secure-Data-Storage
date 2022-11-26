// symmetric_key.cpp

// Copyright (c) Mateusz Jandura. All rights reserved.
// SPDX-License-Identifier: Apache-2.0

#include <build/sdsdll_pch.hpp>
#include <cryptography/cipher/symmetric/symmetric_key.hpp>
#if _SDSDLL_PREPROCESSOR_GUARD

_SDSDLL_BEGIN
// FUNCTION TEMPLATE symmetric_key constructors/destructor
template <size_t _Size>
constexpr symmetric_key<_Size>::symmetric_key() noexcept : _Mybase() {}

template <size_t _Size>
constexpr symmetric_key<_Size>::symmetric_key(const symmetric_key& _Other) noexcept
    : _Mybase(_Other) {}

template <size_t _Size>
constexpr symmetric_key<_Size>::symmetric_key(symmetric_key&& _Other) noexcept
    : _Mybase(_STD move(_Other)) {}

template <size_t _Size>
constexpr symmetric_key<_Size>::symmetric_key(const value_type* const _Ptr) noexcept
    : _Mybase(_Ptr) {}

template <size_t _Size>
_CONSTEXPR20 symmetric_key<_Size>::~symmetric_key() noexcept {}

// FUNCTION TEMPLATE symmetric_key::operator=
template <size_t _Size>
constexpr symmetric_key<_Size>&
    symmetric_key<_Size>::operator=(const symmetric_key& _Other) noexcept {
    _Mybase::operator=(_Other);
    return *this;
}

template <size_t _Size>
constexpr symmetric_key<_Size>&
    symmetric_key<_Size>::operator=(symmetric_key&& _Other) noexcept {
    _Mybase::operator=(_STD move(_Other));
    return *this;
}

template <size_t _Size>
constexpr symmetric_key<_Size>&
    symmetric_key<_Size>::operator=(const value_type* const _Ptr) noexcept {
    _Mybase::operator=(_Ptr);
    return *this;
}

template class _SDSDLL_API symmetric_key<32>;

// FUNCTION TEMPLATE operator==
template <size_t _Size>
_NODISCARD constexpr bool operator==(
    const symmetric_key<_Size>& _Left, const symmetric_key<_Size>& _Right) noexcept {
    using _Key_t  = symmetric_key<_Size>;
    using _Traits = string_traits<typename _Key_t::value_type, typename _Key_t::size_type>;
    return _Traits::compare(_Left.get(), _Right.get(), _Key_t::size) == 0;
}

template _SDSDLL_API _NODISCARD bool operator==(
    const symmetric_key<32>&, const symmetric_key<32>&) noexcept;

// FUNCTION TEMPLATE operator!=
template <size_t _Size>
_NODISCARD constexpr bool operator!=(
    const symmetric_key<_Size>& _Left, const symmetric_key<_Size>& _Right) noexcept {
    using _Key_t  = symmetric_key<_Size>;
    using _Traits = string_traits<typename _Key_t::value_type, typename _Key_t::size_type>;
    return _Traits::compare(_Left.get(), _Right.get(), _Key_t::size) != 0;
}

template _SDSDLL_API _NODISCARD bool operator!=(
    const symmetric_key<32>&, const symmetric_key<32>&) noexcept;

// FUNCTION TEMPLATE make_symmetric_key
template <size_t _Size>
_NODISCARD constexpr symmetric_key<_Size> make_symmetric_key() noexcept {
    using _Key_t   = symmetric_key<_Size>;
    using _Value_t = typename _Key_t::value_type;
    _Value_t _Buf[_Key_t::size];
    ::RAND_bytes(_Buf, static_cast<int>(_Key_t::size));
    return _Key_t{_Buf};
}

template _SDSDLL_API _NODISCARD symmetric_key<32> make_symmetric_key() noexcept;
_SDSDLL_END

#endif // _SDSDLL_PREPROCESSOR_GUARD