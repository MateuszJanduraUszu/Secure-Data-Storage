// hardware_rsa2048.cpp

// Copyright (c) Mateusz Jandura. All rights reserved.
// SPDX-License-Identifier: Apache-2.0

#include <build/sdsdll_pch.hpp>
#include <cryptography/cipher/asymmetric/hardware_rsa2048.hpp>
#if _SDSDLL_PREPROCESSOR_GUARD

_SDSDLL_BEGIN
// FUNCTION _Hardware_storage constructor/destructor
_Hardware_storage::_Hardware_storage() noexcept : _Myhandle(0) {
    (void) ::NCryptOpenStorageProvider(&_Myhandle, MS_PLATFORM_KEY_STORAGE_PROVIDER, 0);
}

_Hardware_storage::~_Hardware_storage() noexcept {
    if (_Myhandle != 0) {
        ::NCryptFreeObject(_Myhandle);
        _Myhandle = 0;
    }
}

// FUNCTION _Hardware_storage::_Good
_NODISCARD bool _Hardware_storage::_Good() const noexcept {
    return _Myhandle != 0;
}

// FUNCTION _Hardware_storage::_Get
_NODISCARD const NCRYPT_PROV_HANDLE _Hardware_storage::_Get() const noexcept {
    return _Myhandle;
}

// FUNCTION _Create_persisted_key
_NODISCARD SECURITY_STATUS _Create_persisted_key(_Hardware_storage& _Storage,
    const wchar_t* const _Name, DWORD _Size, NCRYPT_KEY_HANDLE& _Handle) noexcept {
    // Note: Returns hardware_rsa2048_key::store_status as SECURITY_STATUS;
    if (_Size < 512 || _Size > 2048) { // must be at least 512 and at most 2048 (TPM 2.0 limits)
        return 2; // bad_data
    }

    if (_Size % 2 != 0) { // must be a power of 2
        return 2; // bad_data
    }

    switch (::NCryptCreatePersistedKey(
        _Storage._Get(), &_Handle, NCRYPT_RSA_ALGORITHM, _Name, 0, NCRYPT_MACHINE_KEY_FLAG)) {
    case ERROR_SUCCESS: // no error
        break;
    case NTE_EXISTS:
        return 1; // already_exists
    default:
        return 3; // generic_error
    }

    switch (::NCryptSetProperty(_Handle, NCRYPT_LENGTH_PROPERTY,
        reinterpret_cast<uint8_t*>(&_Size), sizeof(DWORD), NCRYPT_PERSIST_FLAG)) {
    case ERROR_SUCCESS: // no error
        break;
    case NTE_INVALID_PARAMETER:
        return 2; // bad_data
    default:
        return 3; // generic_error
    }

    return ::NCryptFinalizeKey(_Handle, 0) == ERROR_SUCCESS ? 0 : 3; // success or generic_error
}

// FUNCTION hardware_rsa2048_key constructors/destructor
hardware_rsa2048_key::hardware_rsa2048_key() noexcept : _Mystorage(), _Myhandle(0) {}

hardware_rsa2048_key::hardware_rsa2048_key(const hardware_rsa2048_key& _Other) noexcept
    : _Mystorage(), _Myhandle(_STD exchange(const_cast<hardware_rsa2048_key&>(_Other)._Myhandle, 0)) {}

hardware_rsa2048_key::hardware_rsa2048_key(hardware_rsa2048_key&& _Other) noexcept
    : _Mystorage(), _Myhandle(_STD exchange(_Other._Myhandle, 0)) {}

hardware_rsa2048_key::~hardware_rsa2048_key() noexcept {
    unload();
}

// FUNCTION hardware_rsa2048_key::operator=
hardware_rsa2048_key& hardware_rsa2048_key::operator=(const hardware_rsa2048_key& _Other) noexcept {
    if (this != _SDSDLL addressof(_Other)) {
        _Myhandle                                           = _Other._Myhandle;
        const_cast<hardware_rsa2048_key&>(_Other)._Myhandle = 0;
    }

    return *this;
}

