// aes256_cbc.hpp

// Copyright (c) Mateusz Jandura. All rights reserved.
// SPDX-License-Identifier: Apache-2.0

#pragma once
#ifndef _UNIT_CRYPTOGRAPHY_AES256_CBC_HPP_
#define _UNIT_CRYPTOGRAPHY_AES256_CBC_HPP_
#include <core/defs.hpp>
#include <core/optimization/string_view.hpp>
#include <core/traits/string_traits.hpp>
#include <cryptography/cipher/symmetric/aes256_cbc.hpp>
#include <cryptography/cipher/symmetric/aes_key.hpp>
#include <cryptography/cipher/symmetric/iv.hpp>
#include <cstdint>
#include <cstring>
#include <gtest/gtest.h>

// SDSDLL types
using _SDSDLL aes_key;
using _SDSDLL byte_string;
using _SDSDLL iv;
using _SDSDLL string_view;
using _SDSDLL wstring_view;

// CONSTANT global_key
static constexpr uint8_t global_key[32] = {
    0xA3, 0x81, 0xC3, 0x1E, 0x29, 0x3C, 0xE9, 0xA7, 0x65, 0xF4, 0x54, 0x4F, 0x4A, 0x1C, 0x35, 0x74,
    0xC2, 0xB7, 0x49, 0xD4, 0x21, 0x95, 0x77, 0x89, 0xED, 0x89, 0x4F, 0x2A, 0x20, 0xFC, 0x9F, 0xDD
};

// CONSTANT global_iv
static constexpr uint8_t global_iv[16] = {
    0x26, 0x57, 0x8B, 0x3D, 0x4C, 0xF2, 0x4F, 0x57, 0x0A, 0x66, 0x9E, 0xA5, 0x55, 0xCC, 0x2F, 0x39
};

// CONSTANT short_ascii_output
static constexpr uint8_t short_ascii_output[16] = {
    0xE5, 0xF5, 0x64, 0xF4, 0x4E, 0x3B, 0xA5, 0xAF, 0x3E, 0xCC, 0xCD, 0xF7, 0x53, 0xAC, 0xBE, 0x48
};

// CONSTANT long_ascii_output
static constexpr uint8_t long_ascii_output[64] = {
    0x3F, 0x2E, 0xB0, 0x19, 0xAB, 0xCF, 0x31, 0xD4, 0x28, 0x1F, 0xBE, 0x61, 0xA0, 0xE1, 0x6E, 0x4B,
    0xCF, 0x4B, 0x36, 0x8B, 0xE4, 0xB4, 0x3B, 0xE2, 0x9D, 0xEB, 0xD6, 0xC1, 0xFB, 0xA4, 0x1E, 0xBD,
    0xED, 0xD8, 0x02, 0x26, 0xEB, 0x2B, 0x07, 0x5A, 0xD0, 0xA7, 0x6C, 0x50, 0x0C, 0x97, 0x14, 0x36,
    0x1D, 0x44, 0xD3, 0xBC, 0x74, 0xED, 0xD8, 0xED, 0x5C, 0x3B, 0xF0, 0xC3, 0x9C, 0x34, 0x66, 0xE6
};

// CONSTANT short_unicode_output
static constexpr uint8_t short_unicode_output[16] = {
    0xFC, 0xE0, 0x5C, 0x23, 0x7B, 0x6E, 0x77, 0x04, 0xCE, 0xF9, 0x11, 0xA8, 0xAB, 0x72, 0x74, 0x9F
};

// CONSTANT long_unicode_output
static constexpr uint8_t long_unicode_output[] = {
    0x7A, 0x14, 0xEB, 0x87, 0x95, 0xAD, 0xF2, 0xC0, 0x35, 0x40, 0xC6, 0xC6, 0xB4, 0x6C, 0x55, 0x8E,
    0x5F, 0x00, 0x7C, 0x57, 0x45, 0x46, 0x9B, 0x2C, 0x03, 0x21, 0xC3, 0x84, 0x05, 0x15, 0x23, 0xFC,
    0x90, 0x88, 0xCF, 0x3B, 0xE5, 0x6A, 0xDD, 0x2D, 0x82, 0x85, 0x2E, 0x23, 0x33, 0xDF, 0x0F, 0xA3,
    0x60, 0x90, 0x25, 0xC0, 0x34, 0xF7, 0x3C, 0x59, 0x3D, 0x68, 0x11, 0x5D, 0xE1, 0xBE, 0x96, 0x19
};

