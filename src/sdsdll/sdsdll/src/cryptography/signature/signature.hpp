// signature.hpp

// Copyright (c) Mateusz Jandura. All rights reserved.
// SPDX-License-Identifier: Apache-2.0

#pragma once
#ifndef _SDSDLL_CRYPTOGRAPHY_SIGNATURE_SIGNATURE_HPP_
#define _SDSDLL_CRYPTOGRAPHY_SIGNATURE_SIGNATURE_HPP_
#include <core/defs.hpp>
#if _SDSDLL_PREPROCESSOR_GUARD
#include <core/api.hpp>
#include <core/traits/string_traits.hpp>
#include <cryptography/cipher/asymmetric/rsa4096.hpp>
#include <cryptography/hash/generic.hpp>
#include <cryptography/hash/generic/sha512.hpp>
#include <cstdint>
#include <filesystem/file.hpp>
#include <filesystem/path.hpp>
#include <openssl/evp.h>
#include <openssl/types.h>

_SDSDLL_BEGIN
// ENUM CLASS verification_result
enum class verification_result : unsigned char {
    unknown,
    fake,
    authentic
};

// CLASS _Signature_context_proxy
class _Signature_context_proxy {
public:
    EVP_MD_CTX* _Ctx;

    _Signature_context_proxy() noexcept;
    ~_Signature_context_proxy() noexcept;
};

// STRUCT _Signature_traits
struct _Signature_traits {
    using char_type       = char;
    using byte_type       = unsigned char;
    using char_string     = basic_string<char>;
    using byte_string     = basic_string<unsigned char>;
    using size_type       = size_t;
    using difference_type = ptrdiff_t;
    using key             = rsa4096_key;

    // returns signed data length
    _NODISCARD static size_type _Bytes_count() noexcept;

    // returns data max length
    _NODISCARD static size_type _Max_chars_count() noexcept;

    // tries to sign a data
    _NODISCARD static bool _Sign(byte_type* const _Buf, const size_type _Buf_size,
        const byte_type* const _Data, const size_type _Data_size, const key& _Key) noexcept;

    // tries to verify a data
    _NODISCARD static verification_result _Verify(
        const byte_type* const _Signature, const size_type _Signature_size,
        const byte_type* const _Data, const size_type _Data_size, const key& _Key) noexcept;
};

// FUNCTION TEMPLATE sign_data
template <class _Elem>
_SDSDLL_API _NODISCARD constexpr byte_string sign_data(
    const _Elem* const _Data, const size_t _Size, const rsa4096_key& _Key);

template <class _Elem>
_SDSDLL_API _NODISCARD constexpr byte_string sign_data(
    const _Elem* const _Data, const rsa4096_key& _Key);

template <class _Elem>
_SDSDLL_API _NODISCARD constexpr byte_string sign_data(
    const basic_string_view<_Elem>, const rsa4096_key& _Key);

template <class _Elem>
_SDSDLL_API _NODISCARD constexpr byte_string sign_data(
    const basic_string<_Elem>& _Data, const rsa4096_key& _Key);

// FUNCTION TEMPLATE verify_data
template <class _Elem>
_SDSDLL_API _NODISCARD constexpr verification_result verify_data(const _Elem* const _Data,
    const size_t _Size, const rsa4096_key& _Key, const byte_string& _Signature);

template <class _Elem>
_SDSDLL_API _NODISCARD constexpr verification_result verify_data(
    const _Elem* const _Data, const rsa4096_key& _Key, const byte_string& _Signature);

template <class _Elem>
_SDSDLL_API _NODISCARD constexpr verification_result verify_data(
    const basic_string_view<_Elem> _Data, const rsa4096_key& _Key, const byte_string& _Signature);

template <class _Elem>
_SDSDLL_API _NODISCARD constexpr verification_result verify_data(
    const basic_string<_Elem>& _Data, const rsa4096_key& _Key, const byte_string& _Signature);

// FUNCTION sign_file
_SDSDLL_API _NODISCARD byte_string sign_file(
    file& _File, const rsa4096_key& _Key, const file::off_type _Off = 0);
_SDSDLL_API _NODISCARD byte_string sign_file(
    const path& _Target, const rsa4096_key& _Key, const file::off_type _Off = 0);

// FUNCTION verify_file
_SDSDLL_API _NODISCARD verification_result verify_file(file& _File,
    const rsa4096_key& _Key, const byte_string& _Signature, const file::off_type _Off = 0);

_SDSDLL_API _NODISCARD verification_result verify_file(const path& _Target,
    const rsa4096_key& _Key, const byte_string& _Signature, const file::off_type _Off = 0);
_SDSDLL_END

#endif // _SDSDLL_PREPROCESSOR_GUARD
#endif // _SDSDLL_CRYPTOGRAPHY_SIGNATURE_SIGNATURE_HPP_