// blake3.hpp

// Copyright (c) Mateusz Jandura. All rights reserved.
// SPDX-License-Identifier: Apache-2.0

#pragma once
#ifndef _UNIT_CRYPTOGRAPHY_HASH_GENERIC_COMMON_HPP_
#define _UNIT_CRYPTOGRAPHY_HASH_GENERIC_COMMON_HPP_
#include <climits>
#include <core/defs.hpp>
#include <core/traits/string_traits.hpp>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <cryptography/hash/generic.hpp>
#include <cryptography/hash/generic/blake3.hpp>
#include <gtest/gtest.h>

// SDSDLL types
using _SDSDLL byte_string;

namespace tests {
    // CLASS TEMPLATE _Hash_test_base
    template <template <class> class _Traits, size_t _Bits>
    class __declspec(novtable) _Hash_test_base { // base for all hash tests
    protected:
        struct _Test_case;

    public:
        using _Utf8_hash_traits    = _Traits<char>;
        using _Unicode_hash_traits = _Traits<wchar_t>;

        _Hash_test_base() noexcept {}

        virtual ~_Hash_test_base() noexcept {}

        enum _Utf8_or_unicode : bool { _Utf8, _Unicode };

        void _Run_test_case(const _Test_case& _Case, const _Utf8_or_unicode _Enc) noexcept {
            byte_string _Bytes;
            _Bytes.reserve(sizeof(_Case._Bytes));
            if (_Enc == _Utf8) { // run test case for a UTF-8 string
                _Bytes = _SDSDLL hash<_Utf8_hash_traits>(_Case._Utf8_text, _Case._Size);
            } else { // run test case for a Unicode string
                _Bytes = _SDSDLL hash<_Unicode_hash_traits>(_Case._Unicode_text, _Case._Size);
            }

            EXPECT_TRUE(_CSTD memcmp(_Bytes.c_str(), _Case._Bytes, _Bytes.size()) == 0);
        }

        virtual _Test_case _Utf8_empty() noexcept    = 0;
        virtual _Test_case _Utf8_short() noexcept    = 0;
        virtual _Test_case _Utf8_long() noexcept     = 0;
        virtual _Test_case _Unicode_empty() noexcept = 0;
        virtual _Test_case _Unicode_short() noexcept = 0;
        virtual _Test_case _Unicode_long() noexcept  = 0;

    protected:
        struct _Test_case {
#pragma warning(suppress : 26495) // C26495: Variable is uninitialized (expected)
            _Test_case() noexcept {
                // intentionally uninitialized
            }

            _Test_case(const _Test_case& _Other, const _Utf8_or_unicode _Enc) noexcept
                : _Size(_Other._Size) {
                if (_Enc == _Utf8) {
                    _Utf8_text = _Other._Utf8_text;
                } else {
                    _Unicode_text = _Other._Unicode_text;
                }

                _CSTD memcpy(_Bytes, _Other._Bytes, sizeof(_Bytes));
            }

            union {
                const char* _Utf8_text;
                const wchar_t* _Unicode_text;
            };

            size_t _Size; // UTF-8/Unicode text length
            unsigned char _Bytes[_Bits / CHAR_BIT]; // expected bytes
        };
    };
} // namespace tests

#endif // _UNIT_CRYPTOGRAPHY_HASH_GENERIC_COMMON_HPP_