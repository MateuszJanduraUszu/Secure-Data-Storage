// argon2i.cpp

// Copyright (c) Mateusz Jandura. All rights reserved.
// SPDX-License-Identifier: Apache-2.0

#include <build/sdsdll_pch.hpp>
#include <cryptography/hash/password/argon2d.hpp>
#if _SDSDLL_PREPROCESSOR_GUARD

_SDSDLL_BEGIN
// FUNCTION TEMPLATE _Argon2i_fn::operator()
template <class _Simple_types>
_NODISCARD constexpr bool _Argon2i_fn<_Simple_types>::operator()(byte_type* const _Buf,
    const size_type _Buf_size, const char* const _Data, const size_type _Data_size,
    const byte_type* const _Salt, const size_type _Salt_size, const size_type _Memory_amount,
    const size_type _Iterations, const size_type _Parallelism) const noexcept {
    if (!_Buf || _Buf_size == 0) {
        return false;
    }

    if (_Data_size > 0xFFFF'FFFF) { // Argon2 can hash at most 2^32-1 bytes
        return false;
    }

    constexpr uint8_t _Variant = 1; // Argon2 variant (required by Botan)
    try {
        ::Botan::argon2( // throws if something went wrong
            _Buf, _Buf_size, _Data, _Data_size, _Salt, _Salt_size, nullptr, 0,
                nullptr, 0, _Variant, _Parallelism, _Memory_amount, _Iterations);
        return true;
    } catch (...) { // handler not used
        return false;
    }
}

// FUNCTION TEMPLATE _Argon2i_default_engine copy constructor/destructor
template <class _Elem>
constexpr _Argon2i_default_engine<_Elem>::_Argon2i_default_engine(const _Elem* const _Data,
    const size_type _Size, const byte_type* const _Salt, const size_type _Memory_amount,
    const size_type _Iterations, const size_type _Parallelism) noexcept
    : _Mypair(_Ebco_default_init{}, size) {
    if (!_Mypair._Val1._Empty()) { // buffer allocated, complete the task
        if _CONSTEXPR_IF (is_same_v<_Elem, char>) {
            _Release_on_failure(_Mypair._Val1, _Mypair._Get_val2()(
                _Mypair._Val1._Get(), size, _Data, _Size, _Salt, salt_size,
                    _Memory_amount, _Iterations, _Parallelism));
        } else {
            const utf8_string& _Narrow = utf8_string::from_utf16(_Data, _Size);
            _Release_on_failure(_Mypair._Val1, _Mypair._Get_val2()(
                _Mypair._Val1._Get(), size, _Narrow.c_str(), _Narrow.size(),
                    _Salt, salt_size, _Memory_amount, _Iterations, _Parallelism));
        }
    }
}

template <class _Elem>
_CONSTEXPR20 _Argon2i_default_engine<_Elem>::~_Argon2i_default_engine() noexcept {}

// FUNCTION TEMPLATE _Argon2i_default_engine::bytes
template <class _Elem>
_NODISCARD constexpr typename _Argon2i_default_engine<_Elem>::byte_string
    _Argon2i_default_engine<_Elem>::bytes() const {
    return !_Mypair._Val1._Empty() ? byte_string{_Mypair._Val1._Get(), size} : byte_string{};
}

// auxilary macro for sdsdll::argon2i() function export/import
#define _EXPORT_OR_IMPORT_ARGON2I(_Elem, _Str, ...)                        \
    template _SDSDLL_API _NODISCARD byte_string argon2i(_Str, __VA_ARGS__, \
        const salt<_Argon2i_default_engine<_Elem>>&, const size_t, const size_t, const size_t)

// FUNCTION TEMPLATE argon2i
template <class _Elem, class _Eng>
_NODISCARD constexpr byte_string argon2i(
    const _Elem* const _Data, const size_t _Size, const salt<_Eng>& _Salt,
    const size_t _Memory_amount, const size_t _Iterations, const size_t _Parallelism) {
    return _Hash_password<_Elem, _Eng>(_Data, _Size, _Salt, _Memory_amount, _Iterations, _Parallelism);
}

_EXPORT_OR_IMPORT_ARGON2I(char, const char* const, const size_t);
_EXPORT_OR_IMPORT_ARGON2I(wchar_t, const wchar_t* const, const size_t);

template <class _Elem, class _Eng>
_NODISCARD constexpr byte_string argon2i(const _Elem* const _Data, const salt<_Eng>& _Salt,
    const size_t _Memory_amount, const size_t _Iterations, const size_t _Parallelism) {
    using _Size_t = typename _Eng::size_type;
    using _Traits = string_traits<_Elem, _Size_t>;
    return _Hash_password<_Elem, _Eng>(
        _Data, _Traits::length(_Data), _Salt, _Memory_amount, _Iterations, _Parallelism);
}

_EXPORT_OR_IMPORT_ARGON2I(char, const char* const);
_EXPORT_OR_IMPORT_ARGON2I(wchar_t, const wchar_t* const);

template <class _Elem, class _Eng>
_NODISCARD constexpr byte_string argon2i(const basic_string_view<_Elem> _Data, const salt<_Eng>& _Salt,
    const size_t _Memory_amount, const size_t _Iterations, const size_t _Parallelism) {
    return _Hash_password<_Elem, _Eng>(
        _Data.data(), _Data.size(), _Salt, _Memory_amount, _Iterations, _Parallelism);
}

_EXPORT_OR_IMPORT_ARGON2I(char, const string_view);
_EXPORT_OR_IMPORT_ARGON2I(wchar_t, const wstring_view);

template <class _Elem, class _Eng>
_NODISCARD constexpr byte_string argon2i(const basic_string<_Elem>& _Data, const salt<_Eng>& _Salt,
    const size_t _Memory_amount, const size_t _Iterations, const size_t _Parallelism) {
    return _Hash_password<_Elem, _Eng>(
        _Data.c_str(), _Data.size(), _Salt, _Memory_amount, _Iterations, _Parallelism);
}

_EXPORT_OR_IMPORT_ARGON2I(char, const string&);
_EXPORT_OR_IMPORT_ARGON2I(wchar_t, const wstring&);
#undef _EXPORT_OR_IMPORT_ARGON2I
_SDSDLL_END

#endif // _SDSDLL_PREPROCESSOR_GUARD