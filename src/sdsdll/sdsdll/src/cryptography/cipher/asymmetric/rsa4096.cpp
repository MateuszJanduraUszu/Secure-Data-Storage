// rsa4096.cpp

// Copyright (c) Mateusz Jandura. All rights reserved.
// SPDX-License-Identifier: Apache-2.0

#include <build/sdsdll_pch.hpp>
#include <cryptography/cipher/asymmetric/rsa4096.hpp>
#if _SDSDLL_PREPROCESSOR_GUARD

_SDSDLL_BEGIN
// FUNCTION _Bio_buffer_proxy constructors/destructor
_Bio_buffer_proxy::_Bio_buffer_proxy() noexcept : _Buf(::BIO_new(::BIO_s_mem())) {}

_Bio_buffer_proxy::~_Bio_buffer_proxy() noexcept {
    if (_Buf) {
        ::BIO_free(_Buf);
        _Buf = nullptr;
    }
}

// FUNCTION _Rsa4096_key_proxy constructor/destructor
_Rsa4096_key_proxy::_Rsa4096_key_proxy(EVP_PKEY* const _Key) noexcept
    : _Key(_Key ? _Key : ::EVP_PKEY_new()) {}

_Rsa4096_key_proxy::~_Rsa4096_key_proxy() noexcept {
    if (_Key) {
        ::EVP_PKEY_free(_Key);
        _Key = nullptr;
    }
}

// FUNCTION _Rsa4096_key_context_proxy constructor/destructor
_Rsa4096_key_context_proxy::_Rsa4096_key_context_proxy(EVP_PKEY* const _Key) noexcept
    : _Ctx(_Key
        ? ::EVP_PKEY_CTX_new(_Key, nullptr) : ::EVP_PKEY_CTX_new_id(EVP_PKEY_RSA, nullptr)) {}

_Rsa4096_key_context_proxy::~_Rsa4096_key_context_proxy() noexcept {
    if (_Ctx) {
        ::EVP_PKEY_CTX_free(_Ctx);
        _Ctx = nullptr;
    }
}

// FUNCTION _Make_raw_rsa4096_key_pair
_NODISCARD EVP_PKEY* _Make_raw_rsa4096_key_pair() noexcept {
    _Rsa4096_key_context_proxy _Proxy(nullptr);
    if (!_Proxy._Ctx) {
        return nullptr;
    }

    if (::EVP_PKEY_keygen_init(_Proxy._Ctx) <= 0) {
        return nullptr;
    }

    if (::EVP_PKEY_CTX_set_rsa_keygen_bits(_Proxy._Ctx, 4096) <= 0) {
        return nullptr;
    }

    EVP_PKEY* _Result = nullptr; // should be initialized
    return ::EVP_PKEY_keygen(_Proxy._Ctx, _SDSDLL addressof(_Result)) > 0 ? _Result : nullptr;
}

#ifdef _MSC_VER
#pragma warning(push, 3)
#pragma warning(disable : 4996) // C4996: Raw RSA functions since OpenSSL 3.0
#endif // _MSC_VER
// FUNCTION _Extract_private_key_from_raw_rsa4096_key_pair
_NODISCARD RSA* _Extract_private_key_from_raw_rsa4096_key_pair(
    const EVP_PKEY* const _Raw) noexcept {
    _Bio_buffer_proxy _Proxy;
    if (!_Proxy._Buf) {
        return nullptr;
    }

    return ::PEM_write_bio_PrivateKey(
        _Proxy._Buf, _Raw, nullptr, nullptr, 0, nullptr, nullptr) != 0
        ? ::PEM_read_bio_RSAPrivateKey(_Proxy._Buf, nullptr, nullptr, nullptr) : nullptr;
}

// FUNCTION _Extract_public_key_from_raw_rsa4096_key_pair
_NODISCARD RSA* _Extract_public_key_from_raw_rsa4096_key_pair(
    const EVP_PKEY* const _Raw) noexcept {
    _Bio_buffer_proxy _Proxy;
    if (!_Proxy._Buf) {
        return nullptr;
    }

    return ::PEM_write_bio_PUBKEY(_Proxy._Buf, _Raw) != 0
        ? ::PEM_read_bio_RSA_PUBKEY(_Proxy._Buf, nullptr, nullptr, nullptr) : nullptr;
}

// FUNCTION _Cast_rsa4096_key_handle
_NODISCARD EVP_PKEY* _Cast_rsa4096_key_handle(RSA* const _Handle) noexcept {
    EVP_PKEY* const _Result = ::EVP_PKEY_new();
    if (!_Result) {
        return nullptr;
    }

    if (::EVP_PKEY_assign(_Result, EVP_PKEY_RSA, _Handle) != 0) {
        return _Result;
    } else {
        ::EVP_PKEY_free(_Result);
        return nullptr;
    }
}

