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
    const byte_type* const _Data, const size_type _Data_size, const key& _Key) noexcept {
    if (_Buf_size < _Bytes_count() || _Data_size > _Max_chars_count()) {
        return false;
    }

    if (!_Key.good() || _Key.key_type() != key::private_key) {
        return false;
    }
    
    EVP_PKEY* _Key_handle;
    {
        // Note: We must use a temporary key, otherwise the key will be released.
        key _Temp_key = _Key;
        if (!_Temp_key.good()) {
            return false;
        }

        _Key_handle = _Cast_rsa4096_key_handle(_Temp_key.reset());
        if (!_Key_handle) {
            return false;
        }
    }

    _Rsa4096_key_proxy _Key_proxy(_Key_handle);
    _Signature_context_proxy _Ctx_proxy;
    if (!_Ctx_proxy._Ctx) {
        return false;
    }

    if (::EVP_DigestSignInit(
        _Ctx_proxy._Ctx, nullptr, ::EVP_sha3_256(), nullptr, _Key_proxy._Key) <= 0) {
        return false;
    }

    if (::EVP_DigestSignUpdate(_Ctx_proxy._Ctx, _Data, _Data_size) <= 0) {
        return false;
    }

    size_t _Bytes; // signed bytes (unused)
    return ::EVP_DigestSignFinal(_Ctx_proxy._Ctx, _Buf, &_Bytes) > 0;
}

// FUNCTION _Signature_traits::_Verify
_NODISCARD verification_result _Signature_traits::_Verify(
    const byte_type* const _Signature, const size_type _Signature_size,
    const byte_type* const _Data, const size_type _Data_size, const key& _Key) noexcept {
    if (_Signature_size != _Bytes_count() || _Data_size > _Max_chars_count()) {
        return verification_result::fake;
    }

    if (!_Key.good() || _Key.key_type() != key::public_key) {
        return verification_result::unknown;
    }

    EVP_PKEY* _Key_handle;
    {
        // Note: We must use a temporary key, otherwise the key will be released.
        key _Temp_key = _Key;
        if (!_Temp_key.good()) {
            return verification_result::unknown;
        }

        _Key_handle = _Cast_rsa4096_key_handle(_Temp_key.reset());
        if (!_Key_handle) {
            return verification_result::unknown;
        }
    }

    _Rsa4096_key_proxy _Key_proxy(_Key_handle);
    _Signature_context_proxy _Ctx_proxy;
    if (!_Ctx_proxy._Ctx) {
        return verification_result::unknown;
    }

    if (::EVP_DigestVerifyInit(
        _Ctx_proxy._Ctx, nullptr, ::EVP_sha3_256(), nullptr, _Key_proxy._Key) <= 0) {
        return verification_result::unknown;
    }

    if (::EVP_DigestVerifyUpdate(_Ctx_proxy._Ctx, _Data, _Data_size) <= 0) {
        return verification_result::unknown;
    }

    return ::EVP_DigestVerifyFinal(_Ctx_proxy._Ctx, _Signature, _Signature_size) == 1
        ? verification_result::authentic : verification_result::fake;
}

// FUNCTION sign_data
template <class _Elem>
_NODISCARD constexpr byte_string sign_data(
    const _Elem* const _Data, const size_t _Size, const rsa4096_key& _Key) {
    using _Traits            = sha512_traits<_Elem>;
    const byte_string& _Hash = _SDSDLL hash<_Traits>(_Data, _Size);
    if (_Hash.empty()) { // failed to compute a hash
        return byte_string{};
    }

    byte_string _Result(_Signature_traits::_Bytes_count(), uint8_t{0});
    return _Signature_traits::_Sign(_Result.data(), _Result.size(),
        _Hash.c_str(), _Hash.size(), _Key) ? _Result : byte_string{};
}

template _SDSDLL_API _NODISCARD byte_string sign_data(
    const char* const, const size_t, const rsa4096_key&);
template _SDSDLL_API _NODISCARD byte_string sign_data(
    const unsigned char* const, const size_t, const rsa4096_key&);
template _SDSDLL_API _NODISCARD byte_string sign_data(
    const wchar_t* const, const size_t, const rsa4096_key&);

template <class _Elem>
_NODISCARD constexpr byte_string sign_data(const _Elem* const _Data, const rsa4096_key& _Key) {
    using _Traits = string_traits<_Elem, size_t>;
    return _SDSDLL sign_data(_Data, _Traits::length(_Data), _Key);
}

