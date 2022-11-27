// blake3.hpp

// Copyright (c) Mateusz Jandura. All rights reserved.
// SPDX-License-Identifier: Apache-2.0

#pragma once
#ifndef _UNIT_CRYPTOGRAPHY_HASH_GENERIC_BLAKE3_HPP_
#define _UNIT_CRYPTOGRAPHY_HASH_GENERIC_BLAKE3_HPP_
#include <core/defs.hpp>
#include <core/traits/string_traits.hpp>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <cryptography/hash/generic.hpp>
#include <cryptography/hash/generic/blake3.hpp>
#include <gtest/gtest.h>
#include <unit/cryptography/hash/generic/common.hpp>

// SDSDLL types
using _SDSDLL blake3_traits;
using _SDSDLL byte_string;

namespace tests {
    // CLASS _Blake3_test
    class _Blake3_test : public _Hash_test_base<blake3_traits, 256> {
    public:
        _Test_case _Utf8_empty() noexcept override {
            _Test_case _Result;
            _Result._Utf8_text = "";
            _Result._Size      = 0;
            _CSTD memcpy(_Result._Bytes,
                "\xAF\x13\x49\xB9\xF5\xF9\xA1\xA6\xA0\x40\x4D\xEA\x36\xDC\xC9\x49"
                "\x9B\xCB\x25\xC9\xAD\xC1\x12\xB7\xCC\x9A\x93\xCA\xE4\x1F\x32\x62",
                sizeof(_Result._Bytes));
            return _Result;
        }

        _Test_case _Utf8_short() noexcept override {
            _Test_case _Result;
            _Result._Utf8_text = "A short UTF-8 text.";
            _Result._Size      = 19;
            _CSTD memcpy(_Result._Bytes,
                "\x36\x49\x92\x46\x6D\x17\xAB\x72\x10\x84\xD1\xC6\x01\x4E\x4E\xF5"
                "\x50\xA4\xA1\xAA\x45\x68\x10\x14\x49\x8A\xAC\x1B\x36\xE3\xF2\x9F",
                sizeof(_Result._Bytes));
            return _Result;
        }

        _Test_case _Utf8_long() noexcept override {
            _Test_case _Result;
            _Result._Utf8_text = "The quick brown fox jumps over the lazy dog.";
            _Result._Size      = 44;
            _CSTD memcpy(_Result._Bytes,
                "\x4C\x9B\xD6\x8D\x7F\x0B\xAA\x2E\x16\x7C\xEF\x98\x29\x5E\xB1\xEC"
                "\x99\xA3\xEC\x8F\x06\x56\xB3\x3D\xBA\xE9\x43\xB3\x87\xF3\x1D\x5D",
                sizeof(_Result._Bytes));
            return _Result;
        }

        _Test_case _Unicode_empty() noexcept override {
            _Test_case _Result;
            _Result._Unicode_text = L"";
            _Result._Size         = 0;
            _CSTD memcpy(_Result._Bytes,
                "\xAF\x13\x49\xB9\xF5\xF9\xA1\xA6\xA0\x40\x4D\xEA\x36\xDC\xC9\x49"
                "\x9B\xCB\x25\xC9\xAD\xC1\x12\xB7\xCC\x9A\x93\xCA\xE4\x1F\x32\x62",
                sizeof(_Result._Bytes));
            return _Result;
        }

        _Test_case _Unicode_short() noexcept override {
            _Test_case _Result;
            _Result._Unicode_text = L"簡短的 Unicode 文本。";
            _Result._Size         = 15;
            _CSTD memcpy(_Result._Bytes,
                "\xC1\x77\x7D\xEE\xFF\x95\xD3\xD3\x16\x1B\x4D\xAE\x46\x46\x46\x6B"
                "\x60\xCB\x6B\x33\x17\xD6\x49\xE7\xEE\x71\x84\x02\x21\x60\xAA\xC1",
                sizeof(_Result._Bytes));
            return _Result;
        }

        _Test_case _Unicode_long() noexcept override {
            _Test_case _Result;
            _Result._Unicode_text = L"敏捷的棕色狐狸跳過懶狗，因為它想要，而懶狗正在睡覺。";
            _Result._Size         = 26;
            _CSTD memcpy(_Result._Bytes,
                "\x5C\xA1\x9E\xA3\xB1\x0D\x73\x3D\xFA\x5B\xBF\x6A\x30\xCC\x9E\x79"
                "\xE3\x1D\xCA\xB9\x3A\x90\x9B\x24\x49\xE6\xA8\x8B\x95\xC8\x50\xE8",
                sizeof(_Result._Bytes));
            return _Result;
        }
    };

    TEST(cryptography_hash_generic, blake3) {
        _Blake3_test _Test;
        _Test._Run_test_case(_Test._Utf8_empty(), _Blake3_test::_Utf8);
        _Test._Run_test_case(_Test._Utf8_short(), _Blake3_test::_Utf8);
        _Test._Run_test_case(_Test._Utf8_long(), _Blake3_test::_Utf8);
        _Test._Run_test_case(_Test._Unicode_empty(), _Blake3_test::_Unicode);
        _Test._Run_test_case(_Test._Unicode_short(), _Blake3_test::_Unicode);
        _Test._Run_test_case(_Test._Unicode_long(), _Blake3_test::_Unicode);
    }
} // namespace tests

#endif // _UNIT_CRYPTOGRAPHY_HASH_GENERIC_BLAKE3_HPP_