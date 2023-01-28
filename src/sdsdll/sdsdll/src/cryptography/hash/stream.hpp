// ostream.hpp

// Copyright (c) Mateusz Jandura. All rights reserved.
// SPDX-License-Identifier: Apache-2.0

#pragma once
#ifndef _SDSDLL_CRYPTOGRAPHY_HASH_STREAM_HPP_
#define _SDSDLL_CRYPTOGRAPHY_HASH_STREAM_HPP_
#include <core/defs.hpp>
#if _SDSDLL_PREPROCESSOR_GUARD
#include <core/optimization/string_view.hpp>
#include <core/traits/string_traits.hpp>
#include <string>

// STD types
using _STD basic_string;

_SDSDLL_BEGIN
// CLASS TEMPLATE stream_hash
template <class _Traits>
class stream_hash {
public:
    using char_type       = typename _Traits::char_type;
    using byte_type       = typename _Traits::byte_type;
    using char_string     = typename _Traits::char_string;
    using byte_string     = typename _Traits::byte_string;
    using size_type       = typename _Traits::size_type;
    using difference_type = typename _Traits::difference_type;
    using state_type      = typename _Traits::state_type;

    constexpr stream_hash() noexcept : _Mystate() {
        (void) _Traits::init(_Mystate);
    }

    _CONSTEXPR20 ~stream_hash() noexcept {}

    stream_hash(const stream_hash&) = delete;
    stream_hash& operator=(const stream_hash&) = delete;

    _NODISCARD constexpr bool append(
        const char_type* const _Data, const size_type _Count) noexcept {
        return _Traits::append(_Mystate, _Data, _Count);
    }

    _NODISCARD constexpr bool append(const char_type* const _Data) noexcept {
        using _Str_traits = string_traits<char_type, size_type>;
        return _Traits::append(_Mystate, _Data, _Str_traits::length(_Data));
    }
    
    _NODISCARD constexpr bool append(const basic_string_view<char_type> _Data) noexcept {
        return _Traits::append(_Mystate, _Data.data(), _Data.size());
    }
    
    _NODISCARD constexpr bool append(const basic_string<char_type>& _Data) {
        return _Traits::append(_Mystate, _Data.c_str(), _Data.size());
    }

    _NODISCARD constexpr byte_string complete() {
        byte_string _Result(_Traits::bytes_count(), byte_type{});
        return _Traits::complete(
            _Mystate, _Result.data(), _Result.size()) ? _Result : byte_string{};
    }

private:
    state_type _Mystate;
};
_SDSDLL_END

#endif // _SDSDLL_PREPROCESSOR_GUARD
#endif // _SDSDLL_CRYPTOGRAPHY_HASH_STREAM_HPP_