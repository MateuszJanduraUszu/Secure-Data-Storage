// power.cpp

// Copyright (c) Mateusz Jandura. All rights reserved.
// SPDX-License-Identifier: Apache-2.0

#include <build/sdsdll_pch.hpp>
#include <system/power/power.hpp>
#if _SDSDLL_PREPROCESSOR_GUARD

_SDSDLL_BEGIN
// FUNCTION _Check_shutdown_type
_NODISCARD bool _Check_shutdown_type(const _Shutdown_type _Type) noexcept {
    switch (_Type) {
    case _Shutdown_type::_Logoff:
    case _Shutdown_type::_Shutdown:
    case _Shutdown_type::_Reboot:
        return true;
    default: // unknown type
        return false;
    }
}

// FUNCTION _Perform_shutdown
_NODISCARD bool _Perform_shutdown(const _Shutdown_type _Type) noexcept {
    if (!_Check_shutdown_type(_Type)) { // unknown type
        return false;
    }

    generic_handle_wrapper _Handle;
    if (::OpenProcessToken(
        ::GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &_Handle.get()) == 0) {
        return false;
    }

    TOKEN_PRIVILEGES _Token;
    if (::LookupPrivilegeValueA(
        nullptr, "SeShutdownPrivilege", _SDSDLL addressof(_Token.Privileges[0].Luid)) == 0) {
        return false;
    }

    _Token.PrivilegeCount           = 1;
    _Token.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
    const DWORD _Flags              = static_cast<DWORD>(_Type) | EWX_FORCEIFHUNG;
    if (::AdjustTokenPrivileges(
        _Handle.get(), false, _SDSDLL addressof(_Token), 0, nullptr, nullptr) == 0) {
        return false;
    }

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 28159) // C28159: consider using InitiateSystemShutdownEx()
                                 // instead of ExitWindowsEx()
#endif // _MSC_VER
    return ::ExitWindowsEx(_Flags, _Default_shutdown_reason) != 0;
#ifdef _MSC_VER
#pragma warning(pop)
#endif // _MSC_VER
}

// FUNCTION logoff_user
_NODISCARD bool logoff_user() noexcept {
    return _Perform_shutdown(_Shutdown_type::_Logoff);
}

// FUNCTION reboot_computer
_NODISCARD bool reboot_computer() noexcept {
    return _Perform_shutdown(_Shutdown_type::_Reboot);
}

// FUNCTION shutdown_computer
_NODISCARD bool shutdown_computer() noexcept {
    return _Perform_shutdown(_Shutdown_type::_Shutdown);
}
_SDSDLL_END

#endif // _SDSDLL_PREPROCESSOR_GUARD