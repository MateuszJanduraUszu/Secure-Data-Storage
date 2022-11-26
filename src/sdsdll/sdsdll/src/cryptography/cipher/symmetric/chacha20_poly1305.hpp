// chacha20_poly1305.hpp

// Copyright (c) Mateusz Jandura. All rights reserved.
// SPDX-License-Identifier: Apache-2.0

#pragma once
#ifndef _SDSDLL_CRYPTOGRAPHY_CIPHER_SYMMETRIC_CHACHA20_POLY1305_HPP_
#define _SDSDLL_CRYPTOGRAPHY_CIPHER_SYMMETRIC_CHACHA20_POLY1305_HPP_
#include <core/defs.hpp>
#if _SDSDLL_PREPROCESSOR_GUARD
#include <core/api.hpp>
#include <core/container/bytes.hpp>
#include <core/optimization/sbo.hpp>
#include <core/traits/concepts.hpp>
#include <core/traits/memory_traits.hpp>
#include <core/traits/string_traits.hpp>
#include <cryptography/cipher/symmetric/aead.hpp>
#include <cryptography/cipher/symmetric/iv.hpp>
#include <cryptography/cipher/symmetric/symmetric_key.hpp>
#include <cstddef>
#include <encoding/utf8.hpp>
#include <encoding/utf16.hpp>
#include <openssl/evp.h>
#include <openssl/types.h>
#include <string>

// STD types
using _STD basic_string;

_SDSDLL_BEGIN
// STRUCT TEMPLATE chacha20_poly1305_traits
template <class _Elem>
struct _SDSDLL_API chacha20_poly1305_traits { // traits for the ChaCha20-Poly1305 cipher
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
    using key             = symmetric_key<32>;
    using iv              = iv<12>;

    // returns encrypted text length
    _NODISCARD static constexpr size_type bytes_count(const size_type _Count) noexcept;

    // returns decrypted text length
    _NODISCARD static constexpr size_type chars_count(const size_type _Count) noexcept;

    // encrypts a UTF-8/Unicode text
    _NODISCARD static constexpr bool encrypt(
        byte_type* const _Buf, const size_type _Buf_size, const char_type* const _Data,
        const size_type _Data_size, const key& _Key, const iv& _Iv) noexcept;

    // decrypts a UTF-8/Unicode text
    _NODISCARD static constexpr bool decrypt(char_type* const _Buf,
        const size_type _Buf_size, const byte_type* const _Data, const size_type _Data_size,
        const key& _Key, const iv& _Iv, size_type* const _Count = nullptr) noexcept;

private:
    static constexpr size_type _Tag_size = 16; // 16-byte authentication tag
};
_SDSDLL_END

#endif // _SDSDLL_PREPROCESSOR_GUARD
#endif // _SDSDLL_CRYPTOGRAPHY_CIPHER_SYMMETRIC_CHACHA20_POLY1305_HPP_