// FUNCTION rsa4096_key constructors/destructor
rsa4096_key::rsa4096_key() noexcept : _Myimpl(nullptr), _Mytype(none) {}

rsa4096_key::rsa4096_key(
    const rsa4096_key& _Other) noexcept : _Myimpl(nullptr), _Mytype(none) {
    (void) copy(_Other);
}

rsa4096_key::rsa4096_key(rsa4096_key&& _Other) noexcept
    : _Myimpl(_STD exchange(_Other._Myimpl, nullptr)), _Mytype(_Other._Mytype) {}

rsa4096_key::~rsa4096_key() noexcept {
    unload();
}

// FUNCTION rsa4096_key::operator=
rsa4096_key& rsa4096_key::operator=(const rsa4096_key& _Other) noexcept {
    if (this != _SDSDLL addressof(_Other)) {
        (void) copy(_Other);
    }

    return *this;
}

rsa4096_key& rsa4096_key::operator=(rsa4096_key&& _Other) noexcept {
    if (this != _SDSDLL addressof(_Other)) {
        _Mytype = _Other._Mytype;
        _Myimpl = _Other.reset();
    }

    return *this;
}

// FUNCTION rsa4096_key::good
_NODISCARD bool rsa4096_key::good() const noexcept {
    return _Myimpl != nullptr && _Mytype != none;
}

// FUNCTION rsa4096_key::native_handle
_NODISCARD const rsa4096_key::native_handle_type rsa4096_key::native_handle() const noexcept {
    return _Myimpl;
}

// FUNCTION rsa4096_key::key_type
_NODISCARD const rsa4096_key::type rsa4096_key::key_type() const noexcept {
    return _Mytype;
}

// FUNCTION rsa4096_key::unload
void rsa4096_key::unload() noexcept {
    if (_Myimpl) {
        ::RSA_free(_Myimpl);
        _Myimpl = nullptr;
        _Mytype = none;
    }
}

// FUNCTION rsa4096_key::copy
_NODISCARD bool rsa4096_key::copy(const rsa4096_key& _Other) noexcept {
    return store(_Other.load(), _Other._Mytype);
}

_NODISCARD bool rsa4096_key::copy(rsa4096_key&& _Other) noexcept {
    return store(_Other.load(), _Other._Mytype);
}

// FUNCTION rsa4096_key::reset
rsa4096_key::native_handle_type rsa4096_key::reset() noexcept {
    native_handle_type _Old_impl = _Myimpl;
    _Myimpl                      = nullptr;
    _Mytype                      = none;
    return _Old_impl;
}

// FUNCTION rsa4096_key::load
_NODISCARD string rsa4096_key::load() const {
    if (!good()) {
        return string{};
    }

    _Bio_buffer_proxy _Proxy;
    if (!_Proxy._Buf) {
        return string{};
    }

    if (_Mytype == private_key) { // load a private key
        if (::PEM_write_bio_RSAPrivateKey(
            _Proxy._Buf, _Myimpl, nullptr, nullptr, 0, nullptr, nullptr) == 0) {
            return string{};
        }
    } else { // load a public key
        if (::PEM_write_bio_RSA_PUBKEY(_Proxy._Buf, _Myimpl) == 0) {
            return string{};
        }
    }

    const int _Buf_size = BIO_pending(_Proxy._Buf);
    string _Result(static_cast<size_t>(_Buf_size), char{});
    return ::BIO_read(
        _Proxy._Buf, _Result.data(), _Buf_size) == _Buf_size ? _Result : string{};
}

// FUNCTION rsa4096_key::store
_NODISCARD bool rsa4096_key::store(const string& _New_key, const type _New_type) {
    if (good()) { // some key is already loaded
        return false;
    }

    if (_New_type != private_key && _New_type != public_key) { // unknown key type
        return false;
    }

    _Bio_buffer_proxy _Proxy;
    if (!_Proxy._Buf) {
        return false;
    }

    const int _Key_size = static_cast<int>(_New_key.size());
    if (::BIO_write(_Proxy._Buf, _New_key.c_str(), _Key_size) != _Key_size) {
        return false;
    }

    if (_New_type == private_key) { // store a private key
        _Myimpl = ::PEM_read_bio_RSAPrivateKey(_Proxy._Buf, nullptr, nullptr, nullptr);
    } else { // store a public key
        _Myimpl = ::PEM_read_bio_RSA_PUBKEY(_Proxy._Buf, nullptr, nullptr, nullptr);
    }

    if (_Myimpl) {
        _Mytype = _New_type;
        return true;
    } else {
        return false;
    }
}
#ifdef _MSC_VER
#pragma warning(pop)
#endif // _MSC_VER

