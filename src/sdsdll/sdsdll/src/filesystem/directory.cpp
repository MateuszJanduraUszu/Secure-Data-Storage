// directory.cpp

// Copyright (c) Mateusz Jandura. All rights reserved.
// SPDX-License-Identifier: Apache-2.0

#include <build/sdsdll_pch.hpp>
#include <filesystem/directory.hpp>
#if _SDSDLL_PREPROCESSOR_GUARD

_SDSDLL_BEGIN
// FUNCTION directory_entry constructors/destructor
directory_entry::directory_entry() noexcept : _Mycache(), _Mypath() {}

directory_entry::directory_entry(const directory_entry& _Other)
    : _Mycache(_Other._Mycache), _Mypath(_Other._Mypath) {}

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 26439) // C26439: declare function with 'noexcept'
#endif // _MSC_VER
directory_entry::directory_entry(directory_entry&& _Other)
    : _Mycache(_STD move(_Other._Mycache)), _Mypath(_STD move(_Other._Mypath)) {}
#ifdef _MSC_VER
#pragma warning(pop)
#endif // _MSC_VER

directory_entry::directory_entry(const path& _Target) : _Mycache(), _Mypath(_Target) {
    refresh();
}

directory_entry::~directory_entry() noexcept {}

// FUNCTION directory_entry::assign
void directory_entry::assign(const path& _New_target) {
    if (_Mypath != _New_target) {
        _Mypath = _New_target;
        refresh();
    }
}

// FUNCTION directory_entry::replace_filename
void directory_entry::replace_filename(const path& _New_filename) {
    _Mypath.replace_filename(_New_filename);
    refresh();
}

// FUNCTION directory_entry::refresh
void directory_entry::refresh() {
    file_status _Status(_Mypath);
    _Mycache._Attrs = _Status.attributes();
    _Mycache._Perms = _Status.perms();
    _Mycache._Type  = _Status.type();
}

// FUNCTION directory_entry::operator const path&
_NODISCARD directory_entry::operator const path&() const {
    return _Mypath;
}

// FUNCTION directory_entry::full_path
_NODISCARD const path& directory_entry::full_path() const {
    return _Mypath;
}

// FUNCTION directory_entry::exists
_NODISCARD bool directory_entry::exists() const noexcept {
    switch (_Mycache._Type) {
    case file_type::none:
    case file_type::not_found:
        return false;
    default:
        return true;
    }
}

// FUNCTION directory_entry::is_directory
_NODISCARD bool directory_entry::is_directory() const noexcept {
    return _Mycache._Type == file_type::directory;
}

_NODISCARD bool directory_entry::is_directory(any_directory_type_t) const noexcept {
    return _Has_bits(_Mycache._Attrs, file_attributes::directory);
}

// FUNCTION directory_entry::is_other
_NODISCARD bool directory_entry::is_other() const noexcept {
    switch (_Mycache._Type) {
    case file_type::none:
    case file_type::not_found:
    case file_type::directory:
    case file_type::regular:
    case file_type::symlink:
    case file_type::junction:
        return false;
    default:
        return true;
    }
}

// FUNCTION directory_entry::is_regular_file
_NODISCARD bool directory_entry::is_regular_file() const noexcept {
    return _Mycache._Type == file_type::regular;
}

// FUNCTION directory_entry::is_symlink
_NODISCARD bool directory_entry::is_symlink() const noexcept {
    return _Mycache._Type == file_type::symlink;
}

// FUNCTION directory_entry::is_junction
_NODISCARD bool directory_entry::is_junction() const noexcept {
    return _Mycache._Type == file_type::junction;
}

// FUNCTION directory_entry::status
_NODISCARD file_status directory_entry::status() const {
    return file_status(_Mypath, _Mycache._Attrs, _Mycache._Perms, _Mycache._Type);
}

// FUNCTION operator==
_NODISCARD bool operator==(const directory_entry& _Left, const directory_entry& _Right) {
    return _Left.full_path() == _Right.full_path();
}

// FUNCTION operator!=
_NODISCARD bool operator!=(const directory_entry& _Left, const directory_entry& _Right) {
    return _Left.full_path() != _Right.full_path();
}

// FUNCTION _Open_directory_handle
_NODISCARD void* _Open_directory_handle(const path& _Target) noexcept {
    const wstring& _Dir_path = _Target.str() + LR"(\*)";
    WIN32_FIND_DATAW _Data;
    return ::FindFirstFileExW(_Dir_path.c_str(), FindExInfoBasic,
        _SDSDLL addressof(_Data), FindExSearchNameMatch, nullptr, 0);
}

// FUNCTION _Close_directory_handle
void _Close_directory_handle(void*& _Handle) noexcept {
    if (_Handle) {
        ::FindClose(_Handle);
        _Handle = nullptr;
    }
}

// FUNCTION _Should_skip_entry
_NODISCARD bool _Should_skip_entry(const wchar_t* const _Filename) noexcept {
    using _Traits = string_traits<wchar_t, int>;
    return _Traits::compare(_Filename, L".") == 0 || _Traits::compare(_Filename, L"..") == 0;
}

// FUNCTION _Advance_directory_iterator
_NODISCARD bool _Advance_directory_iterator(
    void* const _Handle, _Filename_and_attributes& _Data) noexcept {
    if (!_Handle) { // invalid handle
        return false;
    }

    WIN32_FIND_DATAW _Temp;
    for (;;) {
        if (::FindNextFileW(_Handle, _SDSDLL addressof(_Temp)) == 0) {
            return false;
        }

        if (!_Should_skip_entry(_Temp.cFileName)) {
            break;
        }
    }

    _Data._Attributes = _Temp.dwFileAttributes;
    _Data._Filename   = _Temp.cFileName;
    return true;
}

// FUNCTION _Dir_iter copy constructor/destructor
_Dir_iter::_Dir_iter(const path& _Target)
    : _Entry(), _Data(), _Path(_Target), _Handle(_Open_directory_handle(_Target)) {}

_Dir_iter::~_Dir_iter() noexcept {
    _Close_directory_handle(_Handle);
}

// FUNCTION _Dir_iter
_NODISCARD bool _Dir_iter::_Advance() {
    if (_Advance_directory_iterator(_Handle, _Data)) {
        _Assign_entry(_Data._Filename);
        return true;
    } else {
        return false;
    }
}

// FUNCTION _Dir_iter::_Assign_entry
void _Dir_iter::_Assign_entry(const wstring& _Filename) {
    _Entry.assign(_Path + L'\\' + _Filename);
}

// FUNCTION directory_iterator constructors
directory_iterator::directory_iterator() noexcept : _Myimpl() {}

directory_iterator::directory_iterator(const path& _Target) : _Myimpl(_Target) {}

// FUNCTION directory_iterator::operator*
const directory_entry& directory_iterator::operator*() const {
    return _Myimpl._Entry;
}

// FUNCTION directory_iterator::operator->
const directory_entry* directory_iterator::operator->() const {
    return _SDSDLL addressof(_Myimpl._Entry);
}

// FUNCTION directory_iterator::operator++
_NODISCARD bool directory_iterator::operator++() {
    return _Myimpl._Advance();
}

// FUNCTION directory_iterator::increment
_NODISCARD bool directory_iterator::increment() {
    return _Myimpl._Advance();
}
_SDSDLL_END

#endif // _SDSDLL_PREPROCESSOR_GUARD