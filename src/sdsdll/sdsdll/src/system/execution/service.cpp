// service.cpp

// Copyright (c) Mateusz Jandura. All rights reserved.
// SPDX-License-Identifier: Apache-2.0

#include <build/sdsdll_pch.hpp>
#include <system/execution/service.hpp>
#if _SDSDLL_PREPROCESSOR_GUARD

_SDSDLL_BEGIN
// FUNCTION _Query_service_state
_NODISCARD _Service_state _Get_service_state(const SC_HANDLE _Handle) noexcept {
    SERVICE_STATUS _Status;
    if (::QueryServiceStatus(_Handle, _SDSDLL addressof(_Status)) == 0) {
        return _Service_state::_None;
    }

    return static_cast<_Service_state>(_Status.dwCurrentState);
}

// FUNCTION _Service_manager constructor/destructor
_Service_manager::_Service_manager() noexcept
    : _Myhandle(::OpenSCManagerW(nullptr, nullptr, SC_MANAGER_ALL_ACCESS)) {}

_Service_manager::~_Service_manager() noexcept {
    if (_Myhandle) {
        ::CloseServiceHandle(_Myhandle);
        _Myhandle = nullptr;
    }
}

// FUNCTION _Service_manager::_Get
_NODISCARD const SC_HANDLE _Service_manager::_Get() const noexcept {
    return _Myhandle;
}

// FUNCTION service constructors/destructor
service::service() noexcept : _Myhandle(nullptr) {}

service::service(service&& _Other) noexcept : _Myhandle(_STD exchange(_Other._Myhandle, nullptr)) {}

service::~service() noexcept {
    close();
}

// FUNCTION service::operator=
service& service::operator=(service&& _Other) noexcept {
    if (this != _SDSDLL addressof(_Other)) {
        _Myhandle        = _Other._Myhandle;
        _Other._Myhandle = nullptr;
    }

    return *this;
}

// FUNCTION sercice::create
_NODISCARD bool service::create(
    const path& _Target, const wchar_t* const _Name, const wchar_t* const _Description,
    const service_type _Type, const service_start _Start, const service_error_control _Control) {
    if (_Myhandle) { // some handle is already open
        return false;
    }

    _Service_manager _Mgr;
    if (!_Mgr._Get()) {
        return false;
    }

    _Myhandle = ::CreateServiceW(_Mgr._Get(), _Name, _Description, SERVICE_ALL_ACCESS,
        static_cast<DWORD>(_Type), static_cast<DWORD>(_Start), static_cast<DWORD>(_Control),
            _Target.c_str(), nullptr, nullptr, nullptr, nullptr, nullptr);
    return _Myhandle != nullptr;
}

// FUNCTION service::open
_NODISCARD bool service::open(const wchar_t* const _Name, const service_access _Access) noexcept {
    if (_Myhandle) { // some handle is already open
        return false;
    }

    _Service_manager _Mgr;
    if (!_Mgr._Get()) {
        return false;
    }

    _Myhandle = ::OpenServiceW(_Mgr._Get(), _Name, static_cast<DWORD>(_Access));
    return _Myhandle != nullptr;
}

// FUNCTION service::close
void service::close() noexcept {
    if (_Myhandle) {
        ::CloseServiceHandle(_Myhandle);
        _Myhandle = nullptr;
    }
}

// FUNCTION service::is_open
_NODISCARD bool service::is_open() const noexcept {
    return _Myhandle != nullptr;
}

// FUNCTION service::suspend
_NODISCARD bool service::suspend() noexcept {
    if (!_Myhandle) { // no service handle is open
        return false;
    }

    switch (_Get_service_state(_Myhandle)) {
    case _Service_state::_Running:
    case _Service_state::_Continue_pending:
        break;
    default:
        return false;
    }

    SERVICE_STATUS _Status;
    return ::ControlService(_Myhandle, SERVICE_CONTROL_PAUSE, _SDSDLL addressof(_Status)) != 0;
}

// FUNCTION service::resume
_NODISCARD bool service::resume() noexcept {
    if (!_Myhandle) { // no service handle is open
        return false;
    }

    switch (_Get_service_state(_Myhandle)) {
    case _Service_state::_Pause_pending:
    case _Service_state::_Paused:
        break;
    default:
        return false;
    }

    SERVICE_STATUS _Status;
    return ::ControlService(_Myhandle, SERVICE_CONTROL_CONTINUE, _SDSDLL addressof(_Status)) != 0;
}

// FUNCTION service::start
_NODISCARD bool service::start() noexcept {
    if (!_Myhandle) { // no service handle is open
        return false;
    }

    switch (_Get_service_state(_Myhandle)) {
    case _Service_state::_Stopped:
    case _Service_state::_Stop_pending:
        break;
    default:
        return false;
    }

    return ::StartServiceW(_Myhandle, 0, nullptr) != 0;
}

// FUNCTION service::stop
_NODISCARD bool service::stop() noexcept {
    if (!_Myhandle) { // no service handle is open
        return false;
    }

    switch (_Get_service_state(_Myhandle)) {
    case _Service_state::_Start_pending:
    case _Service_state::_Running:
    case _Service_state::_Continue_pending:
    case _Service_state::_Pause_pending:
    case _Service_state::_Paused:
        break;
    default:
        return false;
    }

    SERVICE_STATUS _Status;
    return ::ControlService(_Myhandle, SERVICE_CONTROL_STOP, _SDSDLL addressof(_Status)) != 0;
}

// FUNCTION service::terminate
_NODISCARD bool service::terminate() noexcept {
    if (!_Myhandle) { // no service handle is open
        return false;
    }

    if (::DeleteService(_Myhandle) != 0) {
        ::CloseServiceHandle(_Myhandle);
        _Myhandle = nullptr;
        return true;
    } else {
        return false;
    }
}
_SDSDLL_END

#endif // _SDSDLL_PREPROCESSOR_GUARD