// sbo.hpp

// Copyright (c) Mateusz Jandura. All rights reserved.
// SPDX-License-Identifier: Apache-2.0

#pragma once
#ifndef _SDSDLL_CORE_OPTIMIZATION_SBO_HPP_
#define _SDSDLL_CORE_OPTIMIZATION_SBO_HPP_
#include <core/defs.hpp>
#if _SDSDLL_PREPROCESSOR_GUARD
#include <core/memory/allocator.hpp>
#include <core/optimization/ebco.hpp>
#include <core/traits/memory_traits.hpp>
#include <cstddef>
#include <utility>

_SDSDLL_BEGIN
// CONSTANT TEMPLATE _Small_type_size
_INLINE_VARIABLE constexpr size_t _Small_type_size = 16;

// CONSTANT TEMPLATE _Is_small_type
template <class _Ty>
_INLINE_VARIABLE constexpr bool _Is_small_type = sizeof(_Ty) <= _Small_type_size;

// CLASS TEMPLATE _Sbo_buffer_val
template <class _Ty>
class _Sbo_buffer_val { // stores buffers for SBO
public:
    using value_type      = _Ty;
    using size_type       = size_t;
    using difference_type = ptrdiff_t;
    using pointer         = _Ty*;
    using const_pointer   = const _Ty*;
    using reference       = _Ty&;
    using const_reference = const _Ty&;

    // size of the internal buffer (at least 1 and at most 16)
    static constexpr size_type _Buffer_size = 16 / sizeof(value_type) < 1 ? 1 : 16 / sizeof(value_type);
    
    constexpr _Sbo_buffer_val() noexcept : _Mystorage(), _Mysize(0) {}
    
    _CONSTEXPR20 ~_Sbo_buffer_val() noexcept {}

    _Sbo_buffer_val(const _Sbo_buffer_val&) = delete;
    _Sbo_buffer_val& operator=(const _Sbo_buffer_val&) = delete;

    _NODISCARD constexpr pointer _Get_ptr() noexcept {
        // choose between the pointer and the internal buffer
        return _Mysize > _Buffer_size ? _Mystorage._Ptr : _Mystorage._Buf;
    }

    _NODISCARD constexpr const_pointer _Get_ptr() const noexcept {
        // choose between the pointer and the internal buffer
        return _Mysize > _Buffer_size ? _Mystorage._Ptr : _Mystorage._Buf;
    }

    _NODISCARD constexpr bool _Uses_sbo() const noexcept {
        return _Mysize <= _Buffer_size;
    }

    constexpr void _Activate_sbo() noexcept {
        // fill the internal buffer with the default values
        for (size_type _Idx = 0; _Idx < _Buffer_size; ++_Idx) {
            _Mystorage._Buf[_Idx] = value_type{};
        }
    }

    union _Storage {
        constexpr _Storage() noexcept : _Buf() {}

        _CONSTEXPR20 ~_Storage() noexcept {}

        value_type _Buf[_Buffer_size]; // pointer to the stack buffer
        pointer _Ptr; // pointer to the heap buffer
    };

    _Storage _Mystorage;
    size_type _Mysize; // current buffer size
};

// CLASS TEMPLATE _Sbo_buffer
template <class _Ty, class _Alloc = allocator<_Ty>, bool = _Is_small_type<_Ty>>
class _Sbo_buffer { // avoidable allocation (SBO active)
private:
    using _Buffer_val = _Sbo_buffer_val<_Ty>;

public:
    using value_type      = typename _Buffer_val::value_type;
    using size_type       = typename _Buffer_val::size_type;
    using difference_type = typename _Buffer_val::difference_type;
    using pointer         = typename _Buffer_val::pointer;
    using const_pointer   = typename _Buffer_val::const_pointer;
    using reference       = typename _Buffer_val::reference;
    using const_reference = typename _Buffer_val::const_reference;

