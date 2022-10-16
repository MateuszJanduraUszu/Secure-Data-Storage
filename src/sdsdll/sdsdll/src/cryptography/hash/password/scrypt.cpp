// scrypt.cpp

// Copyright (c) Mateusz Jandura. All rights reserved.
// SPDX-License-Identifier: Apache-2.0

#include <build/sdsdll_pch.hpp>
#include <cryptography/hash/password/scrypt.hpp>
#if _SDSDLL_PREPROCESSOR_GUARD

_SDSDLL_BEGIN
// FUNCTION TEMPLATE _Scrypt_fn::operator()
template <class _Simple_types>
_NODISCARD constexpr bool _Scrypt_fn<_Simple_types>::operator()(byte_type* const _Buf,
    const size_type _Buf_size, const char* const _Data, const size_type _Data_size,
    const byte_type* const _Salt, const size_type _Salt_size, const size_type _Cost,
    const size_type _Block_size, const size_type _Parallelism) const noexcept {
    if (!_Buf || _Buf_size == 0) {
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

    try {
        ::Botan::scrypt( // throws if one of the arguments is invalid
            _Buf, _Buf_size, _Data, _Data_size, _Salt, _Salt_size, _Cost, _Block_size, _Parallelism);
        return true;
    } catch (...) { // handler not used
        return false;
    }
}

// FUNCTION TEMPLATE _Scrypt_default_engine copy constructor/destructor
template <class _Elem>
constexpr _Scrypt_default_engine<_Elem>::_Scrypt_default_engine(
    const _Elem* const _Data, const size_type _Size, const byte_type* const _Salt,
    const size_type _Cost, const size_type _Block_size, const size_type _Parallelism) noexcept
    : _Mypair(_Ebco_default_init{}, size) {
    if (!_Mypair._Val1._Empty()) { // buffer allocated, complete the task
        if _CONSTEXPR_IF (is_same_v<_Elem, char>) {
            _Release_on_failure(_Mypair._Val1, _Mypair._Get_val2()(
                _Mypair._Val1._Get(), size, _Data, _Size, _Salt,
                    salt_size, _Cost, _Block_size, _Parallelism));
        } else {
            const utf8_string& _Narrow = utf8_string::from_utf16(_Data, _Size);
            _Release_on_failure(_Mypair._Val1, _Mypair._Get_val2()(
                _Mypair._Val1._Get(), size, _Narrow.c_str(), _Narrow.size(),
                    _Salt, salt_size, _Cost, _Block_size, _Parallelism));
        }
    }
}

template <class _Elem>
_CONSTEXPR20 _Scrypt_default_engine<_Elem>::~_Scrypt_default_engine() noexcept {}

// FUNCTION TEMPLATE _Scrypt_default_engine::bytes
template <class _Elem>
_NODISCARD constexpr typename _Scrypt_default_engine<_Elem>::byte_string
    _Scrypt_default_engine<_Elem>::bytes() const {
    return !_Mypair._Val1._Empty() ? byte_string{_Mypair._Val1._Get(), size} : byte_string{};
}

// auxiliary macro for sdsdll::scrypt() function export/import
#define _EXPORT_OR_IMPORT_SCRYPT(_Elem, _Str, ...)                        \
    template _SDSDLL_API _NODISCARD byte_string scrypt(_Str, __VA_ARGS__, \
        const salt<_Scrypt_default_engine<_Elem>>&, const size_t, const size_t, const size_t)

// FUNCTION TEMPLATE scrypt
template <class _Elem, class _Eng>
_NODISCARD constexpr byte_string scrypt(
    const _Elem* const _Data, const size_t _Size, const salt<_Eng>& _Salt,
    const size_t _Cost, const size_t _Block_size, const size_t _Parallelism) {
    return _Hash_password<_Elem, _Eng>(_Data, _Size, _Salt, _Cost, _Block_size, _Parallelism);
}

_EXPORT_OR_IMPORT_SCRYPT(char, const char* const, const size_t);
_EXPORT_OR_IMPORT_SCRYPT(wchar_t, const wchar_t* const, const size_t);

template <class _Elem, class _Eng>
_NODISCARD constexpr byte_string scrypt(const _Elem* const _Data, const salt<_Eng>& _Salt,
    const size_t _Cost, const size_t _Block_size, const size_t _Parallelism) {
    using _Size_t = typename _Eng::size_type;
    using _Traits = string_traits<_Elem, _Size_t>;
    return _Hash_password<_Elem, _Eng>(
        _Data, _Traits::length(_Data), _Salt, _Cost, _Block_size, _Parallelism);
}

_EXPORT_OR_IMPORT_SCRYPT(char, const char* const);
_EXPORT_OR_IMPORT_SCRYPT(wchar_t, const wchar_t* const);

template <class _Elem, class _Eng>
_NODISCARD constexpr byte_string scrypt(const basic_string_view<_Elem> _Data, const salt<_Eng>& _Salt,
    const size_t _Cost, const size_t _Block_size, const size_t _Parallelism) {
    return _Hash_password<_Elem, _Eng>(_Data.data(), _Data.size(), _Salt, _Cost, _Block_size, _Parallelism);
}

_EXPORT_OR_IMPORT_SCRYPT(char, const string_view);
_EXPORT_OR_IMPORT_SCRYPT(wchar_t, const wstring_view);

template <class _Elem, class _Eng>
_NODISCARD constexpr byte_string scrypt(const basic_string<_Elem>& _Data, const salt<_Eng>& _Salt,
    const size_t _Cost, const size_t _Block_size, const size_t _Parallelism) {
    return _Hash_password<_Elem, _Eng>(_Data.c_str(), _Data.size(), _Salt, _Cost, _Block_size, _Parallelism);
}

_EXPORT_OR_IMPORT_SCRYPT(char, const string&);
_EXPORT_OR_IMPORT_SCRYPT(wchar_t, const wstring&);
#undef _EXPORT_OR_IMPORT_SCRYPT
_SDSDLL_END

#endif // _SDSDLL_PREPROCESSOR_GUARD