template _SDSDLL_API _NODISCARD byte_string sign_data(const char* const, const rsa4096_key&);
template _SDSDLL_API _NODISCARD byte_string sign_data(const unsigned char* const, const rsa4096_key&);
template _SDSDLL_API _NODISCARD byte_string sign_data(const wchar_t* const, const rsa4096_key&);

template <class _Elem>
_NODISCARD constexpr byte_string sign_data(
    const basic_string_view<_Elem> _Data, const rsa4096_key& _Key) {
    return _SDSDLL sign_data(_Data.data(), _Data.size(), _Key);
}

template _SDSDLL_API _NODISCARD byte_string sign_data(const string_view, const rsa4096_key&);
template _SDSDLL_API _NODISCARD byte_string sign_data(const byte_string_view, const rsa4096_key&);
template _SDSDLL_API _NODISCARD byte_string sign_data(const wstring_view, const rsa4096_key&);

template <class _Elem>
_NODISCARD constexpr byte_string sign_data(
    const basic_string<_Elem>& _Data, const rsa4096_key& _Key) {
    return _SDSDLL sign_data(_Data.c_str(), _Data.size(), _Key);
}

template _SDSDLL_API _NODISCARD byte_string sign_data(const string&, const rsa4096_key&);
template _SDSDLL_API _NODISCARD byte_string sign_data(const byte_string&, const rsa4096_key&);
template _SDSDLL_API _NODISCARD byte_string sign_data(const wstring&, const rsa4096_key&);

// FUNCTION TEMPLATE verify_data
template <class _Elem>
_NODISCARD constexpr verification_result verify_data(const _Elem* const _Data,
    const size_t _Size, const rsa4096_key& _Key, const byte_string& _Signature) {
    using _Traits            = sha512_traits<_Elem>;
    const byte_string& _Hash = _SDSDLL hash<_Traits>(_Data, _Size);
    if (_Hash.empty()) { // failed to compute a hash
        return verification_result::unknown;
    }

    return _Signature_traits::_Verify(
        _Signature.c_str(), _Signature.size(), _Hash.c_str(), _Hash.size(), _Key);
}

template _SDSDLL_API _NODISCARD verification_result verify_data(
    const char* const, const size_t, const rsa4096_key&, const byte_string&);
template _SDSDLL_API _NODISCARD verification_result verify_data(
    const unsigned char* const, const size_t, const rsa4096_key&, const byte_string&);
template _SDSDLL_API _NODISCARD verification_result verify_data(
    const wchar_t* const, const size_t, const rsa4096_key&, const byte_string&);

template <class _Elem>
_NODISCARD constexpr verification_result verify_data(
    const _Elem* const _Data, const rsa4096_key& _Key, const byte_string& _Signature) {
    using _Traits = string_traits<_Elem, size_t>;
    return _SDSDLL verify_data(_Data, _Traits::length(_Data), _Key, _Signature);
}

template _SDSDLL_API _NODISCARD verification_result verify_data(
    const char* const, const rsa4096_key&, const byte_string&);
template _SDSDLL_API _NODISCARD verification_result verify_data(
    const unsigned char* const, const rsa4096_key&, const byte_string&);
template _SDSDLL_API _NODISCARD verification_result verify_data(
    const wchar_t* const, const rsa4096_key&, const byte_string&);

template <class _Elem>
_NODISCARD constexpr verification_result verify_data(
    const basic_string_view<_Elem> _Data, const rsa4096_key& _Key, const byte_string& _Signature) {
    return _SDSDLL verify_data(_Data.data(), _Data.size(), _Key, _Signature);
}

template _SDSDLL_API _NODISCARD verification_result verify_data(
    const string_view, const rsa4096_key&, const byte_string&);
template _SDSDLL_API _NODISCARD verification_result verify_data(
    const byte_string_view, const rsa4096_key&, const byte_string&);
template _SDSDLL_API _NODISCARD verification_result verify_data(
    const wstring_view, const rsa4096_key&, const byte_string&);

template <class _Elem>
_NODISCARD constexpr verification_result verify_data(
    const basic_string<_Elem>& _Data, const rsa4096_key& _Key, const byte_string& _Signature) {
    return _SDSDLL verify_data(_Data.c_str(), _Data.size(), _Key, _Signature);
}

template _SDSDLL_API _NODISCARD verification_result verify_data(
    const string&, const rsa4096_key&, const byte_string&);
template _SDSDLL_API _NODISCARD verification_result verify_data(
    const byte_string&, const rsa4096_key&, const byte_string&);
template _SDSDLL_API _NODISCARD verification_result verify_data(
    const wstring&, const rsa4096_key&, const byte_string&);

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