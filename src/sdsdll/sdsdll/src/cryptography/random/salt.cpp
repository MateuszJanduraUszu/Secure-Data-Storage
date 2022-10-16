// salt.cpp

// Copyright (c) Mateusz Jandura. All rights reserved.
// SPDX-License-Identifier: Apache-2.0

#include <build/sdsdll_pch.hpp>
#include <cryptography/random/salt.hpp>
#if _SDSDLL_PREPROCESSOR_GUARD

_SDSDLL_BEGIN
// FUNCTION TEMPLATE salt constructors/destructor
template <class _Eng>
constexpr salt<_Eng>::salt() noexcept : _Mybase() {}

template <class _Eng>
constexpr salt<_Eng>::salt(const salt& _Other) noexcept : _Mybase(_Other) {}

template <class _Eng>
constexpr salt<_Eng>::salt(salt&& _Other) noexcept : _Mybase(_STD move(_Other)) {}

template <class _Eng>
constexpr salt<_Eng>::salt(const value_type* const _Ptr) noexcept : _Mybase(_Ptr) {}

template <class _Eng>
_CONSTEXPR20 salt<_Eng>::~salt() noexcept {}

// FUNCTION TEMPLATE salt::operator=
template <class _Eng>
constexpr salt<_Eng>& salt<_Eng>::operator=(const salt& _Other) noexcept {
    _Mybase::operator=(_Other);
    return *this;
}

template <class _Eng>
constexpr salt<_Eng>& salt<_Eng>::operator=(salt&& _Other) noexcept {
    _Mybase::operator=(_STD move(_Other));
    return *this;
}

template <class _Eng>
constexpr salt<_Eng>& salt<_Eng>::operator=(const value_type* const _Ptr) noexcept {
    _Mybase::operator=(_Ptr);
    return *this;
}

// FUNCTION TEMPLATE operator==
template <class _Eng>
_NODISCARD constexpr bool operator==(const salt<_Eng>& _Left, const salt<_Eng>& _Right) noexcept {
    using _Salt_t = salt<_Eng>;
    return memory_traits::compare(
        _Left.get(), _Right.get(), _Salt_t::size * sizeof(typename _Salt_t::value_type)) == 0;
}

// FUNCTION TEMPLATE operator!=
template <class _Eng>
_NODISCARD constexpr bool operator!=(const salt<_Eng>& _Left, const salt<_Eng>& _Right) noexcept {
    using _Salt_t = salt<_Eng>;
    return memory_traits::compare(
        _Left.get(), _Right.get(), _Salt_t::size * sizeof(typename _Salt_t::value_type)) != 0;
}

// auxilary macros for sdsdll::salt class export/import
#define _EXPORT_OR_IMPORT_SALT_BASE(_Eng, _Elem)     \
    template _SDSDLL_API class salt<_Eng<_Elem>>;    \
    template _SDSDLL_API _NODISCARD bool operator==( \
        const salt<_Eng<_Elem>>&, const salt<_Eng<_Elem>>&);

#define _EXPORT_OR_IMPORT_SALT(_Eng)        \
    _EXPORT_OR_IMPORT_SALT_BASE(_Eng, char) \
    _EXPORT_OR_IMPORT_SALT_BASE(_Eng, wchar_t)

_EXPORT_OR_IMPORT_SALT(_Argon2d_default_engine)
_EXPORT_OR_IMPORT_SALT(_Argon2i_default_engine)
_EXPORT_OR_IMPORT_SALT(_Argon2id_default_engine)
_EXPORT_OR_IMPORT_SALT(_Scrypt_default_engine)
#undef _EXPORT_OR_IMPORT_SALT_BASE
#undef _EXPORT_OR_IMPORT_SALT
_SDSDLL_END

#endif // _SDSDLL_PREPROCESSOR_GUARD