// hex.cpp

// Copyright (c) Mateusz Jandura. All rights reserved.
// SPDX-License-Identifier: Apache-2.0

#include <build/sdsdll_pch.hpp>
#include <encoding/hex.hpp>
#if _SDSDLL_PREPROCESSOR_GUARD

_SDSDLL_BEGIN
// FUNCTION TEMPLATE _Fast_strcvt
template <class _To, class _From>
_NODISCARD constexpr basic_string<_To> _Fast_strcvt(const basic_string<_From>& _Str) {
    basic_string<_To> _Result;
    _Result.reserve(_Str.size());
    for (const _From& _Ch : _Str) {
        _Result.push_back(static_cast<_To>(_Ch));
    }

    return _Result;
}

// FUNCTION TEMPLATE _String_to_hex
template <class _Elem>
_NODISCARD constexpr basic_string<_Elem> _String_to_hex(const _Elem* _Str, size_t _Count) {
    basic_string<_Elem> _Buf(_Count * 2, _Elem{0});
    _Elem* _Buf_ptr = _Buf.data();
    while (_Count-- > 0) {
        if constexpr (sizeof(_Elem) == 1) {
            _CSTD snprintf(reinterpret_cast<char*>(_Buf_ptr), 3, "%02x", static_cast<int>(*_Str));
        } else {
            ::_snwprintf_s(_Buf_ptr, 3, 3, L"%02x", static_cast<int>(*_Str));
        }

        ++_Str;
        _Buf_ptr += 2; // next chunk from the buffer
    }

    return _Buf;
}

// FUNCTION TEMPLATE as_hex
template <class _Elem, hex_format _Fmt>
_NODISCARD constexpr hex_string<_Fmt> as_hex(const _Elem* const _Str, const size_t _Count) {
    static_assert(is_any_of_v<_Elem, char, unsigned char, wchar_t>,
        "Requires a byte/UTF-8/UTF-16 element type.");
    using _Str_t = hex_string<_Fmt>;
    using _Val_t = typename _Str_t::value_type;
    if (_Count == 0) { // no conversion
        return _Str_t{};
    }

    const auto& _Hex = _String_to_hex(_Str, _Count);
    if constexpr (is_same_v<_Elem, _Val_t>) { // no additional conversion
        return _Hex;
    } else { // convert to the correct format
        return _Fast_strcvt<_Val_t>(_Hex);
    }
}

// auxilary macro for sdsdll::as_hex() function export/import
#define _EXPORT_OR_IMPORT_AS_HEX(_Elem, ...)                                                   \
    template _SDSDLL_API _NODISCARD byte_string as_hex<_Elem, hex_format::bytes>(__VA_ARGS__); \
    template _SDSDLL_API _NODISCARD string as_hex<_Elem, hex_format::narrow>(__VA_ARGS__);     \
    template _SDSDLL_API _NODISCARD wstring as_hex<_Elem, hex_format::wide>(__VA_ARGS__);

_EXPORT_OR_IMPORT_AS_HEX(char, const char* const, const size_t)
_EXPORT_OR_IMPORT_AS_HEX(unsigned char, const unsigned char* const, const size_t)
_EXPORT_OR_IMPORT_AS_HEX(wchar_t, const wchar_t* const, const size_t);

template <class _Elem, hex_format _Fmt>
_NODISCARD constexpr hex_string<_Fmt> as_hex(const _Elem* const _Str) {
    using _Traits = string_traits<_Elem, size_t>;
    return _SDSDLL as_hex<_Elem, _Fmt>(_Str, _Traits::length(_Str));
}

_EXPORT_OR_IMPORT_AS_HEX(char, const char* const)
_EXPORT_OR_IMPORT_AS_HEX(unsigned char, const unsigned char* const)
_EXPORT_OR_IMPORT_AS_HEX(wchar_t, const wchar_t* const);

template <class _Elem, hex_format _Fmt>
_NODISCARD constexpr hex_string<_Fmt> as_hex(const basic_string_view<_Elem> _Str) {
    return _SDSDLL as_hex<_Elem, _Fmt>(_Str.data(), _Str.size());
}

_EXPORT_OR_IMPORT_AS_HEX(char, const string_view)
_EXPORT_OR_IMPORT_AS_HEX(unsigned char, const byte_string_view)
_EXPORT_OR_IMPORT_AS_HEX(wchar_t, const wstring_view);

template <class _Elem, hex_format _Fmt>
_NODISCARD constexpr hex_string<_Fmt> as_hex(const basic_string<_Elem>& _Str) {
    return _SDSDLL as_hex<_Elem, _Fmt>(_Str.c_str(), _Str.size());
}

_EXPORT_OR_IMPORT_AS_HEX(char, const string&)
_EXPORT_OR_IMPORT_AS_HEX(unsigned char, const byte_string&)
_EXPORT_OR_IMPORT_AS_HEX(wchar_t, const wstring&);

#undef _EXPORT_OR_IMPORT_AS_HEX
_SDSDLL_END

#endif // _SDSDLL_PREPROCESSOR_GUARD