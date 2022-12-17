// process.cpp

// Copyright (c) Mateusz Jandura. All rights reserved.
// SPDX-License-Identifier: Apache-2.0

#include <build/sdsdll_pch.hpp>
#include <system/execution/process.hpp>
#if _SDSDLL_PREPROCESSOR_GUARD

_SDSDLL_BEGIN
// FUNCTION _Get_current_process_name
_NODISCARD wstring _Get_current_process_name() {
    return _Query_executable_path().filename().str();
}

// FUNCTION _Is_current_process_name
_NODISCARD bool _Is_current_process_name(const wchar_t* const _Name) noexcept {
    return !_Name || _Name == _Get_current_process_name();
}

// FUNCTION _Get_process_id
_NODISCARD DWORD _Get_process_id(const wchar_t* const _Name) noexcept {
    if (_Is_current_process_name(_Name)) {
        return ::GetCurrentProcessId();
    } else {
        generic_handle_wrapper _Snapshot = ::CreateToolhelp32Snapshot(TH32CS_SNAPALL, 0);
        if (!_Snapshot) { // failed to take a snapshot
            return 0;
        }

        PROCESSENTRY32W _Entry = {0};
        _Entry.dwSize          = sizeof(PROCESSENTRY32W);
        bool _Next             = ::Process32FirstW(_Snapshot, _SDSDLL addressof(_Entry));
        while (_Next) {
            if (string_traits<wchar_t, int>::compare(_Entry.szExeFile, _Name) == 0) {
                return _Entry.th32ProcessID;
            }

            _Next = ::Process32NextW(_Snapshot, _SDSDLL addressof(_Entry));
        }

        return 0; // process not found
    }
}

// FUNCTION _Open_process_handle
_NODISCARD void* _Open_process_handle(
    const wchar_t* const _Name, const process_access _Access, DWORD* const _Id) noexcept {
    if (_Id) {
        *_Id = _Get_process_id(_Name);
        return ::OpenProcess(static_cast<DWORD>(_Access), false, *_Id);
    } else {
        return ::OpenProcess(static_cast<DWORD>(_Access), false, _Get_process_id(_Name));
    }
}

// FUNCTION _Open_process_token
_NODISCARD void* _Open_process_token(
    const wchar_t* const _Name, const _Token_access _Access) noexcept {
    process_handle_wrapper _Handle = _Is_current_process_name(_Name) ?
        ::GetCurrentProcess() : _Open_process_handle(_Name, process_access::query_information);
    if (!_Handle) { // failed to open a process handle
        return nullptr;
    }

    void* _Result;
    return ::OpenProcessToken(_Handle, static_cast<DWORD>(_Access), &_Result) != 0 ? _Result : nullptr;
}

// FUNCTION _Is_current_process_elevated
_NODISCARD bool _Is_current_process_elevated() noexcept {
    generic_handle_wrapper _Handle = _Open_process_token(nullptr, _Token_access::_Query);
    if (!_Handle) { // failed to open a token
        return false;
    }

    TOKEN_ELEVATION _Elevation;
    DWORD _Len; // returned length
    if (::GetTokenInformation(_Handle, TokenElevation, _SDSDLL addressof(_Elevation),
        sizeof(TOKEN_ELEVATION), &_Len) == 0) { // failed to get a token informations
        return false;
    }

    return _Elevation.TokenIsElevated != 0;
}

// FUNCTION _Is_process_elevated
_NODISCARD bool _Is_process_elevated(const wchar_t* const _Name) noexcept {
    if (_Is_current_process_name(_Name)) {
        return _Is_current_process_elevated();
    } else {
        generic_handle_wrapper _Handle = _Open_process_token(_Name, _Token_access::_Query);
        if (!_Handle) { // failed to open a token
            return false;
        }

        TOKEN_ELEVATION _Elevation;
        DWORD _Len; // returned length
        if (::GetTokenInformation(_Handle, TokenElevation, _SDSDLL addressof(_Elevation),
            sizeof(TOKEN_ELEVATION), &_Len) == 0) { // failed to get a token informations
            return false;
        }

        return _Elevation.TokenIsElevated != 0;
    }
}

