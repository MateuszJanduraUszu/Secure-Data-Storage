// aes256_gcm.cpp

// Copyright (c) Mateusz Jandura. All rights reserved.
// SPDX-License-Identifier: Apache-2.0

#include <build/sdsdll_pch.hpp>
#include <cryptography/cipher/symmetric/aes256_gcm.hpp>
#if _SDSDLL_PREPROCESSOR_GUARD

_SDSDLL_BEGIN
// FUNCTION TEMPLATE aes256_gcm_traits::bytes_count
template <class _Elem>
_NODISCARD constexpr typename aes256_gcm_traits<_Elem>::size_type
    aes256_gcm_traits<_Elem>::bytes_count(const size_type _Count) noexcept {
    return _Count + _Tag_size; // always plain text length + 16-byte tag
}

// FUNCTION TEMPLATE aes256_gcm_traits::chars_count
template <class _Elem>
_NODISCARD constexpr typename aes256_gcm_traits<_Elem>::size_type
    aes256_gcm_traits<_Elem>::chars_count(const size_type _Count) noexcept {
    return _Count - _Tag_size; // always cipher text length - 16-byte tag
}

// FUNCTION TEMPLATE aes256_gcm_traits::encrypt
template <class _Elem>
_NODISCARD constexpr bool aes256_gcm_traits<_Elem>::encrypt(
    byte_type* const _Buf, const size_type _Buf_size, const char_type* const _Data,
    const size_type _Data_size, const key& _Key, const iv& _Iv) noexcept {
    if (!_Buf) {
        return false;
    }

    byte_string _Raw_data;
    if constexpr (sizeof(_Elem) == 1) { // encrypt a UTF-8 string
        _Raw_data.assign(reinterpret_cast<const byte_type*>(_Data), _Data_size);
    } else { // encrypt a Unicode string
        const utf8_string& _Narrow = utf8_string::from_utf16(_Data, _Data_size);
        _Raw_data.assign(reinterpret_cast<const byte_type*>(_Narrow.c_str()), _Narrow.size());
    }

    const size_type _Optimal_buf_size = bytes_count(_Raw_data.size());
    if (_Buf_size < _Optimal_buf_size) {
        return false;
    }

    _Aead_cipher_context_proxy _Proxy;
    if (!_Proxy._Ctx) {
        return false;
    }

    if (::EVP_EncryptInit_ex(_Proxy._Ctx, ::EVP_aes_256_gcm(), nullptr, _Key.get(), _Iv.get()) == 0) {
        return false;
    }

    int _Bytes = 0; // encrypted bytes (unused)
    if (::EVP_EncryptUpdate(
        _Proxy._Ctx, _Buf, &_Bytes, _Raw_data.c_str(), static_cast<int>(_Raw_data.size())) == 0) {
        return false;
    }

    if (::EVP_EncryptFinal_ex(_Proxy._Ctx, _Buf + _Bytes, &_Bytes) == 0) {
        return false;
    }

    return _Get_aead_tag(_Proxy, _Buf + (_Optimal_buf_size - _Tag_size));
}

// FUNCTION TEMPLATE aes256_gcm_traits::decrypt
template <class _Elem>
_NODISCARD constexpr bool aes256_gcm_traits<_Elem>::decrypt(char_type* const _Buf,
    const size_type _Buf_size, const byte_type* const _Data, const size_type _Data_size,
    const key& _Key, const iv& _Iv, size_type* const _Count) noexcept {
    if (!_Buf) {
        return false;
    }

    const size_type _Optimal_buf_size = chars_count(_Data_size);
    if constexpr (sizeof(_Elem) == 1) {
        // Note: A UTF-8 string length is always equal to _Optimal_buf_size.
        if (_Buf_size < _Optimal_buf_size) {
            return false;
        }
    }

    _Aead_cipher_context_proxy _Proxy;
    if (!_Proxy._Ctx) {
        return false;
    }

    if (::EVP_DecryptInit_ex(_Proxy._Ctx, ::EVP_aes_256_gcm(), nullptr, _Key.get(), _Iv.get()) == 0) {
        return false;
    }

    _Sbo_buffer<byte_type> _Temp_buf(_Optimal_buf_size);
    if (_Temp_buf._Empty()) { // failed to allocate a buffer
        return false;
    }

    int _Bytes = 0; // decrypted bytes (unused)
    if (::EVP_DecryptUpdate(_Proxy._Ctx,
        _Temp_buf._Get(), &_Bytes, _Data, static_cast<int>(_Data_size - _Tag_size)) == 0) {
        return false;
    }

    if (!_Set_aead_tag(_Proxy, _Data + (_Data_size - _Tag_size))) {
        return false;
    }

    if (::EVP_DecryptFinal_ex(_Proxy._Ctx, _Temp_buf._Get() + _Bytes, &_Bytes) == 0) {
        return false;
    }

    if constexpr (sizeof(_Elem) == 1) { // decrypt a UTF-8 string
        memory_traits::copy(_Buf, _Temp_buf._Get(), _Optimal_buf_size);
        if (_Count) {
            *_Count = _Optimal_buf_size;
        }
    } else { // decrypt a Unicode string
        const utf16_string& _Wide = utf16_string::from_utf8(
            reinterpret_cast<const char*>(_Temp_buf._Get()), _Temp_buf._Size());
        memory_traits::copy(_Buf, _Wide.c_str(), _Wide.size() * sizeof(_Elem));
        if (_Count) {
            *_Count = _Wide.size();
        }
    }

    return true;
}

template struct _SDSDLL_API aes256_gcm_traits<char>;
template struct _SDSDLL_API aes256_gcm_traits<unsigned char>;
template struct _SDSDLL_API aes256_gcm_traits<wchar_t>;
_SDSDLL_END

#endif // _SDSDLL_PREPROCESSOR_GUARD