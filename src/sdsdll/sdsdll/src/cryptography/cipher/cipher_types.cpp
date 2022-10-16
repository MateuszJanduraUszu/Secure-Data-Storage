// cipher_types.cpp

// Copyright (c) Mateusz Jandura. All rights reserved.
// SPDX-License-Identifier: Apache-2.0

#include <build/sdsdll_pch.hpp>
#include <cryptography/cipher/cipher_types.hpp>
#if _SDSDLL_PREPROCESSOR_GUARD

_SDSDLL_BEGIN
// FUNCTION _Cipher_context_proxy constructor/destructor
_Cipher_context_proxy::_Cipher_context_proxy() noexcept : _Ctx(::EVP_CIPHER_CTX_new()) {}

_Cipher_context_proxy::~_Cipher_context_proxy() noexcept {
    if (_Ctx) {
        ::EVP_CIPHER_CTX_free(_Ctx);
        _Ctx = nullptr;
    }
}
_SDSDLL_END

#endif // _SDSDLL_PREPROCESSOR_GUARD