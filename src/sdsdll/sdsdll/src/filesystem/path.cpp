// path.cpp

// Copyright (c) Mateusz Jandura. All rights reserved.
// SPDX-License-Identifier: Apache-2.0

#include <build/sdsdll_pch.hpp>
#include <filesystem/path.hpp>
#if _SDSDLL_PREPROCESSOR_GUARD

_SDSDLL_BEGIN
// FUNCTION _Is_slash
_NODISCARD constexpr bool _Is_slash(const wchar_t _Ch) noexcept {
    return _Ch == L'\\' || _Ch == L'/';
}

// FUNCTION path copy constructors
path::path(const value_type* const _Path) : _Mystr(_Path) {}

path::path(const string_view_type _Path) : _Mystr(_Path.data(), _Path.size()) {}

path::path(const string_type& _Path) : _Mystr(_Path) {}

// FUNCTION path::_Find_last_slash
_NODISCARD path::size_type path::_Find_last_slash() const noexcept {
    if (_Mystr.empty()) {
        return _Npos;
    }

    const size_type _Preferred_slash = _Mystr.find_last_of(L'\\');
    const size_type _Slash           = _Mystr.find_last_of(L'/');
    if (_Preferred_slash == _Npos && _Slash == _Npos) {
        return _Npos;
    }

    size_type _Last_slash = (_STD max)(_Preferred_slash, _Slash);
    if (_Last_slash == _Npos) { // slash not found, switch to the other one
        _Last_slash = (_STD min)(_Preferred_slash, _Slash);
    }

    return _Last_slash;
}

// FUNCTION path::assign
path& path::assign(path&& _Path) {
    if (this != _SDSDLL addressof(_Path)) {
        _Mystr.assign(_Path._Mystr.c_str(), _Path._Mystr.size());
    }

    return *this;
}

path& path::assign(const path& _Path) {
    if (this != _SDSDLL addressof(_Path)) {
        _Mystr.assign(_Path._Mystr);
    }

    return *this;
}

path& path::assign(const value_type* const _Path) {
    _Mystr.assign(_Path);
    return *this;
}

path& path::assign(const string_view_type _Path) {
    _Mystr.assign(_Path.data(), _Path.size());
    return *this;
}

path& path::assign(const string_type& _Path) {
    _Mystr.assign(_Path);
    return *this;
}

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 26439) // C26439: declare this function with 'noexcept' specifier
#endif // _MSC_VER
// FUNCTION path::operator=
path& path::operator=(path&& _Path) {
    return assign(_STD move(_Path));
}
#ifdef _MSC_VER
#pragma warning(pop)
#endif // _MSC_VER

path& path::operator=(const path& _Path) {
    return assign(_Path);
}

path& path::operator=(const value_type* const _Path) {
    return assign(_Path);
}

path& path::operator=(const string_view_type _Path) {
    return assign(_Path);
}

path& path::operator=(const string_type& _Path) {
    return assign(_Path);
}

// FUNCTION path::append
path& path::append(path&& _Path) {
    if (this != _SDSDLL addressof(_Path)) {
        _Mystr.append(_Path._Mystr.c_str(), _Path._Mystr.size());
    }

    return *this;
}

path& path::append(const path& _Path) {
    if (this != _SDSDLL addressof(_Path)) {
        _Mystr.append(_Path._Mystr);
    }

    return *this;
}

path& path::append(const value_type _Ch) {
    _Mystr.push_back(_Ch);
    return *this;
}

path& path::append(const value_type* const _Path) {
    _Mystr.append(_Path);
    return *this;
}

path& path::append(const string_view_type _Path) {
    _Mystr.append(_Path.data(), _Path.size());
    return *this;
}

path& path::append(const string_type& _Path) {
    _Mystr.append(_Path);
    return *this;
}

// FUNCTION path::operator+=
path& path::operator+=(path&& _Path) {
    return append(_STD move(_Path));
}

path& path::operator+=(const path& _Path) {
    return append(_Path);
}

path& path::operator+=(const value_type _Ch) {
    return append(_Ch);
}

path& path::operator+=(const value_type* const _Path) {
    return append(_Path);
}

path& path::operator+=(const string_view_type _Path) {
    return append(_Path);
}

