// process.hpp

// Copyright (c) Mateusz Jandura. All rights reserved.
// SPDX-License-Identifier: Apache-2.0

#pragma once
#ifndef _SDSDLL_SYSTEM_EXECUTION_PROCESS_HPP_
#define _SDSDLL_SYSTEM_EXECUTION_PROCESS_HPP_
#include <core/defs.hpp>
#if _SDSDLL_PREPROCESSOR_GUARD
#include <core/api.hpp>
#include <core/optimization/sbo.hpp>
#include <core/pattern/singleton.hpp>
#include <core/symbol/loader.hpp>
#include <core/traits/string_traits.hpp>
#include <core/traits/type_traits.hpp>
#include <cstdint>
#include <filesystem/path.hpp>
#include <handleapi.h>
#include <minwindef.h>
#include <processthreadsapi.h>
#include <securitybaseapi.h>
#include <shellapi.h>
#include <string>
#include <system/handle/generic_handle.hpp>
#include <system/handle/handle_wrapper.hpp>
#include <system/handle/process_handle.hpp>
#include <TlHelp32.h>
#include <WinBase.h>
#include <winnt.h>
#include <winternl.h>
#include <WinUser.h>

// STD types
using _STD wstring;

_SDSDLL_BEGIN
// ENUM CLASS process_access
enum class process_access : unsigned long {
    none                      = 0,
    terminate                 = 0x0001, // PROCESS_TERMINATE
    create_process            = 0x0080, // PROCESS_CREATE_PROCESS
    query_information         = 0x0400, // PROCESS_QUERY_INFORMATION
    query_limited_information = 0x1000, // PROCESS_QUERY_LIMITED_INFORMATION
    all                       = 0x000F'0000 | 0x0010'0000 | 0xFFFF // PROCESS_ALL_ACCESS
};

_BIT_OPS(process_access)

// FUNCTION _Get_current_process_name
extern _NODISCARD wstring _Get_current_process_name();

// FUNCTION _Is_current_process_name
extern _NODISCARD bool _Is_current_process_name(const wchar_t* const _Name) noexcept;

// FUNCTION _Get_process_id
extern _NODISCARD DWORD _Get_process_id(const wchar_t* const _Name) noexcept;

// FUNCTION _Open_process_handle
extern _NODISCARD void* _Open_process_handle(const wchar_t* const _Name,
    const process_access _Access, DWORD* const _Id = nullptr) noexcept;

// ENUM CLASS _Token_access
enum class _Token_access : unsigned long {
    _None              = 0,
    _Assign_primary    = 0x0001, // TOKEN_ASSIGN_PRIMARY
    _Duplicate         = 0x0002, // TOKEN_DUPLICATE
    _Impersonate       = 0x0004, // TOKEN_IMPERSONATE
    _Query             = 0x0008, // TOKEN_QUERY
    _Query_source      = 0x0010, // TOKEN_QUERY_SOURCE
    _Adjust_privileges = 0x0020, // TOKEN_ADJUST_PRIVILEGES
    _Adjust_groups     = 0x0040, // TOKEN_ADJUST_GROUPS
    _Adjust_default    = 0x0080, // TOKEN_ADJUST_DEFAULT
    _Adjust_session_id = 0x0100, // TOKEN_SESSIONID
    _All               = _Assign_primary | _Duplicate | _Impersonate | _Query | // TOKEN_ALL_ACCESS
        _Query_source | _Adjust_privileges | _Adjust_groups | _Adjust_default | _Adjust_session_id
};

_BIT_OPS(_Token_access)

// FUNCTION _Open_process_token
extern _NODISCARD void* _Open_process_token(
    const wchar_t* const _Name, const _Token_access _Access) noexcept;

// FUNCTION _Is_current_process_elevated
extern _NODISCARD bool _Is_current_process_elevated() noexcept;

// FUNCITON _Is_process_elevated
extern _NODISCARD bool _Is_process_elevated(const wchar_t* const _Name) noexcept;

// FUNCTION _Is_process_running
extern _NODISCARD bool _Is_process_running(const wchar_t* const _Name) noexcept;

// FUNCTION _Suspend_process
extern _NODISCARD bool _Suspend_process(void* const _Handle) noexcept;

// FUNCTION _Resume_process
extern _NODISCARD bool _Resume_process(void* const _Handle) noexcept;

// CLASS this_process
class _SDSDLL_API this_process {
public:
    ~this_process() noexcept;

    this_process() = delete;
    this_process(const this_process&) = delete;
    this_process& operator=(const this_process&) = delete;

    // returns the current process ID
    _NODISCARD static const DWORD id() noexcept;

    // checks if the current process has administrator privileges
    _NODISCARD static const bool is_elevated() noexcept;
    
    // returns the current process name
    _NODISCARD static const wstring& name() noexcept;
    
    // terminates the current process
    static void terminate(const uint32_t _Code = 0) noexcept;

private:
    struct _This_process_data {
        _This_process_data();
        ~_This_process_data() noexcept;

        DWORD _Id; // current process ID
        bool _Elevated; // true if the current process has administrator privileges
        wstring _Name; // the current process name
    };

    static _This_process_data _Mydata;
};

// STRUCT _Process_data
struct _Process_data {
    _Process_data() noexcept;
    _Process_data(_Process_data&& _Other);
    ~_Process_data() noexcept;

    _Process_data& operator=(_Process_data&& _Other);

    _Process_data(const _Process_data&) = delete;
    _Process_data& operator=(const _Process_data&) = delete;

    process_handle_wrapper _Handle; // handle to an open/created process
    DWORD _Id; // ID of the open/created process
    bool _Elevated; // true if an open/created process has administrator privileges
    wstring _Name; // name of the open/created process
};

// CLASS process
class _SDSDLL_API process { // manages an open/created process
public:
    process() noexcept;
    process(process&& _Other);
    ~process() noexcept;

    process& operator=(process&& _Other);

    process(const process&) = delete;
    process& operator=(const process&) = delete;

    // tries to create a new process
    _NODISCARD bool create(const path& _Target, const wchar_t* const _Args);
    
    // tries to create a new process with the administrator privileges
    _NODISCARD bool create_elevated(const path& _Target, const wchar_t* const _Args);

    // tries to open an existing process handle
    _NODISCARD bool open(const wchar_t* const _Name, const process_access _Access) noexcept;

    // closes an open/created process handle
    void close() noexcept;

    // returns an open/created process ID
    _NODISCARD const DWORD id() const noexcept;

    // checks if an open/created process has administrator privileges
    _NODISCARD const bool is_elevated() const noexcept;

    // checks if any process handle is open
    _NODISCARD bool is_open() const noexcept;

    // returns an open/created process name
    _NODISCARD const wstring& name() const noexcept;

    // tries to suspend an open/created process
    _NODISCARD bool suspend() noexcept;

    // tries to resume an open/created process
    _NODISCARD bool resume() noexcept;

    // tries to terminate an open/created process
    _NODISCARD bool terminate(const uint32_t _Code = 0) noexcept;

private:
#ifdef _MSC_VER
#pragma warning(push, 1)
#pragma warning(disable : 4251) // C4251: _Process_data requires dll-interface
#endif // _MSC_VER
    _Process_data _Mydata;
#ifdef _MSC_VER
#pragma warning(pop)
#endif // _MSC_VER
};
_SDSDLL_END

#endif // _SDSDLL_PREPROCESSOR_GUARD
#endif // _SDSDLL_SYSTEM_EXECUTION_PROCESS_HPP_