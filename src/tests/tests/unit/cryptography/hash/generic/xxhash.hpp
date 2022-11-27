// xxhash.hpp

// Copyright (c) Mateusz Jandura. All rights reserved.
// SPDX-License-Identifier: Apache-2.0

#pragma once
#ifndef _UNIT_CRYPTOGRAPHY_HASH_GENERIC_XXHASH_HPP_
#define _UNIT_CRYPTOGRAPHY_HASH_GENERIC_XXHASH_HPP_
#include <core/defs.hpp>
#include <core/traits/string_traits.hpp>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <cryptography/hash/generic.hpp>
#include <cryptography/hash/generic/xxhash.hpp>
#include <gtest/gtest.h>
#include <unit/cryptography/hash/generic/common.hpp>

// SDSDLL types
using _SDSDLL byte_string;
using _SDSDLL xxhash_traits;

namespace tests {
    // CLASS _Xxhash_test
    class _Xxhash_test : public _Hash_test_base<xxhash_traits, 64> {
    public:
        _Test_case _Utf8_empty() noexcept override {
            _Test_case _Result;
            _Result._Utf8_text = "";
            _Result._Size      = 0;
            _CSTD memcpy(_Result._Bytes, "\xC2\x94\xD3\x38\x05\x80\x06\x2D", sizeof(_Result._Bytes));
            return _Result;
        }

        _Test_case _Utf8_short() noexcept override {
            _Test_case _Result;
            _Result._Utf8_text = "A short UTF-8 text.";
            _Result._Size      = 19;
            _CSTD memcpy(_Result._Bytes, "\xEE\x4D\x97\xB4\x80\xBF\xD9\x48", sizeof(_Result._Bytes));
            return _Result;
        }

        _Test_case _Utf8_long() noexcept override {
            _Test_case _Result;
            _Result._Utf8_text = "The quick brown fox jumps over the lazy dog.";
            _Result._Size      = 44;
            _CSTD memcpy(_Result._Bytes, "\x19\xDB\x51\x5D\x22\xE0\x14\xB6", sizeof(_Result._Bytes));
            return _Result;
        }

        _Test_case _Unicode_empty() noexcept override {
            _Test_case _Result;
            _Result._Unicode_text = L"";
            _Result._Size         = 0;
            _CSTD memcpy(_Result._Bytes, "\xC2\x94\xD3\x38\x05\x80\x06\x2D", sizeof(_Result._Bytes));
            return _Result;
        }

        _Test_case _Unicode_short() noexcept override {
            _Test_case _Result;
            _Result._Unicode_text = L"簡短的 Unicode 文本。";
            _Result._Size         = 15;
            _CSTD memcpy(_Result._Bytes, "\x69\x33\x1B\x5E\x39\x85\x32\x21", sizeof(_Result._Bytes));
            return _Result;
        }

        _Test_case _Unicode_long() noexcept override {
            _Test_case _Result;
            _Result._Unicode_text = L"敏捷的棕色狐狸跳過懶狗，因為它想要，而懶狗正在睡覺。";
            _Result._Size         = 26;
            _CSTD memcpy(_Result._Bytes, "\x59\x37\x54\xBE\x8F\xE0\x4D\x2D", sizeof(_Result._Bytes));
            return _Result;
        }
    };

    TEST(cryptography_hash_generic, xxhash) {
        _Xxhash_test _Test;
        _Test._Run_test_case(_Test._Utf8_empty(), _Xxhash_test::_Utf8);
        _Test._Run_test_case(_Test._Utf8_short(), _Xxhash_test::_Utf8);
        _Test._Run_test_case(_Test._Utf8_long(), _Xxhash_test::_Utf8);
        _Test._Run_test_case(_Test._Unicode_empty(), _Xxhash_test::_Unicode);
        _Test._Run_test_case(_Test._Unicode_short(), _Xxhash_test::_Unicode);
        _Test._Run_test_case(_Test._Unicode_long(), _Xxhash_test::_Unicode);
    }
} // namespace tests

#endif // _UNIT_CRYPTOGRAPHY_HASH_GENERIC_XXHASH_HPP_