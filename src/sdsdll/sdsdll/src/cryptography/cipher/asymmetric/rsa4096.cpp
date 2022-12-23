// rsa4096.cpp

// Copyright (c) Mateusz Jandura. All rights reserved.
// SPDX-License-Identifier: Apache-2.0

#include <build/sdsdll_pch.hpp>
#include <cryptography/cipher/asymmetric/rsa4096.hpp>
#if _SDSDLL_PREPROCESSOR_GUARD

_SDSDLL_BEGIN
// FUNCTION _Bio_buffer constructors/destructor
_Bio_buffer::_Bio_buffer() noexcept : _Ptr(::BIO_new(::BIO_s_mem())) {}

_Bio_buffer::_Bio_buffer(BIO* const _Ptr) noexcept : _Ptr(_Ptr) {}

_Bio_buffer::~_Bio_buffer() noexcept {
    if (_Ptr) {
        ::BIO_free(_Ptr);
        _Ptr = nullptr;
    }
}

// FUNCTION _Rsa4096_public_key_to_buffer
_NODISCARD BIO* _Rsa4096_public_key_to_buffer(const EVP_PKEY* const _Key) {
    BIO* const _Buf = ::BIO_new(::BIO_s_mem());
    if (!_Buf) {
        return nullptr;
    }

    return ::PEM_write_bio_PUBKEY(_Buf, _Key) != 0 ? _Buf : nullptr;
}

// FUNCTION _Rsa4096_private_key_to_buffer
_NODISCARD BIO* _Rsa4096_private_key_to_buffer(const EVP_PKEY* const _Key) {
    BIO* const _Buf = ::BIO_new(::BIO_s_mem());
    if (!_Buf) {
        return nullptr;
    }

    return ::PEM_write_bio_PrivateKey(
        _Buf, _Key, nullptr, nullptr, 0, nullptr, nullptr) != 0 ? _Buf : nullptr;
}

// FUNCTION _Export_rsa4096_key_from_buffer
_NODISCARD string _Export_rsa4096_key_from_buffer(_Bio_buffer& _Bio_buf) {
    const int _Buf_size = BIO_pending(_Bio_buf._Ptr);
    _Sbo_buffer<char> _Buf(static_cast<size_t>(_Buf_size));
    if (_Buf._Empty()) { // allocation failed
        return string{};
    }

    return ::BIO_read(_Bio_buf._Ptr, _Buf._Get(), _Buf_size) == _Buf_size
        ? string{_Buf._Get(), static_cast<size_t>(_Buf_size)} : string{};
}

// FUNCTION _Import_rsa4096_key_to_buffer
_NODISCARD bool _Import_rsa4096_key_to_buffer(_Bio_buffer& _Bio_buf, const string& _Key) {
    const int _Key_size = static_cast<int>(_Key.size());
    return ::BIO_write(_Bio_buf._Ptr, _Key.c_str(), _Key_size) == _Key_size;
}

// FUNCTION _Bio_buffer_to_file
_NODISCARD bool _Bio_buffer_to_file(const path& _Target, _Bio_buffer& _Bio_buf) {
    if (_SDSDLL exists(_Target)) { // use an existing file
        if (!_SDSDLL clear_file(_Target)) {
            return false;
        }
    } else { // use a new file
        if (!_SDSDLL create_file(_Target)) {
            return false;
        }
    }

    const int _Buf_size = BIO_pending(_Bio_buf._Ptr);
    _Sbo_buffer<char> _Buf(static_cast<size_t>(_Buf_size));
    if (_Buf._Empty()) { // allocation failed
        return false;
    }

    if (::BIO_read(_Bio_buf._Ptr, _Buf._Get(), _Buf_size) != _Buf_size) {
        return false;
    }

    file _File(_Target);
    if (!_File.is_open()) {
        return false;
    }

    return _File.write(_Buf._Get(), static_cast<size_t>(_Buf_size));
}

// FUNCTION _File_to_bio_buffer
_NODISCARD bool _File_to_bio_buffer(const path& _Target, _Bio_buffer& _Bio_buf) {
    if (!_SDSDLL exists(_Target)) {
        return false;
    }

#ifdef _M_X64
    const size_t _File_size = _SDSDLL file_size(_Target);
#else // ^^^ _M_X64 ^^^ / vvv _M_IX86 vvv
    const size_t _File_size = static_cast<size_t>(_SDSDLL file_size(_Target));
#endif // _M_X64
    if (_File_size == 0) {
        return false;
    }

    file _File(_Target, file_access::read);
    if (!_File.is_open()) {
        return false;
    }

    _Sbo_buffer<char> _Buf(_File_size);
    if (_Buf._Empty()) { // allocation failed
        return false;
    }

    size_t _Read = 0; // read bytes
    if (!_File.read(_Buf._Get(), _File_size, _File_size, &_Read) || _Read != _File_size) {
        return false;
    }

    const int _Buf_size = static_cast<int>(_Buf._Size());
    return ::BIO_write(_Bio_buf._Ptr, _Buf._Get(), _Buf_size) == _Buf_size;
}

