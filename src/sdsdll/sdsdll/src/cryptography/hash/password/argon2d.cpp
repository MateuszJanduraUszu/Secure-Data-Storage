// argon2d.cpp

// Copyright (c) Mateusz Jandura. All rights reserved.
// SPDX-License-Identifier: Apache-2.0

#include <build/sdsdll_pch.hpp>
#include <cryptography/hash/password/argon2d.hpp>
#if _SDSDLL_PREPROCESSOR_GUARD

_SDSDLL_BEGIN
// FUNCTION TEMPLATE argon2d_traits::bytes_count
template <class _Elem>
_NODISCARD constexpr typename argon2d_traits<_Elem>::size_type
    argon2d_traits<_Elem>::bytes_count() noexcept {
    return bits / CHAR_BIT; // always 64 bytes
}

// FUNCTION TEMPLATE argon2d_traits::hash
template <class _Elem>
_NODISCARD constexpr bool argon2d_traits<_Elem>::hash(
    byte_type* const _Buf, const size_type _Buf_size, const char_type* const _Data,
    const size_type _Data_size, const salt& _Salt, const size_type _Memory_amount,
    const size_type _Iterations, const size_type _Parallelism) noexcept {
    constexpr size_type _Optimal_buf_size = bytes_count();
    if (!_Buf || _Buf_size < _Optimal_buf_size) {
        return false;
    }

    constexpr uint8_t _Variant = 0; // Argon2 variant (required by Botan)
    if constexpr (sizeof(_Elem) == 1) { // hash a UTF-8 password
        if (_Data_size > 0xFFFF'FFFF) { // Argon2d can hash at most 2^32 - 1 bytes
            return false;
        }

        try { // throws if one of the arguments is invalid
            ::Botan::argon2(_Buf, _Optimal_buf_size, reinterpret_cast<const char*>(_Data),
                _Data_size, _Salt.get(), _Salt.size, nullptr, 0, nullptr, 0, _Variant,
                _Parallelism, _Memory_amount, _Iterations);
        } catch (...) {
            return false;
        }
    } else { // hash a Unicode password
        const utf8_string& _Narrow = utf8_string::from_utf16(_Data, _Data_size);
        if (_Narrow.size() > 0xFFFF'FFFF) { // Argon2d can hash at most 2^32 - 1 bytes
            return false;
        }

        try { // throws if one of the arguments is invalid
            ::Botan::argon2(_Buf, _Optimal_buf_size, _Narrow.c_str(), _Narrow.size(),
                _Salt.get(), _Salt.size, nullptr, 0, nullptr, 0, _Variant,
                _Parallelism, _Memory_amount, _Iterations);
        } catch (...) {
            return false;
        }
    }

    return true;
}

template struct _SDSDLL_API argon2d_traits<char>;
template struct _SDSDLL_API argon2d_traits<unsigned char>;
template struct _SDSDLL_API argon2d_traits<wchar_t>;
_SDSDLL_END

#endif // _SDSDLL_PREPROCESSOR_GUARD