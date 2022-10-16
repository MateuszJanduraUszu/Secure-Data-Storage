// ebco.hpp

// Copyright (c) Mateusz Jandura. All rights reserved.
// SPDX-License-Identifier: Apache-2.0

#pragma once
#ifndef _SDSDLL_CORE_OPTIMIZATION_EBCO_HPP_
#define _SDSDLL_CORE_OPTIMIZATION_EBCO_HPP_
#include <core/defs.hpp>
#if _SDSDLL_PREPROCESSOR_GUARD
#include <core/traits/concepts.hpp>
#include <core/traits/type_traits.hpp>
#include <type_traits>

_SDSDLL_BEGIN
// STRUCT _Ebco_default_init 
struct _Ebco_default_init { // tag for _Ebco_pair constructor (default initialization)
    _Ebco_default_init() = default;
};

// STRUCT _Ebco_custom_init
struct _Ebco_custom_init { // tag for _Ebco_pair constructor (custom initialization)
    _Ebco_custom_init() = default;
};

// CLASS TEMPLATE _Ebco_pair
template <class _Ty1, class _Ty2, bool = is_empty_v<_Ty2> && !is_final_v<_Ty2>>
class _Ebco_pair final : public _Ty2 { // store _Ty1 and derive from _Ty2 (EBCO active)
public:
    _Ty1 _Val1;

    template <class... _Types,
        enable_if_t<!is_any_of_v<_Ebco_pair, remove_reference_t<_Types>...>, int> = 0>
    constexpr _Ebco_pair(_Ebco_default_init, _Types&&... _Args) noexcept(
        conjunction_v<is_nothrow_constructible<_Ty1, _Types...>,
            is_nothrow_default_constructible<_Ty2>>) : _Val1(_STD forward<_Types>(_Args)...), _Ty2() {}

    template <class _Other, class... _Types,
        enable_if_t<!is_same_v<remove_reference_t<_Other>, _Ebco_pair>
            && !is_any_of_v<_Ebco_pair, remove_reference_t<_Types>...>, int> = 0>
    constexpr _Ebco_pair(_Ebco_custom_init, _Other&& _Val, _Types&&... _Args) noexcept(
        conjunction_v<is_nothrow_constructible<_Ty1, _Types...>, is_nothrow_move_constructible<_Ty2>>)
        : _Val1(_STD forward<_Types>(_Args)...), _Ty2(_STD forward<_Other>(_Val)) {}

    _NODISCARD constexpr _Ty2& _Get_val2() noexcept {
        return *this;
    }

    _NODISCARD constexpr const _Ty2& _Get_val2() const noexcept {
        return *this;
    }
};

template <class _Ty1, class _Ty2>
class _Ebco_pair<_Ty1, _Ty2, false> final { // store _Ty1 and _Ty2 (EBCO inactive)
public:
    _Ty1 _Val1;
    _Ty2 _Val2;

    template <class... _Types,
        enable_if_t<!is_any_of_v<_Ebco_pair, remove_reference_t<_Types>...>, int> = 0>
    constexpr _Ebco_pair(_Ebco_default_init, _Types&&... _Args) noexcept(
        conjunction_v<is_nothrow_constructible<_Ty1, _Types...>,
            is_nothrow_default_constructible<_Ty2>>) : _Val1(_STD forward<_Types>(_Args)...), _Val2() {}

    template <class _Other, class... _Types,
        enable_if_t<!is_same_v<remove_reference_t<_Other>, _Ebco_pair>
            && !is_any_of_v<_Ebco_pair, remove_reference_t<_Types>...>, int> = 0>
    constexpr _Ebco_pair(_Ebco_custom_init, _Other&& _Val, _Types&&... _Args) noexcept(
        conjunction_v<is_nothrow_constructible<_Ty1, _Types...>, is_nothrow_move_constructible<_Ty2>>)
        : _Val1(_STD forward<_Types>(_Args)...), _Val2(_STD forward<_Other>(_Val)) {}

    _NODISCARD constexpr _Ty2& _Get_val2() noexcept {
        return _Val2;
    }

    _NODISCARD constexpr const _Ty2& _Get_val2() const noexcept {
        return _Val2;
    }
};
_SDSDLL_END

#endif // _SDSDLL_PREPROCESSOR_GUARD
#endif // _SDSDLL_CORE_OPTIMIZATION_EBCO_HPP_