path& path::operator+=(const string_type& _Path) {
    return append(_Path);
}

// FUNCTION path::clear
void path::clear() noexcept {
    _Mystr.clear();
}

// FUNCTION path::empty
_NODISCARD bool path::empty() const noexcept {
    return _Mystr.empty();
}

// FUNCTION path::drive
_NODISCARD path path::drive() const noexcept {
    return has_drive() ? string_type(1, _Mystr.front()) + LR"(:\)" : path{};
}

// FUNCTION path::extension
_NODISCARD path path::extension() const noexcept {
    return has_extension() ? _Mystr.substr(_Mystr.find_last_of(L'.') + 1, _Mystr.size()) : path{};
}

// FUNCTION path::filename
_NODISCARD path path::filename() const noexcept {
    if (has_filename()) {
        const size_type _Pos = _Find_last_slash();
        return _Pos == _Npos ? _Mystr : _Mystr.substr(_Pos + 1, _Mystr.size());
    }

    return path{};
}

// FUNCTION path::fix
path& path::fix() noexcept {
    string_type _New_path;
    for (const value_type& _Ch : _Mystr) { // skip unnecessary slashes
        if (_New_path.size() > 0 && _Is_slash(_Ch) && _Is_slash(_New_path.back())) {
            continue;
        }

        if (_Ch == L'/') { // use correct slash
            _New_path.push_back(L'\\');
        } else {
            _New_path.push_back(_Ch);
        }
    }

    _Mystr.assign(_New_path);
    return *this;
}

// FUNCTION path::has_drive
_NODISCARD bool path::has_drive() const noexcept {
    if (_Mystr.size() < 3) { // requires at least 3 elements
        return false;
    }

    if (_Mystr[1] != L':') { // the second element must be ':'
        return false;
    }

    if (!_Is_slash(_Mystr[2])) { // the third element must be any slash
        return false;
    }

    const wchar_t _Letter = _Mystr.front();
    return (_Letter >= L'C' && _Letter <= L'Z') || (_Letter >= L'c' && _Letter <= L'z');
}

// FUNCTION path::has_extension
_NODISCARD bool path::has_extension() const noexcept {
    const size_type _Dot = _Mystr.find_last_of(L'.');
    if (_Dot != _Npos) { // must contains dot
        const size_type _Path_size = _Mystr.size();
        if (_Dot == _Path_size - 1) { // dot must not be the last element
            return false;
        }

        const size_type _Slash = _Find_last_slash();
        if (_Slash == _Npos) {
            return true;
        } else {
            return _Slash != _Path_size - 1;
        }
    }

    return false;
}

// FUNCTION path::has_filename
_NODISCARD bool path::has_filename() const noexcept {
    // file cannot exists without any extension
    return has_extension();
}

// FUNCTION path::has_root_directory
_NODISCARD bool path::has_root_directory() const noexcept {
    // path must contains a drive or a slash at the first position to be a root directory
    return has_drive() || _Is_slash(_Mystr.front());
}

// FUNCTION path::is_absolute
_NODISCARD bool path::is_absolute() const noexcept {
    // path with the root directory must be absolute
    return has_root_directory();
}

// FUNCTION path::is_relative
_NODISCARD bool path::is_relative() const noexcept {
    return !is_absolute();
}

// FUNCTION path::make_preferred
path& path::make_preferred() noexcept {
    if (!_Mystr.empty()) {
        for (value_type& _Ch : _Mystr) { // replace each '/' with '\'
            if (_Ch == L'/') {
                _Ch = L'\\';
            }
        }
    }

    return *this;
}

// FUNCTION path::remove_extension
path& path::remove_extension() noexcept {
    if (has_extension()) {
        _Mystr.resize(_Mystr.find_last_of(L'.'));
    }

    return *this;
}

// FUNCTION path::remove_filename
path& path::remove_filename() noexcept {
    if (has_filename()) {
        const size_type _Slash = _Find_last_slash();
        if (_Slash == _Npos) {
            _Mystr.clear();
        } else {
            _Mystr.resize(_Slash);
        }
    }

    return *this;
}

// FUNCTION path::replace_extension
path& path::replace_extension(const path& _Replacement) {
    if (has_extension()) {
        remove_extension();
        append(L'.' + _Replacement);
    }

    return *this;
}

