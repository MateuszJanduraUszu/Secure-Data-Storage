// allocator.hpp

// Copyright (c) Mateusz Jandura. All rights reserved.
// SPDX-License-Identifier: Apache-2.0

#pragma once
#ifndef _SDSDLL_CORE_MEMORY_ALLOCATOR_HPP_
#define _SDSDLL_CORE_MEMORY_ALLOCATOR_HPP_
#include <core/defs.hpp>
#if _SDSDLL_PREPROCESSOR_GUARD
#include <core/api.hpp>
#include <core/traits/type_traits.hpp>
#include <cstddef>
#include <cstdlib>
#include <memory>
#include <new>
#include <type_traits>
#include <utility>

#ifdef __cpp_aligned_new
// STD types
using _STD align_val_t;
#endif // __cpp_aligned_new

_SDSDLL_BEGIN
// CONSTANT TEMPLATE _Default_new_alignof
template <class _Ty>
inline constexpr size_t _Default_new_alignof = (_STD max)(
    __STDCPP_DEFAULT_NEW_ALIGNMENT__, alignof(_Ty)); // choose default allocation alignment

// FUNCTION _Is_pow_of_2
extern _NODISCARD constexpr bool _Is_pow_of_2(const size_t _Val) noexcept;

// FUNCTION _Allocate
extern _NODISCARD constexpr _MSVC_ALLOCATOR void* _Allocate(const size_t _Count) noexcept;

#ifdef __cpp_aligned_new
// FUNCTION _Allocate_aligned
extern _NODISCARD constexpr _MSVC_ALLOCATOR void* _Allocate_aligned(
    const size_t _Count, const size_t _Align) noexcept;
#endif // __cpp_aligned_new

// FUNCTION _Deallocate
extern constexpr void _Deallocate(void* const _Ptr, const size_t _Size);

#ifdef __cpp_aligned_new
// FUNCTION _Deallocate_aligned
extern constexpr void _Deallocate_aligned(void* const _Ptr, const size_t _Size, const size_t _Align);
#endif // __cpp_aligned_new

// CLASS TEMPLATE allocator
template <class _Ty>
class allocator { // class for thread-safe allocation/deallocation
public:
    using value_type      = _Ty;
    using size_type       = size_t;
    using difference_type = ptrdiff_t;
    using pointer         = _Ty*;
    using const_pointer   = const _Ty*;
    using reference       = _Ty&;
    using const_reference = const _Ty&;

    template <class _Other>
    struct rebind {
        using other = allocator<_Other>;
    };

    constexpr allocator() noexcept                 = default;
    constexpr allocator(const allocator&) noexcept = default;
    _CONSTEXPR20 ~allocator() noexcept             = default;

    template <class _Other>
    constexpr allocator(const allocator<_Other>&) noexcept {}

    template <class _Other>
    constexpr allocator& operator=(const allocator<_Other>&) noexcept {
        return *this;
    }

    _NODISCARD constexpr _MSVC_ALLOCATOR _Ty* allocate(const size_type _Count) noexcept {
#ifdef __cpp_aligned_new
        return static_cast<_Ty*>(_Allocate_aligned(_Count * sizeof(_Ty), _Default_new_alignof<_Ty>));
#else // ^^^ __cpp_aligned_new ^^^ / vvv !__cpp_aligned_new vvv
        return static_cast<_Ty*>(_Allocate(_Count * sizeof(_Ty)));
#endif // __cpp_aligned_new
    }

    template <class _Other, class... _Types>
    _CONSTEXPR20 void construct(_Other* const _Ptr, _Types&&... _Args) noexcept {
        try {
#if _HAS_CXX20
            _STD construct_at(_Ptr, _STD forward<_Types>(_Args)...);
#else // ^^^ _HAS_CXX20 ^^^ / vvv !_HAS_CXX20 vvv
            ::new (_Ptr) _Ty(_STD forward<_Types>(_Args)...);
#endif // _HAS_CXX20
        } catch (...) {
            // handler not used
        }
    }
    
    constexpr void deallocate(_Ty* const _Ptr, const size_type _Count) noexcept {
        try {
#ifdef __cpp_aligned_new
            _Deallocate_aligned(_Ptr, _Count * sizeof(_Ty), _Default_new_alignof<_Ty>);
#else // ^^^ __cpp_aligned_new ^^^ / vvv !__cpp_aligned_new vvv
            _Deallocate(_Ptr, _Count * sizeof(_Ty));
#endif // __cpp_aligned_new
        } catch (...) {
            // handler not used
        }
    }

    template <class _Other>
    constexpr void destroy(_Other* const _Ptr) noexcept {
        _STD destroy_at(_Ptr);
    }

    _NODISCARD constexpr size_type max_size() const noexcept {
        return static_cast<size_type>(-1) / sizeof(_Ty);
    }
};

template <>
class allocator<void> { // class for thread-safe allocation/deallocation (spacial case)
public:
    using value_type      = void;
    using size_type       = size_t;
    using difference_type = ptrdiff_t;
    using pointer         = void*;
    using const_pointer   = const void*;

    template <class _Other>
    struct rebind {
        using other = allocator<_Other>;
    };

    constexpr allocator() noexcept                 = default;
    constexpr allocator(const allocator&) noexcept = default;
    _CONSTEXPR20 ~allocator() noexcept             = default;

    template <class _Other>
    constexpr allocator(const allocator<_Other>&) noexcept {}

    template <class _Other>
    constexpr allocator& operator=(const allocator<_Other>&) noexcept {
        return *this;
    }

    _NODISCARD constexpr _MSVC_ALLOCATOR void* allocate(const size_type _Count) noexcept {
#ifdef __cpp_aligned_new
        return _Allocate_aligned(_Count, __STDCPP_DEFAULT_NEW_ALIGNMENT__);
#else // ^^^ __cpp_aligned_new ^^^ / vvv !__cpp_aligned_new vvv
        return _Allocate(_Count);
#endif // __cpp_aligned_new
    }

    constexpr void deallocate(void* const _Ptr, const size_type _Count) noexcept {
        try {
#ifdef __cpp_aligned_new
            _Deallocate_aligned(_Ptr, _Count, __STDCPP_DEFAULT_NEW_ALIGNMENT__);
#else // ^^^ __cpp_aligned_new ^^^ / vvv !__cpp_aligned_new vvv
            _Deallocate(_Ptr, _Count);
#endif // __cpp_aligned_new
        } catch (...) {
            // handler not used
        }
    }

    _NODISCARD constexpr size_type max_size() const noexcept {
        return static_cast<size_type>(-1);
    }
};
_SDSDLL_END

#endif // _SDSDLL_PREPROCESSOR_GUARD
#endif // _SDSDLL_CORE_MEMORY_ALLOCATOR_HPP_