hardware_rsa2048_key& hardware_rsa2048_key::operator=(hardware_rsa2048_key&& _Other) noexcept {
    if (this != _SDSDLL addressof(_Other)) {
        _Myhandle                                           = _Other._Myhandle;
        const_cast<hardware_rsa2048_key&>(_Other)._Myhandle = 0;
    }

    return *this;
}

// FUNCTION hardware_rsa2048_key::good
_NODISCARD bool hardware_rsa2048_key::good() const noexcept {
    return _Myhandle != 0;
}

// FUNCTION hardware_rsa2048_key::native_handle
_NODISCARD const typename hardware_rsa2048_key::native_handle_type
    hardware_rsa2048_key::native_handle() const noexcept {
    return _Myhandle;
}

// FUNCTION hardware_rsa2048_key::load
_NODISCARD bool hardware_rsa2048_key::load(const wchar_t* const _Name) noexcept {
    if (_Myhandle != 0) { // some handle is already open
        return false;
    }

    return ::NCryptOpenKey(
        _Mystorage._Get(), &_Myhandle, _Name, 0, NCRYPT_MACHINE_KEY_FLAG) == ERROR_SUCCESS;
}

_NODISCARD bool hardware_rsa2048_key::load(const wstring_view _Name) noexcept {
    return load(_Name.data());
}

_NODISCARD bool hardware_rsa2048_key::load(const wstring& _Name) {
    return load(_Name.c_str());
}

// FUNCTION hardware_rsa2048_key::unload
void hardware_rsa2048_key::unload() noexcept {
    if (_Myhandle != 0) {
        ::NCryptFreeObject(_Myhandle);
        _Myhandle = 0;
    }
}

// FUNCTION hardware_rsa2048_key::store
_NODISCARD typename hardware_rsa2048_key::store_status hardware_rsa2048_key::store(
    const wchar_t* const _Name) noexcept {
    // Note: A new key handle will be assigned to the _Myhandle.
    if (_Myhandle != 0) { // some key is alread open
        return generic_error;
    }

    const store_status _Status = static_cast<store_status>(
        _Create_persisted_key(_Mystorage, _Name, 2048, _Myhandle));
    switch (_Status) {
    case success:
        return _Status;
    default:
        unload(); // handle could be assigned
        return _Status;
    }
}

_NODISCARD typename hardware_rsa2048_key::store_status hardware_rsa2048_key::store(
    const wstring_view _Name) noexcept {
    return store(_Name.data());
}

_NODISCARD typename hardware_rsa2048_key::store_status hardware_rsa2048_key::store(const wstring& _Name) {
    return store(_Name.c_str());
}

// FUNCTION hardware_rsa2048_key::remove
_NODISCARD bool hardware_rsa2048_key::remove() noexcept {
    if (_Myhandle == 0) {
        return false;
    }

    if (::NCryptDeleteKey(_Myhandle, 0) == ERROR_SUCCESS) {
        _Myhandle = 0;
        return true;
    } else {
        return false;
    }
}

// FUNCTION TEMPLATE hardware_rsa2048_traits::bytes_count
template <class _Elem>
_NODISCARD constexpr typename hardware_rsa2048_traits<_Elem>::size_type
    hardware_rsa2048_traits<_Elem>::bytes_count(const size_type) noexcept {
    // always 2048-bit (256-byte) cipher text
    return bits / CHAR_BIT;
}

// FUNCTION TEMPLATE hardware_rsa2048_traits::chars_count
template <class _Elem>
_NODISCARD constexpr typename hardware_rsa2048_traits<_Elem>::size_type
    hardware_rsa2048_traits<_Elem>::chars_count(const size_type) noexcept {
    // always at most 2048-bit (256-byte) - 11 bytes (PKCS#1 padding) plain text
    return (bits / CHAR_BIT) - 11;
}

