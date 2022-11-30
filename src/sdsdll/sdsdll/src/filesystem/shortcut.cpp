// shortcut.cpp

// Copyright (c) Mateusz Jandura. All rights reserved.
// SPDX-License-Identifier: Apache-2.0

#include <build/sdsdll_pch.hpp>
#include <filesystem/shortcut.hpp>
#if _SDSDLL_PREPROCESSOR_GUARD

_SDSDLL_BEGIN
// FUNCTION _Shortcut_handle copy constructor/destructor
_Shortcut_handle::_Shortcut_handle(const path& _Target) : _Path(_Target) {
    if (FAILED(::CoCreateInstance(::CLSID_ShellLink, nullptr, CLSCTX_ALL, ::IID_IShellLinkW,
        reinterpret_cast<void**>(_SDSDLL addressof(_Link))))) {
        _File = nullptr;
    }

    _Link->QueryInterface(::IID_IPersistFile, reinterpret_cast<void**>(_SDSDLL addressof(_File)));
}

_Shortcut_handle::~_Shortcut_handle() noexcept {
    _SDSDLL release_com_object(_File);
    _SDSDLL release_com_object(_Link);
}

// FUNCTION _Shortcut_handle::_Valid
_NODISCARD bool _Shortcut_handle::_Valid() const noexcept {
    return _Link != nullptr && _File != nullptr;
}

// FUNCTION _Shortcut_handle::_Submit
_NODISCARD bool _Shortcut_handle::_Submit() noexcept {
    return _Valid() ? SUCCEEDED(_File->Save(_Path.c_str(), false)) : false;
}

// FUNCTION create_shortcut
_NODISCARD bool create_shortcut(const path& _Target, const path& _Shortcut,
    const wchar_t* const _Description, const shortcut_icon& _Icon, const path& _Directory) {
    { // check if the shortcut can be created
        const file_status _Status(_Target);
        if (!_SDSDLL exists(_Target) || _SDSDLL is_directory(_Status, any_directory_type)) {
            return false;
        }
    }
    
    _Shortcut_handle _Handle(_Shortcut);
    if (!_Handle._Valid()) {
        return false;
    }

    _Handle._Link->SetPath(_Target.c_str());
    if (_Description) { // set shortcut description
        _Handle._Link->SetDescription(_Description);
    }

    if (!_Icon.path.empty()) { // set shortcut icon
        _Handle._Link->SetIconLocation(_Icon.path.c_str(), _Icon.index);
    }

    if (!_Directory.empty()) { // set shortcut working directory
        _Handle._Link->SetWorkingDirectory(_Directory.c_str());
    }

    return _Handle._Submit();
}

_NODISCARD bool create_shortcut(const path& _Target, const path& _Shortcut,
    const wstring_view _Description, const shortcut_icon& _Icon, const path& _Directory) {
    return _SDSDLL create_shortcut(_Target, _Shortcut, _Description.data(), _Icon, _Directory);
}

_NODISCARD bool create_shortcut(const path& _Target, const path& _Shortcut,
    const wstring& _Description, const shortcut_icon& _Icon, const path& _Directory) {
    return _SDSDLL create_shortcut(_Target, _Shortcut, _Description.c_str(), _Icon, _Directory);
}

// FUNCTION read_shortcut
_NODISCARD path read_shortcut(const path& _Target) {
    _Shortcut_handle _Handle(_Target);
    if (!_Handle._Valid()) {
        return path{};
    }

    _Sbo_buffer<wchar_t> _Buf(_Short_path_size + 1);
    if (_Buf._Empty()) { // allocation failed
        return path{};
    }

    return SUCCEEDED(_Handle._Link->GetPath(
        _Buf._Get(), static_cast<int>(_Buf._Size()), nullptr, 0)) ? path{_Buf._Get()} : path{};
}

// FUNCTION remove_shortcut
_NODISCARD bool remove_shortcut(const path& _Target) {
    { // check if deletion is possible or necessary
        const file_status _Status(_Target);
        if (!_SDSDLL exists(_Status)) {
            return true;
        }

        if (_SDSDLL is_directory(_Status, any_directory_type)) {
            return false;
        }
    }

    return _SDSDLL delete_file(_Target);
}

// FUNCTION change_shortcut_description
_NODISCARD bool change_shortcut_description(const path& _Target, const wchar_t* const _New_description) {
    _Shortcut_handle _Handle(_Target);
    if (!_Handle._Valid()) {
        return false;
    }

    return SUCCEEDED(_Handle._Link->SetDescription(_New_description)) ? _Handle._Submit() : false;
}

_NODISCARD bool change_shortcut_description(const path& _Target, const wstring_view _New_description) {
    _Shortcut_handle _Handle(_Target);
    if (!_Handle._Valid()) {
        return false;
    }

    return SUCCEEDED(_Handle._Link->SetDescription(_New_description.data())) ? _Handle._Submit() : false;
}

_NODISCARD bool change_shortcut_description(const path& _Target, const wstring& _New_description) {
    _Shortcut_handle _Handle(_Target);
    if (!_Handle._Valid()) {
        return false;
    }

    return SUCCEEDED(_Handle._Link->SetDescription(_New_description.c_str())) ? _Handle._Submit() : false;
}

// FUNCTION change_shortcut_icon
_NODISCARD bool change_shortcut_icon(const path& _Target, const shortcut_icon& _New_icon) {
    _Shortcut_handle _Handle(_Target);
    if (!_Handle._Valid()) {
        return false;
    }

    return SUCCEEDED(_Handle._Link->SetIconLocation(_New_icon.path.c_str(), _New_icon.index))
        ? _Handle._Submit() : false;
}

// FUNCTION change_shortcut_target
_NODISCARD bool change_shortcut_target(const path& _Target, const path& _New_target) {
    _Shortcut_handle _Handle(_Target);
    if (!_Handle._Valid()) {
        return false;
    }

    return SUCCEEDED(_Handle._Link->SetPath(_New_target.c_str())) ? _Handle._Submit() : false;
}

// FUNCTION change_shortcut_working_directory
_NODISCARD bool change_shortcut_working_directory(const path& _Target, const path& _New_directory) {
    _Shortcut_handle _Handle(_Target);
    if (!_Handle._Valid()) {
        return false;
    }

    return SUCCEEDED(_Handle._Link->SetWorkingDirectory(_New_directory.c_str()))
        ? _Handle._Submit() : false;
}
_SDSDLL_END

#endif // _SDSDLL_PREPROCESSOR_GUARD