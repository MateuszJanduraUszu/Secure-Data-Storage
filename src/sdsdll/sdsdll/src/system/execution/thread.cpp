// thread.cpp

// Copyright (c) Mateusz Jandura. All rights reserved.
// SPDX-License-Identifier: Apache-2.0

#include <build/sdsdll_pch.hpp>
#include <system/execution/thread.hpp>
#if _SDSDLL_PREPROCESSOR_GUARD

_SDSDLL_BEGIN
// FUNCTION _Hardware_concurrency
_NODISCARD size_t _Hardware_concurrency() noexcept {
    SYSTEM_INFO _Info;
    ::GetNativeSystemInfo(_SDSDLL addressof(_Info));
    return static_cast<size_t>(_Info.dwNumberOfProcessors);
}

// FUNCTION _Suspend_current_thread
void _Suspend_current_thread() noexcept {
    ::SuspendThread(::GetCurrentThread());
}

// FUNCTION _Suspend_thread
_NODISCARD bool _Suspend_thread(void* const _Handle) noexcept {
    return ::SuspendThread(_Handle) != static_cast<DWORD>(-1);
}

// FUNCTION _Resume_thread
_NODISCARD bool _Resume_thread(void* const _Handle) noexcept {
    return ::ResumeThread(_Handle) != static_cast<DWORD>(-1);
}

// FUNCTION _Thread_task_storage copy constructor
_Thread_task_storage::_Thread_task_storage(const thread_state _State) noexcept
    : _State(_State), _Queue() {}

// FUNCTION _Thread_task
DWORD __stdcall _Thread_task(void* const _Data) _NOEXCEPT_FNTYPE {
    // Note: The CreateThread() requires the thread routine to return a DWORD.
    _Thread_task_storage* _Storage = static_cast<_Thread_task_storage*>(_Data);
    for (;;) {
        switch (_Storage->_State.load(_STD memory_order_relaxed)) {
        case thread_state::terminated:
            ::ExitThread(0); // terminate itself
            break;
        case thread_state::waiting: // suspend itself
            _Suspend_current_thread();
            break;
        case thread_state::working: // perform task
            if (!_Storage->_Queue.empty()) {
                const _Thread_task_data& _Data = _Storage->_Queue.pop();
                _Data._Task(_Data._Data);
            } else { // nothing to do, wait for any task
                _Storage->_State.store(thread_state::waiting, _STD memory_order_relaxed);
            }

            break;
        default:
#if _HAS_CXX23
            // Note: The std::unreachable() may optimize impossible code branches.
            _STD unreachable();
#endif // _HAS_CXX23
        }
    }

    return 0;
}

// FUNCTION thread constructors/destructor
thread::thread() noexcept : _Myid(0), _Mystorage(thread_state::waiting), _Mycbs() {
    _Start();
}

thread::thread(const task _Task, void* const _Data) noexcept
    : _Myid(0), _Mystorage(thread_state::working), _Mycbs() {
    _Mystorage._Queue.push(_Thread_task_data{_Task, _Data});
    _Start();
    if (_Myid == 0) {
        _Mystorage._Queue.clear();
    }
}

thread::~thread() noexcept {
    (void) terminate_and_wait(); // wait for the current task to finish, discard the rest
}

// FUNCTION thread::_Invoke_callbacks
void thread::_Invoke_callbacks(const event _Event) noexcept {
    for (const _Callback_data& _Cbs : _Mycbs) {
        if (_Cbs._Event == _Event) {
            _Cbs._Callback(_Cbs._Event, _Cbs._Data);
        }
    }
}

// FUNCTION thread::_Set_state
void thread::_Set_state(const thread_state _New_state) noexcept {
    _Mystorage._State.store(_New_state, _STD memory_order_relaxed);
}

// FUNCTION thread::_Clear_cached_data
void thread::_Clear_cached_data() noexcept {
    _Set_state(thread_state::terminated);
    _Mystorage._Queue.clear(); // clear task queue
    _Mycbs.clear(); // clear event callbacks
    ::CloseHandle(_Myhandle);
    _Myhandle = nullptr;
    _Myid     = 0;
}

