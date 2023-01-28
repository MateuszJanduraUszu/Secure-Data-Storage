// xxhash.cpp

// Copyright (c) Mateusz Jandura. All rights reserved.
// SPDX-License-Identifier: Apache-2.0

#include <build/sdsdll_pch.hpp>
#include <cryptography/hash/generic/xxhash.hpp>
#if _SDSDLL_PREPROCESSOR_GUARD

_SDSDLL_BEGIN
// FUNCTION xxhash_state constructor/destructor
xxhash_state::xxhash_state() noexcept : _Myimpl(::XXH3_createState()) {
    if (_Myimpl) {
        ::XXH3_64bits_reset(_Myimpl);
    }
}

xxhash_state::~xxhash_state() noexcept {
    if (_Myimpl) {
        ::XXH3_freeState(_Myimpl);
        _Myimpl = nullptr;
    }
}

// FUNCTION xxhash_state::get
_NODISCARD xxhash_state::pointer xxhash_state::get() noexcept {
    return _Myimpl;
}

_NODISCARD xxhash_state::const_pointer xxhash_state::get() const noexcept {
    return _Myimpl;
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

    constexpr size_type _Chunk_size = 1024; // 1024-byte chunk
    byte_type _Chunk[_Chunk_size];
    size_type _Read = 0; // read bytes, must be initialized
    state_type _State;
    if (!_State.get()) {
        return false;
    }

    for (;;) {
        if (!_File.read(_Chunk, _Chunk_size, _Chunk_size, &_Read)) {
            return false;
        }

        if (_Read == 0) { // no more data
            break;
        }

        if (::XXH3_64bits_update(_State.get(), _Chunk, _Read) != XXH_OK) {
            return false;
        }
    }

    const uint64_t _Num = ::XXH3_64bits_digest(_State.get());
    const auto& _Bytes  = _SDSDLL unpack_integer(_Num); // unpack into a 8-byte string
    memory_traits::copy(_Buf, _Bytes.data(), _Bytes.size());
    return true;
}

template struct _SDSDLL_API xxhash_traits<char>;
template struct _SDSDLL_API xxhash_traits<unsigned char>;
template struct _SDSDLL_API xxhash_traits<wchar_t>;

// FUNCTION TEMPLATE xxhash_stream_traits::bytes_count
template <class _Elem>
_NODISCARD constexpr typename xxhash_stream_traits<_Elem>::size_type
    xxhash_stream_traits<_Elem>::bytes_count() noexcept {
    return bits / CHAR_BIT; // always 8 bytes
}

// FUNCTION TEMPLATE xxhash_stream_traits::init
template <class _Elem>
_NODISCARD constexpr bool xxhash_stream_traits<_Elem>::init(state_type&) noexcept {
    return true; // no initialization
}

// FUNCTION TEMPLATE xxhash_stream_traits::append
template <class _Elem>
_NODISCARD constexpr bool xxhash_stream_traits<_Elem>::append(
    state_type& _State, const char_type* const _Data, const size_type _Count) noexcept {
    if (!_State.get()) {
        return false;
    }

    return ::XXH3_64bits_update(_State.get(), _Data, _Count * sizeof(char_type)) == XXH_OK;
}

// FUNCTION TEMPLATE xxhash_stream_traits::complete
template <class _Elem>
_NODISCARD constexpr bool xxhash_stream_traits<_Elem>::complete(
    state_type& _State, byte_type* const _Buf, const size_type _Buf_size) noexcept {
    if (!_State.get()) {
        return false;
    }

    constexpr size_type _Optimal_buf_size = bytes_count();
    if (!_Buf || _Buf_size < _Optimal_buf_size) {
        return false;
    }

    const uint64_t _Num = ::XXH3_64bits_digest(_State.get());
    const auto& _Bytes  = _SDSDLL unpack_integer(_Num); // unpack into a 8-byte string
    memory_traits::copy(_Buf, _Bytes.data(), _Bytes.size());
    return true;
}

template struct _SDSDLL_API xxhash_stream_traits<char>;
template struct _SDSDLL_API xxhash_stream_traits<unsigned char>;
template struct _SDSDLL_API xxhash_stream_traits<wchar_t>;
_SDSDLL_END

#endif // _SDSDLL_PREPROCESSOR_GUARD