// password.hpp

// Copyright (c) Mateusz Jandura. All rights reserved.
// SPDX-License-Identifier: Apache-2.0

#pragma once
#ifndef _SDSDLL_CRYPTOGRAPHY_HASH_PASSWORD_HPP_
#define _SDSDLL_CRYPTOGRAPHY_HASH_PASSWORD_HPP_
#include <core/defs.hpp>
#if _SDSDLL_PREPROCESSOR_GUARD
#include <core/optimization/string_view.hpp>
#include <core/traits/string_traits.hpp>
#include <string>

// STD types
using _STD basic_string;

_SDSDLL_BEGIN
// FUNCTION TEMPLATE hash_password
template <class _Traits>
_NODISCARD constexpr typename _Traits::byte_string hash_password(
    const typename _Traits::char_type* const _Data, const typename _Traits::size_type _Count,
    const typename _Traits::salt& _Salt, const typename _Traits::size_type _Memory_amount_or_cost,
    const typename _Traits::size_type _Iterations_or_block_size,
    const typename _Traits::size_type _Parallelism) {
    typename _Traits::byte_string _Buf(_Traits::bytes_count(), typename _Traits::byte_type{0});
    return _Traits::hash(_Buf.data(), _Buf.size(), _Data, _Count, _Salt, _Memory_amount_or_cost,
        _Iterations_or_block_size, _Parallelism) ? _Buf : typename _Traits::byte_string{};
}

template <class _Traits>
_NODISCARD constexpr typename _Traits::byte_string hash_password(
    const typename _Traits::char_type* const _Data, const typename _Traits::salt& _Salt,
    const typename _Traits::size_type _Memory_amount_or_cost,
    const typename _Traits::size_type _Iterations_or_block_size,
    const typename _Traits::size_type _Parallelism) {
    using _Str_traits = string_traits<typename _Traits::char_type, typename _Traits::size_type>;
    return _SDSDLL hash_password<_Traits>(_Data, _Str_traits::length(_Data), _Salt,
        _Memory_amount_or_cost, _Iterations_or_block_size, _Parallelism);
}

template <class _Traits>
_NODISCARD constexpr typename _Traits::byte_string hash_password(
    const basic_string_view<typename _Traits::char_type> _Data, const typename _Traits::salt& _Salt,
    const typename _Traits::size_type _Memory_amount_or_cost,
    const typename _Traits::size_type _Iterations_or_block_size,
    const typename _Traits::size_type _Parallelism) {
    return _SDSDLL hash_password<_Traits>(_Data.data(), _Data.size(), _Salt,
        _Memory_amount_or_cost, _Iterations_or_block_size, _Parallelism);
}

template <class _Traits>
_NODISCARD constexpr typename _Traits::byte_string hash_password(
    const basic_string<typename _Traits::char_type>& _Data, const typename _Traits::salt& _Salt,
    const typename _Traits::size_type _Memory_amount_or_cost,
    const typename _Traits::size_type _Iterations_or_block_size,
    const typename _Traits::size_type _Parallelism) {
    return _SDSDLL hash_password<_Traits>(_Data.c_str(), _Data.size(), _Salt,
        _Memory_amount_or_cost, _Iterations_or_block_size, _Parallelism);
}
_SDSDLL_END

#endif // _SDSDLL_PREPROCESSOR_GUARD
#endif // _SDSDLL_CRYPTOGRAPHY_HASH_PASSWORD_HPP_