// FUNCTION path::replace_filename
path& path::replace_filename(const path& _Replacement) {
    if (has_filename()) {
        remove_filename();
        append(L'\\' + _Replacement);
    }

    return *this;
}

// FUNCTION path::resize
_NODISCARD bool path::resize(const size_type _New_size, const value_type _Ch) noexcept {
    try {
        _Mystr.resize(_New_size, _Ch);
    } catch (...) {
        return false;
    }

    return true;
}

// FUNCTION path::max_size
_NODISCARD path::size_type path::max_size() const noexcept {
    return _Mystr.max_size();
}

// FUNCTION path::size
_NODISCARD path::size_type path::size() const noexcept {
    return _Mystr.size();
}

// FUNCTION path::c_str
_NODISCARD path::const_pointer path::c_str() const noexcept {
    return _Mystr.c_str();
}

// FUNCTION path::data
_NODISCARD path::pointer path::data() noexcept {
    return _Mystr.data();
}

// FUNCTION path::view
_NODISCARD path::string_view_type path::view() const noexcept {
    return string_view_type{_Mystr.c_str(), _Mystr.size()};
}

// FUNCTION path::str
_NODISCARD const path::string_type& path::str() const noexcept {
    return _Mystr;
}

// FUNCTION path::begin
_NODISCARD path::iterator path::begin() noexcept {
    return _Mystr.begin();
}

_NODISCARD path::const_iterator path::begin() const noexcept {
    return _Mystr.begin();
}

// FUNCTION path::end
_NODISCARD path::iterator path::end() noexcept {
    return _Mystr.end();
}

_NODISCARD path::const_iterator path::end() const noexcept {
    return _Mystr.end();
}

// FUNCTION path::rbegin
_NODISCARD path::reverse_iterator path::rbegin() noexcept {
    return _Mystr.rbegin();
}

_NODISCARD path::const_reverse_iterator path::rbegin() const noexcept {
    return _Mystr.rbegin();
}

// FUNCTION path::rend
_NODISCARD path::reverse_iterator path::rend() noexcept {
    return _Mystr.rend();
}

_NODISCARD path::const_reverse_iterator path::rend() const noexcept {
    return _Mystr.rend();
}

// FUNCTION path::cbegin
_NODISCARD path::const_iterator path::cbegin() const noexcept {
    return _Mystr.cbegin();
}

// FUNCTION path::cend
_NODISCARD path::const_iterator path::cend() const noexcept {
    return _Mystr.cend();
}

// FUNCTION path::crbegin
_NODISCARD path::const_reverse_iterator path::crbegin() const noexcept {
    return _Mystr.crbegin();
}

// FUNCTION path::crend
_NODISCARD path::const_reverse_iterator path::crend() const noexcept {
    return _Mystr.crend();
}

// FUNCTION operator==
_NODISCARD bool operator==(const path& _Left, const path& _Right) noexcept {
    return _Left.str() == _Right.str();
}

_NODISCARD bool operator==(const path& _Left, const wchar_t* const _Right) noexcept {
    return _Left.str() == _Right;
}

_NODISCARD bool operator==(const wchar_t* const _Left, const path& _Right) noexcept {
    return _Left == _Right.str();
}

_NODISCARD bool operator==(const path& _Left, const wstring_view _Right) noexcept {
    return wstring_view{_Left.c_str(), _Left.size()} == _Right;
}

_NODISCARD bool operator==(const wstring_view _Left, const path& _Right) noexcept {
    return _Left == wstring_view{_Right.c_str(), _Right.size()};
}

_NODISCARD bool operator==(const path& _Left, const wstring& _Right) noexcept {
    return _Left.str() == _Right;
}

_NODISCARD bool operator==(const wstring& _Left, const path& _Right) noexcept {
    return _Left == _Right.str();
}

// FUNCTION operator!=
_NODISCARD bool operator!=(const path& _Left, const path& _Right) noexcept {
    return !(_Left == _Right);
}

_NODISCARD bool operator!=(const path& _Left, const wchar_t* const _Right) noexcept {
    return !(_Left == _Right);
}

_NODISCARD bool operator!=(const wchar_t* const _Left, const path& _Right) noexcept {
    return !(_Left == _Right);
}

