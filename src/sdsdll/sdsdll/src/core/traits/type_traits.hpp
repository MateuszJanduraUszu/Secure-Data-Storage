// type_traits.hpp

// Copyright (c) Mateusz Jandura. All rights reserved.
// SPDX-License-Identifier: Apache-2.0

#pragma once
#ifndef _SDSDLL_CORE_TYPE_TRAITS_HPP_
#define _SDSDLL_CORE_TYPE_TRAITS_HPP_
#include <core/defs.hpp>
#if _SDSDLL_PREPROCESSOR_GUARD
#include <core/traits/concepts.hpp>
#include <cstdint>

_SDSDLL_BEGIN
// FUNCTION TEMPLATE addressof
template <class _Ty>
_CONSTEXPR17 _Ty* addressof(_Ty& _Val) noexcept {
#if _HAS_BUILTIN(__builtin_addressof)
    return __builtin_addressof(_Val);
#else // ^^^ _HAS_BUILTIN(__builtin_addressof) ^^^ / vvv !_HAS_BUILTIN(__builtin_addressof) vvv
    return reinterpret_cast<_Ty*>(&const_cast<char&>(reinterpret_cast<const volatile char&>(_Val)));
#endif // _HAS_BUILTIN(__builtin_addressof)
}

template <class _Ty>
const _Ty* addressof(const _Ty&&) = delete;

// ALIAS TEMPLATE void_t
template <class...>
using void_t = void;

// CONSTANT TEMPLATE is_const_v
template <class _Ty>
_INLINE_VARIABLE constexpr bool is_const_v = false;

template <class _Ty>
_INLINE_VARIABLE constexpr bool is_const_v<const _Ty> = true;

// STRUCT TEMPLATE is_const
template <class _Ty>
struct is_const : bool_constant<is_const_v<_Ty>> {};

// CONSTANT TEMPLATE is_volatile_v
template <class _Ty>
_INLINE_VARIABLE constexpr bool is_volatile_v = false;

template <class _Ty>
_INLINE_VARIABLE constexpr bool is_volatile_v<volatile _Ty> = true;

// STRUCT TEMPLATE is_volatile
template <class _Ty>
struct is_volatile : bool_constant<is_volatile_v<_Ty>> {};

// STRUCT TEMPLATE _Add_reference
template <class _Ty, class = void>
struct _Add_reference { // non-referencable type, do nothing
    using _Lvalue = _Ty;
    using _Rvalue = _Ty;
};

template <class _Ty>
struct _Add_reference<_Ty, void_t<_Ty&>> { // referencable type, add reference
    using _Lvalue = _Ty&;
    using _Rvalue = _Ty&&;
};

// STRUCT TEMPLATE add_lvalue_reference
template <class _Ty>
struct add_lvalue_reference { // add lvalue reference to the _Ty (if possible)
    using type = typename _Add_reference<_Ty>::_Lvalue;
};

// ALIAS TEMPLATE add_lvalue_reference_t
template <class _Ty>
using add_lvalue_reference_t = typename add_lvalue_reference<_Ty>::type;

// STRUCT TEMPLATE add_rvalue_reference
template <class _Ty>
struct add_rvalue_reference { // add lvalue reference to the _Ty (if possible)
    using type = typename _Add_reference<_Ty>::_Rvalue;
};

// ALIAS TEMPLATE add_rvalue_reference_t
template <class _Ty>
using add_rvalue_reference_t = typename add_rvalue_reference<_Ty>::type;

// STRUCT TEMPLATE remove_reference
template <class _Ty>
struct remove_reference {
    using type = _Ty;
};

template <class _Ty>
struct remove_reference<_Ty&> {
    using type = _Ty;
};

template <class _Ty>
struct remove_reference<_Ty&&> {
    using type = _Ty;
};

// ALIAS TEMPLATE remove_reference_t
template <class _Ty>
using remove_reference_t = typename remove_reference<_Ty>::type;