    constexpr _Sbo_buffer() noexcept : _Mypair(_Ebco_default_init{}) {
        _Mypair._Val1._Activate_sbo();
    }

    constexpr _Sbo_buffer(const size_type _Size) noexcept : _Mypair(_Ebco_default_init{}) {
        if (_Size <= _Buffer_val::_Buffer_size) { // SBO possible
            _Mypair._Val1._Activate_sbo();
        }

        (void) _Resize(_Size);
    }

    _CONSTEXPR20 ~_Sbo_buffer() noexcept {
        _Tidy();
    }

    _Sbo_buffer(const _Sbo_buffer&) = delete;
    _Sbo_buffer& operator=(const _Sbo_buffer&) = delete;

    _NODISCARD constexpr size_type _Max_size() const noexcept {
        return _Mypair._Get_val2().max_size();
    }

    _NODISCARD constexpr const size_type _Size() const noexcept {
        return _Mypair._Val1._Mysize;
    }

    _NODISCARD constexpr bool _Empty() const noexcept {
        return _Mypair._Val1._Mysize == 0;
    }

    _NODISCARD constexpr pointer _Get() noexcept {
        return _Mypair._Val1._Get_ptr();
    }

    _NODISCARD constexpr const_pointer _Get() const noexcept {
        return _Mypair._Val1._Get_ptr();
    }

    _NODISCARD constexpr bool _Resize(const size_type _New_size) noexcept {
        _Buffer_val& _Data = _Mypair._Val1;
        if (_New_size == _Data._Mysize) { // nothing has changed
            return true;
        } else if (_New_size == 0) { // release the buffer
            _Release();
            return true;
        } else if (_Data._Mysize == 0) { // allocate or prepare a new buffer
            if (_New_size <= _Buffer_val::_Buffer_size) { // prepare the internal buffer
                _Data._Mysize = _New_size;
                memory_traits::set(_Data._Mystorage._Buf,
                    0, _Buffer_val::_Buffer_size * sizeof(_Ty)); // fill the internal buffer with zeros
            } else { // allocate the buffer
                _Data._Mystorage._Ptr = _Mypair._Get_val2().allocate(_New_size);
                if (!_Data._Mystorage._Ptr) { // allocation failed
                    return false;
                }

                _Data._Mysize = _New_size;
                memory_traits::set(_Data._Mystorage._Ptr,
                    0, _Data._Mysize * sizeof(_Ty)); // fill the allocated buffer with zeros
            }

            return true;
        } else { // increase or decrease an old buffer (may change the buffer location)
            if (_New_size > _Max_size()) { // overflow possible
                return false;
            }

            if (_Data._Uses_sbo()) { // continue use SBO if possible
                return _Resize_using_sbo(_New_size);
            } else { // use SBO if possible
                return _Resize_not_using_sbo(_New_size);
            }
        }
    }

    constexpr void _Release() noexcept {
        _Buffer_val& _Data = _Mypair._Val1;
        if (_Data._Mysize == 0) {
            return;
        } else if (_Data._Mysize <= _Buffer_val::_Buffer_size) { // fill the internal buffer with zeros
            _Data._Mysize = 0;
            memory_traits::set(_Data._Mystorage._Buf, 0, _Buffer_val::_Buffer_size * sizeof(_Ty));
        } else { // deallocate the allocated buffer
            _Mypair._Get_val2().deallocate(_Data._Mystorage._Ptr, _Data._Mysize);
            _Data._Mystorage._Ptr = nullptr;
            _Data._Mysize         = 0;
        }
    }

private:
    _NODISCARD constexpr bool _Resize_using_sbo(const size_type _New_size) noexcept {
        _Buffer_val& _Data = _Mypair._Val1;
        if (_New_size <= _Buffer_val::_Buffer_size) { // SBO is still available
            if (_New_size < _Data._Mysize) { // decrease the buffer, fill the rest of the buffer with zeros
                memory_traits::set(_Data._Mystorage._Buf + _New_size,
                    0, (_Buffer_val::_Buffer_size - _New_size) * sizeof(_Ty));
            }

            _Data._Mysize = _New_size;
        } else { // SBO is no longer available
            _Alloc& _Al      = _Mypair._Get_val2();
            pointer _New_ptr = _Al.allocate(_New_size);
            if (!_New_ptr) { // allocation failed
                return false;
            }

            _Data._Mysize = _New_size;
            memory_traits::move(_New_ptr, _Data._Mystorage._Buf, _New_size * sizeof(_Ty));
            memory_traits::set(_Data._Mystorage._Buf, 0, _Buffer_val::_Buffer_size * sizeof(_Ty));
            _Data._Mystorage._Ptr = _New_ptr; // assign a new buffer
        }

        return true;
    }

