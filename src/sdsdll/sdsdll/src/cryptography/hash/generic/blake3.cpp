// blake3.cpp

// Copyright (c) Mateusz Jandura. All rights reserved.
// SPDX-License-Identifier: Apache-2.0

#include <build/sdsdll_pch.hpp>
#include <cryptography/hash/generic/blake3.hpp>
#if _SDSDLL_PREPROCESSOR_GUARD

_SDSDLL_BEGIN
// FUNCTION TEMPLATE _Blake3_fn::operator()
template <class _Simple_types>
_NODISCARD constexpr bool _Blake3_fn<_Simple_types>::operator()(byte_type* const _Buf,
    const size_type _Buf_size, const char_type* const _Data, const size_type _Data_size) const noexcept {
    if (!_Buf || _Buf_size == 0) {
        return false;
    }

    blake3_hasher _Hasher;
    ::blake3_hasher_init(_SDSDLL addressof(_Hasher));
    ::blake3_hasher_update(_SDSDLL addressof(_Hasher), _Data, _Data_size * sizeof(char_type));
    ::blake3_hasher_finalize(_SDSDLL addressof(_Hasher), _Buf, _Buf_size);
    return true;
}

// FUNCTION TEMPLATE _Blake3_stream_fn::operator()
template <class _Simple_types>
_NODISCARD constexpr bool _Blake3_stream_fn<_Simple_types>::operator()(
    blake3_hasher* const _Hasher, byte_type* const _Buf, const size_type _Buf_size,
    const char_type* const _Data, const size_type _Data_size) const noexcept {
    if (!_Hasher || !_Buf || _Buf_size == 0) {
        return false;
    }

    ::blake3_hasher_update(_Hasher, _Data, _Data_size * sizeof(char_type));
    return true;
}

// FUNCTION TEMPLATE _Blake3_default_engine copy constructor/destructor
template <class _Elem>
constexpr _Blake3_default_engine<_Elem>::_Blake3_default_engine(const _Elem* const _Data,
    const size_type _Size) noexcept : _Mypair(_Ebco_default_init{}, size) {
    if (!_Mypair._Val1._Empty()) { // buffer allocated, complete the task
        _Release_on_failure(_Mypair._Val1,
            _Mypair._Get_val2()(_Mypair._Val1._Get(), size, _Data, _Size));
    }
}

template <class _Elem>
_CONSTEXPR20 _Blake3_default_engine<_Elem>::~_Blake3_default_engine() noexcept {}

// FUNCTION TEMPLATE _Blake3_default_engine::bytes
template <class _Elem>
_NODISCARD constexpr typename _Blake3_default_engine<_Elem>::byte_string
    _Blake3_default_engine<_Elem>::bytes() const {
    return !_Mypair._Val1._Empty() ? byte_string{_Mypair._Val1._Get(), size} : byte_string{};
}

// FUNCTION _Blake3_stream_default_engine constructor/destructor
_Blake3_stream_default_engine::_Blake3_stream_default_engine() noexcept
    : _Mypair(_Ebco_default_init{}, size) {
    if (!_Mypair._Val1._Empty()) { // buffer allocated, try to allocate a hasher
        allocator<blake3_hasher> _Al;
        _Myhasher = _Al.allocate(1);
        if (_Myhasher) { // hasher allocated, initialize it
            ::blake3_hasher_init(_Myhasher);
        }
    } else {
        _Myhasher = nullptr;
    }
}

_Blake3_stream_default_engine::~_Blake3_stream_default_engine() noexcept {
    if (_Myhasher) {
        allocator<blake3_hasher> _Al;
        _Al.deallocate(_Myhasher, 1);
        _Myhasher = nullptr;
    }
}

// FUNCTION _Blake3_stream_default_engine::next_chunk
_NODISCARD bool _Blake3_stream_default_engine::next_chunk(
    const char_type* const _Chunk, const size_type _Count) noexcept {
    if (!_Myhasher) { // invalid hasher
        return false;
    }

    return _Mypair._Get_val2()(_Myhasher, _Mypair._Val1._Get(), size, _Chunk, _Count);
}

// FUNCTION _Blake3_stream_default_engine::bytes
_NODISCARD typename _Blake3_stream_default_engine::byte_string
    _Blake3_stream_default_engine::bytes() {
    if (!_Myhasher) { // invalid hasher
        return byte_string{};
    }

    ::blake3_hasher_finalize(_Myhasher, _Mypair._Val1._Get(), size);
    return byte_string{_Mypair._Val1._Get(), size};
}

// FUNCTION TEMPLATE blake3
template <class _Elem, class _Eng>
_NODISCARD constexpr byte_string blake3(const _Elem* const _Data, const size_t _Count) {
    return _Hash_data<_Elem, _Eng>(_Data, _Count);
}

template _SDSDLL_API _NODISCARD byte_string blake3(const char* const, const size_t);
template _SDSDLL_API _NODISCARD byte_string blake3(const wchar_t* const, const size_t);

template <class _Elem, class _Eng>
_NODISCARD constexpr byte_string blake3(const _Elem* const _Data) {
    using _Size_t = typename _Eng::size_type;
    using _Traits = string_traits<_Elem, _Size_t>;
    return _Hash_data<_Elem, _Eng>(_Data, _Traits::length(_Data));
}

template _SDSDLL_API _NODISCARD byte_string blake3(const char* const);
template _SDSDLL_API _NODISCARD byte_string blake3(const wchar_t* const);

template <class _Elem, class _Eng>
_NODISCARD constexpr byte_string blake3(const basic_string_view<_Elem> _Data) {
    return _Hash_data<_Elem, _Eng>(_Data.data(), _Data.size());
}

template _SDSDLL_API _NODISCARD byte_string blake3(const string_view);
template _SDSDLL_API _NODISCARD byte_string blake3(const wstring_view);

template <class _Elem, class _Eng>
_NODISCARD constexpr byte_string blake3(const basic_string<_Elem>& _Data) {
    return _Hash_data<_Elem, _Eng>(_Data.c_str(), _Data.size());
}

template _SDSDLL_API _NODISCARD byte_string blake3(const string&);
template _SDSDLL_API _NODISCARD byte_string blake3(const wstring&);

// FUNCTION TEMPLATE blake3_file
template <class _Eng>
_NODISCARD constexpr byte_string blake3_file(file& _File, const size_t _Off) {
    return _Hash_file<_Eng>(_File, _Off);
}

template _SDSDLL_API _NODISCARD byte_string blake3_file(file&, const size_t);

template <class _Eng>
_NODISCARD constexpr byte_string blake3_file(const path& _Target, const size_t _Off) {
    file _File(_Target);
    return _Hash_file<_Eng>(_File, _Off);
}

template _SDSDLL_API _NODISCARD byte_string blake3_file(const path&, const size_t);
_SDSDLL_END

#endif // _SDSDLL_PREPROCESSOR_GUARD