_NODISCARD bool operator!=(const path& _Left, const wstring_view _Right) noexcept {
    return !(_Left == _Right);
}

_NODISCARD bool operator!=(const wstring_view _Left, const path& _Right) noexcept {
    return !(_Left == _Right);
}

_NODISCARD bool operator!=(const path& _Left, const wstring& _Right) noexcept {
    return !(_Left == _Right);
}

_NODISCARD bool operator!=(const wstring& _Left, const path& _Right) noexcept {
    return !(_Left == _Right);
}

// FUNCTION operator+
_NODISCARD path operator+(const path& _Left, const path& _Right) noexcept {
    return _Left.str() + _Right.str();
}

_NODISCARD path operator+(const path& _Left, const wchar_t _Right) noexcept {
    return _Left.str() + _Right;
}

_NODISCARD path operator+(const wchar_t _Left, const path& _Right) noexcept {
    return _Left + _Right.str();
}

_NODISCARD path operator+(const path& _Left, const wchar_t* const _Right) noexcept {
    return _Left.str() + _Right;
}

_NODISCARD path operator+(const wchar_t* const _Left, const path& _Right) noexcept {
    return _Left + _Right.str();
}

_NODISCARD path operator+(const path& _Left, const wstring_view _Right) noexcept {
    return _Left.str() + _Right.data();
}

_NODISCARD path operator+(const wstring_view _Left, const path& _Right) noexcept {
    return _Left.data() + _Right.str();
}

_NODISCARD path operator+(const path& _Left, const wstring& _Right) noexcept {
    return _Left.str() + _Right;
}

_NODISCARD path operator+(const wstring& _Left, const path& _Right) noexcept {
    return _Left + _Right.str();
}

// FUNCTION current_path
_NODISCARD path current_path() {
    const DWORD _Buf_size = GetCurrentDirectoryW(0, nullptr);
    _Sbo_buffer<wchar_t> _Buf(static_cast<size_t>(_Buf_size));
    if (GetCurrentDirectoryW(_Buf_size, _Buf._Get()) == _Buf_size - 1) { // exclude null-terminator
        return wstring{_Buf._Get(), static_cast<size_t>(_Buf_size)};
    } else {
        return path{};
    }
}

_NODISCARD bool current_path(const path& _Path) {
    return SetCurrentDirectoryW(_Path.c_str()) != 0;
}

// FUNCTION _Query_executable_path
_NODISCARD bool _Query_executable_path(_Sbo_buffer<wchar_t>& _Buf, size_t& _Buf_size) noexcept {
    const DWORD _Copied = GetModuleFileNameW(nullptr, _Buf._Get(), static_cast<DWORD>(_Buf_size));
    if (_Copied > 0 && GetLastError() != ERROR_INSUFFICIENT_BUFFER) {
        _Buf_size = static_cast<size_t>(_Copied);
        return true;
    } else {
        return false;
    }
}

_NODISCARD path _Query_executable_path() {
    size_t _Buf_size = _Short_path_size + 1; // include null-terminator
    _Sbo_buffer<wchar_t> _Buf(_Buf_size);
    if (_Buf._Empty()) { // failed to allocate the buffer
        return path{};
    }
    
    if (_Query_executable_path(_Buf, _Buf_size)) {
        return wstring{_Buf._Get(), _Buf_size};
    } else { // short path was insufficient, try again with a long one
        _Buf_size = _Long_path_size + 1; // include null-terminator
        if (!_Buf._Resize(_Buf_size)) { // failed to increase the buffer
            return path{};
        }

        if (_Query_executable_path(_Buf, _Buf_size)) {
            return wstring{_Buf._Get(), _Buf_size};
        } else { // failed to query an executable filename
            return path{};
        }
    }
}

// FUNCTION make_path
_NODISCARD path make_path(const path& _Path, const path_base _Base) {
    switch (_Base) {
    case path_base::directory:
        return _SDSDLL current_path() + L'\\' + _Path;
    case path_base::executable:
    default:
        path _Result = _Query_executable_path();
        if (!_Result.empty()) {
            _Result.remove_filename();
            _Result.append(L'\\' + _Path);
            return _Result;
        } else {
            return path{};
        }
    }
}
_SDSDLL_END

#endif // _SDSDLL_PREPROCESSOR_GUARD