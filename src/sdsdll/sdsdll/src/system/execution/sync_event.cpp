// sync_event.cpp

// Copyright (c) Mateusz Jandura. All rights reserved.
// SPDX-License-Identifier: Apache-2.0

#include <build/sdsdll_pch.hpp>
#include <system/execution/sync_event.hpp>
#if _SDSDLL_PREPROCESSOR_GUARD

_SDSDLL_BEGIN
// FUNCTION sync_event constructors/destructor
sync_event::sync_event() noexcept : _Myhandle(nullptr), _Myowns(false) {}

sync_event::sync_event(sync_event&& _Other) noexcept
    : _Myhandle(_Other._Myhandle), _Myowns(_Other._Myowns) {
    _Other._Myhandle = nullptr;
    _Other._Myowns   = false;
}

sync_event::sync_event(native_handle_type _Handle, const bool _Owns) noexcept
    : _Myhandle(_Handle), _Myowns(_Owns) {}

sync_event::sync_event(const wchar_t* const _Name, const initial_state _State,
    const bool _Manual) noexcept : _Myhandle(
        ::CreateEventW(nullptr, _Manual, static_cast<bool>(_State), _Name)), _Myowns(true) {}

sync_event::~sync_event() noexcept {
    close();
}

// FUNCTION sync_event::operator=
sync_event& sync_event::operator=(sync_event&& _Other) noexcept {
    if (this != _SDSDLL addressof(_Other)) {
        _Myhandle        = _Other._Myhandle;
        _Myowns          = _Other._Myowns;
        _Other._Myhandle = nullptr;
        _Other._Myowns   = false;
    }

    return *this;
}

// FUNCTION sync_event::open
_NODISCARD bool sync_event::open(const wchar_t* const _Name) noexcept {
    if (_Myhandle) { // some event is already open
        return false;
    }

    _Myhandle = ::OpenEventW(EVENT_ALL_ACCESS, false, _Name);
    if (_Myhandle) {
        _Myowns = true;
        return true;
    } else {
        return false;
    }
}

// FUNCTION sync_event::is_open
_NODISCARD bool sync_event::is_open() const noexcept {
    return _Myhandle != nullptr;
}

// FUNCTION sync_event::native_handle
_NODISCARD sync_event::native_handle_type sync_event::native_handle() const noexcept {
    return _Myhandle;
}

// FUNCTION sync_event::close
void sync_event::close() noexcept {
    if (_Myhandle && _Myowns) { // only owner can close the handle
        ::CloseHandle(_Myhandle);
        _Myhandle = nullptr;
        _Myowns   = false;
    }
}

// FUNCTION sync_event::reset
_NODISCARD bool sync_event::reset() noexcept {
    return _Myhandle ? ::ResetEvent(_Myhandle) != 0 : false;
}

// FUNCTION sync_event::signal
_NODISCARD bool sync_event::signal() noexcept {
    return _Myhandle ? ::SetEvent(_Myhandle) != 0 : false;
}

// FUNCTION sync_event::wait
_NODISCARD bool sync_event::wait() noexcept {
    return _Myhandle ? ::WaitForSingleObject(_Myhandle, INFINITE) == WAIT_OBJECT_0 : false;
}
_SDSDLL_END

#endif // _SDSDLL_PREPROCESSOR_GUARD