// FUNCTION _Rsa4096_key_context constructor/destructor
_Rsa4096_key_context::_Rsa4096_key_context(EVP_PKEY* const _Key) noexcept
    : _Ptr(_Key ? ::EVP_PKEY_CTX_new(_Key, nullptr) : ::EVP_PKEY_CTX_new_id(EVP_PKEY_RSA, nullptr)) {}

_Rsa4096_key_context::~_Rsa4096_key_context() noexcept {
    if (_Ptr) {
        ::EVP_PKEY_CTX_free(_Ptr);
        _Ptr = nullptr;
    }
}

// FUNCTION rsa4096_key constructors/destructor
rsa4096_key::rsa4096_key() noexcept : _Myhandle(nullptr) {}

rsa4096_key::rsa4096_key(const rsa4096_key& _Other) noexcept
    : _Myhandle(_STD exchange(const_cast<rsa4096_key&>(_Other)._Myhandle, nullptr)) {}

rsa4096_key::rsa4096_key(rsa4096_key&& _Other) noexcept
    : _Myhandle(_STD exchange(_Other._Myhandle, nullptr)) {}

rsa4096_key::~rsa4096_key() noexcept {
    unload();
}

// FUNCTION rsa4096_key::operator=
rsa4096_key& rsa4096_key::operator=(const rsa4096_key& _Other) noexcept {
    if (this != _SDSDLL addressof(_Other)) {
        _Myhandle                                  = _Other._Myhandle;
        const_cast<rsa4096_key&>(_Other)._Myhandle = nullptr;
    }

    return *this;
}

rsa4096_key& rsa4096_key::operator=(rsa4096_key&& _Other) noexcept {
    if (this != _SDSDLL addressof(_Other)) {
        _Myhandle        = _Other._Myhandle;
        _Other._Myhandle = nullptr;
    }

    return *this;
}

// FUNCTION rsa4096_key::good
_NODISCARD bool rsa4096_key::good() const noexcept {
    return _Myhandle != nullptr;
}

// FUNCTION rsa4096_key::native_handle
_NODISCARD rsa4096_key::native_handle_type rsa4096_key::native_handle() noexcept {
    return _Myhandle;
}

_NODISCARD const rsa4096_key::native_handle_type rsa4096_key::native_handle() const noexcept {
    return _Myhandle;
}

// FUNCTION rsa4096_key::unload
void rsa4096_key::unload() noexcept {
    if (_Myhandle) {
        ::EVP_PKEY_free(_Myhandle);
        _Myhandle = nullptr;
    }
}

// FUNCTION rsa4096_key::export_public
_NODISCARD string rsa4096_key::export_public() {
    if (!_Myhandle) {
        return string{};
    }

    _Bio_buffer _Bio_buf(_Rsa4096_public_key_to_buffer(_Myhandle));
    return _Bio_buf._Ptr ? _Export_rsa4096_key_from_buffer(_Bio_buf) : string{};
}

// FUNCTION rsa4096_key::export_private
_NODISCARD string rsa4096_key::export_private() {
    if (!_Myhandle) {
        return string{};
    }

    _Bio_buffer _Bio_buf(_Rsa4096_private_key_to_buffer(_Myhandle));
    return _Bio_buf._Ptr ? _Export_rsa4096_key_from_buffer(_Bio_buf) : string{};
}

// FUNCTION rsa4096_key::import_public
_NODISCARD bool rsa4096_key::import_public(const string& _Key) {
    if (_Myhandle) { // some key is already loaded
        return false;
    }

    _Bio_buffer _Bio_buf;
    if (!_Bio_buf._Ptr) {
        return false;
    }

    if (!_Import_rsa4096_key_to_buffer(_Bio_buf, _Key)) {
        return false;
    }

    _Myhandle = ::PEM_read_bio_PUBKEY(_Bio_buf._Ptr, nullptr, nullptr, nullptr);
    return _Myhandle != nullptr;
}

// FUNCTION rsa4096_key::import_private
_NODISCARD bool rsa4096_key::import_private(const string& _Key) {
    if (_Myhandle) { // some key is already loaded
        return false;
    }

    _Bio_buffer _Bio_buf;
    if (!_Bio_buf._Ptr) {
        return false;
    }

    if (!_Import_rsa4096_key_to_buffer(_Bio_buf, _Key)) {
        return false;
    }

    _Myhandle = ::PEM_read_bio_PrivateKey(_Bio_buf._Ptr, nullptr, nullptr, nullptr);
    return _Myhandle != nullptr;
}

