// generic.hpp

// Copyright (c) Mateusz Jandura. All rights reserved.
// SPDX-License-Identifier: Apache-2.0

#pragma once
#ifndef _SDSDLL_CRYPTOGRAPHY_HASH_GENERIC_HPP_
#define _SDSDLL_CRYPTOGRAPHY_HASH_GENERIC_HPP_
#include <core/defs.hpp>
#if _SDSDLL_PREPROCESSOR_GUARD
#include <core/optimization/string_view.hpp>
#include <core/traits/string_traits.hpp>
#include <filesystem/file.hpp>
#include <filesystem/path.hpp>
#include <string>

// STD types
using _STD basic_string;

_SDSDLL_BEGIN
// FUNCTION TEMPLATE hash
template <class _Traits>
_NODISCARD constexpr typename _Traits::byte_string hash(
    const typename _Traits::char_type* const _Data, const typename _Traits::size_type _Size) {
    typename _Traits::byte_string _Buf(_Traits::bytes_count(), typename _Traits::byte_type{0});
    return _Traits::hash(
        _Buf.data(), _Buf.size(), _Data, _Size) ? _Buf : typename _Traits::byte_string{};
}

template <class _Traits>
_NODISCARD constexpr typename _Traits::byte_string hash(
    const typename _Traits::char_type* const _Data) {
    using _Str_traits = string_traits<typename _Traits::char_type, typename _Traits::size_type>;
    return _SDSDLL hash<_Traits>(_Data, _Str_traits::length(_Data));
}

template <class _Traits>
_NODISCARD constexpr typename _Traits::byte_string hash(
    const basic_string_view<typename _Traits::char_type> _Data) {
    return _SDSDLL hash<_Traits>(_Data.data(), _Data.size());
}

template <class _Traits>
_NODISCARD constexpr typename _Traits::byte_string hash(
    const basic_string<typename _Traits::char_type>& _Data) {
    return _SDSDLL hash<_Traits>(_Data.c_str(), _Data.size());
}

// FUNCTION TEMPLATE hash_file
template <class _Traits>
_NODISCARD constexpr typename _Traits::byte_string hash_file(
    file& _File, const file::off_type _Off = 0) {
    typename _Traits::byte_string _Buf(_Traits::bytes_count(), typename _Traits::byte_type{0});
    return _Traits::hash_file(
        _Buf.data(), _Buf.size(), _File, _Off) ? _Buf : typename _Traits::byte_string{};
}

template <class _Traits>
_NODISCARD constexpr typename _Traits::byte_string hash_file(
    const path& _Target, const file::off_type _Off = 0) {
    file _File(_Target, file_access::read); // open the file with read-only access
    return _SDSDLL hash_file<_Traits>(_File, _Off);
}
_SDSDLL_END

#endif // _SDSDLL_PREPROCESSOR_GUARD
#endif // _SDSDLL_CRYPTOGRAPHY_HASH_GENERIC_HPP_