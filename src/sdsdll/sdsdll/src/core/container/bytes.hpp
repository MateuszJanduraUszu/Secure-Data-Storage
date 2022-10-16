// bytes.hpp

// Copyright (c) Mateusz Jandura. All rights reserved.
// SPDX-License-Identifier: Apache-2.0

#pragma once
#ifndef _SDSDLL_CORE_CONTAINER_BYTES_HPP_
#define _SDSDLL_CORE_CONTAINER_BYTES_HPP_
#include <core/defs.hpp>
#if _SDSDLL_PREPROCESSOR_GUARD
#include <core/traits/memory_traits.hpp>
#include <core/traits/type_traits.hpp>
#include <cstddef>

_SDSDLL_BEGIN
// CLASS TEMPLATE _Bytes_container
template <size_t _Count>
class _Bytes_container { // base class for all byte-based containers
public:
    using value_type      = unsigned char;
    using size_type       = size_t;
    using difference_type = ptrdiff_t;
    using pointer         = unsigned char*;
    using const_pointer   = const unsigned char*;

    static constexpr size_type size = _Count;

    constexpr _Bytes_container() noexcept : _Myarray{0} {}

    constexpr _Bytes_container(const _Bytes_container& _Other) noexcept {
        _Copy(_Other._Myarray);
    }

    constexpr _Bytes_container(_Bytes_container&& _Other) noexcept {
        _Copy(_Other._Myarray);
    }

    _CONSTEXPR20 ~_Bytes_container() noexcept {}

    explicit constexpr _Bytes_container(const value_type* const _Ptr) noexcept {
        _Move(_Ptr);
    }

    constexpr _Bytes_container& operator=(const _Bytes_container& _Other) noexcept {
        if (this != _SDSDLL addressof(_Other)) {
            _Copy(_Other._Myarray);
        }

        return *this;
    }

    constexpr _Bytes_container& operator=(_Bytes_container&& _Other) noexcept {
        if (this != _SDSDLL addressof(_Other)) {
            _Copy(_Other._Myarray);
        }

        return *this;
    }

    constexpr _Bytes_container& operator=(const value_type* const _Ptr) noexcept {
        _Move(_Ptr);
        return *this;
    }

    // erases all data
    constexpr void clear() noexcept {
        memory_traits::set(_Myarray, 0, size * sizeof(value_type));
    }

    // checks if any value is currently stored
    _NODISCARD constexpr bool empty() const noexcept {
        return memory_traits::compare(_Myarray, decltype(_Myarray){0}, size * sizeof(value_type)) == 0;
    }

    // returns the current array as a mutable pointer
    _NODISCARD constexpr pointer get() noexcept {
        return _Myarray;
    }

    // returns the current array as a non-mutable pointer
    _NODISCARD constexpr const_pointer get() const noexcept {
        return _Myarray;
    }

private:
    // copies a pointer into an array
    constexpr void _Copy(const value_type* const _Ptr) noexcept {
        memory_traits::copy(_Myarray, _Ptr, size * sizeof(value_type));
    }

    // moves a pointer into an array
    constexpr void _Move(const value_type* const _Ptr) noexcept {
        memory_traits::move(_Myarray, _Ptr, size * sizeof(value_type));
    }

    value_type _Myarray[_Count];
};
_SDSDLL_END

#endif // _SDSDLL_PREPROCESSOR_GUARD
#endif // _SDSDLL_CORE_CONTAINER_BYTES_HPP_