// FUNCTION rsa4096_key::load_public
_NODISCARD bool rsa4096_key::load_public(const path& _Target) {
    if (_Myhandle) { // some key is already loaded
        return false;
    }

    _Bio_buffer _Bio_buf;
    if (!_Bio_buf._Ptr) {
        return false;
    }

    if (!_File_to_bio_buffer(_Target, _Bio_buf)) {
        return false;
    }

    _Myhandle = ::PEM_read_bio_PUBKEY(_Bio_buf._Ptr, nullptr, nullptr, nullptr);
    return _Myhandle != nullptr;
}

// FUNCTION rsa4096_key::load_private
_NODISCARD bool rsa4096_key::load_private(const path& _Target) {
    if (_Myhandle) { // some key is already loaded
        return false;
    }

    _Bio_buffer _Bio_buf;
    if (!_Bio_buf._Ptr) {
        return false;
    }

    if (!_File_to_bio_buffer(_Target, _Bio_buf)) {
        return false;
    }

    _Myhandle = ::PEM_read_bio_PrivateKey(_Bio_buf._Ptr, nullptr, nullptr, nullptr);
    return _Myhandle != nullptr;
}

// FUNCTION rsa4096_key::store_public
_NODISCARD bool rsa4096_key::store_public(const path& _Target) {
    if (!_Myhandle) { // no key is loaded
        return false;
    }

    _Bio_buffer _Bio_buf(_Rsa4096_public_key_to_buffer(_Myhandle));
    return _Bio_buf._Ptr ? _Bio_buffer_to_file(_Target, _Bio_buf) : false;
}

// FUNCTION rsa4096_key::store_private
_NODISCARD bool rsa4096_key::store_private(const path& _Target) {
    if (!_Myhandle) { // no key is loaded
        return false;
    }

    _Bio_buffer _Bio_buf(_Rsa4096_private_key_to_buffer(_Myhandle));
    return _Bio_buf._Ptr ? _Bio_buffer_to_file(_Target, _Bio_buf) : false;
}

// FUNCTION make_rsa4096_key
_NODISCARD rsa4096_key make_rsa4096_key() noexcept {
    _Rsa4096_key_context _Ctx(nullptr);
    if (!_Ctx._Ptr) {
        return rsa4096_key{};
    }

    if (::EVP_PKEY_keygen_init(_Ctx._Ptr) == 0) {
        return rsa4096_key{};
    }

    if (::EVP_PKEY_CTX_set_rsa_keygen_bits(_Ctx._Ptr, 4096) == 0) {
        return rsa4096_key{};
    }

    rsa4096_key _Key;
    return ::EVP_PKEY_keygen(
        _Ctx._Ptr, _SDSDLL addressof(_Key._Myhandle)) != 0 ? _Key : rsa4096_key{};
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

    if (!_Key.good()) {
        return false;
    }
    
    _Rsa4096_key_context _Ctx(_Key.native_handle());
    if (!_Ctx._Ptr) {
        return false;
    }

    if (::EVP_PKEY_encrypt_init(_Ctx._Ptr) <= 0) {
        return false;
    }

    if (::EVP_PKEY_CTX_set_rsa_padding(_Ctx._Ptr, RSA_PKCS1_OAEP_PADDING) <= 0) {
        return false;
    }

    if (::EVP_PKEY_CTX_set_rsa_oaep_md(_Ctx._Ptr, ::EVP_sha3_256()) <= 0) {
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
    return ::EVP_PKEY_encrypt(_Ctx._Ptr, _Buf, &_Bytes, _Raw_data.c_str(), _Raw_data.size()) > 0;
}

// FUNCTION TEMPLATE rsa4096_traits::decrypt
template <class _Elem>
_NODISCARD constexpr bool rsa4096_traits<_Elem>::decrypt(
    char_type* const _Buf, const size_type _Buf_size, const byte_type* const _Data,
    const size_type _Data_size, const key& _Key, size_type* const _Count) noexcept {
    if (_Data_size != bytes_count(0)) {
        return false;
    }
 
    if (!_Key.good()) {
        return false;
    }

    _Rsa4096_key_context _Ctx(_Key.native_handle());
    if (!_Ctx._Ptr) {
        return false;
    }

    if (::EVP_PKEY_decrypt_init(_Ctx._Ptr) <= 0) {
        return false;
    }

    if (::EVP_PKEY_CTX_set_rsa_padding(_Ctx._Ptr, RSA_PKCS1_OAEP_PADDING) <= 0) {
        return false;
    }

    if (::EVP_PKEY_CTX_set_rsa_oaep_md(_Ctx._Ptr, ::EVP_sha3_256()) <= 0) {
        return false;
    }

    size_type _Bytes = 0; // decrypted bytes
    if (::EVP_PKEY_decrypt(_Ctx._Ptr, nullptr, &_Bytes, _Data, _Data_size) <= 0) {
        return false;
    }

    _Sbo_buffer<byte_type> _Temp_buf(_Bytes);
    if (_Temp_buf._Empty()) { // allocation failed
        return false;
    }
    
    if (::EVP_PKEY_decrypt(_Ctx._Ptr, _Temp_buf._Get(), &_Bytes, _Data, _Data_size) <= 0) {
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