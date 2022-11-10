// sha512.cpp

// Copyright (c) Mateusz Jandura. All rights reserved.
// SPDX-License-Identifier: Apache-2.0

#include <build/sdsdll_pch.hpp>
#include <cryptography/hash/generic/sha512.hpp>
#if _SDSDLL_PREPROCESSOR_GUARD

_SDSDLL_BEGIN
// FUNCTION _Sha512_context_proxy constructor/destructor
_Sha512_context_proxy::_Sha512_context_proxy() noexcept : _Ctx(::EVP_MD_CTX_new()) {}

_Sha512_context_proxy::~_Sha512_context_proxy() noexcept {
    if (_Ctx) {
        ::EVP_MD_CTX_free(_Ctx);
        _Ctx = nullptr;
    }
}

// FUNCTION TEMPLATE sha512_traits::bytes_count
template <class _Elem>
_NODISCARD constexpr typename sha512_traits<_Elem>::size_type sha512_traits<_Elem>::bytes_count() noexcept {
    return bits / CHAR_BIT; // always 64 bytes
}

// FUNCTION TEMPLATE sha512_traits::hash
template <class _Elem>
_NODISCARD constexpr bool sha512_traits<_Elem>::hash(byte_type* const _Buf,
    const size_type _Buf_size, const char_type* const _Data, const size_type _Data_size) noexcept {
    if (!_Buf || _Buf_size < bytes_count()) {
        return false;
    }

    uint32_t _Bytes = 0; // hashed bytes (unused)
    return ::EVP_Digest(
        _Data, _Data_size * sizeof(char_type), _Buf, &_Bytes, ::EVP_sha3_512(), nullptr) != 0;
}

// FUNCTION TEMPLATE sha512_traits::hash_file
template <class _Elem>
_NODISCARD constexpr bool sha512_traits<_Elem>::hash_file(byte_type* const _Buf,
    const size_type _Buf_size, file& _File, const file::off_type _Off) noexcept {
    if (!_Buf || _Buf_size < bytes_count()) {
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

    _Sha512_context_proxy _Proxy;
    if (!_Proxy._Ctx) {
        return false;
    }

    if (::EVP_DigestInit_ex(_Proxy._Ctx, ::EVP_sha3_512(), nullptr) == 0) {
        return false;
    }

    constexpr size_type _Chunk_size = 1024; // 1024-byte chunk
    byte_type _Chunk[_Chunk_size];
    size_type _Read = 0; // read bytes, must be initialized
    for (;;) {
        if (!_File.read(_Chunk, _Chunk_size, _Chunk_size, &_Read)) {
            return false;
        }

        if (_Read == 0) { // no more data
            break;
        }

        if (::EVP_DigestUpdate(_Proxy._Ctx, _Chunk, _Read) == 0) {
            return false;
        }
    }

    uint32_t _Bytes = 0; // hashed bytes (unused)
    return ::EVP_DigestFinal_ex(_Proxy._Ctx, _Buf, &_Bytes) != 0;
}

template _SDSDLL_API struct sha512_traits<char>;
template _SDSDLL_API struct sha512_traits<unsigned char>;
template _SDSDLL_API struct sha512_traits<wchar_t>;
_SDSDLL_END

#endif // _SDSDLL_PREPROCESSOR_GUARD