// signature.cpp

// Copyright (c) Mateusz Jandura. All rights reserved.
// SPDX-License-Identifier: Apache-2.0

#include <build/sdsdll_pch.hpp>
#include <cryptography/signature/signature.hpp>
#if _SDSDLL_PREPROCESSOR_GUARD

_SDSDLL_BEGIN
// FUNCTION _Signature_context_proxy constructor/destructor
_Signature_context_proxy::_Signature_context_proxy() noexcept : _Ctx(::EVP_MD_CTX_new()) {}

_Signature_context_proxy::~_Signature_context_proxy() noexcept {
    if (_Ctx) {
        ::EVP_MD_CTX_free(_Ctx);
        _Ctx = nullptr;
    }
}

// FUNCTION _Signature_traits::_Bytes_count
_NODISCARD _Signature_traits::size_type _Signature_traits::_Bytes_count() noexcept {
    return rsa4096_traits<char_type>::bytes_count(0);
}

// FUNCTION _Signature_traits::_Max_chars_count
_NODISCARD _Signature_traits::size_type _Signature_traits::_Max_chars_count() noexcept {
    return rsa4096_traits<char_type>::max_chars_count();
}

// FUNCTION _Signature_traits::_Sign
_NODISCARD bool _Signature_traits::_Sign(byte_type* const _Buf, const size_type _Buf_size,
    const byte_type* const _Data, const size_type _Data_size, const rsa4096_key& _Key) noexcept {
    if (_Buf_size < _Bytes_count() || _Data_size > _Max_chars_count()) {
        return false;
    }

    if (!_Key.good()) {
        return false;
    }
    
    _Signature_context_proxy _Proxy;
    if (!_Proxy._Ctx) {
        return false;
    }

    if (::EVP_DigestSignInit(
        _Proxy._Ctx, nullptr, ::EVP_sha3_256(), nullptr, _Key.native_handle()) <= 0) {
        return false;
    }

    if (::EVP_DigestSignUpdate(_Proxy._Ctx, _Data, _Data_size) <= 0) {
        return false;
    }

    size_t _Bytes; // signed bytes (unused)
    return ::EVP_DigestSignFinal(_Proxy._Ctx, _Buf, &_Bytes) > 0;
}

// FUNCTION _Signature_traits::_Verify
_NODISCARD verification_result _Signature_traits::_Verify(
    const byte_type* const _Signature, const size_type _Signature_size,
    const byte_type* const _Data, const size_type _Data_size, const key& _Key) noexcept {
    if (_Signature_size != _Bytes_count() || _Data_size > _Max_chars_count()) {
        return verification_result::fake;
    }

    if (!_Key.good()) {
        return verification_result::unknown;
    }

    _Signature_context_proxy _Proxy;
    if (!_Proxy._Ctx) {
        return verification_result::unknown;
    }

    if (::EVP_DigestVerifyInit(
        _Proxy._Ctx, nullptr, ::EVP_sha3_256(), nullptr, _Key.native_handle()) <= 0) {
        return verification_result::unknown;
    }

    if (::EVP_DigestVerifyUpdate(_Proxy._Ctx, _Data, _Data_size) <= 0) {
        return verification_result::unknown;
    }

    return ::EVP_DigestVerifyFinal(_Proxy._Ctx, _Signature, _Signature_size) == 1
        ? verification_result::authentic : verification_result::fake;
}

// FUNCTION sign_file
_NODISCARD byte_string sign_file(file& _File, const rsa4096_key& _Key, const file::off_type _Off) {
    using _Traits            = sha512_traits<char>;
    const byte_string& _Hash = _SDSDLL hash_file<_Traits>(_File, _Off);
    if (_Hash.empty()) { // failed to compute a hash
        return byte_string{};
    }

    byte_string _Result(_Signature_traits::_Bytes_count(), uint8_t{0});
    return _Signature_traits::_Sign(_Result.data(), _Result.size(),
        _Hash.c_str(), _Hash.size(), _Key) ? _Result : byte_string{};
}

_NODISCARD byte_string sign_file(
    const path& _Target, const rsa4096_key& _Key, const file::off_type _Off) {
    file _File(_Target, file_access::read); // open the file with read-only access
    return _SDSDLL sign_file(_File, _Key, _Off);
}

// FUNCTION verify_file
_NODISCARD verification_result verify_file(file& _File,
    const rsa4096_key& _Key, const byte_string& _Signature, const file::off_type _Off) {
    using _Traits            = sha512_traits<char>;
    const byte_string& _Hash = _SDSDLL hash_file<_Traits>(_File, _Off);
    if (_Hash.empty()) { // failed to compute a hash
        return verification_result::unknown;
    }

    return _Signature_traits::_Verify(
        _Signature.c_str(), _Signature.size(), _Hash.c_str(), _Hash.size(), _Key);
}

_NODISCARD verification_result verify_file(const path& _Target,
    const rsa4096_key& _Key, const byte_string& _Signature, const file::off_type _Off) {
    file _File(_Target, file_access::read); // open the file with read-only access
    return _SDSDLL verify_file(_File, _Key, _Signature, _Off);
}
_SDSDLL_END

#endif // _SDSDLL_PREPROCESSOR_GUARD