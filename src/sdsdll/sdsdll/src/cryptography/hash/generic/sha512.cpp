// sha512.cpp

// Copyright (c) Mateusz Jandura. All rights reserved.
// SPDX-License-Identifier: Apache-2.0

#include <build/sdsdll_pch.hpp>
#include <cryptography/hash/generic/sha512.hpp>
#if _SDSDLL_PREPROCESSOR_GUARD

_SDSDLL_BEGIN
// FUNCTION sha512_state constructor/destructor
sha512_state::sha512_state() noexcept : _Myimpl(::EVP_MD_CTX_new()) {}

sha512_state::~sha512_state() noexcept {
    if (_Myimpl) {
        ::EVP_MD_CTX_free(_Myimpl);
        _Myimpl = nullptr;
    }
}

// FUNCTION sha512_state::get
_NODISCARD sha512_state::pointer sha512_state::get() noexcept {
    return _Myimpl;
}

_NODISCARD sha512_state::const_pointer sha512_state::get() const noexcept {
    return _Myimpl;
}

// FUNCTION TEMPLATE sha512_traits::bytes_count
template <class _Elem>
_NODISCARD constexpr typename sha512_traits<_Elem>::size_type
    sha512_traits<_Elem>::bytes_count() noexcept {
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

    state_type _State;
    if (!_State.get()) {
        return false;
    }

    if (::EVP_DigestInit_ex(_State.get(), ::EVP_sha3_512(), nullptr) == 0) {
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

        if (::EVP_DigestUpdate(_State.get(), _Chunk, _Read) == 0) {
            return false;
        }
    }

    uint32_t _Bytes = 0; // hashed bytes (unused)
    return ::EVP_DigestFinal_ex(_State.get(), _Buf, &_Bytes) != 0;
}

template struct _SDSDLL_API sha512_traits<char>;
template struct _SDSDLL_API sha512_traits<unsigned char>;
template struct _SDSDLL_API sha512_traits<wchar_t>;

// FUNCTION TEMPLATE sha512_stream_traits::bytes_count
template <class _Elem>
_NODISCARD constexpr typename sha512_stream_traits<_Elem>::size_type
    sha512_stream_traits<_Elem>::bytes_count() noexcept {
    return bits / CHAR_BIT; // always 64 bytes
}

// FUNCTION TEMPLATE sha512_stream_traits::init
template <class _Elem>
_NODISCARD constexpr bool sha512_stream_traits<_Elem>::init(state_type& _State) noexcept {
    if (!_State.get()) {
        return false;
    }

    return ::EVP_DigestInit_ex(_State.get(), ::EVP_sha3_512(), nullptr) != 0;
}

// FUNCTION TEMPLATE sha512_stream_traits::append
template <class _Elem>
_NODISCARD constexpr bool sha512_stream_traits<_Elem>::append(
    state_type& _State, const char_type* const _Data, const size_type _Count) noexcept {
    if (!_State.get()) {
        return false;
    }

    return ::EVP_DigestUpdate(_State.get(), _Data, _Count * sizeof(char_type)) != 0;
}

// FUNCTION TEMPLATE sha512_stream_traits::complete
template <class _Elem>
_NODISCARD constexpr bool sha512_stream_traits<_Elem>::complete(
    state_type& _State, byte_type* const _Buf, const size_type _Buf_size) noexcept {
    if (!_State.get()) {
        return false;
    }

    if (!_Buf || _Buf_size < bytes_count()) {
        return false;
    }

    uint32_t _Bytes = 0; // hashed bytes (unused)
    return ::EVP_DigestFinal_ex(_State.get(), _Buf, &_Bytes) != 0;
}

template struct _SDSDLL_API sha512_stream_traits<char>;
template struct _SDSDLL_API sha512_stream_traits<unsigned char>;
template struct _SDSDLL_API sha512_stream_traits<wchar_t>;
_SDSDLL_END

#endif // _SDSDLL_PREPROCESSOR_GUARD