    _NODISCARD constexpr bool _Resize_not_using_sbo(const size_type _New_size) noexcept {
        _Alloc& _Al        = _Mypair._Get_val2();
        _Buffer_val& _Data = _Mypair._Val1;
        if (_New_size > _Buffer_val::_Buffer_size) { // SBO is still not available
            pointer _New_ptr = _Al.allocate(_New_size);
            if (!_New_ptr) { // allocation failed
                return false;
            }

            memory_traits::move(
                _New_ptr, _Data._Mystorage._Ptr, (_STD min)(_Data._Mysize, _New_size) * sizeof(_Ty));
            if (_New_size > _Data._Mysize) { // fill the rest of the buffer with zeros
                memory_traits::set(_New_ptr + _Data._Mysize, 0, (_New_size - _Data._Mysize) * sizeof(_Ty));
            }

            _Al.deallocate(_Data._Mystorage._Ptr, _Data._Mysize); // deallocate an old buffer
            _Data._Mystorage._Ptr = _New_ptr; // assign a new buffer
            _Data._Mysize         = _New_size;
        } else { // SBO is now available
            value_type _Temp_buf[_Buffer_val::_Buffer_size]; // intentionally uninitialized
            const size_type _Min_size = (_STD min)(_Data._Mysize, _New_size);
            memory_traits::move(_Temp_buf, _Data._Mystorage._Ptr, _Min_size * sizeof(_Ty));
            _Al.deallocate(_Data._Mystorage._Ptr, _Data._Mysize); // deallocate an old buffer
            _Data._Mystorage._Ptr = nullptr;
            memory_traits::move(_Data._Mystorage._Buf, _Temp_buf, _Min_size * sizeof(_Ty));
            _Data._Mysize = _New_size;
        }

        return true;
    }

    constexpr void _Tidy() noexcept {
        _Buffer_val& _Data = _Mypair._Val1;
        if (_Data._Mysize > _Buffer_val::_Buffer_size) { // buffer not empty, deallocate
            // Note: The _Data._Mystorage._Ptr is always allocated if the buffer size
            //       is greater than _Buffer_val::_Buffer_size.
            _Mypair._Get_val2().deallocate(_Data._Mystorage._Ptr, _Data._Mysize);
        }
    }

    _Ebco_pair<_Buffer_val, _Alloc> _Mypair;
};

template <class _Ty, class _Alloc>
class _Sbo_buffer<_Ty, _Alloc, false> { // unavoidable allocation (SBO inactive)
private:
    using _Buffer_val = _Sbo_buffer_val<_Ty>;

public:
    using value_type      = typename _Buffer_val::value_type;
    using size_type       = typename _Buffer_val::size_type;
    using difference_type = typename _Buffer_val::difference_type;
    using pointer         = typename _Buffer_val::pointer;
    using const_pointer   = typename _Buffer_val::const_pointer;
    using reference       = typename _Buffer_val::reference;
    using const_reference = typename _Buffer_val::const_reference;