// CONSTANT TEMPLATE is_constructible_v
template <class _Ty, class... _Types>
_INLINE_VARIABLE constexpr bool is_constructible_v = __is_constructible(_Ty, _Types...);

// STRUCT TEMPLATE is_constructible
template <class _Ty, class... _Types>
struct is_constructible : bool_constant<__is_constructible(_Ty, _Types...)> {};

// CONSTANT TEMPLATE is_nothrow_constructible_v
template <class _Ty, class... _Types>
_INLINE_VARIABLE constexpr bool is_nothrow_constructible_v = __is_nothrow_constructible(_Ty, _Types...);

// STRUCT TEMPLATE is_nothrow_constructible
template <class _Ty, class... _Types>
struct is_nothrow_constructible : bool_constant<__is_nothrow_constructible(_Ty, _Types...)> {};

// CONSTANT TEMPLATE is_nothrow_default_constructible_v
template <class _Ty>
_INLINE_VARIABLE constexpr bool is_nothrow_default_constructible_v = __is_nothrow_constructible(_Ty);

// STRUCT TEMPLATE is_nothrow_default_constructible
template <class _Ty>
struct is_nothrow_default_constructible : bool_constant<__is_nothrow_constructible(_Ty)> {};

// CONSTANT TEMPLATE is_nothrow_copy_constructible_v
template <class _Ty>
_INLINE_VARIABLE constexpr bool is_nothrow_copy_constructible_v = __is_nothrow_constructible(
    _Ty, add_lvalue_reference_t<const _Ty>);

// STRUCT TEMPLATE is_nothrow_copy_constructible
template <class _Ty>
struct is_nothrow_copy_constructible
    : bool_constant<__is_nothrow_constructible(_Ty, add_lvalue_reference_t<const _Ty>)> {};

// CONSTANT TEMPLATE is_nothrow_move_constructible_v
template <class _Ty>
_INLINE_VARIABLE constexpr bool is_nothrow_move_constructible_v = __is_nothrow_constructible(
    _Ty, add_rvalue_reference_t<_Ty>);

// STRUCT TEMPLATE is_nothrow_move_constructible
template <class _Ty>
struct is_nothrow_move_constructible
    : bool_constant<__is_nothrow_constructible(_Ty, add_rvalue_reference_t<_Ty>)> {};

// CONSTANT TEMPLATE is_nothrow_destructible_v
template <class _Ty>
_INLINE_VARIABLE constexpr bool is_nothrow_destructible_v = __is_nothrow_destructible(_Ty);

// STRUCT TEMPLATE is_nothrow_destructible
template <class _Ty>
struct is_nothrow_destructible : bool_constant<__is_nothrow_destructible(_Ty)> {};

// STRUCT TEMPLATE _Underlying_type
template <class _Ty, bool = is_enum_v<_Ty>>
struct _Underlying_type {
    using type = __underlying_type(_Ty);
};

template <class _Ty>
struct _Underlying_type<_Ty, false> {}; // no type member

// STRUCT TEMPLATE underlying_type
template <class _Ty>
struct underlying_type : _Underlying_type<_Ty> {};

// ALIAS TEMPLATE underlying_type_t
template <class _Ty>
using underlying_type_t = typename underlying_type<_Ty>::type;

// FUNCTION TEMPLATE _Has_bits
template <class _Ty>
_NODISCARD constexpr bool _Has_bits(const _Ty _Bitmask, const _Ty _Bits) noexcept {
    if _CONSTEXPR_IF (is_enum_v<_Ty>) {
        using _Int_t = underlying_type_t<_Ty>;
        return (static_cast<_Int_t>(_Bitmask) & static_cast<_Int_t>(_Bits)) != _Int_t{};
    } else {
        return (_Bitmask & _Bits) != _Ty{};
    }
}

// STRUCT TEMPLATE unequivocal_result
template <class _Ty>
struct unequivocal_result {
    _Ty value;
    bool success;
};
_SDSDLL_END

#endif // _SDSDLL_PREPROCESSOR_GUARD
#endif // _SDSDLL_CORE_TYPE_TRAITS_HPP_