// FUNCTION thread::_Start
void thread::_Start() noexcept {
    _Myhandle = ::CreateThread(nullptr, 0, _Thread_task,
        _SDSDLL addressof(_Mystorage), 0, reinterpret_cast<DWORD*>(&_Myid));
    if (!_Myhandle) { // creation failed
        _Myid = 0;
        _Set_state(thread_state::terminated);
    }
}

// FUNCTION thread::_Tidy
void thread::_Tidy() noexcept {
    if (state() != thread_state::waiting) {
        (void) suspend(); // must be suspended
    }

    _Set_state(thread_state::terminated);
    _Invoke_callbacks(terminate_event);
    
    // Note: We cannot use resume() here, because it will set the state to thread_state::working.
    //       The thread would continue doing its task.
    (void) _Resume_thread(_Myhandle); // the thread will terminate itself
}

// FUNCTION thread::hardware_concurrency
_NODISCARD size_t thread::hardware_concurrency() noexcept {
    static atomic<size_t> _Count(_Hardware_concurrency());
    return _Count.load(_STD memory_order_relaxed);
}

// FUNCTION thread::register_event_callback
void thread::register_event_callback(
    const event _Event, const event_callback _Callback, void* const _Data) {
    _Mycbs.push_back(_Callback_data{_Event, _Callback, _Data});
}

// FUNCTION thread::joinable
_NODISCARD bool thread::joinable() const noexcept {
    return state() != thread_state::terminated;
}

// FUNCTION thread::get_id
_NODISCARD const typename thread::id thread::get_id() const noexcept {
    return _Myid;
}

// FUNCTION thread::native_handle
_NODISCARD const typename thread::native_handle_type thread::native_handle() const noexcept {
    return _Myhandle;
}

// FUNCTION thread::state
_NODISCARD thread_state thread::state() const noexcept {
    return _Mystorage._State.load(_STD memory_order_relaxed);
}

// FUNCTION thread::tasks
_NODISCARD size_t thread::tasks() const noexcept {
    return _Mystorage._Queue.size();
}

// FUNCTION thread::submit_task
_NODISCARD bool thread::submit_task(const task _Task, void* const _Data) noexcept {
    const thread_state _State = state();
    if (_State == thread_state::terminated || _Mystorage._Queue.full()) {
        return false;
    }

    _Mystorage._Queue.push(_Thread_task_data{_Task, _Data});
    if (_State != thread_state::working) { // notify waiting thread
        (void) resume();
    }

    return true;
}

// FUNCTION thread::terminate
_NODISCARD bool thread::terminate() noexcept {
    if (!joinable()) {
        return false;
    }

    _Tidy(); // forces the thread to terminate itself
    _Clear_cached_data();
    return true;
}

// FUNCTION thread::terminate_and_wait
_NODISCARD bool thread::terminate_and_wait() noexcept {
    if (!joinable()) {
        return false;
    }

    _Tidy(); // forces the thread to terminate itself
    ::WaitForSingleObject(_Myhandle, INFINITE);
    _Clear_cached_data();
    return true;
}

// FUNCTION thread::suspend
_NODISCARD bool thread::suspend() noexcept {
    const thread_state _State = state();
    if (_State != thread_state::working) {
        return false;
    }

    _Invoke_callbacks(suspend_event);
    _Set_state(thread_state::waiting);
    if (!_Suspend_thread(_Myhandle)) { // failed to suspend thread, restore an old state
        _Set_state(_State);
        return false;
    }

    return true;
}

// FUNCTION thread::resume
_NODISCARD bool thread::resume() noexcept {
    const thread_state _State = state();
    if (_State != thread_state::waiting) {
        return false;
    }

    _Invoke_callbacks(resume_event);
    _Set_state(thread_state::working);
    if (!_Resume_thread(_Myhandle)) { // failed to resume the thread, restore an old state
        _Set_state(_State);
        return false;
    }

    return true;
}
_SDSDLL_END

#endif // _SDSDLL_PREPROCESSOR_GUARD