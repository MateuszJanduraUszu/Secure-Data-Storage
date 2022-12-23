// asymmetric.hpp

// Copyright (c) Mateusz Jandura. All rights reserved.
// SPDX-License-Identifier: Apache-2.0

#pragma once
#ifndef _SDSDLL_CRYPTOGRAPHY_CIPHER_ASYMMETRIC_HPP_
#define _SDSDLL_CRYPTOGRAPHY_CIPHER_ASYMMETRIC_HPP_
#include <core/defs.hpp>
#if _SDSDLL_PREPROCESSOR_GUARD
#include <core/optimization/string_view.hpp>
#include <core/traits/string_traits.hpp>
#include <string>

// STD types
using _STD basic_string;

_SDSDLL_BEGIN
// FUNCTION TEMPLATE encrypt_asymmetric
template <class _Traits>
_NODISCARD constexpr typename _Traits::byte_string encrypt_asymmetric(
    const typename _Traits::char_type* const _Data, const typename _Traits::size_type _Size,
    const typename _Traits::key& _Key) {
    typename _Traits::byte_string _Buf(_Traits::bytes_count(_Size), typename _Traits::byte_type{0});
    return _Traits::encrypt(
        _Buf.data(), _Buf.size(), _Data, _Size, _Key) ? _Buf : typename _Traits::byte_string{};
}

template <class _Traits>
_NODISCARD constexpr typename _Traits::byte_string encrypt_asymmetric(
    const typename _Traits::char_type* const _Data, const typename _Traits::key& _Key) {
    using _Str_traits = string_traits<typename _Traits::char_type, typename _Traits::size_type>;
    return _SDSDLL encrypt_asymmetric<_Traits>(_Data, _Str_traits::length(_Data), _Key);
}

template <class _Traits>
_NODISCARD constexpr typename _Traits::byte_string encrypt_asymmetric(
    const basic_string_view<typename _Traits::char_type> _Data, const typename _Traits::key& _Key) {
    return _SDSDLL encrypt_asymmetric<_Traits>(_Data.data(), _Data.size(), _Key);
}

template <class _Traits>
_NODISCARD constexpr typename _Traits::byte_string encrypt_asymmetric(
    const basic_string<typename _Traits::char_type>& _Data, const typename _Traits::key& _Key) {
    return _SDSDLL encrypt_asymmetric<_Traits>(_Data.c_str(), _Data.size(), _Key);
}

// FUNCTION TEMPLATE decrypt_asymmetric
template <class _Traits>
_NODISCARD constexpr typename _Traits::char_string decrypt_asymmetric(
    const typename _Traits::byte_type* const _Data, const typename _Traits::size_type _Size,
    const typename _Traits::key& _Key) {
    typename _Traits::size_type _Buf_size = _Traits::chars_count(_Size);
    typename _Traits::char_string _Buf(_Buf_size, typename _Traits::char_type{0});
    if (!_Traits::decrypt(_Buf.data(), _Buf_size, _Data, _Size, _Key, &_Buf_size)) {
        return typename _Traits::char_string{};
    }

    _Buf.resize(_Buf_size);
    return _Buf;
}

template <class _Traits>
_NODISCARD constexpr typename _Traits::char_string decrypt_asymmetric(
    const typename _Traits::byte_type* const _Data, const typename _Traits::key& _Key) {
    using _Str_traits = string_traits<typename _Traits::byte_type, typename _Traits::size_type>;
    return _SDSDLL decrypt_asymmetric<_Traits>(_Data, _Str_traits::length(_Data), _Key);
}

template <class _Traits>
_NODISCARD constexpr typename _Traits::char_string decrypt_asymmetric(
    const basic_string_view<typename _Traits::byte_type> _Data, const typename _Traits::key& _Key) {
    return _SDSDLL decrypt_asymmetric<_Traits>(_Data.data(), _Data.size(), _Key);
}

template <class _Traits>
_NODISCARD constexpr typename _Traits::char_string decrypt_asymmetric(
    const basic_string<typename _Traits::byte_type>& _Data, const typename _Traits::key& _Key) {
    return _SDSDLL decrypt_asymmetric<_Traits>(_Data.c_str(), _Data.size(), _Key);
}
_SDSDLL_END

#endif // _SDSDLL_PREPROCESSOR_GUARD
#endif // _SDSDLL_CRYPTOGRAPHY_CIPHER_ASYMMETRIC_HPP_