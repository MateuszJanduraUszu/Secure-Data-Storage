// aes256_gcm.hpp

// Copyright (c) Mateusz Jandura. All rights reserved.
// SPDX-License-Identifier: Apache-2.0

#pragma once
#ifndef _SDSDLL_CRYPTOGRAPHY_CIPHER_SYMMETRIC_AES256_GCM_HPP_
#define _SDSDLL_CRYPTOGRAPHY_CIPHER_SYMMETRIC_AES256_GCM_HPP_
#include <core/defs.hpp>
#if _SDSDLL_PREPROCESSOR_GUARD
#include <core/api.hpp>
#include <core/optimization/ebco.hpp>
#include <core/optimization/sbo.hpp>
#include <core/optimization/string_view.hpp>
#include <core/traits/concepts.hpp>
#include <core/traits/memory_traits.hpp>
#include <core/traits/string_traits.hpp>
#include <cryptography/cipher/cipher_types.hpp>
#include <cryptography/cipher/symmetric/aes_key.hpp>
#include <cryptography/cipher/symmetric/iv.hpp>
#include <cstddef>
#include <encoding/utf8.hpp>
#include <openssl/evp.h>
#include <string>

// STD types
using _STD basic_string;
using _STD string;
using _STD wstring;

_SDSDLL_BEGIN
// FUNCTION _Get_aes256_gcm_tag
extern _NODISCARD bool _Get_aes256_gcm_tag(_Cipher_context_proxy& _Proxy, uint8_t* const _Buf) noexcept;

// FUNCTION _Set_aes256_gcm_tag
extern _NODISCARD bool _Set_aes256_gcm_tag(_Cipher_context_proxy& _Proxy, uint8_t* const _Tag) noexcept;

// CLASS TEMPLATE _Aes256_gcm_fn
template <class _Simple_types>
class _Aes256_gcm_fn { // wrapper for the EVP AES-256 GCM functions
public:
    using char_type       = typename _Simple_types::char_type;
    using byte_type       = typename _Simple_types::byte_type;
    using char_string     = typename _Simple_types::char_string;
    using byte_string     = typename _Simple_types::byte_string;
    using size_type       = typename _Simple_types::size_type;
    using difference_type = typename _Simple_types::difference_type;
    using engine          = typename _Simple_types::engine;
    using key             = typename _Simple_types::key;
    using iv              = typename _Simple_types::iv;

    constexpr _Aes256_gcm_fn() noexcept     = default;
    _CONSTEXPR20 ~_Aes256_gcm_fn() noexcept = default;

    _Aes256_gcm_fn(const _Aes256_gcm_fn&) = delete;
    _Aes256_gcm_fn& operator=(const _Aes256_gcm_fn&) = delete;

    // tries to encrypt/decrypt the data
    _NODISCARD constexpr bool operator()(byte_type* const _Buf, size_type _Buf_size,
        const byte_type* const _Data, const size_type _Data_size, const key& _Key,
        const iv& _Iv, const cipher_task _Task) const noexcept;
};

// CLASS TEMPLATE _Aes256_gcm_default_engine
template <class _Elem>
class _Aes256_gcm_default_engine { // default engine for the AES-256 GCM algorithm
private:
    static_assert(is_any_of_v<_Elem, char, uint8_t, wchar_t>, "Requires UTF-8/UTF-16 element type.");

    using _Simple_types = _Simple_cipher_types<_Elem, _Aes256_gcm_default_engine<_Elem>, aes_key<32>, iv<12>>;
    using _Fn           = _Aes256_gcm_fn<_Simple_types>;

public:
    using char_type       = typename _Simple_types::char_type;
    using byte_type       = typename _Simple_types::byte_type;
    using char_string     = typename _Simple_types::char_string;
    using byte_string     = typename _Simple_types::byte_string;
    using size_type       = typename _Simple_types::size_type;
    using difference_type = typename _Simple_types::difference_type;
    using engine          = typename _Simple_types::engine;
    using key             = typename _Simple_types::key;
    using iv              = typename _Simple_types::iv;