TEST(cryptography, aes256_cbc_short_ascii) {
    // Note: Encrypt "Hello world!", then decrypt.
    // Pre: Encrypted "Hello world!" should be the same as short_ascii_output.
    //      Decrypted short_ascii_output should be the same as "Hello world!".
    const aes_key<32> _Key(global_key);
    const iv<16> _Iv(global_iv);
    static constexpr string_view _Data = "Hello world!";
    const byte_string& _Cipher         = _SDSDLL encrypt_aes256_cbc(_Data, _Key, _Iv);
    const string& _Plain               = _SDSDLL decrypt_aes256_cbc<char>(_Cipher, _Key, _Iv);
    EXPECT_TRUE(_CSTD memcmp(short_ascii_output, _Cipher.c_str(), _Cipher.size()) == 0);
    EXPECT_TRUE(_CSTD memcmp(_Data.data(), _Plain.c_str(), _Plain.size()) == 0);
}

TEST(cryptography, aes256_cbc_long_ascii) {
    // Note: Encrypt "Hello world from the Secure Data Storage (SDS) application!", then decrypt.
    // Pre: Encrypted "Hello world from the Secure Data Storage (SDS) application!"
    //      should be the same as long_ascii_output.
    // Pre: Decrypted long_ascii_output should be the same as
    //      "Hello world from the Secure Data Storage (SDS) application!".
    const aes_key<32> _Key(global_key);
    const iv<16> _Iv(global_iv);
    static constexpr string_view _Data = "Hello world from the Secure Data Storage (SDS) application!";
    const byte_string& _Cipher         = _SDSDLL encrypt_aes256_cbc(_Data, _Key, _Iv);
    const string& _Plain               = _SDSDLL decrypt_aes256_cbc<char>(_Cipher, _Key, _Iv);
    EXPECT_TRUE(_CSTD memcmp(long_ascii_output, _Cipher.c_str(), _Cipher.size()) == 0);
    EXPECT_TRUE(_CSTD memcmp(_Data.data(), _Plain.c_str(), _Plain.size()) == 0);
}

TEST(cryptography, aes256_cbc_short_unicode) {
    // Note: Encrypt "你好世界！", then decrypt.
    // Pre: Encrypted "你好世界！" should be the same as short_unicode_output.
    //      Decrypted short_ascii_output should be the same as "你好世界！".
    const aes_key<32> _Key(global_key);
    const iv<16> _Iv(global_iv);
    static constexpr wstring_view _Data = L"你好世界！";
    const byte_string& _Cipher          = _SDSDLL encrypt_aes256_cbc(_Data, _Key, _Iv);
    const wstring& _Plain               = _SDSDLL decrypt_aes256_cbc<wchar_t>(_Cipher, _Key, _Iv);
    EXPECT_TRUE(_CSTD memcmp(short_unicode_output, _Cipher.c_str(), _Cipher.size()) == 0);
    EXPECT_TRUE(_CSTD memcmp(_Data.data(), _Plain.c_str(), _Plain.size()) == 0);
}

TEST(cryptography, aes256_cbc_long_unicode) {
    // Note: Encrypt "來自安全數據存儲 (SDS) 應用程序的世界您好！", then decrypt.
    // Pre: Encrypted "來自安全數據存儲 (SDS) 應用程序的世界您好！" should be the same as long_unicode_output.
    // Pre: Decrypted long_unicode_output should be the same as "來自安全數據存儲 (SDS) 應用程序的世界您好！".
    const aes_key<32> _Key(global_key);
    const iv<16> _Iv(global_iv);
    static constexpr wstring_view _Data = L"來自安全數據存儲 (SDS) 應用程序的世界您好！";
    const byte_string& _Cipher          = _SDSDLL encrypt_aes256_cbc(_Data, _Key, _Iv);
    const wstring& _Plain               = _SDSDLL decrypt_aes256_cbc<wchar_t>(_Cipher, _Key, _Iv);
    EXPECT_TRUE(_CSTD memcmp(long_unicode_output, _Cipher.c_str(), _Cipher.size()) == 0);
    EXPECT_TRUE(_CSTD memcmp(_Data.data(), _Plain.c_str(), _Plain.size()) == 0);
}

#endif // _UNIT_CRYPTOGRAPHY_AES256_CBC_HPP_