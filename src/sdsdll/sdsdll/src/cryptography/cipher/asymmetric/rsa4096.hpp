// rsa4096.hpp

// Copyright (c) Mateusz Jandura. All rights reserved.
// SPDX-License-Identifier: Apache-2.0

#pragma once
#ifndef _SDSDLL_CRYPTOGRAPHY_CIPHER_ASYMMETRIC_RSA4096_HPP_
#define _SDSDLL_CRYPTOGRAPHY_CIPHER_ASYMMETRIC_RSA4096_HPP_
#include <core/defs.hpp>
#if _SDSDLL_PREPROCESSOR_GUARD
#include <climits>
#include <core/api.hpp>
#include <core/optimization/sbo.hpp>
#include <core/traits/concepts.hpp>
#include <core/traits/memory_traits.hpp>
#include <core/traits/type_traits.hpp>
#include <cstddef>
#include <cstdint>
#include <encoding/utf16.hpp>
#include <encoding/utf8.hpp>
#include <filesystem/file.hpp>
#include <filesystem/path.hpp>
#include <filesystem/status.hpp>
#include <openssl/bio.h>
#include <openssl/evp.h>
#include <openssl/pem.h>
#include <openssl/rsa.h>
#include <string>
#include <utility>

// STD types
using _STD basic_string;
using _STD string;

_SDSDLL_BEGIN
// STRUCT _Bio_buffer
struct _Bio_buffer {
    BIO* _Ptr;

    _Bio_buffer() noexcept;
    explicit _Bio_buffer(BIO* const _Ptr) noexcept;
    ~_Bio_buffer() noexcept;
};

// FUNCTION _Rsa4096_public_key_to_buffer
extern _NODISCARD BIO* _Rsa4096_public_key_to_buffer(const EVP_PKEY* const _Key);

// FUNCTION _Rsa4096_private_key_to_buffer
extern _NODISCARD BIO* _Rsa4096_private_key_to_buffer(const EVP_PKEY* const _Key);

// FUNCTION _Export_rsa4096_key_from_buffer
extern _NODISCARD string _Export_rsa4096_key_from_buffer(_Bio_buffer& _Bio_buf);

// FUNCTION _Import_rsa4096_key_to_buffer
extern _NODISCARD bool _Import_rsa4096_key_to_buffer(
    _Bio_buffer& _Bio_buf, const string& _Key);

// FUNCTION _Bio_buffer_to_file
extern _NODISCARD bool _Bio_buffer_to_file(const path& _Target, _Bio_buffer& _Bio_buf);

// FUNCTION _File_to_bio_buffer
extern _NODISCARD bool _File_to_bio_buffer(const path& _Target, _Bio_buffer& _Bio_buf);

// CLASS _Rsa4096_key_context
class _Rsa4096_key_context {
public:
    EVP_PKEY_CTX* _Ptr;

    explicit _Rsa4096_key_context(EVP_PKEY* const _Key) noexcept;
    ~_Rsa4096_key_context() noexcept;
};

// CLASS rsa4096_key
class _SDSDLL_API rsa4096_key {
public:
    using native_handle_type = EVP_PKEY*;

    rsa4096_key() noexcept;
    rsa4096_key(const rsa4096_key& _Other) noexcept;
    rsa4096_key(rsa4096_key&& _Other) noexcept;
    ~rsa4096_key() noexcept;

    rsa4096_key& operator=(const rsa4096_key& _Other) noexcept;
    rsa4096_key& operator=(rsa4096_key&& _Other) noexcept;

    // checks if the handle is good
    _NODISCARD bool good() const noexcept;

    // returns a native handle to the key
    _NODISCARD native_handle_type native_handle() noexcept;
    
    // returns a native handle to the key for view
    _NODISCARD const native_handle_type native_handle() const noexcept;

    // unloads the current key
    void unload() noexcept;

    // exports the public key
    _NODISCARD string export_public();

    // exports the private key
    _NODISCARD string export_private();

    // imports the public key
    _NODISCARD bool import_public(const string& _Key);

    // imports the private key
    _NODISCARD bool import_private(const string& _Key);

    // loads the public key from a file
    _NODISCARD bool load_public(const path& _Target);

    // loads the private key from a file
    _NODISCARD bool load_private(const path& _Target);

    // stores the public key to a file
    _NODISCARD bool store_public(const path& _Target);

    // stores the private key to a file
    _NODISCARD bool store_private(const path& _Target);

private:
    native_handle_type _Myhandle;

    friend _SDSDLL_API rsa4096_key make_rsa4096_key() noexcept;
};

// FUNCTION make_rsa4096_key
_SDSDLL_API _NODISCARD rsa4096_key make_rsa4096_key() noexcept;

// STRUCT TEMPLATE rsa4096_traits
template <class _Elem>
struct _SDSDLL_API rsa4096_traits { // traits for the RSA-4096 encryption/decryption
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
    using key             = rsa4096_key;

    static constexpr size_type bits = 4096;

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
#endif // _SDSDLL_CRYPTOGRAPHY_CIPHER_ASYMMETRIC_RSA4096_HPP_