// iv.cpp

// Copyright (c) Mateusz Jandura. All rights reserved.
// SPDX-License-Identifier: Apache-2.0

#include <build/sdsdll_pch.hpp>
#include <cryptography/cipher/symmetric/iv.hpp>
#if _SDSDLL_PREPROCESSOR_GUARD

_SDSDLL_BEGIN
// FUNCTION TEMPLATE iv constructors/destructor
template <size_t _Size>
constexpr iv<_Size>::iv() noexcept : _Mybase() {}

template <size_t _Size>
constexpr iv<_Size>::iv(const iv& _Other) noexcept : _Mybase(_Other) {}

template <size_t _Size>
constexpr iv<_Size>::iv(iv&& _Other) noexcept : _Mybase(_STD move(_Other)) {}

template <size_t _Size>
constexpr iv<_Size>::iv(const value_type* const _Ptr) noexcept : _Mybase(_Ptr) {}

template <size_t _Size>
_CONSTEXPR20 iv<_Size>::~iv() noexcept {}

// FUNCTION TEMPLATE iv::operator=
template <size_t _Size>
constexpr iv<_Size>& iv<_Size>::operator=(const iv& _Other) noexcept {
    _Mybase::operator=(_Other);
    return *this;
}

template <size_t _Size>
constexpr iv<_Size>& iv<_Size>::operator=(iv&& _Other) noexcept {
    _Mybase::operator=(_STD move(_Other));
    return *this;
}

template <size_t _Size>
constexpr iv<_Size>& iv<_Size>::operator=(const value_type* const _Ptr) noexcept {
    _Mybase::operator=(_Ptr);
    return *this;
}

template class _SDSDLL_API iv<12>;

// FUNCTION TEMPLATE operator==
template <size_t _Size>
_NODISCARD constexpr bool operator==(const iv<_Size>& _Left, const iv<_Size>& _Right) noexcept {
    using _Iv_t   = iv<_Size>;
    using _Traits = string_traits<typename _Iv_t::value_type, typename _Iv_t::size_type>;
    return _Traits::compare(_Left.get(), _Right.get(), _Iv_t::size) == 0;
}

template _SDSDLL_API _NODISCARD bool operator==(const iv<12>&, const iv<12>&) noexcept;

// FUNCTION TEMPLATE operator!=
template <size_t _Size>
_NODISCARD constexpr bool operator!=(const iv<_Size>& _Left, const iv<_Size>& _Right) noexcept {
    using _Iv_t   = iv<_Size>;
    using _Traits = string_traits<typename _Iv_t::value_type, typename _Iv_t::size_type>;
    return _Traits::compare(_Left.get(), _Right.get(), _Iv_t::size) != 0;
}

template _SDSDLL_API _NODISCARD bool operator!=(const iv<12>&, const iv<12>&) noexcept;

// FUNCTION TEMPLATE make_iv
template <size_t _Size>
_NODISCARD constexpr iv<_Size> make_iv() noexcept {
    using _Iv_t    = iv<_Size>;
    using _Value_t = typename _Iv_t::value_type;
    _Value_t _Buf[_Iv_t::size];
    ::RAND_bytes(_Buf, static_cast<int>(_Iv_t::size));
    return _Iv_t{_Buf};
}

template _SDSDLL_API _NODISCARD iv<12> make_iv() noexcept;
_SDSDLL_END

#endif // _SDSDLL_PREPROCESSOR_GUARD