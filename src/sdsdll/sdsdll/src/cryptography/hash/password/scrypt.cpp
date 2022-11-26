// scrypt.cpp

// Copyright (c) Mateusz Jandura. All rights reserved.
// SPDX-License-Identifier: Apache-2.0

#include <build/sdsdll_pch.hpp>
#include <cryptography/hash/password/scrypt.hpp>
#if _SDSDLL_PREPROCESSOR_GUARD

_SDSDLL_BEGIN
// FUNCTION TEMPLATE scrypt_traits::bytes_count
template <class _Elem>
_NODISCARD constexpr typename scrypt_traits<_Elem>::size_type
    scrypt_traits<_Elem>::bytes_count() noexcept {
    return bits / CHAR_BIT; // always 64 bytes
}

// FUNCTION TEMPLATE scrypt_traits::hash
template <class _Elem>
_NODISCARD constexpr bool scrypt_traits<_Elem>::hash(byte_type* const _Buf, const size_type _Buf_size,
    const char_type* const _Data, const size_type _Data_size, const salt& _Salt,
    const size_type _Cost, const size_type _Block_size, const size_type _Parallelism) noexcept {
    constexpr size_type _Optimal_buf_size = bytes_count();
    if (!_Buf || _Buf_size < _Optimal_buf_size) {
        return false;
    }

    if (!_Is_pow_of_2(_Cost)) { // cost must be 2^n
        return false;
    }

    if (_Block_size == 0 || _Block_size > 256) { // block size must be at least 1 and at most 256
        return false;
    }

    if (_Parallelism == 0 || _Parallelism > 1024) { // threads count must be at least 1 and at most 1024
        return false;
    }

    if _CONSTEXPR_IF (sizeof(_Elem) == 1) { // hash a UTF-8 password
        try { // throws if one of the arguments is invalid
            ::Botan::scrypt(_Buf, _Optimal_buf_size, reinterpret_cast<const char*>(_Data),
                _Data_size, _Salt.get(), _Salt.size, _Cost, _Block_size, _Parallelism);
        } catch (...) {
            return false;
        }
    } else { // hash a Unicode password
        const utf8_string& _Narrow = utf8_string::from_utf16(_Data, _Data_size);
        try { // throws if one of the arguments is invalid
            ::Botan::scrypt(_Buf, _Optimal_buf_size, _Narrow.c_str(), _Narrow.size(),
                _Salt.get(), _Salt.size, _Cost, _Block_size, _Parallelism);
        } catch (...) {
            return false;
        }
    }

    return true;
}

template struct _SDSDLL_API scrypt_traits<char>;
template struct _SDSDLL_API scrypt_traits<unsigned char>;
template struct _SDSDLL_API scrypt_traits<wchar_t>;
_SDSDLL_END

#endif // _SDSDLL_PREPROCESSOR_GUARD