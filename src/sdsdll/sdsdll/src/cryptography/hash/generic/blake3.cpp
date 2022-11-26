// blake3.cpp

// Copyright (c) Mateusz Jandura. All rights reserved.
// SPDX-License-Identifier: Apache-2.0

#include <build/sdsdll_pch.hpp>
#include <cryptography/hash/generic/blake3.hpp>
#if _SDSDLL_PREPROCESSOR_GUARD

_SDSDLL_BEGIN
// FUNCTION TEMPLATE blake3_traits::bytes_count
template <class _Elem>
_NODISCARD constexpr typename blake3_traits<_Elem>::size_type blake3_traits<_Elem>::bytes_count() noexcept {
    return bits / CHAR_BIT; // always 32 bytes
}

// FUNCTION TEMPLATE blake3_traits::hash
template <class _Elem>
_NODISCARD constexpr bool blake3_traits<_Elem>::hash(byte_type* const _Buf,
    const size_type _Buf_size, const char_type* const _Data, const size_type _Data_size) noexcept {
    constexpr size_type _Optimal_buf_size = bytes_count();
    if (!_Buf || _Buf_size < _Optimal_buf_size) {
        return false;
    }

    blake3_hasher _Hasher;
    ::blake3_hasher_init(_SDSDLL addressof(_Hasher));
    ::blake3_hasher_update(_SDSDLL addressof(_Hasher), _Data, _Data_size * sizeof(char_type));
    ::blake3_hasher_finalize(_SDSDLL addressof(_Hasher), _Buf, _Optimal_buf_size);
    return true;
}

// FUNCTION TEMPLATE blake3_traits::hash_file
template <class _Elem>
_NODISCARD constexpr bool blake3_traits<_Elem>::hash_file(byte_type* const _Buf,
    const size_type _Buf_size, file& _File, const file::off_type _Off) noexcept {
    constexpr size_type _Optimal_buf_size = bytes_count();
    if (!_Buf || _Buf_size < _Optimal_buf_size) {
        return false;
    }

    if (!_File.is_open()) {
        return false;
    }

    if (_File.tell() != _Off) { // change file offset
        if (!_File.seek(_Off)) {
            return false;
        }
    }

    constexpr size_type _Chunk_size = 1024; // 1024-byte chunk
    byte_type _Chunk[_Chunk_size];
    size_type _Read = 0; // read bytes, must be initialized
    blake3_hasher _Hasher;
    ::blake3_hasher_init(_SDSDLL addressof(_Hasher));
    for (;;) {
        if (!_File.read(_Chunk, _Chunk_size, _Chunk_size, &_Read)) {
            return false;
        }

        if (_Read == 0) { // no more data
            break;
        }

        ::blake3_hasher_update(_SDSDLL addressof(_Hasher), _Chunk, _Read);
    }

    ::blake3_hasher_finalize(_SDSDLL addressof(_Hasher), _Buf, _Optimal_buf_size);
    return true;
}

template struct _SDSDLL_API blake3_traits<char>;
template struct _SDSDLL_API blake3_traits<unsigned char>;
template struct _SDSDLL_API blake3_traits<wchar_t>;
_SDSDLL_END

#endif // _SDSDLL_PREPROCESSOR_GUARD