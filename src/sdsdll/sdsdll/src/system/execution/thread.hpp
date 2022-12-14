// thread.hpp

// Copyright (c) Mateusz Jandura. All rights reserved.
// SPDX-License-Identifier: Apache-2.0

#pragma once
#ifndef _SDSDLL_SYSTEM_EXECUTION_THREAD_HPP_
#define _SDSDLL_SYSTEM_EXECUTION_THREAD_HPP_
#include <core/defs.hpp>
#if _SDSDLL_PREPROCESSOR_GUARD
#include <atomic>
#include <core/api.hpp>
#include <core/traits/type_traits.hpp>
#include <cstddef>
#include <minwindef.h>
#include <processthreadsapi.h>
#include <synchapi.h>
#include <sysinfoapi.h>
#include <system/execution/shared_queue.hpp>
#include <utility>
#include <vector>
#include <WinBase.h>

// STD types
using _STD atomic;
using _STD vector;

_SDSDLL_BEGIN
// FUNCTION _Hardware_concurrency
extern _NODISCARD size_t _Hardware_concurrency() noexcept;

// FUNCTION _Suspend_current_thread
extern void _Suspend_current_thread() noexcept;

// FUNCTION _Suspend_thread
extern _NODISCARD bool _Suspend_thread(void* const _Handle) noexcept;

// FUNCTION _Resume_thread
extern _NODISCARD bool _Resume_thread(void* const _Handle) noexcept;

// ALIAS _Thread_task_t
using _Thread_task_t = void(__stdcall*)(void*) _NOEXCEPT_FNTYPE;

// ENUM CLASS thread_state
enum class thread_state : unsigned char {
    terminated,
    waiting,
    working
};

// STRUCT _Thread_task_data
struct _Thread_task_data {
    _Thread_task_t _Task;
    void* _Data;
};

// STRUCT _Thread_task_storage
struct _Thread_task_storage {
    explicit _Thread_task_storage(const thread_state _State) noexcept;

    atomic<thread_state> _State;
    shared_queue<_Thread_task_data> _Queue;
};

// FUNCTION _Thread_task
extern DWORD __stdcall _Thread_task(void* const _Data) _NOEXCEPT_FNTYPE;

// CLASS thread
class _SDSDLL_API thread { // non-copyable thread manager
public:
    using native_handle_type = void*;
    using id                 = unsigned int;
    using task               = _Thread_task_t;

    enum event : unsigned char {
        suspend_event,
        resume_event,
        terminate_event
    };

    using event_callback = void(__STDCALL_OR_CDECL*)(const event, void* const) _NOEXCEPT_FNTYPE;

    thread() noexcept;
    ~thread() noexcept;

    thread(const thread&) = delete;
    thread& operator=(const thread&) = delete;

    explicit thread(const task _Task, void* const _Data) noexcept;

    // returns max threads count
    _NODISCARD static size_t hardware_concurrency() noexcept;

    // registers a new callback
    void register_event_callback(const event _Event, const event_callback _Callback, void* const _Data);

    // checks if the thread is joinable
    _NODISCARD bool joinable() const noexcept;

    // returns thread ID
    _NODISCARD const id get_id() const noexcept;

    // returns thread native handle
    _NODISCARD const native_handle_type native_handle() const noexcept;

    // returns the current state
    _NODISCARD thread_state state() const noexcept;

    // returns the tasks count
    _NODISCARD size_t tasks() const noexcept;

    // tries to submit a new task
    _NODISCARD bool submit_task(const task _Task, void* const _Data) noexcept;

    // tries to terminate the thread
    _NODISCARD bool terminate() noexcept;

    // tries to terminate the thread and wait for its completion
    _NODISCARD bool terminate_and_wait() noexcept;

    // tries to suspend the thread
    _NODISCARD bool suspend() noexcept;

    // tries to resume the thread
    _NODISCARD bool resume() noexcept;

private:
    // invokes registered event callbacks
    void _Invoke_callbacks(const event _Event) noexcept;

    // sets a new state
    void _Set_state(const thread_state _New_state) noexcept;

    // clears whole thread data
    void _Clear_cached_data() noexcept;

    // starts a new thread
    void _Start() noexcept;

    // prepares thread termination
    void _Tidy() noexcept;

    struct _Callback_data {
        event _Event;
        event_callback _Callback;
        void* _Data;
    };

    native_handle_type _Myhandle;
    id _Myid;
    _Thread_task_storage _Mystorage;
    vector<_Callback_data> _Mycbs;
};
_SDSDLL_END

#endif // _SDSDLL_SYSTEM_EXECUTION_THREAD_HPP_
#endif // _SDSDLL_SYSTEM_EXECUTION_THREAD_HPP_