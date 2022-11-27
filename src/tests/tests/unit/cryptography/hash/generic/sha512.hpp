// sha512.hpp

// Copyright (c) Mateusz Jandura. All rights reserved.
// SPDX-License-Identifier: Apache-2.0

#pragma once
#ifndef _UNIT_CRYPTOGRAPHY_HASH_GENERIC_SHA512_HPP_
#define _UNIT_CRYPTOGRAPHY_HASH_GENERIC_SHA512_HPP_
#include <core/defs.hpp>
#include <core/traits/string_traits.hpp>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <cryptography/hash/generic.hpp>
#include <cryptography/hash/generic/sha512.hpp>
#include <gtest/gtest.h>
#include <unit/cryptography/hash/generic/common.hpp>

// SDSDLL types
using _SDSDLL byte_string;
using _SDSDLL sha512_traits;

namespace tests {
    // CLASS _Sha512_test
    class _Sha512_test : public _Hash_test_base<sha512_traits, 512> {
    public:
        _Test_case _Utf8_empty() noexcept override {
            _Test_case _Result;
            _Result._Utf8_text = "";
            _Result._Size      = 0;
            _CSTD memcpy(_Result._Bytes,
                "\xA6\x9F\x73\xCC\xA2\x3A\x9A\xC5\xC8\xB5\x67\xDC\x18\x5A\x75\x6E"
                "\x97\xC9\x82\x16\x4F\xE2\x58\x59\xE0\xD1\xDC\xC1\x47\x5C\x80\xA6"
                "\x15\xB2\x12\x3A\xF1\xF5\xF9\x4C\x11\xE3\xE9\x40\x2C\x3A\xC5\x58"
                "\xF5\x00\x19\x9D\x95\xB6\xD3\xE3\x01\x75\x85\x86\x28\x1D\xCD\x26",
                sizeof(_Result._Bytes));
            return _Result;
        }

        _Test_case _Utf8_short() noexcept override {
            _Test_case _Result;
            _Result._Utf8_text = "A short UTF-8 text.";
            _Result._Size      = 19;
            _CSTD memcpy(_Result._Bytes,
                "\x29\xAF\x14\xDD\x29\xDE\x69\xB3\x2F\xAB\x21\x58\xD3\xA5\xEF\x8B"
                "\xE7\x4B\xB8\xD8\xEF\x20\x60\x23\x38\x0E\xEA\x8B\xCA\xA1\x95\x8D"
                "\xF8\xB1\x9E\x2E\xEC\x1D\xE5\xC1\x36\x2C\xD7\x16\x5B\xF7\xEE\xB8"
                "\x80\xF8\xE5\x69\x37\x93\x39\x43\xB3\xB4\x76\x4A\xDD\x97\xF0\xBA",
                sizeof(_Result._Bytes));
            return _Result;
        }

        _Test_case _Utf8_long() noexcept override {
            _Test_case _Result;
            _Result._Utf8_text = "The quick brown fox jumps over the lazy dog.";
            _Result._Size      = 44;
            _CSTD memcpy(_Result._Bytes,
                "\x18\xF4\xF4\xBD\x41\x96\x03\xF9\x55\x38\x83\x70\x03\xD9\xD2\x54"
                "\xC2\x6C\x23\x76\x55\x65\x16\x22\x47\x48\x3F\x65\xC5\x03\x03\x59"
                "\x7B\xC9\xCE\x4D\x28\x9F\x21\xD1\xC2\xF1\xF4\x58\x82\x8E\x33\xDC"
                "\x44\x21\x00\x33\x1B\x35\xE7\xEB\x03\x1B\x5D\x38\xBA\x64\x60\xF8",
                sizeof(_Result._Bytes));
            return _Result;
        }

        _Test_case _Unicode_empty() noexcept override {
            _Test_case _Result;
            _Result._Unicode_text = L"";
            _Result._Size         = 0;
            _CSTD memcpy(_Result._Bytes,
                "\xA6\x9F\x73\xCC\xA2\x3A\x9A\xC5\xC8\xB5\x67\xDC\x18\x5A\x75\x6E"
                "\x97\xC9\x82\x16\x4F\xE2\x58\x59\xE0\xD1\xDC\xC1\x47\x5C\x80\xA6"
                "\x15\xB2\x12\x3A\xF1\xF5\xF9\x4C\x11\xE3\xE9\x40\x2C\x3A\xC5\x58"
                "\xF5\x00\x19\x9D\x95\xB6\xD3\xE3\x01\x75\x85\x86\x28\x1D\xCD\x26",
                sizeof(_Result._Bytes));
            return _Result;
        }

        _Test_case _Unicode_short() noexcept override {
            _Test_case _Result;
            _Result._Unicode_text = L"簡短的 Unicode 文本。";
            _Result._Size         = 15;
            _CSTD memcpy(_Result._Bytes,
                "\xF4\xDA\x5A\xAC\xBA\xCE\x92\xE8\xED\x31\xAB\x65\xDC\xDE\x05\x09"
                "\x7B\x39\x1A\xB8\xFA\x66\xFB\xCD\xF7\x8A\xEF\x84\x78\x99\xA4\x70"
                "\x0A\xE2\x5D\xBE\xA4\xE6\x59\x25\x8B\x83\x04\xC4\x7E\xC2\x8B\x65"
                "\x22\x31\x73\x24\x21\x3B\x7E\x83\x8C\x0B\x62\x57\xE5\x69\xB6\xE8",
                sizeof(_Result._Bytes));
            return _Result;
        }

        _Test_case _Unicode_long() noexcept override {
            _Test_case _Result;
            _Result._Unicode_text = L"敏捷的棕色狐狸跳過懶狗，因為它想要，而懶狗正在睡覺。";
            _Result._Size         = 26;
            _CSTD memcpy(_Result._Bytes,
                "\x22\xD1\xA2\xAB\x03\xF5\x0A\xA7\x72\xDA\x3D\xA2\x73\x5F\x11\xD3"
                "\xFE\x47\xC3\xE0\x6E\x53\x95\x81\x45\xA6\xC3\xE3\x9C\x7D\xC1\x34"
                "\x87\xE8\xB9\x8C\x55\x5F\xF9\x5E\xBD\x4E\xB4\x98\x32\x9A\xE2\x4C"
                "\xEC\xF1\x4B\x57\x2F\x2B\x7B\x76\xB3\x75\xD6\x01\x5E\xED\x56\xBA",
                sizeof(_Result._Bytes));
            return _Result;
        }
    };

    TEST(cryptography_hash_generic, sha512) {
        _Sha512_test _Test;
        _Test._Run_test_case(_Test._Utf8_empty(), _Sha512_test::_Utf8);
        _Test._Run_test_case(_Test._Utf8_short(), _Sha512_test::_Utf8);
        _Test._Run_test_case(_Test._Utf8_long(), _Sha512_test::_Utf8);
        _Test._Run_test_case(_Test._Unicode_empty(), _Sha512_test::_Unicode);
        _Test._Run_test_case(_Test._Unicode_short(), _Sha512_test::_Unicode);
        _Test._Run_test_case(_Test._Unicode_long(), _Sha512_test::_Unicode);
    }
} // namespace tests

#endif // _UNIT_CRYPTOGRAPHY_HASH_GENERIC_SHA512_HPP_