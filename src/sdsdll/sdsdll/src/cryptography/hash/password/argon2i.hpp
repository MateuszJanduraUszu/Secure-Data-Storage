// argon2i.hpp

// Copyright (c) Mateusz Jandura. All rights reserved.
// SPDX-License-Identifier: Apache-2.0

#pragma once
#ifndef _SDSDLL_CRYPTOGRAPHY_HASH_PASSWORD_ARGON2I_HPP_
#define _SDSDLL_CRYPTOGRAPHY_HASH_PASSWORD_ARGON2I_HPP_
#include <core/defs.hpp>
#if _SDSDLL_PREPROCESSOR_GUARD
#include <botan/argon2.h>
#include <climits>
#include <core/api.hpp>
#include <core/traits/concepts.hpp>
#include <core/traits/string_traits.hpp>
#include <cryptography/random/salt.hpp>
#include <cstddef>
#include <encoding/utf8.hpp>
#include <string>

// STD types
using _STD basic_string;

_SDSDLL_BEGIN
// STRUCT TEMPLATE argon2i_traits
template <class _Elem>
struct _SDSDLL_API argon2i_traits { // traits for the Argon2i password hash
private:
    static_assert(is_any_of_v<_Elem, char, unsigned char, wchar_t>,
        "Requires a byte/UTF-8/Unicode element type.");

public:
    using char_type       = _Elem;
    using byte_type       = unsigned char;
    using char_string     = basic_string<_Elem>;
    using byte_string     = basic_string<unsigned char>;
    using size_type       = size_t;
    using difference_type = ptrdiff_t;
    using salt            = salt<16>;

    static constexpr size_type bits = 512;

    // returns hashed password length
    _NODISCARD static constexpr size_type bytes_count() noexcept;

    // hashes a UTF-8/Unicode password
    _NODISCARD static constexpr bool hash(
        byte_type* const _Buf, const size_type _Buf_size, const char_type* const _Data,
        const size_type _Data_size, const salt& _Salt, const size_type _Memory_amount,
        const size_type _Iterations, const size_type _Parallelism) noexcept;
};
_SDSDLL_END

#endif // _SDSDLL_PREPROCESSOR_GUARD
#endif // _SDSDLL_CRYPTOGRAPHY_HASH_PASSWORD_ARGON2I_HPP_