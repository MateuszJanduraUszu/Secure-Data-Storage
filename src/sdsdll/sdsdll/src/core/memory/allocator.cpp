// allocator.cpp

// Copyright (c) Mateusz Jandura. All rights reserved.
// SPDX-License-Identifier: Apache-2.0

#include <build/sdsdll_pch.hpp>
#include <core/memory/allocator.hpp>
#if _SDSDLL_PREPROCESSOR_GUARD

_SDSDLL_BEGIN
// FUNCTION _Is_pow_of_2
_NODISCARD constexpr bool _Is_pow_of_2(const size_t _Val) noexcept {
    return _Val > 0 && (_Val & (_Val - 1)) == 0;
}

// FUNCTION _Allocate
_NODISCARD constexpr _CRT_ALLOCATOR void* _Allocate(const size_t _Count) noexcept {
    // try allocate _Count bytes with default alignment (thread-safe)
    if (_Count == 0 || _Count >= static_cast<size_t>(-1)) { // no allocation or risk of overflow
        return nullptr;
    }

#ifdef __cpp_aligned_new
    return ::operator new(_Count, align_val_t{__STDCPP_DEFAULT_NEW_ALIGNMENT__}, _STD nothrow);
#else // ^^^ __cpp_aligned_new ^^^ / vvv !__cpp_aligned_new vvv
    return ::operator new(_Count, _STD nothrow);
#endif // __cpp_aligned_new
}

#ifdef __cpp_aligned_new
// FUNCTION _Allocate_aligned
_NODISCARD constexpr _CRT_ALLOCATOR void* _Allocate_aligned(const size_t _Count,
    const size_t _Align) noexcept { // try allocate _Count bytes with custom alignment (thread-safe)
    if (_Count == 0 || _Count >= static_cast<size_t>(-1)) { // no allocation or risk of overflow
        return nullptr;
    }

    if (!_Is_pow_of_2(_Align)) { // alignment must be 2^n
        return nullptr;
    }

    return ::operator new(_Count, align_val_t{_Align}, _STD nothrow);
}
#endif // __cpp_aligned_new

// FUNCTION _Deallocate
constexpr void _Deallocate(void* const _Ptr, const size_t _Size) {
    // try deallocate _Count bytes of _Ptr with default alignment (non-thread-safe)
    if (!_Ptr || _Size == 0) {
        return;
    }

#ifdef __cpp_aligned_new
    ::operator delete(_Ptr, _Size, align_val_t{__STDCPP_DEFAULT_NEW_ALIGNMENT__});
#else // ^^^ __cpp_aligned_new ^^^ / vvv !__cpp_aligned_new vvv
    ::operator delete(_Ptr, _Size);
#endif // __cpp_aligned_new
}

#ifdef __cpp_aligned_new
// FUNCTION _Deallocate_aligned
constexpr void _Deallocate_aligned(void* const _Ptr, const size_t _Size, const size_t _Align) {
    // try deallocate _Count bytes of _Ptr with custom alignment (non-thread-safe)
    if (!_Ptr || _Size == 0) {
        return;
    }

    if (!_Is_pow_of_2(_Align)) { // alignment must be 2^n
        return;
    }

    ::operator delete(_Ptr, _Size, align_val_t{_Align});
}
#endif // __cpp_aligned_new
_SDSDLL_END

#endif // _SDSDLL_PREPROCESSOR_GUARD