// FUNCTION rsa4096_key_pair constructors/destructor
rsa4096_key_pair::rsa4096_key_pair() noexcept : _Mypriv(), _Mypub() {}

rsa4096_key_pair::rsa4096_key_pair(const rsa4096_key_pair& _Other) noexcept
    : _Mypriv(_Other._Mypriv), _Mypub(_Other._Mypub) {}

rsa4096_key_pair::rsa4096_key_pair(rsa4096_key_pair&& _Other) noexcept
    : _Mypriv(_STD move(_Other._Mypriv)), _Mypub(_STD move(_Other._Mypub)) {}

rsa4096_key_pair::rsa4096_key_pair(const rsa4096_key& _Private,
    const rsa4096_key& _Public) noexcept : _Mypriv(_Private), _Mypub(_Public) {}

rsa4096_key_pair::rsa4096_key_pair(rsa4096_key&& _Private, rsa4096_key&& _Public) noexcept
    : _Mypriv(_STD move(_Private)), _Mypub(_STD move(_Public)) {}

rsa4096_key_pair::~rsa4096_key_pair() noexcept {}

// FUNCTION rsa4096_key_pair::operator=
rsa4096_key_pair& rsa4096_key_pair::operator=(const rsa4096_key_pair& _Other) noexcept {
    if (this != _SDSDLL addressof(_Other)) {
        _Mypriv = _Other._Mypriv;
        _Mypub  = _Other._Mypub;
    }

    return *this;
}

rsa4096_key_pair& rsa4096_key_pair::operator=(rsa4096_key_pair&& _Other) noexcept {
    if (this != _SDSDLL addressof(_Other)) {
        _Mypriv = _STD move(_Other._Mypriv);
        _Mypub  = _STD move(_Other._Mypub);
    }

    return *this;
}

// FUNCTION rsa4096_key_pair::good
_NODISCARD bool rsa4096_key_pair::good() const noexcept {
    return _Mypriv.good() && _Mypriv.key_type() == rsa4096_key::private_key
        && _Mypub.good() && _Mypub.key_type() == rsa4096_key::public_key;
}

// FUNCTION rsa4096_key_pair::private_key
_NODISCARD rsa4096_key& rsa4096_key_pair::private_key() noexcept {
    return _Mypriv;
}

_NODISCARD const rsa4096_key& rsa4096_key_pair::private_key() const noexcept {
    return _Mypriv;
}

// FUNCTION rsa4096_key_pair::public_key
_NODISCARD rsa4096_key& rsa4096_key_pair::public_key() noexcept {
    return _Mypub;
}

_NODISCARD const rsa4096_key& rsa4096_key_pair::public_key() const noexcept {
    return _Mypub;
}

// FUNCTION make_rsa4096_key_pair
_NODISCARD rsa4096_key_pair make_rsa4096_key_pair() noexcept {
    EVP_PKEY* const _Raw = _Make_raw_rsa4096_key_pair();
    if (!_Raw) { // failed to generate the keys
        return rsa4096_key_pair{};
    }

    rsa4096_key_pair _Result;
    rsa4096_key& _Private = _Result.private_key();
    rsa4096_key& _Public  = _Result.public_key();
    _Private._Myimpl      = _Extract_private_key_from_raw_rsa4096_key_pair(_Raw);
    _Private._Mytype      = rsa4096_key::private_key;
    _Public._Myimpl       = _Extract_public_key_from_raw_rsa4096_key_pair(_Raw);
    _Public._Mytype       = rsa4096_key::public_key;
    ::EVP_PKEY_free(_Raw);
    return _Result;
}

// FUNCTION TEMPLATE rsa4096_traits::bytes_count
template <class _Elem>
_NODISCARD constexpr typename rsa4096_traits<_Elem>::size_type
    rsa4096_traits<_Elem>::bytes_count(const size_type) noexcept {
    // always 4096-bit (512-byte) cipher text
    return bits / CHAR_BIT;
}

// FUNCTION TEMPLATE rsa4096_traits::chars_count
template <class _Elem>
_NODISCARD constexpr typename rsa4096_traits<_Elem>::size_type
    rsa4096_traits<_Elem>::chars_count(const size_type) noexcept {
    // always at most 4096-bit (512-byte) - 66 bytes (SHA3-256 OAEP padding) plain text
    return (bits / CHAR_BIT) - 66;
}

// FUNCTION TEMPLATE rsa4096_traits::max_chars_count
template <class _Elem>
_NODISCARD constexpr typename rsa4096_traits<_Elem>::size_type
    rsa4096_traits<_Elem>::max_chars_count() noexcept {
    // always at most 4096-bit (512-byte) - 66 bytes (SHA3-256 OAEP padding) plain text
    return (bits / CHAR_BIT) - 66;
}