    constexpr _Sbo_buffer() noexcept : _Mypair(_Ebco_default_init{}) {}

    constexpr _Sbo_buffer(const size_type _Size) noexcept : _Mypair(_Ebco_default_init{}) {
        (void) _Resize(_Size);
    }

    _CONSTEXPR20 ~_Sbo_buffer() noexcept {
        _Release();
    }

    _Sbo_buffer(const _Sbo_buffer&) = delete;
    _Sbo_buffer& operator=(const _Sbo_buffer&) = delete;

    _NODISCARD constexpr size_type _Max_size() const noexcept {
        return _Mypair._Get_val2().max_size();
    }

    _NODISCARD constexpr const size_type _Size() const noexcept {
        return _Mypair._Val1._Mysize;
    }

    _NODISCARD constexpr bool _Empty() const noexcept {
        return _Mypair._Val1._Mysize == 0;
    }

    _NODISCARD constexpr pointer _Get() noexcept {
        return _Mypair._Val1._Mystorage._Ptr;
    }

    _NODISCARD constexpr const_pointer _Get() const noexcept {
        return _Mypair._Val1._Mystorage._Ptr;
    }

    _NODISCARD constexpr bool _Resize(const size_type _New_size) noexcept {
        _Buffer_val& _Data = _Mypair._Val1;
        if (_New_size == _Data._Mysize) { // nothing has changed
            return true;
        } else if (_New_size == 0) { // release the buffer
            _Release();
            return true;
        } else if (_Data._Mysize == 0) { // allocate a new buffer
            _Data._Mystorage._Ptr = _Mypair._Get_val2().allocate(_New_size);
            if (!_Data._Mystorage._Ptr) { // allocation failed
                return false;
            }

            _Data._Mysize = _New_size;
            memory_traits::set(
                _Data._Mystorage._Ptr, 0, _Data._Mysize * sizeof(_Ty)); // fill the buffer with zeros
            return true;
        } else { // increase or decrease an old buffer
            if (_New_size > _Max_size()) { // overflow possible
                return false;
            }

            _Alloc& _Al      = _Mypair._Get_val2();
            pointer _New_ptr = _Al.allocate(_New_size);
            if (!_New_ptr) { // allocation failed
                return false;
            }

            const size_type _Min_size = (_STD min)(_Data._Mysize, _New_size);
            memory_traits::move(_New_ptr, _Data._Mystorage._Ptr, _Min_size * sizeof(_Ty));
            if (_New_size > _Data._Mysize) { // fill the rest of the buffer with zeros
                memory_traits::set(_New_ptr + _Min_size, 0, (_New_size - _Data._Mysize) * sizeof(_Ty));
            }

            _Al.deallocate(_Data._Mystorage._Ptr, _Data._Mysize); // deallocate an old buffer
            _Data._Mystorage._Ptr = _New_ptr; // assign a new buffer
            _Data._Mysize         = _New_size;
            return true;
        }
    }

    constexpr void _Release() noexcept {
        _Buffer_val& _Data = _Mypair._Val1;
        if (_Data._Mysize > 0) { // buffer not empty, deallocate
            _Mypair._Get_val2().deallocate(_Data._Mystorage._Ptr, _Data._Mysize);
            _Data._Mystorage._Ptr = nullptr;
            _Data._Mysize         = 0;
        }
    }

private:
    _Ebco_pair<_Buffer_val, _Alloc> _Mypair;
};

// FUNCTION TEMPLATE _Release_on_failure
template <class _Ty, class _Alloc>
constexpr void _Release_on_failure(_Sbo_buffer<_Ty, _Alloc>& _Buf, const bool _Success) noexcept {
    if (!_Success) { // failure, release the buffer
        _Buf._Release();
    }
}
_SDSDLL_END

#endif // _SDSDLL_PREPROCESSOR_GUARD
#endif // _SDSDLL_CORE_OPTIMIZATION_SBO_HPP_