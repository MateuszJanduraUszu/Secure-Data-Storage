// handle_wrapper.hpp

// Copyright (c) Mateusz Jandura. All rights reserved.
// SPDX-License-Identifier: Apache-2.0

#pragma once
#ifndef _SDSDLL_SYSTEM_HANDLE_HANDLE_WRAPPER_HPP_
#define _SDSDLL_SYSTEM_HANDLE_HANDLE_WRAPPER_HPP_
#include <core/defs.hpp>
#if _SDSDLL_PREPROCESSOR_GUARD
#include <core/api.hpp>
#include <core/traits/type_traits.hpp>
#include <type_traits>

_SDSDLL_BEGIN
// CLASS TEMPLATE handle_wrapper
template <class _Traits>
class handle_wrapper { // non-copyable handle wrapper
public:
    using value_type      = typename _Traits::value_type;
    using pointer         = typename _Traits::pointer;
    using const_pointer   = typename _Traits::const_pointer;
    using reference       = typename _Traits::reference;
    using const_reference = typename _Traits::const_reference;
    using traits_type     = _Traits;

    constexpr handle_wrapper() noexcept : _Myval(_Traits::invalid_value) {}

    constexpr handle_wrapper(handle_wrapper& _Other) noexcept {
        *this = _Other;
    }

    constexpr handle_wrapper(handle_wrapper&& _Other) noexcept {
        *this = _STD move(_Other);
    }

    constexpr handle_wrapper(const value_type& _Val) noexcept : _Myval(_Val) {}

    _CONSTEXPR20 ~handle_wrapper() noexcept {
        close();
    }

    constexpr handle_wrapper& operator=(handle_wrapper& _Other) noexcept {
        if (this != _SDSDLL addressof(_Other)) {
            _Myval        = _Other._Myval;
            _Other._Myval = _Traits::invalid_value;
        }

        return *this;
    }

    constexpr handle_wrapper& operator=(handle_wrapper&& _Other) noexcept {
        if (this != _SDSDLL addressof(_Other)) {
            _Myval        = _STD move(_Other._Myval);
            _Other._Myval = _Traits::invalid_value;
        }

        return *this;
    }

    constexpr handle_wrapper& operator=(const value_type& _Val) noexcept {
        _Myval = _Val;
        return *this;
    }

    handle_wrapper(const handle_wrapper&) = delete;
    handle_wrapper& operator=(const handle_wrapper&) = delete;

    constexpr void close() noexcept {
        if (_Myval != _Traits::invalid_value) {
            _Traits::close(_Myval);
            _Myval = _Traits::invalid_value;
        }
    }

    _NODISCARD constexpr explicit operator bool() const noexcept {
        return _Myval != _Traits::invalid_value;
    }

    _NODISCARD constexpr operator value_type() noexcept {
        return _Myval;
    }

    _NODISCARD constexpr operator const value_type() const noexcept {
        return _Myval;
    }

    _NODISCARD constexpr reference get() noexcept {
        return _Myval;
    }

    _NODISCARD constexpr const_reference get() const noexcept {
        return _Myval;
    }

private:
    value_type _Myval;
};
_SDSDLL_END

#endif // _SDSDLL_PREPROCESSOR_GUARD
#endif // _SDSDLL_SYSTEM_HANDLE_HANDLE_WRAPPER_HPP_