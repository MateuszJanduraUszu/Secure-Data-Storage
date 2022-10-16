// singleton.hpp

// Copyright (c) Mateusz Jandura. All rights reserved.
// SPDX-License-Identifier: Apache-2.0

#pragma once
#ifndef _SDSDLL_CORE_PATTERN_SINGLETON_HPP_
#define _SDSDLL_CORE_PATTERN_SINGLETON_HPP_
#include <core/defs.hpp>
#if _SDSDLL_PREPROCESSOR_GUARD
#include <core/traits/type_traits.hpp>
#include <type_traits>

_SDSDLL_BEGIN
// CLASS TEMPLATE singleton
template <class _Ty>
class singleton { // class for one-time initialization
public:
    _CONSTEXPR20 ~singleton() noexcept {}

    singleton() = delete;
    singleton(const singleton&) = delete;
    singleton& operator=(const singleton&) = delete;

    _NODISCARD static _Ty& get() noexcept(is_nothrow_default_constructible_v<_Ty>) {
        static _Ty _Obj{}; // one-time initialization
        return _Obj;
    }

    template <class... _Types>
    _NODISCARD static _Ty& get(_Types&&... _Args) noexcept(is_nothrow_constructible_v<_Ty, _Types...>) {
        static _Ty _Obj(_STD forward<_Types>(_Args)...); // one-time initialization
        return _Obj;
    }
};

// FUNCTION TEMPLATE instanceof
template <class _Ty>
_NODISCARD _Ty& instanceof() noexcept(is_nothrow_default_constructible_v<_Ty>) {
    return singleton<_Ty>::get();
}

template <class _Ty, class... _Types>
_NODISCARD _Ty& instanceof(_Types&&... _Args) noexcept(is_nothrow_constructible_v<_Ty, _Types...>) {
    return singleton<_Ty>::get(_STD forward<_Types>(_Args)...);
}
_SDSDLL_END

#endif // _SDSDLL_PREPROCESSOR_GUARD
#endif // _SDSDLL_CORE_PATTERN_SINGLETON_HPP_