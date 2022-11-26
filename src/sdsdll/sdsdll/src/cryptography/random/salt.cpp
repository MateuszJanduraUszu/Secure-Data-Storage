// salt.cpp

// Copyright (c) Mateusz Jandura. All rights reserved.
// SPDX-License-Identifier: Apache-2.0

#include <build/sdsdll_pch.hpp>
#include <cryptography/random/salt.hpp>
#if _SDSDLL_PREPROCESSOR_GUARD

_SDSDLL_BEGIN
// FUNCTION TEMPLATE salt constructors/destructor
template <size_t _Size>
constexpr salt<_Size>::salt() noexcept : _Mybase() {}

template <size_t _Size>
constexpr salt<_Size>::salt(const salt& _Other) noexcept : _Mybase(_Other) {}

template <size_t _Size>
constexpr salt<_Size>::salt(salt&& _Other) noexcept : _Mybase(_STD move(_Other)) {}

template <size_t _Size>
constexpr salt<_Size>::salt(const value_type* const _Ptr) noexcept : _Mybase(_Ptr) {}

template <size_t _Size>
_CONSTEXPR20 salt<_Size>::~salt() noexcept {}

// FUNCTION TEMPLATE salt::operator=
template <size_t _Size>
constexpr salt<_Size>& salt<_Size>::operator=(const salt& _Other) noexcept {
    _Mybase::operator=(_Other);
    return *this;
}

template <size_t _Size>
constexpr salt<_Size>& salt<_Size>::operator=(salt&& _Other) noexcept {
    _Mybase::operator=(_STD move(_Other));
    return *this;
}

template <size_t _Size>
constexpr salt<_Size>& salt<_Size>::operator=(const value_type* const _Ptr) noexcept {
    _Mybase::operator=(_Ptr);
    return *this;
}

template class _SDSDLL_API salt<16>;

// FUNCTION TEMPLATE operator==
template <size_t _Size>
_NODISCARD constexpr bool operator==(const salt<_Size>& _Left, const salt<_Size>& _Right) noexcept {
    using _Salt_t = salt<_Size>;
    return memory_traits::compare(
        _Left.get(), _Right.get(), _Salt_t::size * sizeof(typename _Salt_t::value_type)) == 0;
}

template _SDSDLL_API _NODISCARD bool operator==(const salt<16>&, const salt<16>&) noexcept;

// FUNCTION TEMPLATE operator!=
template <size_t _Size>
_NODISCARD constexpr bool operator!=(const salt<_Size>& _Left, const salt<_Size>& _Right) noexcept {
    using _Salt_t = salt<_Size>;
    return memory_traits::compare(
        _Left.get(), _Right.get(), _Salt_t::size * sizeof(typename _Salt_t::value_type)) != 0;
}

template _SDSDLL_API _NODISCARD bool operator!=(const salt<16>&, const salt<16>&) noexcept;

// FUNCTION TEMPLATE make_salt
template <size_t _Size>
_NODISCARD constexpr salt<_Size> make_salt() noexcept {
    using _Salt_t  = salt<_Size>;
    using _Value_t = typename _Salt_t::value_type;
    _Value_t _Buf[_Salt_t::size];
    ::RAND_bytes(_Buf, static_cast<int>(_Salt_t::size));
    return _Salt_t{_Buf};
}

template _SDSDLL_API _NODISCARD salt<16> make_salt() noexcept;
_SDSDLL_END

#endif // _SDSDLL_PREPROCESSOR_GUARD