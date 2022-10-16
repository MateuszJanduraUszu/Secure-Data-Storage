// concepts.hpp

// Copyright (c) Mateusz Jandura. All rights reserved.
// SPDX-License-Identifier: Apache-2.0

#pragma once
#ifndef _SDSDLL_CORE_TRAITS_CONCEPTS_HPP_
#define _SDSDLL_CORE_TRAITS_CONCEPTS_HPP_
#include <core/defs.hpp>
#if _SDSDLL_PREPROCESSOR_GUARD

_SDSDLL_BEGIN
// CONSTANT TEMPLATE _Always_false
template <class...>
_INLINE_VARIABLE constexpr bool _Always_false = false;

// STRUCT TEMPLATE integral_constant
template <class _Ty, _Ty _Val>
struct integral_constant { // stores fixed value of _Ty
    using value_type = _Ty;
    using type       = integral_constant;

    static constexpr _Ty value = _Val;

    constexpr operator value_type() const noexcept {
        return value;
    }

    constexpr value_type operator()() const noexcept {
        return value;
    }
};

// ALIAS TEMPLATE bool_constant
template <bool _Val>
using bool_constant = integral_constant<bool, _Val>;

// ALIAS TEMPLATE false_type
using false_type = bool_constant<false>;

// ALIAS TEMPLATE true_type
using true_type = bool_constant<true>;

// STRUCT TEMPLATE enable_if
template <bool _Cond, class _Ty = void>
struct enable_if {
    using type = _Ty;
};

template <class _Ty>
struct enable_if<false, _Ty> {}; // no type member

// ALIAS TEMPLATE enable_if_t
template <bool _Cond, class _Ty = void>
using enable_if_t = typename enable_if<_Cond, _Ty>::type;

#if _HAS_BUILTIN(__is_same)
// CONSTANT TEMPLATE is_same_v
template <class _Ty1, class _Ty2>
_INLINE_VARIABLE constexpr bool is_same_v = __is_same(_Ty1, _Ty2);
#elif _HAS_BUILTIN(__is_same_as)
// CONSTANT TEMPLATE is_same_v
template <class _Ty1, class _Ty2>
_INLINE_VARIABLE constexpr bool is_same_v = __is_same_as(_Ty1, _Ty2);
#else // ^^^ Use built-in ^^^ / vvv Use workaround vvv
// CONSTANT TEMPLATE _Is_same_impl
template <class, class>
_INLINE_VARIABLE constexpr bool _Is_same_impl = false;

template <class _Ty>
_INLINE_VARIABLE constexpr bool _Is_same_impl<_Ty, _Ty> = true;

// CONSTANT TEMPLATE is_same_v
template <class _Ty1, class _Ty2>
_INLINE_VARIABLE constexpr bool is_same_v = _Is_same_impl<_Ty1, _Ty2>;
#endif // _HAS_BUILTIN(__is_same)

// STRUCT TEMPLATE is_same
template <class _Ty1, class _Ty2>
struct is_same : bool_constant<is_same_v<_Ty1, _Ty2>> {};

// CONSTANT TEMPLATE is_array_v
template <class _Ty>
_INLINE_VARIABLE constexpr bool is_array_v = false;

template <class _Ty>
_INLINE_VARIABLE constexpr bool is_array_v<_Ty[]> = true;

template <class _Ty, size_t _Size>
_INLINE_VARIABLE constexpr bool is_array_v<_Ty[_Size]> = true;

// STRUCT TEMPLATE is_array
template <class _Ty>
struct is_array : bool_constant<is_array_v<_Ty>> {};

// STRUCT TEMPLATE is_enum
template <class _Ty>
struct is_enum : bool_constant<__is_enum(_Ty)> {};

// CONSTANT TEMPLATE is_enum_v
template <class _Ty>
_INLINE_VARIABLE constexpr bool is_enum_v = __is_enum(_Ty);

// CONSTANT TEMPLATE is_empty_v
template <class _Ty>
_INLINE_VARIABLE constexpr bool is_empty_v = __is_empty(_Ty);

// STRUCT TEMPLATE is_empty
template <class _Ty>
struct is_empty : bool_constant<__is_empty(_Ty)> {};

// CONSTANT TEMPLATE is_final_v
template <class _Ty>
_INLINE_VARIABLE constexpr bool is_final_v = __is_final(_Ty);

// STRUCT TEMPLATE is_final
template <class _Ty>
struct is_final : bool_constant<__is_final(_Ty)> {};

// STRUCT TEMPLATE conditional
template <bool, class _Ty1, class>
struct conditional { // condition true, choose _Ty1
    using type = _Ty1;
};

template <class _Ty1, class _Ty2>
struct conditional<false, _Ty1, _Ty2> { // condition false, choose _Ty2
    using type = _Ty2;
};

// ALIAS TEMPLATE conditional_t
template <bool _Cond, class _Ty1, class _Ty2>
using conditional_t = typename conditional<_Cond, _Ty1, _Ty2>::type;

// STRUCT TEMPLATE conjunction
template <class... _Traits>
struct conjunction : true_type {};

template <class _Trait>
struct conjunction<_Trait> : _Trait {};

template <class _Trait, class... _Traits>
struct conjunction<_Trait, _Traits...> : conditional_t<_Trait::value, conjunction<_Traits...>, _Trait> {};

// CONSTANT TEMPLATE conjunction_v
template <class... _Traits>
_INLINE_VARIABLE constexpr bool conjunction_v = conjunction<_Traits...>::value;

// STRUCT TEMPLATE negation
template <class _Trait>
struct negation : bool_constant<!_Trait::value> {};

// CONSTANT TEMPLATE negation_v
template <class _Trait>
_INLINE_VARIABLE constexpr bool negation_v = negation<_Trait>::value;

// STRUC TEMPLATE disjunction
template <class... _Traits>
struct disjunction : false_type {};

template <class _Trait>
struct disjunction<_Trait> : _Trait {};

template <class _Trait, class... _Traits>
struct disjunction<_Trait, _Traits...> : conditional_t<_Trait::value, _Trait, disjunction<_Traits...>> {};

// CONSTANT TEMPLATE disjunction_v
template <class... _Traits>
_INLINE_VARIABLE constexpr bool disjunction_v = disjunction<_Traits...>::value;

// CONSTANT TEMPLATE is_any_of_v
template <class _Ty, class... _Types>
_INLINE_VARIABLE constexpr bool is_any_of_v = disjunction_v<is_same<_Ty, _Types>...>;

// STRUCT TEMPLATE is_any_of
template <class _Ty, class... _Types>
struct is_any_of : bool_constant<is_any_of_v<_Ty, _Types...>> {};

// CONSTANT TEMPLATE all_same_v
template <class _Ty, class... _Types>
_INLINE_VARIABLE constexpr bool all_same_v = conjunction_v<is_same<_Ty, _Types>...>;

// STRUCT TEMPLATE all_same
template <class _Ty, class... _Types>
struct all_same : bool_constant<all_same_v<_Ty, _Types...>> {};

// CONSTANT TEMPLATE is_standard_integer_v
template <class _Ty>
_INLINE_VARIABLE constexpr bool is_standard_integer_v = is_any_of_v<_Ty, char, unsigned char,
    short, unsigned short, int, unsigned int, long, unsigned long, long long, unsigned long long>;
_SDSDLL_END

#endif // _SDSDLL_PREPROCESSOR_GUARD
#endif // _SDSDLL_CORE_TRAITS_CONCEPTS_HPP_