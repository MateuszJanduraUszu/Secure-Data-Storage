// thread_pool.hpp

// Copyright (c) Mateusz Jandura. All rights reserved.
// SPDX-License-Identifier: Apache-2.0

#pragma once
#ifndef _SDSDLL_SYSTEM_EXECUTION_THREAD_POOL_HPP_
#define _SDSDLL_SYSTEM_EXECUTION_THREAD_POOL_HPP_
#include <core/defs.hpp>
#if _SDSDLL_PREPROCESSOR_GUARD
#include <core/api.hpp>
#include <core/memory/allocator.hpp>
#include <core/optimization/ebco.hpp>
#include <core/traits/type_traits.hpp>
#include <cstddef>
#include <synchapi.h>
#include <system/execution/thread.hpp>
#include <WinBase.h>

_SDSDLL_BEGIN
// STRUCT _Thread_list_node
struct _Thread_list_node {
    _Thread_list_node() noexcept;
    ~_Thread_list_node() noexcept;

    _Thread_list_node* _Next; // pointer to the next node
    _Thread_list_node* _Prev; // pointer to the previous node
    thread _Thread;
};

// STRUCT _Thread_list_storage
struct _Thread_list_storage {
    _Thread_list_storage() noexcept;
    ~_Thread_list_storage() noexcept;

    _Thread_list_node* _First; // pointer to the first node
    _Thread_list_node* _Last; // pointer to the last node
    size_t _Size; // size of the list
};

// CLASS _Thread_list
class _Thread_list { // base container for the thread objects
public:
    _Thread_list() noexcept;
    ~_Thread_list() noexcept;

    _Thread_list(const _Thread_list&) = delete;
    _Thread_list& operator=(const _Thread_list&) = delete;

    explicit _Thread_list(const size_t _Count) noexcept;

    // returns the threads count
    _NODISCARD const size_t _Size() const noexcept;

    // tries to hire _Count additional threads
    _NODISCARD bool _Grow(size_t _Count) noexcept;
    
    // tries to dismiss _Count existing threads
    _NODISCARD bool _Reduce(size_t _Count) noexcept;

    // dismisses all threads
    void _Release() noexcept;

    // returns a pointer to the selected thread
    _NODISCARD thread* _Select_thread(size_t _Which) noexcept;
    
    // returns a non-mutable pointer to the selected thread
    _NODISCARD const thread* _Select_thread(size_t _Which) const noexcept;

    // returns a pointer to the first thread with the selected state
    _NODISCARD thread* _Select_thread_by_state(const thread_state _State) noexcept;

    // returns a non-mutable pointer to the first thread with the selected state
    _NODISCARD const thread* _Select_thread_by_state(const thread_state _State) const noexcept;

    // returns a pointer to the thread with the fewest tasks
    _NODISCARD thread* _Select_thread_by_tasks() noexcept;

    // returns a non-mutable pointer to the thread with the fewest tasks
    _NODISCARD const thread* _Select_thread_by_tasks() const noexcept;

    template <class _Fn, class... _Types>
    void _For_each_thread(_Fn&& _Func, _Types&&... _Args) noexcept {
        _Thread_list_storage& _Storage = _Mypair._Val1;
        if (_Storage._Size > 0) {
            _Thread_list_node* _Node = _Storage._First;
            while (_Node) {
                (void) _Func(_Node->_Thread, _STD forward<_Types>(_Args)...);
                _Node = _Node->_Next;
            }
        }
    }

private:
    using _Alloc = allocator<void>;
    
    // allocates a thread list node
    _NODISCARD static bool _Allocate_node(_Thread_list_node** const _Node, _Alloc& _Al) noexcept;

    // reduces _Count waiting threads
    void _Reduce_waiting_threads(size_t _Count, size_t& _Reduced) noexcept;

    // deallocates one node
    void _Free_node(_Thread_list_node* _Node) noexcept;

    mutable _Ebco_pair<_Thread_list_storage, _Alloc> _Mypair;
};

// CLASS thread_pool
class _SDSDLL_API thread_pool {
public:
    explicit thread_pool(const size_t _Count) noexcept;
    ~thread_pool() noexcept;

    thread_pool() = delete;
    thread_pool(const thread_pool&) = delete;
    thread_pool& operator=(const thread_pool&) = delete;

    // returns threads count
    _NODISCARD size_t threads() const noexcept;

    // checks if the thread-pool is still open
    _NODISCARD bool is_open() const noexcept;

    // checks if the thread-pool is working
    _NODISCARD bool is_working() const noexcept;

    // checks if the thread-pool is waiting
    _NODISCARD bool is_waiting() const noexcept;

    // tries to release at most _Count threads from the thread-pool
    _NODISCARD bool increase_threads(const size_t _Count) noexcept;

    // tries to hire _Count threads to the thread-pool
    _NODISCARD bool decrease_threads(const size_t _Count) noexcept;

    // tries to submit a new task
    _NODISCARD bool submit_task(const thread::task _Task, void* const _Data) noexcept;

    // tries to suspend the thread-pool
    _NODISCARD bool suspend() noexcept;

    // tries to resume the thread-pool
    _NODISCARD bool resume() noexcept;

    // closes the thread-pool
    void close() noexcept;

private:
    enum _Internal_state : unsigned char {
        _Closed,
        _Waiting,
        _Working
    };

    _Thread_list _Mylist;
    _Internal_state _Mystate;
};
_SDSDLL_END

#endif // _SDSDLL_PREPROCESSOR_GUARD
#endif // _SDSDLL_SYSTEM_EXECUTION_THREAD_POOL_HPP_