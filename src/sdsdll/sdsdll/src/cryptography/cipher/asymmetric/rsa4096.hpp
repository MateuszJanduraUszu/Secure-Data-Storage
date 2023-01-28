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
#include <openssl/bio.h>
#include <openssl/evp.h>
#include <openssl/pem.h>
#include <openssl/rsa.h>
#include <string>
#include <type_traits>
#include <utility>

// STD types
using _STD basic_string;
using _STD string;

_SDSDLL_BEGIN
// STRUCT _Bio_buffer_proxy
struct _Bio_buffer_proxy {
    BIO* _Buf;

    _Bio_buffer_proxy() noexcept;
    ~_Bio_buffer_proxy() noexcept;
};

// CLASS _Rsa4096_key_proxy
class _Rsa4096_key_proxy {
public:
    EVP_PKEY* _Key;

    explicit _Rsa4096_key_proxy(EVP_PKEY* const _Key) noexcept;
    ~_Rsa4096_key_proxy() noexcept;
};

// CLASS _Rsa4096_key_context_proxy
class _Rsa4096_key_context_proxy {
public:
    EVP_PKEY_CTX* _Ctx;

    explicit _Rsa4096_key_context_proxy(EVP_PKEY* const _Key) noexcept;
    ~_Rsa4096_key_context_proxy() noexcept;
};

// FUNCTION _Make_raw_rsa4096_key_pair
extern _NODISCARD EVP_PKEY* _Make_raw_rsa4096_key_pair() noexcept;

// FUNCTION _Extract_private_key_from_raw_rsa4096_key_pair
extern _NODISCARD RSA* _Extract_private_key_from_raw_rsa4096_key_pair(
    const EVP_PKEY* const _Raw) noexcept;

// FUNCTION _Extract_public_key_from_raw_rsa4096_key_pair
extern _NODISCARD RSA* _Extract_public_key_from_raw_rsa4096_key_pair(
    const EVP_PKEY* const _Raw) noexcept;

// FUNCTION _Cast_rsa4096_key_handle
extern _NODISCARD EVP_PKEY* _Cast_rsa4096_key_handle(RSA* const _Handle) noexcept;

// CLASS rsa4096_key_pair
class rsa4096_key_pair;

// CLASS rsa4096_key
class _SDSDLL_API rsa4096_key { // stores an RSA-4096 private or public key
public:
    using native_handle_type = RSA*;

    rsa4096_key() noexcept;
    rsa4096_key(const rsa4096_key& _Other) noexcept;
    rsa4096_key(rsa4096_key&& _Other) noexcept;
    ~rsa4096_key() noexcept;

    rsa4096_key& operator=(const rsa4096_key& _Other) noexcept;
    rsa4096_key& operator=(rsa4096_key&& _Other) noexcept;

    enum type : unsigned char {
        none,
        private_key,
        public_key
    };

    // checks if the handle is good
    _NODISCARD bool good() const noexcept;

    // returns a handle to the native implementation
    _NODISCARD const native_handle_type native_handle() const noexcept;

    // returns the key type
    _NODISCARD const type key_type() const noexcept;

    // unloads the current key
    void unload() noexcept;

    // copies the key
    _NODISCARD bool copy(const rsa4096_key& _Other) noexcept;
    _NODISCARD bool copy(rsa4096_key&& _Other) noexcept;

    // resets the key
    native_handle_type reset() noexcept;

    // returns the key in Base64 format
    _NODISCARD string load() const;

    // stores a new key
    _NODISCARD bool store(const string& _New_key, const type _New_type);

private:
    friend _SDSDLL_API rsa4096_key_pair make_rsa4096_key_pair() noexcept;

    native_handle_type _Myimpl;
    type _Mytype;
};

// CLASS rsa4096_key_pair
class _SDSDLL_API rsa4096_key_pair { // stores a RSA-4096 key pair
public:
    rsa4096_key_pair() noexcept;
    rsa4096_key_pair(const rsa4096_key_pair& _Other) noexcept;
    rsa4096_key_pair(rsa4096_key_pair&& _Other) noexcept;
    ~rsa4096_key_pair() noexcept;

    explicit rsa4096_key_pair(const rsa4096_key& _Private, const rsa4096_key& _Public) noexcept;
    explicit rsa4096_key_pair(rsa4096_key&& _Private, rsa4096_key&& _Public) noexcept;

    rsa4096_key_pair& operator=(const rsa4096_key_pair& _Other) noexcept;
    rsa4096_key_pair& operator=(rsa4096_key_pair&& _Other) noexcept;

    // checks if the keys are good
    _NODISCARD bool good() const noexcept;

    // returns the private key
    _NODISCARD rsa4096_key& private_key() noexcept;

    // returns the private key for view
    _NODISCARD const rsa4096_key& private_key() const noexcept;

    // returns the public key
    _NODISCARD rsa4096_key& public_key() noexcept;

    // returns the public key for view
    _NODISCARD const rsa4096_key& public_key() const noexcept;

private:
    rsa4096_key _Mypriv;
    rsa4096_key _Mypub;
};

// FUNCTION make_rsa4096_key_pair
_SDSDLL_API _NODISCARD rsa4096_key_pair make_rsa4096_key_pair() noexcept;

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