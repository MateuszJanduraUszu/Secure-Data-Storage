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

// FUNCTION TEMPLATE _Sha512_fn::operator()
template <class _Simple_types>
_NODISCARD constexpr bool _Sha512_fn<_Simple_types>::operator()(
    byte_type* const _Buf, const size_type _Buf_size, const char_type* const _Data,
    const size_type _Data_size) const noexcept {
    if (!_Buf || _Buf_size == 0) {
        return false;
    }

    _Sha512_context_proxy _Proxy;
    if (!_Proxy._Ctx) {
        return false;
    }

    if (::EVP_DigestInit_ex(_Proxy._Ctx, ::EVP_sha512(), nullptr) == 0) {
        return false;
    }

    if (::EVP_DigestUpdate(_Proxy._Ctx, _Data, _Data_size * sizeof(char_type)) == 0) {
        return false;
    }

    return ::EVP_DigestFinal_ex(_Proxy._Ctx, _Buf, nullptr) != 0;
}

// FUNCTION TEMPLATE _Sha512_stream_fn::operator()
template <class _Simple_types>
_NODISCARD constexpr bool _Sha512_stream_fn<_Simple_types>::operator()(
    EVP_MD_CTX* const _Ctx, byte_type* const _Buf, const size_type _Buf_size,
    const char_type* const _Data, const size_type _Data_size) const noexcept {
    if (!_Ctx || !_Buf || _Buf_size == 0) {
        return false;
    }

    return ::EVP_DigestUpdate(_Ctx, _Data, _Data_size) != 0;
}

// FUNCTION TEMPLATE _Sha512_default_engine copy constructor/destructor
template <class _Elem>
constexpr _Sha512_default_engine<_Elem>::_Sha512_default_engine(const _Elem* const _Data,
    const size_type _Size) noexcept : _Mypair(_Ebco_default_init{}, size) {
    if (!_Mypair._Val1._Empty()) { // buffer allocated, complete the task
        _Release_on_failure(_Mypair._Val1,
            _Mypair._Get_val2()(_Mypair._Val1._Get(), size, _Data, _Size));
    }
}

template <class _Elem>
_CONSTEXPR20 _Sha512_default_engine<_Elem>::~_Sha512_default_engine() noexcept {}

// FUNCTION TEMPLATE _Sha512_default_engine::bytes
template <class _Elem>
_NODISCARD constexpr typename _Sha512_default_engine<_Elem>::byte_string
    _Sha512_default_engine<_Elem>::bytes() const {
    return !_Mypair._Val1._Empty() ? byte_string{_Mypair._Val1._Get(), size} : byte_string{};
}

// FUNCTION _Sha512_stream_default_engine constructor/destructor
_Sha512_stream_default_engine::_Sha512_stream_default_engine() noexcept
    : _Mypair(_Ebco_default_init{}, size), _Myproxy() {
    if (_Myproxy._Ctx) { // allocated context, initialize it
        ::EVP_DigestInit_ex(_Myproxy._Ctx, ::EVP_sha3_512(), nullptr);
    }
}

_Sha512_stream_default_engine::~_Sha512_stream_default_engine() noexcept {}

// FUNCTION _Sha512_stream_default_engine::next_chunk
_NODISCARD bool _Sha512_stream_default_engine::next_chunk(
    const char_type* const _Chunk, const size_type _Count) noexcept {
    if (!_Myproxy._Ctx) { // invalid context
        return false;
    }

    return _Mypair._Get_val2()(_Myproxy._Ctx, _Mypair._Val1._Get(), size, _Chunk, _Count);
}

// FUNCTION _Sha512_stream_default_engine::bytes
_NODISCARD typename _Sha512_stream_default_engine::byte_string
    _Sha512_stream_default_engine::bytes() {
    if (!_Myproxy._Ctx) { // invalid context
        return byte_string{};
    }

    return ::EVP_DigestFinal_ex(_Myproxy._Ctx, _Mypair._Val1._Get(), nullptr) != 0 ?
        byte_string{_Mypair._Val1._Get(), size} : byte_string{};
}

// FUNCTION TEMPLATE sha512
template <class _Elem, class _Eng>
_NODISCARD constexpr byte_string sha512(const _Elem* const _Data, const size_t _Count) {
    return _Hash_data<_Elem, _Eng>(_Data, _Count);
}

template _SDSDLL_API _NODISCARD byte_string sha512(const char* const, const size_t);
template _SDSDLL_API _NODISCARD byte_string sha512(const wchar_t* const, const size_t);

template <class _Elem, class _Eng>
_NODISCARD constexpr byte_string sha512(const _Elem* const _Data) {
    using _Size_t = typename _Eng::size_type;
    using _Traits = string_traits<_Elem, _Size_t>;
    return _Hash_data<_Elem, _Eng>(_Data, _Traits::length(_Data));
}

template _SDSDLL_API _NODISCARD byte_string sha512(const char* const);
template _SDSDLL_API _NODISCARD byte_string sha512(const wchar_t* const);

template <class _Elem, class _Eng>
_NODISCARD constexpr byte_string sha512(const basic_string_view<_Elem> _Data) {
    return _Hash_data<_Elem, _Eng>(_Data.data(), _Data.size());
}

template _SDSDLL_API _NODISCARD byte_string sha512(const string_view);
template _SDSDLL_API _NODISCARD byte_string sha512(const wstring_view);

template <class _Elem, class _Eng>
_NODISCARD constexpr byte_string sha512(const basic_string<_Elem>& _Data) {
    return _Hash_data<_Elem, _Eng>(_Data.c_str(), _Data.size());
}

template _SDSDLL_API _NODISCARD byte_string sha512(const string&);
template _SDSDLL_API _NODISCARD byte_string sha512(const wstring&);

// FUNCTION TEMPLATE sha512_file
template <class _Eng>
_NODISCARD constexpr byte_string sha512_file(file& _File, const size_t _Off) {
    return _Hash_file<_Eng>(_File, _Off);
}

template _SDSDLL_API _NODISCARD byte_string sha512_file(file&, const size_t);

template <class _Eng>
_NODISCARD constexpr byte_string sha512_file(const path& _Target, const size_t _Off) {
    file _File(_Target);
    return _Hash_file<_Eng>(_File, _Off);
}

template _SDSDLL_API _NODISCARD byte_string sha512_file(const path&, const size_t);
_SDSDLL_END

#endif // _SDSDLL_PREPROCESSOR_GUARD