// sync_event.hpp

// Copyright (c) Mateusz Jandura. All rights reserved.
// SPDX-License-Identifier: Apache-2.0

#pragma once
#ifndef _SDSDLL_SYSTEM_EXECUTION_SYNC_EVENT_HPP_
#define _SDSDLL_SYSTEM_EXECUTION_SYNC_EVENT_HPP_
#include <core/defs.hpp>
#if _SDSDLL_PREPROCESSOR_GUARD
#include <core/api.hpp>
#include <core/traits/type_traits.hpp>
#include <handleapi.h>
#include <synchapi.h>
#include <WinBase.h>
#include <winnt.h>

_SDSDLL_BEGIN
// CLASS sync_event
class _SDSDLL_API sync_event {
public:
    using native_handle_type = void*;

    enum class initial_state : bool {
        reset,
        set
    };

    sync_event() noexcept;
    sync_event(sync_event&& _Other) noexcept;
    ~sync_event() noexcept;

    explicit sync_event(native_handle_type _Handle, const bool _Owns) noexcept;
    
    sync_event& operator=(sync_event&& _Other) noexcept;

    sync_event(const sync_event&) = delete;
    sync_event& operator=(const sync_event&) = delete;

    // creates a new sync event
    _NODISCARD static sync_event&& create_sync_event(const wchar_t* const _Name,
        const initial_state _State = initial_state::reset, const bool _Manual = true) noexcept;

    // opens an existing sync event
    _NODISCARD bool open(const wchar_t* const _Name) noexcept;

    // checks if any event is open
    _NODISCARD bool is_open() const noexcept;

    // returns a native handle to the event
    _NODISCARD native_handle_type native_handle() const noexcept;

    // closes the current event
    void close() noexcept;

    // tries to reset the event
    _NODISCARD bool reset() noexcept;

    // tries to signal the event
    _NODISCARD bool signal() noexcept;

    // tries to wait on signal
    _NODISCARD bool wait() noexcept;

private:
    native_handle_type _Myhandle;
    bool _Myowns;
};
_SDSDLL_END

#endif // _SDSDLL_PREPROCESSOR_GUARD
#endif // _SDSDLL_SYSTEM_EXECUTION_SYNC_EVENT_HPP_