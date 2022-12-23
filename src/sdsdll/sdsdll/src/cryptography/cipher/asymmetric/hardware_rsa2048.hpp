// hardware_rsa2048.hpp

// Copyright (c) Mateusz Jandura. All rights reserved.
// SPDX-License-Identifier: Apache-2.0

#pragma once
#ifndef _SDSDLL_CRYPTOGRAPHY_CIPHER_ASYMMETRIC_HARDWARE_RSA2048_HPP_
#define _SDSDLL_CRYPTOGRAPHY_CIPHER_ASYMMETRIC_HARDWARE_RSA2048_HPP_
#include <core/defs.hpp>
#if _SDSDLL_PREPROCESSOR_GUARD
#include <climits>
#include <core/api.hpp>
#include <core/optimization/sbo.hpp>
#include <core/optimization/string_view.hpp>
#include <core/traits/type_traits.hpp>
#include <cstddef>
#include <cstdint>
#include <encoding/utf16.hpp>
#include <encoding/utf8.hpp>
#include <ncrypt.h>
#include <string>
#include <utility>

// STD types
using _STD basic_string;
using _STD string;
using _STD wstring;

_SDSDLL_BEGIN
// CLASS _Hardware_storage
class _Hardware_storage { // platform (TPM 2.0) key storage
public:
    _Hardware_storage() noexcept;
    ~_Hardware_storage() noexcept;

    _Hardware_storage(const _Hardware_storage&) = delete;
    _Hardware_storage& operator=(const _Hardware_storage&) = delete;

    // checks if the handle is good
    _NODISCARD bool _Good() const noexcept;

    // returns handle to the storage
    _NODISCARD const NCRYPT_PROV_HANDLE _Get() const noexcept;

private:
    NCRYPT_PROV_HANDLE _Myhandle;
};

// FUNCTION _Create_persisted_key
extern _NODISCARD SECURITY_STATUS _Create_persisted_key(_Hardware_storage& _Storage,
    const wchar_t* const _Name, DWORD _Size, NCRYPT_KEY_HANDLE& _Handle) noexcept;

// CLASS hardware_rsa2048_key
class _SDSDLL_API hardware_rsa2048_key { // stores handle to the hardware RSA-2048 key
public:
    using native_handle_type = NCRYPT_KEY_HANDLE;

    hardware_rsa2048_key() noexcept;
    hardware_rsa2048_key(const hardware_rsa2048_key& _Other) noexcept;
    hardware_rsa2048_key(hardware_rsa2048_key&& _Other) noexcept;
    ~hardware_rsa2048_key() noexcept;

    hardware_rsa2048_key& operator=(const hardware_rsa2048_key& _Other) noexcept;
    hardware_rsa2048_key& operator=(hardware_rsa2048_key&& _Other) noexcept;

    // checks if the handle is good
    _NODISCARD bool good() const noexcept;

    // returns handle to the key
    _NODISCARD const native_handle_type native_handle() const noexcept;

    // loads a key from the storage
    _NODISCARD bool load(const wchar_t* const _Name) noexcept;
    _NODISCARD bool load(const wstring_view _Name) noexcept;
    _NODISCARD bool load(const wstring& _Name);

    // unloads the current key
    void unload() noexcept;

    enum store_status : unsigned char {
        success,
        already_exists,
        bad_data,
        generic_error
    };

    // stores a key to the storage
    _NODISCARD store_status store(const wchar_t* const _Name) noexcept;
    _NODISCARD store_status store(const wstring_view _Name) noexcept;
    _NODISCARD store_status store(const wstring& _Name);

    // tries to remove a key from the storage
    _NODISCARD bool remove() noexcept;

private:
    _Hardware_storage _Mystorage;
    native_handle_type _Myhandle;
};

// STRUCT TEMPLATE hardware_rsa2048_traits
template <class _Elem>
struct _SDSDLL_API hardware_rsa2048_traits { // traits for the hardware (TPM 2.0) RSA-2048
private:
    static_assert(is_any_of_v<_Elem, char, unsigned char, wchar_t>,
        "Requires a byte/UTF-8/Unicode element type.");

public:
    using char_type       = _Elem;
    using byte_type	      = unsigned char;
    using char_string     = basic_string<_Elem>;
    using byte_string     = basic_string<unsigned char>;
    using size_type       = size_t;
    using difference_type = ptrdiff_t;
    using key             = hardware_rsa2048_key;

    static constexpr size_type bits = 2048;

    // returns encrypted text length
    _NODISCARD static constexpr size_type bytes_count(const size_type _Count) noexcept;

    // returns decrypted text length
    _NODISCARD static constexpr size_type chars_count(const size_type _Count) noexcept;

    // returns plain text max length
    _NODISCARD static constexpr size_type max_chars_count() noexcept;

    // encrypts a UTF-8/Unicode text
    _NODISCARD static constexpr bool encrypt(byte_type* const _Buf, const size_type _Buf_size,
        const char_type* const _Data, const size_type _Data_size, const key& _Key) noexcept;

    // decrypts a UTF-8/Unicode text
    _NODISCARD static constexpr bool decrypt(
        char_type* const _Buf, const size_type _Buf_size, const byte_type* const _Data,
        const size_type _Data_size, const key& _Key, size_type* const _Count = nullptr) noexcept;
};
_SDSDLL_END

#endif // _SDSDLL_PREPROCESSOR_GUARD
#endif // _SDSDLL_CRYPTOGRAPHY_CIPHER_ASYMMETRIC_HARDWARE_RSA2048_HPP_