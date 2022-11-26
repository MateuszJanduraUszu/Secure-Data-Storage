// aead.hpp

// Copyright (c) Mateusz Jandura. All rights reserved.
// SPDX-License-Identifier: Apache-2.0

#pragma once
#ifndef _SDSDLL_CRYPTOGRAPHY_CIPHER_SYMMETRIC_AEAD_HPP_
#define _SDSDLL_CRYPTOGRAPHY_CIPHER_SYMMETRIC_AEAD_HPP_
#include <core/defs.hpp>
#if _SDSDLL_PREPROCESSOR_GUARD
#include <openssl/evp.h>
#include <openssl/types.h>

_SDSDLL_BEGIN
// CLASS _Aead_cipher_context_proxy
struct _Aead_cipher_context_proxy {
    EVP_CIPHER_CTX* _Ctx;

    _Aead_cipher_context_proxy() noexcept;
    ~_Aead_cipher_context_proxy() noexcept;
};

// FUNCTION _Get_aead_tag
extern _NODISCARD bool _Get_aead_tag(
    _Aead_cipher_context_proxy& _Proxy, unsigned char* const _Tag) noexcept;

// FUNCTION _Set_aead_tag
extern _NODISCARD bool _Set_aead_tag(
    _Aead_cipher_context_proxy& _Proxy, const unsigned char* const _Tag) noexcept;
_SDSDLL_END

#endif // _SDSDLL_PREPROCESSOR_GUARD
#endif // _SDSDLL_CRYPTOGRAPHY_CIPHER_SYMMETRIC_AEAD_HPP_