    constexpr _Aes256_gcm_default_engine(const _Elem* const _Data, const size_type _Size,
        const key& _Key, const iv& _Iv, const cipher_task _Task) noexcept;
    _CONSTEXPR20 ~_Aes256_gcm_default_engine() noexcept;

    _Aes256_gcm_default_engine() = delete;
    _Aes256_gcm_default_engine(const _Aes256_gcm_default_engine&) = delete;
    _Aes256_gcm_default_engine& operator=(const _Aes256_gcm_default_engine&) = delete;

    // returns encrypted/decrypted data or an empty string
    _NODISCARD constexpr byte_string bytes() const;

private:
    // calculates the buffer size for the AES-256 GCM encryption/decryption
    _NODISCARD constexpr size_type _Calculate_buffer_size(
        const size_type _Data_size, const cipher_task _Task) const noexcept;

    _Ebco_pair<_Sbo_buffer<byte_type>, _Fn> _Mypair;
};

// FUNCTION TEMPLATE decrypt_aes256_gcm
template <class _Elem, class _Eng = _Aes256_gcm_default_engine<uint8_t>>
_SDSDLL_API _NODISCARD constexpr basic_string<_Elem> decrypt_aes256_gcm(
    const uint8_t* const _Data, const size_t _Count, const aes_key<32>& _Key, const iv<12>& _Iv);

template <class _Elem, class _Eng = _Aes256_gcm_default_engine<uint8_t>>
_SDSDLL_API _NODISCARD constexpr basic_string<_Elem> decrypt_aes256_gcm(
    const uint8_t* const _Data, const aes_key<32>& _Key, const iv<12>& _Iv);

template <class _Elem, class _Eng = _Aes256_gcm_default_engine<uint8_t>>
_SDSDLL_API _NODISCARD constexpr basic_string<_Elem> decrypt_aes256_gcm(
    const byte_string_view _Data, const aes_key<32>& _Key, const iv<12>& _Iv);

template <class _Elem, class _Eng = _Aes256_gcm_default_engine<uint8_t>>
_SDSDLL_API _NODISCARD constexpr basic_string<_Elem> decrypt_aes256_gcm(
    const byte_string& _Data, const aes_key<32>& _Key, const iv<12>& _Iv);

// FUNCTION TEMPLATE encrypt_aes256_gcm
template <class _Elem, class _Eng = _Aes256_gcm_default_engine<_Elem>>
_SDSDLL_API _NODISCARD constexpr byte_string encrypt_aes256_gcm(
    const _Elem* const _Data, const size_t _Count, const aes_key<32>& _Key, const iv<12>& _Iv);

template <class _Elem, class _Eng = _Aes256_gcm_default_engine<_Elem>>
_SDSDLL_API _NODISCARD constexpr byte_string encrypt_aes256_gcm(
    const _Elem* const _Data, const aes_key<32>& _Key, const iv<12>& _Iv);

template <class _Elem, class _Eng = _Aes256_gcm_default_engine<_Elem>>
_SDSDLL_API _NODISCARD constexpr byte_string encrypt_aes256_gcm(
    const basic_string_view<_Elem> _Data, const aes_key<32>& _Key, const iv<12>& _Iv);

template <class _Elem, class _Eng = _Aes256_gcm_default_engine<_Elem>>
_SDSDLL_API _NODISCARD constexpr byte_string encrypt_aes256_gcm(
    const basic_string<_Elem>& _Data, const aes_key<32>& _Key, const iv<12>& _Iv);
_SDSDLL_END

#endif // _SDSDLL_PREPROCESSOR_GUARD
#endif // _SDSDLL_CRYPTOGRAPHY_CIPHER_SYMMETRIC_AES256_GCM_HPP_