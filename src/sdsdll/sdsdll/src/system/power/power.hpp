// power.hpp

// Copyright (c) Mateusz Jandura. All rights reserved.
// SPDX-License-Identifier: Apache-2.0

#pragma once
#ifndef _SDSDLL_SYSTEM_POWER_POWER_HPP_
#define _SDSDLL_SYSTEM_POWER_POWER_HPP_
#include <core/defs.hpp>
#if _SDSDLL_PREPROCESSOR_GUARD
#include <core/api.hpp>
#include <core/traits/type_traits.hpp>
#include <handleapi.h>
#include <minwindef.h>
#include <processthreadsapi.h>
#include <reason.h>
#include <securitybaseapi.h>
#include <system/handle/generic_handle.hpp>
#include <WinBase.h>
#include <winnt.h>
#include <WinUser.h>

_SDSDLL_BEGIN
// CONSTANT _Default_shutdown_reason
inline constexpr DWORD _Default_shutdown_reason = SHTDN_REASON_MAJOR_APPLICATION
    | SHTDN_REASON_MINOR_UPGRADE | SHTDN_REASON_FLAG_PLANNED;

// ENUM CLASS _Shutdown_type
enum class _Shutdown_type : unsigned long {
    _Logoff   = 0x00000000, // EWX_LOGOFF
    _Shutdown = 0x00000001, // EWX_SHUTDOWN
    _Reboot   = 0x00000002 // EWX_REBOOT
};

// FUNCTION _Check_shutdown_type
extern _NODISCARD bool _Check_shutdown_type(const _Shutdown_type _Type) noexcept;

// FUNCTION _Perform_shutdown
extern _NODISCARD bool _Perform_shutdown(const _Shutdown_type _Type) noexcept;

// FUNCTION logoff_user
_SDSDLL_API _NODISCARD bool logoff_user() noexcept;

// FUNCTION reboot_computer
_SDSDLL_API _NODISCARD bool reboot_computer() noexcept;

// FUNCTION shutdown_computer
_SDSDLL_API _NODISCARD bool shutdown_computer() noexcept;
_SDSDLL_END

#endif // _SDSDLL_PREPROCESSOR_GUARD
#endif // _SDSDLL_SYSTEM_POWER_POWER_HPP_