// FUNCTION TEMPLATE rsa4096_traits::encrypt
template <class _Elem>
_NODISCARD constexpr bool rsa4096_traits<_Elem>::encrypt(
    byte_type* const _Buf, const size_type _Buf_size, const char_type* const _Data,
    const size_type _Data_size, const key& _Key) noexcept {
    if (_Buf_size < bytes_count(0)) {
        return false;
    }

    if (_Data_size > max_chars_count()) {
        return false;
    }

    if (!_Key.good() || _Key.key_type() != key::public_key) {
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
    _Rsa4096_key_context_proxy _Ctx_proxy(_Key_proxy._Key);
    if (!_Ctx_proxy._Ctx) {
        return false;
    }

    if (::EVP_PKEY_encrypt_init(_Ctx_proxy._Ctx) <= 0) {
        return false;
    }

    if (::EVP_PKEY_CTX_set_rsa_padding(_Ctx_proxy._Ctx, RSA_PKCS1_OAEP_PADDING) <= 0) {
        return false;
    }

    if (::EVP_PKEY_CTX_set_rsa_oaep_md(_Ctx_proxy._Ctx, ::EVP_sha3_256()) <= 0) {
        return false;
    }

    byte_string _Raw_data;
    if constexpr (sizeof(char_type) == 1) { // encrypt a UTF-8 string
        _Raw_data.assign(reinterpret_cast<const byte_type*>(_Data), _Data_size);
    } else { // encrypt a Unicode string
        const utf8_string& _Narrow = utf8_string::from_utf16(_Data, _Data_size);
        _Raw_data.assign(reinterpret_cast<const byte_type*>(_Narrow.c_str()), _Narrow.size());
    }

    size_type _Bytes; // encrypted bytes (unused)
    return ::EVP_PKEY_encrypt(
        _Ctx_proxy._Ctx, _Buf, &_Bytes, _Raw_data.c_str(), _Raw_data.size()) > 0;
}

// FUNCTION TEMPLATE rsa4096_traits::decrypt
template <class _Elem>
_NODISCARD constexpr bool rsa4096_traits<_Elem>::decrypt(
    char_type* const _Buf, const size_type _Buf_size, const byte_type* const _Data,
    const size_type _Data_size, const key& _Key, size_type* const _Count) noexcept {
    if (_Data_size != bytes_count(0)) {
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
    _Rsa4096_key_context_proxy _Ctx_proxy(_Key_proxy._Key);
    if (!_Ctx_proxy._Ctx) {
        return false;
    }

    if (::EVP_PKEY_decrypt_init(_Ctx_proxy._Ctx) <= 0) {
        return false;
    }

    if (::EVP_PKEY_CTX_set_rsa_padding(_Ctx_proxy._Ctx, RSA_PKCS1_OAEP_PADDING) <= 0) {
        return false;
    }

    if (::EVP_PKEY_CTX_set_rsa_oaep_md(_Ctx_proxy._Ctx, ::EVP_sha3_256()) <= 0) {
        return false;
    }

    size_type _Bytes = 0; // decrypted bytes
    if (::EVP_PKEY_decrypt(_Ctx_proxy._Ctx, nullptr, &_Bytes, _Data, _Data_size) <= 0) {
        return false;
    }

    _Sbo_buffer<byte_type> _Temp_buf(_Bytes);
    if (_Temp_buf._Empty()) { // allocation failed
        return false;
    }
    
    if (::EVP_PKEY_decrypt(_Ctx_proxy._Ctx, _Temp_buf._Get(), &_Bytes, _Data, _Data_size) <= 0) {
        return false;
    }

    if constexpr (sizeof(char_type) == 1) { // decrypt a UTF-8 string
        if (_Buf_size < _Bytes) {
            return false;
        }

        memory_traits::copy(_Buf, _Temp_buf._Get(), _Bytes);
        if (_Count) {
            *_Count = _Bytes;
        }
    } else { // decrypt a Unicode string
        const utf16_string& _Wide = utf16_string::from_utf8(
            reinterpret_cast<const char*>(_Temp_buf._Get()), _Bytes);
        if (_Buf_size < _Wide.size()) {
            return false;
        }

        memory_traits::copy(_Buf, _Wide.c_str(), _Wide.size() * sizeof(wchar_t));
        if (_Count) {
            *_Count = _Wide.size();
        }
    }

    return true;
}

template struct _SDSDLL_API rsa4096_traits<char>;
template struct _SDSDLL_API rsa4096_traits<unsigned char>;
template struct _SDSDLL_API rsa4096_traits<wchar_t>;
_SDSDLL_END

#endif // _SDSDLL_PREPROCESSOR_GUARD