// FUNCTION _Is_process_running
_NODISCARD bool _Is_process_running(const wchar_t* const _Name) noexcept {
    // try to open a process handle with the lowest access
    process_handle_wrapper _Handle = _Open_process_handle(_Name, process_access::terminate);
    return _Handle.good();
}

// FUNCTION _Suspend_process
_NODISCARD bool _Suspend_process(void* const _Handle) noexcept {
    using _Fn        = LONG(__stdcall*)(void*);
    static _Fn _Func = _Load_symbol<_Fn>(L"ntdll", "NtSuspendProcess");
    return _Func ? NT_SUCCESS(_Func(_Handle)) : false;
}

// FUNCTION _Resume_process
_NODISCARD bool _Resume_process(void* const _Handle) noexcept {
    using _Fn        = LONG(__stdcall*)(void*);
    static _Fn _Func = _Load_symbol<_Fn>(L"ntdll", "NtResumeProcess");
    return _Func ? NT_SUCCESS(_Func(_Handle)) : false;
}

// VARIABLE this_process::_Mydata
this_process::_This_process_data this_process::_Mydata{};

// FUNCTION this_process::_This_process_data constructor/destructor
this_process::_This_process_data::_This_process_data() : _Id(::GetCurrentProcessId()),
    _Elevated(_Is_current_process_elevated()), _Name(_Get_current_process_name()) {}

this_process::_This_process_data::~_This_process_data() noexcept {}

// FUNCTION this_process destructor
this_process::~this_process() noexcept {}

// FUNCTION this_process::id
_NODISCARD const DWORD this_process::id() noexcept {
    return _Mydata._Id;
}

// FUNCTION this_process::is_elevated
_NODISCARD const bool this_process::is_elevated() noexcept {
    return _Mydata._Elevated;
}

// FUNCTION this_process::name
_NODISCARD const wstring& this_process::name() noexcept {
    return _Mydata._Name;
}

// FUNCTION this_process::terminate
void this_process::terminate(const uint32_t _Code) noexcept {
    ::ExitProcess(_Code);
}

// FUNCTION _Process_data constructors/destructor
_Process_data::_Process_data() noexcept : _Handle(), _Id(0), _Elevated(false), _Name() {}

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 26439) // C26439: declare function 'noexcept'
#endif // _MSC_VER
_Process_data::_Process_data(_Process_data&& _Other) {
    *this = _STD move(_Other);
}

_Process_data::~_Process_data() noexcept {}

// FUNCTION _Process_data::operator=
_Process_data& _Process_data::operator=(_Process_data&& _Other) {
    if (this != _SDSDLL addressof(_Other)) {
        _Handle   = _STD move(_Other._Handle);
        _Id       = _STD move(_Other._Id);
        _Elevated = _STD move(_Other._Elevated);
        _Name     = _STD move(_Other._Name);
    }

    return *this;
}

// FUNCTION process constructors/destructor
process::process() noexcept : _Mydata() {}

process::process(process&& _Other) : _Mydata(_STD move(_Other._Mydata)) {}

process::~process() noexcept {}

// FUNCTION process::operator=
process& process::operator=(process&& _Other) {
    if (this != _SDSDLL addressof(_Other)) {
        _Mydata = _STD move(_Other._Mydata);
    }

    return *this;
}
#ifdef _MSC_VER
#pragma warning(pop)
#endif // _MSC_VER

