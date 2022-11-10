// xxhash.hpp

// Copyright (c) Mateusz Jandura. All rights reserved.
// SPDX-License-Identifier: Apache-2.0

#pragma once
#ifndef _SDSDLL_CRYPTOGRAPHY_HASH_GENERIC_XXHASH_HPP_
#define _SDSDLL_CRYPTOGRAPHY_HASH_GENERIC_XXHASH_HPP_
#include <core/defs.hpp>
#if _SDSDLL_PREPROCESSOR_GUARD
#include <climits>
#include <core/api.hpp>
#include <core/traits/concepts.hpp>
#include <core/traits/integer.hpp>
#include <core/traits/memory_traits.hpp>
#include <core/traits/string_traits.hpp>
#include <core/traits/type_traits.hpp>
#include <cstddef>
#include <cstdint>
#include <filesystem/file.hpp>
#include <string>
#include <xxhash.h>

// STD types
using _STD basic_string;

_SDSDLL_BEGIN
// STRUCT _Xxhash_state_proxy
struct _Xxhash_state_proxy {
    XXH3_state_t* _State;

    _Xxhash_state_proxy() noexcept;
    ~_Xxhash_state_proxy() noexcept;
};

// STRUCT TEMPLATE xxhash_traits
template <class _Elem>
struct _SDSDLL_API xxhash_traits { // traits for the xxHash hash
private:
    static_assert(is_any_of_v<_Elem, char, unsigned char, wchar_t>,
        "Requires a byte/UTF-8/Unicode element type.");

public:
    using char_type       = _Elem;
    using byte_type	      = unsigned char;
    using char_string     = basic_string<_Elem>;
    using byte_string     = basic_string<unsigned char>;
    using size_type       = size_t;
    using difference_type = ptrdiff_t;

    static constexpr size_type bits = 64;

    // returns hashed text length
    _NODISCARD static constexpr size_type bytes_count() noexcept;

    // hashes a UTF-8/Unicode text
    _NODISCARD static constexpr bool hash(byte_type* const _Buf, const size_type _Buf_size,
        const char_type* const _Data, const size_type _Data_size) noexcept;

    // hashes a file
    _NODISCARD static constexpr bool hash_file(byte_type* const _Buf,
        const size_type _Buf_size, file& _File, const file::off_type _Off) noexcept;
};
_SDSDLL_END

#endif // _SDSDLL_PREPROCESSOR_GUARD
#endif // _SDSDLL_CRYPTOGRAPHY_HASH_GENERIC_XXHASH_HPP_