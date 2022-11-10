// xxhash.cpp

// Copyright (c) Mateusz Jandura. All rights reserved.
// SPDX-License-Identifier: Apache-2.0

#include <build/sdsdll_pch.hpp>
#include <cryptography/hash/generic/xxhash.hpp>
#if _SDSDLL_PREPROCESSOR_GUARD

_SDSDLL_BEGIN
// FUNCTION _Xxhash_state_proxy constructor/destructor
_Xxhash_state_proxy::_Xxhash_state_proxy() noexcept : _State(::XXH3_createState()) {
    if (_State) {
        ::XXH3_64bits_reset(_State);
    }
}

_Xxhash_state_proxy::~_Xxhash_state_proxy() noexcept {
    if (_State) {
        ::XXH3_freeState(_State);
        _State = nullptr;
    }
}

// FUNCTION TEMPLATE xxhash_traits::bytes_count
template <class _Elem>
_NODISCARD constexpr typename xxhash_traits<_Elem>::size_type xxhash_traits<_Elem>::bytes_count() noexcept {
    return bits / CHAR_BIT; // always 8 bytes
}

// FUNCTION TEMPLATE xxhash_traits::hash
template <class _Elem>
_NODISCARD constexpr bool xxhash_traits<_Elem>::hash(byte_type* const _Buf,
    const size_type _Buf_size, const char_type* const _Data, const size_type _Data_size) noexcept {
    if (!_Buf || _Buf_size < bytes_count()) {
        return false;
    }

    const uint64_t _Num = ::XXH3_64bits(_Data, _Data_size * sizeof(char_type));
    const auto& _Bytes  = _SDSDLL unpack_integer(_Num); // unpack into a 8-byte string
    memory_traits::copy(_Buf, _Bytes.data(), _Bytes.size());
    return true;
}

// FUNCTION TEMPLATE xxhash_traits::hash_file
template <class _Elem>
_NODISCARD constexpr bool xxhash_traits<_Elem>::hash_file(byte_type* const _Buf,
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
    
    _Xxhash_state_proxy _Proxy;
    if (!_Proxy._State) {
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

        if (::XXH3_64bits_update(_Proxy._State, _Chunk, _Read) != XXH_OK) {
            return false;
        }
    }

    const uint64_t _Num = ::XXH3_64bits_digest(_Proxy._State);
    const auto& _Bytes  = _SDSDLL unpack_integer(_Num); // unpack into a 8-byte string
    memory_traits::copy(_Buf, _Bytes.data(), _Bytes.size());
    return true;
}

template _SDSDLL_API struct xxhash_traits<char>;
template _SDSDLL_API struct xxhash_traits<unsigned char>;
template _SDSDLL_API struct xxhash_traits<wchar_t>;
_SDSDLL_END

#endif // _SDSDLL_PREPROCESSOR_GUARD