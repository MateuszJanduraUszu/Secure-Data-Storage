// shared_queue.hpp

// Copyright (c) Mateusz Jandura. All rights reserved.
// SPDX-License-Identifier: Apache-2.0

#pragma once
#ifndef _SDSDLL_SYSTEM_EXECUTION_SHARED_QUEUE_HPP_
#define _SDSDLL_SYSTEM_EXECUTION_SHARED_QUEUE_HPP_
#include <core/defs.hpp>
#if _SDSDLL_PREPROCESSOR_GUARD
#include <algorithm>
#include <core/traits/type_traits.hpp>
#include <list>
#include <system/execution/shared_lock.hpp>
#include <type_traits>

// STD types
using _STD list;

_SDSDLL_BEGIN
// CLASS TEMPLATE shared_queue
template <class _Ty, class _Cont = list<_Ty>>
class shared_queue {
public:
    using value_type      = _Ty;
    using size_type       = typename _Cont::size_type;
    using difference_type = typename _Cont::difference_type;
    using container_type  = _Cont;

    shared_queue() noexcept(is_nothrow_default_constructible_v<_Cont>) : _Mycont(), _Mylock() {}

    template <class _InIt>
    explicit shared_queue(_InIt _First, _InIt _Last) : _Mycont(_First, _Last), _Mylock() {}

    ~shared_queue() noexcept(is_nothrow_destructible_v<_Cont>) {}

    shared_queue(const shared_queue&) = delete;
    shared_queue& operator=(const shared_queue&) = delete;

    void clear() noexcept {
        exclusive_lock_guard _Guard(_Mylock);
        _Mycont.clear();
    }

    _NODISCARD bool empty() const noexcept {
        shared_lock_guard _Guard(_Mylock);
        return _Mycont.empty();
    }

    _NODISCARD bool full() const noexcept {
        shared_lock_guard _Guard(_Mylock);
        return _Mycont.size() == _Mycont.max_size();
    }

    _NODISCARD size_type size() const noexcept {
        shared_lock_guard _Guard(_Mylock);
        return _Mycont.size();
    }

    _NODISCARD size_type max_size() const noexcept {
        shared_lock_guard _Guard(_Mylock);
        return _Mycont.max_size();
    }

    _NODISCARD const _Cont& get_container() const noexcept {
        shared_lock_guard _Guard(_Mylock);
        return _Mycont;
    }

    shared_queue& assign(const _Cont& _Other) {
        exclusive_lock_guard _Guard(_Mylock);
        _Mycont.assign(_Other.begin(), _Other.end());
        return *this;
    }

    template <class _InIt>
    shared_queue& assign(_InIt _First, _InIt _Last) {
        exclusive_lock_guard _Guard(_Mylock);
        _Mycont.assign(_First, _Last);
        return *this;
    }

    _NODISCARD value_type top() const noexcept {
        shared_lock_guard _Guard(_Mylock);
        return !_Mycont.empty() ? _Mycont.front() : value_type{};
    }

    void push(const _Ty& _Val) noexcept(noexcept(_Mycont.push_back(_Val))) {
        exclusive_lock_guard _Guard(_Mylock);
        if (_Mycont.size() < _Mycont.max_size()) {
            _Mycont.push_back(_Val);
        }
    }

    void push(_Ty&& _Val) noexcept(noexcept(_Mycont.push_back(_STD move(_Val)))) {
        exclusive_lock_guard _Guard(_Mylock);
        if (_Mycont.size() < _Mycont.max_size()) {
            _Mycont.push_back(_STD move(_Val));
        }
    }

    value_type pop() noexcept(noexcept(_Mycont.pop_front())) {
        exclusive_lock_guard _Guard(_Mylock);
        if (_Mycont.empty()) {
            return value_type{};
        }

        const _Ty _Val = static_cast<_Ty&&>(_Mycont.front());
        _Mycont.pop_front();
        return _Val;
    }

private:
    _Cont _Mycont;
    mutable shared_lock _Mylock;
};
_SDSDLL_END

#endif // _SDSDLL_PREPROCESSOR_GUARD
#endif // _SDSDLL_SYSTEM_EXECUTION_SHARED_QUEUE_HPP_