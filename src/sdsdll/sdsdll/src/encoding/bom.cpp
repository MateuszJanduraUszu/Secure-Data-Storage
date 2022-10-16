// bom.cpp

// Copyright (c) Mateusz Jandura. All rights reserved.
// SPDX-License-Identifier: Apache-2.0

#include <build/sdsdll_pch.hpp>
#include <encoding/bom.hpp>
#if _SDSDLL_PREPROCESSOR_GUARD

_SDSDLL_BEGIN
// FUNCTION fold_bom
_NODISCARD const char* fold_bom(const bom& _Bom) noexcept {
    switch (_Bom.encoding) {
    case encoding::utf8:
        return _Utf8_bom;
    case encoding::utf16:
        switch (_Bom.endian) {
        case endian::big:
            return _Utf16_big_endian_bom;
        case endian::little:
            return _Utf16_little_endian_bom;
        default: // unknown endian
            return nullptr;
        }
    default: // unknown encoding
        return nullptr;
    }
}

// FUNCTION unfold_bom
_NODISCARD bom unfold_bom(const char* const _Bom) noexcept {
    using _Traits = string_traits<char, int>;
    if (_Traits::compare(_Bom, _Utf8_bom) == 0) {
        return {encoding::utf8, endian::none};
    } else if (_Traits::compare(_Bom, _Utf16_big_endian_bom) == 0) {
        return {encoding::utf16, endian::big};
    } else if (_Traits::compare(_Bom, _Utf16_little_endian_bom) == 0) {
        return {encoding::utf16, endian::little};
    } else { // unknown encoding
        return {encoding::none, endian::none};
    }
}
_SDSDLL_END

#endif // _SDSDLL_PREPROCESSOR_GUARD