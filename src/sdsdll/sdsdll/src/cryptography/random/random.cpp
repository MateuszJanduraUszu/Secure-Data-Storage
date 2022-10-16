// random.cpp

// Copyright (c) Mateusz Jandura. All rights reserved.
// SPDX-License-Identifier: Apache-2.0

#include <build/sdsdll_pch.hpp>
#include <cryptography/random/random.hpp>
#if _SDSDLL_PREPROCESSOR_GUARD

_SDSDLL_BEGIN
// FUNCTION _Generate_random_byte
_NODISCARD uint8_t _Generate_random_byte(const uint8_t _Min, const uint8_t _Max) {
    random_device _Device;
    mt19937 _Eng(_Device());
    return static_cast<uint8_t>(uniform_int_distribution<uint16_t>{_Min, _Max}(_Eng));
}

// FUNCTION random_string
_NODISCARD string random_string(const size_t _Size, const random_format _Fmt) {
    _Random_elements<char> _Elems;
    string _Result(_Size, char{});
    for (char& _Ch : _Result) {
        _Generate_random_elements(_Elems, _Fmt);
        _Ch = _Choose_random_element(_Elems, _Fmt);
    }

    return _Result;
}

// FUNCTION random_bytes
_NODISCARD byte_string random_bytes(const size_t _Size, const random_format _Fmt) {
    _Random_elements<uint8_t> _Elems;
    byte_string _Result(_Size, uint8_t{});
    for (uint8_t& _Ch : _Result) {
        _Generate_random_elements(_Elems, _Fmt);
        _Ch = _Choose_random_element(_Elems, _Fmt);
    }

    return _Result;
}
_SDSDLL_END

#endif // _SDSDLL_PREPROCESSOR_GUARD