// FUNCTION TEMPLATE hardware_rsa2048_traits::max_chars_count
template <class _Elem>
_NODISCARD constexpr typename hardware_rsa2048_traits<_Elem>::size_type
hardware_rsa2048_traits<_Elem>::max_chars_count() noexcept {
    // always at most 2048-bit (256-byte) - 11 bytes (PKCS#1 padding) plain text
    return (bits / CHAR_BIT) - 11;
}

// FUNCTION TEMPLATE hardware_rsa2048_traits::encrypt
template <class _Elem>
_NODISCARD constexpr bool hardware_rsa2048_traits<_Elem>::encrypt(
    byte_type* const _Buf, const size_type _Buf_size, const char_type* const _Data,
    const size_type _Data_size, const key& _Key) noexcept {
    if (_Data_size > max_chars_count() || _Buf_size < bytes_count(0)) {
        return false;
    }

    DWORD _Bytes = 0; // encrypted bytes (unused)
    if constexpr (sizeof(_Elem) == 1) { // encrypt a UTF-8 text
        return ::NCryptEncrypt(_Key.native_handle(), const_cast<byte_type*>(
            reinterpret_cast<const byte_type*>(_Data)), static_cast<DWORD>(_Data_size), nullptr,
                _Buf, static_cast<DWORD>(_Buf_size), &_Bytes, NCRYPT_PAD_PKCS1_FLAG) == ERROR_SUCCESS;
    } else { // encrypt a Unicode text
        const utf8_string& _Narrow = utf8_string::from_utf16(_Data, _Data_size);
        return ::NCryptEncrypt(_Key.native_handle(), const_cast<byte_type*>(
            reinterpret_cast<const byte_type*>(_Narrow.c_str())), static_cast<DWORD>(_Narrow.size()),
                nullptr, _Buf, static_cast<DWORD>(_Buf_size), &_Bytes,
                    NCRYPT_PAD_PKCS1_FLAG) == ERROR_SUCCESS;
    }
}

// FUNCTION TEMPLATE hardware_rsa2048_traits::decrypt
template <class _Elem>
_NODISCARD constexpr bool hardware_rsa2048_traits<_Elem>::decrypt(
    char_type* const _Buf, const size_type _Buf_size, const byte_type* const _Data,
    const size_type _Data_size, const key& _Key, size_type* const _Count) noexcept {
    if (_Data_size != bytes_count(0)) {
        return false;
    }

    if constexpr (sizeof(_Elem) == 1) { // decrypt a UTF-8 text
        return ::NCryptDecrypt(_Key.native_handle(), const_cast<byte_type*>(_Data),
            static_cast<DWORD>(_Data_size), nullptr, reinterpret_cast<byte_type*>(_Buf),
                static_cast<DWORD>(_Buf_size), reinterpret_cast<DWORD*>(_Count),
                    NCRYPT_PAD_PKCS1_FLAG) == ERROR_SUCCESS;
    } else { // decrypt a Unicode text
        constexpr size_type _Temp_buf_size  = chars_count(0) + 1;
        byte_type _Temp_buf[_Temp_buf_size] = {0}; // max buffer size + null-terminator
        if (::NCryptDecrypt(_Key.native_handle(), const_cast<byte_type*>(_Data),
            static_cast<DWORD>(_Data_size), nullptr, _Temp_buf, static_cast<DWORD>(_Temp_buf_size),
            reinterpret_cast<DWORD*>(_Count), NCRYPT_PAD_PKCS1_FLAG) != ERROR_SUCCESS) {
            return false;
        }

        using _Traits             = string_traits<char_type, size_type>;
        const utf16_string& _Wide = utf16_string::from_utf8(
            reinterpret_cast<const char*>(_Temp_buf), *_Count);
        *_Count                   = _Wide.size();
        _Traits::copy(_Buf, _Wide.c_str(), _Wide.size());
        return true;
    }
}

template struct _SDSDLL_API hardware_rsa2048_traits<char>;
template struct _SDSDLL_API hardware_rsa2048_traits<unsigned char>;
template struct _SDSDLL_API hardware_rsa2048_traits<wchar_t>;
_SDSDLL_END

#endif // _SDSDLL_PREPROCESSOR_GUARD