// service.hpp

// Copyright (c) Mateusz Jandura. All rights reserved.
// SPDX-License-Identifier: Apache-2.0

#pragma once
#ifndef _SDSDLL_SYSTEM_EXECUTION_SERVICE_HPP_
#define _SDSDLL_SYSTEM_EXECUTION_SERVICE_HPP_
#include <core/defs.hpp>
#if _SDSDLL_PREPROCESSOR_GUARD
#include <core/api.hpp>
#include <core/traits/type_traits.hpp>
#include <filesystem/path.hpp>
#include <filesystem/status.hpp>
#include <utility>
#include <winsvc.h>

_SDSDLL_BEGIN
// ENUM CLASS _Service_state
enum class _Service_state : unsigned char {
    _None,
    _Stopped,
    _Start_pending,
    _Stop_pending,
    _Running,
    _Continue_pending,
    _Pause_pending,
    _Paused
};

// FUNCTION _Get_service_state
extern _NODISCARD _Service_state _Get_service_state(const SC_HANDLE _Handle) noexcept;

// CLASS _Service_manager
class _Service_manager {
public:
    _Service_manager() noexcept;
    ~_Service_manager() noexcept;

    _Service_manager(const _Service_manager&) = delete;
    _Service_manager& operator=(const _Service_manager&) = delete;

    // returns handle to the Service Manager
    _NODISCARD const SC_HANDLE _Get() const noexcept;

private:
    SC_HANDLE _Myhandle;
};

// ENUM CLASS service_access
enum class service_access : unsigned long {
    none                 = 0,
    query_config         = 0x0001, // SERVICE_QUERY_CONFIG
    change_config        = 0x0002, // SERVICE_CHANGE_CONFIG
    query_status         = 0x0004, // SERVICE_QUERY_STATUS
    enumerate_dependents = 0x0008, // SERVICE_ENUMERATE_DEPENDENTS
    start                = 0x0010, // SERVICE_START
    stop                 = 0x0020, // SERVICE_STOP
    pause_continue       = 0x0040, // SERVICE_PAUSE_CONTINUE
    interrogate          = 0x0080, // SERVICE_INTERROGATE
    user_defined_control = 0x0100, // SERVICE_USER_DEFINED_CONTROLS
    all                  = 0x000F0000 | query_config | change_config | query_status | enumerate_dependents
        | start | stop | pause_continue | interrogate | user_defined_control // SERVICE_ALL_ACCESS
};

_BIT_OPS(service_access)

// ENUM CLASS service_type
enum class service_type : unsigned long {
    none              = 0,
    kernel_driver     = 0x00000001, // SERVICE_KERNEL_DRIVER
    filesystem_driver = 0x00000002, // SERVICE_FILE_SYSTEM_DRIVER
    adapter           = 0x00000004, // SERVICE_ADAPTER
    recognizer_driver = 0x00000008, // SERVICE_RECOGNIZER_DRIVER
    own_process       = 0x00000010, // SERVICE_WIN32_OWN_PROCESS
    share_process     = 0x00000020 // SERVICE_WIN32_SHARE_PROCESS
};

// ENUM CLASS service_start
enum class service_start : unsigned long {
    boot_start, // SERVICE_BOOT_START
    system_start, // SERVICE_SYSTEM_START
    auto_start, // SERVICE_AUTO_START
    demand_start, // SERVICE_DEMAND_START
    disabled, // SERVICE_DISABLED
    unknown
};

// ENUM CLASS service_error_control
enum class service_error_control : unsigned long {
    ignore,
    normal,
    severe,
    critical
};

// CLASS service
class _SDSDLL_API service { // manages an open/created service
public:
    service() noexcept;
    service(service&& _Other) noexcept;
    ~service() noexcept;

    service& operator=(service&& _Other) noexcept;

    service(const service&) = delete;
    service& operator=(const service&) = delete;

    // tries to create a new service
    _NODISCARD bool create(const path& _Target, const wchar_t* const _Name,
        const wchar_t* const _Description, const service_type _Type = service_type::own_process,
        const service_start _Start = service_start::demand_start,
        const service_error_control _Control = service_error_control::normal);

    // tries to open an existing service handle
    _NODISCARD bool open(
        const wchar_t* const _Name, const service_access _Access = service_access::all) noexcept;

    // closes an open/created service handle
    void close() noexcept;

    // checks if any service handle is open
    _NODISCARD bool is_open() const noexcept;
    
    // tries to suspend an open/created service
    _NODISCARD bool suspend() noexcept;
    
    // tries to resume an open/created service
    _NODISCARD bool resume() noexcept;

    // tries to start an open/created service
    _NODISCARD bool start() noexcept;

    // tries to stop an open/created service
    _NODISCARD bool stop() noexcept;

    // tries to terminate an open/created service
    _NODISCARD bool terminate() noexcept;
    
private:
    SC_HANDLE _Myhandle;
};
_SDSDLL_END

#endif // _SDSDLL_PREPROCESSOR_GUARD
#endif // _SDSDLL_SYSTEM_EXECUTION_SERVICE_HPP_