// FUNCTION process::create
_NODISCARD bool process::create(const path& _Target, const wchar_t* const _Args) {
    if (_Mydata._Handle) { // some process handle is already open
        return false;
    }

    STARTUPINFOW _Startup_info     = {0};
    _Startup_info.cb               = sizeof(STARTUPINFOW);
    PROCESS_INFORMATION _Proc_info = {0};
    if (::CreateProcessW(
        _Target.c_str(), const_cast<wchar_t*>(_Args), nullptr, nullptr, false, CREATE_NEW_CONSOLE,
        nullptr, nullptr, _SDSDLL addressof(_Startup_info), _SDSDLL addressof(_Proc_info)) != 0) {
        if (_Proc_info.hThread) { // close a redundant handle
            ::CloseHandle(_Proc_info.hThread);
        }

        _Mydata._Handle   = _Proc_info.hProcess;
        _Mydata._Name     = _Target.filename().str();
        _Mydata._Id       = _Proc_info.dwProcessId;
        _Mydata._Elevated = false; // always false
        return true;
    } else {
        return false;
    }
}

// FUNCTION process::create_elevated
_NODISCARD bool process::create_elevated(const path& _Target, const wchar_t* const _Args) {
    if (_Mydata._Handle) { // some process handle is already open
        return false;
    }

    SHELLEXECUTEINFOW _Info = {0};
    _Info.cbSize            = sizeof(SHELLEXECUTEINFOW);
    _Info.fMask             = SEE_MASK_UNICODE | SEE_MASK_NOCLOSEPROCESS;
    _Info.lpFile            = _Target.c_str();
    _Info.lpParameters      = _Args; // command line arguments
    _Info.lpVerb            = L"runas"; // launches a new process as the Administrator
    _Info.nShow             = SW_SHOW;
    if (::ShellExecuteExW(_SDSDLL addressof(_Info)) != 0) {
        _Mydata._Handle   = _Info.hProcess;
        _Mydata._Name     = _Target.filename().str();
        _Mydata._Id       = ::GetProcessId(_Mydata._Handle);
        _Mydata._Elevated = true; // always true
        return true;
    } else {
        return false;
    }
}

// FUNCITON process::open
_NODISCARD bool process::open(const wchar_t* const _Name, const process_access _Access) noexcept {
    if (_Mydata._Handle) { // some process handle is already open
        return false;
    }

    _Mydata._Handle = _Open_process_handle(_Name, _Access, &_Mydata._Id);
    if (_Mydata._Handle) {
        _Mydata._Name     = _Name;
        _Mydata._Elevated = _Is_process_elevated(_Name);
        return true;
    } else {
        return false;
    }
}

// FUNCTION process::close
void process::close() noexcept {
    if (_Mydata._Handle) {
        _Mydata._Handle.close();
        _Mydata._Name.clear();
        _Mydata._Id       = 0;
        _Mydata._Elevated = false;
    }
}

// FUNCTION process::id
_NODISCARD const DWORD process::id() const noexcept {
    return _Mydata._Id;
}

// FUNCTION process::is_elevated
_NODISCARD const bool process::is_elevated() const noexcept {
    return _Mydata._Elevated;
}

// FUNCTION process::is_open
_NODISCARD bool process::is_open() const noexcept {
    return _Mydata._Handle.good();
}

// FUNCTION process::name
_NODISCARD const wstring& process::name() const noexcept {
    return _Mydata._Name;
}

// FUNCTION process::suspend
_NODISCARD bool process::suspend() noexcept {
    if (!_Mydata._Handle) { // no process handle is open
        return false;
    }

    return _Suspend_process(_Mydata._Handle);
}

// FUNCTION process::resume
_NODISCARD bool process::resume() noexcept {
    if (!_Mydata._Handle) { // no process handle is open
        return false;
    }

    return _Resume_process(_Mydata._Handle);
}

// FUNCTION process::terminate
_NODISCARD bool process::terminate(const uint32_t _Code) noexcept {
    if (!_Mydata._Handle) { // no process handle is open
        return false;
    }
    
    if (::TerminateProcess(_Mydata._Handle, _Code) != 0) {
        _Mydata._Handle.close();
        _Mydata._Name.clear();
        _Mydata._Id       = 0;
        _Mydata._Elevated = false;
        return true;
    } else {
        return false;
    }
}
_SDSDLL_END

#endif // _SDSDLL_PREPROCESSOR_GUARD