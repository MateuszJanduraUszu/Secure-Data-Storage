// aead.cpp

// Copyright (c) Mateusz Jandura. All rights reserved.
// SPDX-License-Identifier: Apache-2.0

#include <build/sdsdll_pch.hpp>
#include <cryptography/cipher/symmetric/aead.hpp>
#if _SDSDLL_PREPROCESSOR_GUARD

_SDSDLL_BEGIN
// FUNCTION _Aead_cipher_context_proxy constructor/destructor
_Aead_cipher_context_proxy::_Aead_cipher_context_proxy() noexcept
    : _Ctx(::EVP_CIPHER_CTX_new()) {}

_Aead_cipher_context_proxy::~_Aead_cipher_context_proxy() noexcept {
    if (_Ctx) {
        ::EVP_CIPHER_CTX_free(_Ctx);
        _Ctx = nullptr;
    }
}

// FUNCTION _Get_aead_tag
_NODISCARD bool _Get_aead_tag(
    _Aead_cipher_context_proxy& _Proxy, unsigned char* const _Tag) noexcept {
    return ::EVP_CIPHER_CTX_ctrl(_Proxy._Ctx, EVP_CTRL_AEAD_GET_TAG, 16, _Tag) != 0;
}

// FUNCTION _Set_aead_tag
_NODISCARD bool _Set_aead_tag(
    _Aead_cipher_context_proxy& _Proxy, const unsigned char* const _Tag) noexcept {
    return ::EVP_CIPHER_CTX_ctrl(
        _Proxy._Ctx, EVP_CTRL_AEAD_SET_TAG, 16, const_cast<unsigned char*>(_Tag)) != 0;
}
_SDSDLL_END

#endif // _SDSDLL_PREPROCESSOR_GUARD