// xxhash.cpp

// Copyright (c) Mateusz Jandura. All rights reserved.
// SPDX-License-Identifier: Apache-2.0

#include <build/sdsdll_pch.hpp>
#include <cryptography/hash/generic/xxhash.hpp>
#if _SDSDLL_PREPROCESSOR_GUARD

_SDSDLL_BEGIN
// FUNCTION TEMPLATE _Xxhash_fn::operator()
template <class _Simple_types>
_NODISCARD constexpr bool _Xxhash_fn<_Simple_types>::operator()(byte_type* const _Buf,
    const size_type _Buf_size, const char_type* const _Data, const size_type _Data_size) const noexcept {
    if (!_Buf || _Buf_size < sizeof(uint64_t)) { // buffer size must be at least 64 bits (8 bytes)
        return false;
    }

    const uint64_t _Hash = ::XXH3_64bits(_Data, _Data_size * sizeof(char_type));
    const auto& _Bytes   = _SDSDLL unpack_integer(_Hash); // convert into a 8-byte string
    memory_traits::move(_Buf, _Bytes.data(), _Bytes.size());
    return true;
}

// FUNCTION TEMPLATE _Xxhash_stream_fn::operator()
template <class _Simple_types>
_NODISCARD constexpr bool _Xxhash_stream_fn<_Simple_types>::operator()(
    XXH3_state_t* const _State, const char_type* const _Data, const size_type _Data_size) const noexcept {
    return ::XXH3_64bits_update(_State, _Data, _Data_size * sizeof(char_type)) == XXH_OK;
}

// FUNCTION TEMPLATE _Xxhash_default_engine copy constructor/destructor
template <class _Elem>
constexpr _Xxhash_default_engine<_Elem>::_Xxhash_default_engine(
    const _Elem* const _Data, const size_type _Size) noexcept : _Mypair(_Ebco_default_init{}, size) {
    if (!_Mypair._Val1._Empty()) { // buffer allocated, complete the task 
        _Release_on_failure(
            _Mypair._Val1, _Mypair._Get_val2()(_Mypair._Val1._Get(), size, _Data, _Size));
    }
}

template <class _Elem>
_CONSTEXPR20 _Xxhash_default_engine<_Elem>::~_Xxhash_default_engine() noexcept {}

// FUNCTION TEMPLATE _Xxhash_default_engine::bytes
template <class _Elem>
_NODISCARD constexpr typename _Xxhash_default_engine<_Elem>::byte_string
    _Xxhash_default_engine<_Elem>::bytes() const {
    return !_Mypair._Val1._Empty() ? byte_string{_Mypair._Val1._Get(), size} : byte_string{};
}

// FUNCTION _Xxhash_stream_default_engine constructor/destructor
_Xxhash_stream_default_engine::_Xxhash_stream_default_engine() noexcept
    : _Myfunc(), _Mystate(::XXH3_createState()) {
    if (_Mystate) { // state allocate, try to initialize it
        if (::XXH3_64bits_reset(_Mystate) != XXH_OK) { // initialization failed, release the state
            ::XXH3_freeState(_Mystate);
            _Mystate = nullptr;
        }
    }
}

_Xxhash_stream_default_engine::~_Xxhash_stream_default_engine() noexcept {
    if (_Mystate) {
        ::XXH3_freeState(_Mystate);
        _Mystate = nullptr;
    }
}

// FUNCTION _Xxhash_stream_default_engine::next_chunk
_NODISCARD bool _Xxhash_stream_default_engine::next_chunk(
    const char_type* const _Data, const size_type _Count) noexcept {
    if (!_Mystate) { // invalid state
        return false;
    }

    return _Myfunc(_Mystate, _Data, _Count);
}

// FUNCTION _Xxhash_stream_default_engine::bytes
_NODISCARD typename _Xxhash_stream_default_engine::byte_string
    _Xxhash_stream_default_engine::bytes() {
    if (!_Mystate) { // invalid state
        return byte_string{};
    }

    const uint64_t _Hash = ::XXH3_64bits_digest(_Mystate);
    const auto& _Bytes   = _SDSDLL unpack_integer(_Hash);
    return byte_string{_Bytes.data(), size};
}

// FUNCTION TEMPLATE xxhash
template <class _Elem, class _Eng>
_NODISCARD constexpr byte_string xxhash(const _Elem* const _Data, const size_t _Count) {
    return _Hash_data<_Elem, _Eng>(_Data, _Count);
}

template _SDSDLL_API _NODISCARD byte_string xxhash(const char* const, const size_t);
template _SDSDLL_API _NODISCARD byte_string xxhash(const wchar_t* const, const size_t);

template <class _Elem, class _Eng>
_NODISCARD constexpr byte_string xxhash(const _Elem* const _Data) {
    using _Size_t = typename _Eng::size_type;
    using _Traits = string_traits<_Elem, _Size_t>;
    return _Hash_data<_Elem, _Eng>(_Data, _Traits::length(_Data));
}

template _SDSDLL_API _NODISCARD byte_string xxhash(const char* const);
template _SDSDLL_API _NODISCARD byte_string xxhash(const wchar_t* const);

template <class _Elem, class _Eng>
_NODISCARD constexpr byte_string xxhash(const basic_string_view<_Elem> _Data) {
    return _Hash_data<_Elem, _Eng>(_Data.data(), _Data.size());
}

template _SDSDLL_API _NODISCARD byte_string xxhash(const string_view);
template _SDSDLL_API _NODISCARD byte_string xxhash(const wstring_view);

template <class _Elem, class _Eng>
_NODISCARD constexpr byte_string xxhash(const basic_string<_Elem>& _Data) {
    return _Hash_data<_Elem, _Eng>(_Data.c_str(), _Data.size());
}

template _SDSDLL_API _NODISCARD byte_string xxhash(const string&);
template _SDSDLL_API _NODISCARD byte_string xxhash(const wstring&);

// FUNCTION TEMPLATE xxhash_file
template <class _Eng>
_NODISCARD constexpr byte_string xxhash_file(file& _File, const size_t _Off) {
    return _Hash_file<_Eng>(_File, _Off);
}

template _SDSDLL_API _NODISCARD byte_string xxhash_file(file&, const size_t);

template <class _Eng>
_NODISCARD constexpr byte_string xxhash_file(const path& _Target, const size_t _Off) {
    file _File(_Target);
    return _Hash_file<_Eng>(_File, _Off);
}

template _SDSDLL_API _NODISCARD byte_string xxhash_file(const path&, const size_t);
_SDSDLL_END

#endif // _SDSDLL_PREPROCESSOR_GUARD