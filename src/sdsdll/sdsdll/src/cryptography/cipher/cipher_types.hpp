// cipher_types.hpp

// Copyright (c) Mateusz Jandura. All rights reserved.
// SPDX-License-Identifier: Apache-2.0

#pragma once
#ifndef _SDSDLL_CRYPTOGRAPHY_CIPHER_CIPHER_TYPES_HPP_
#define _SDSDLL_CRYPTOGRAPHY_CIPHER_CIPHER_TYPES_HPP_
#include <core/defs.hpp>
#if _SDSDLL_PREPROCESSOR_GUARD
#include <core/api.hpp>
#include <cstddef>
#include <openssl/evp.h>
#include <openssl/types.h>
#include <string>

// STD types
using _STD basic_string;

_SDSDLL_BEGIN
// STRUCT TEMPLATE _Simple_cipher_types
template <class _Elem, class _Eng, class _Key, class _Iv>
struct _Simple_cipher_types {
    using char_type       = _Elem;
    using byte_type       = unsigned char;
    using char_string     = basic_string<char_type>;
    using byte_string     = basic_string<byte_type>;
    using size_type       = size_t;
    using difference_type = ptrdiff_t;
    using engine          = _Eng;
    using key             = _Key;
    using iv              = _Iv;
};

// ENUM CLASS cipher_task
enum class cipher_task : bool {
    encryption,
    decryption
};

// STRUCT _Cipher_context_proxy
struct _Cipher_context_proxy {
    EVP_CIPHER_CTX* _Ctx;

    _Cipher_context_proxy() noexcept;
    ~_Cipher_context_proxy() noexcept;

    _Cipher_context_proxy(const _Cipher_context_proxy&) = delete;
    _Cipher_context_proxy& operator=(const _Cipher_context_proxy&) = delete;
};
_SDSDLL_END

#endif // _SDSDLL_PREPROCESSOR_GUARD
#endif // _SDSDLL_CRYPTOGRAPHY_CIPHER_CIPHER_TYPES_HPP_