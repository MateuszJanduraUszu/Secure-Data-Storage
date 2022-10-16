// path.hpp

// Copyright (c) Mateusz Jandura. All rights reserved.
// SPDX-License-Identifier: Apache-2.0

#pragma once
#ifndef _SDSDLL_FILESYSTEM_PATH_HPP_
#define _SDSDLL_FILESYSTEM_PATH_HPP_
#include <core/defs.hpp>
#if _SDSDLL_PREPROCESSOR_GUARD
#include <core/api.hpp>
#include <core/optimization/sbo.hpp>
#include <core/optimization/string_view.hpp>
#include <core/traits/type_traits.hpp>
#include <cstddef>
#include <minwindef.h>
#include <processenv.h>
#include <string>
#include <type_traits>
#include <utility>

// STD types
using _STD basic_string;
using _STD wstring;

_SDSDLL_BEGIN
// CONSTANT _Short_path_size
_INLINE_VARIABLE constexpr size_t _Short_path_size = 260; // MAX_PATH

// CONSTANT _Long_path_size
_INLINE_VARIABLE constexpr size_t _Long_path_size = 32767;

// FUNCTION _Is_slash
extern _NODISCARD constexpr bool _Is_slash(const wchar_t _Ch) noexcept;

// CLASS path
class _SDSDLL_API path {
public:
    using value_type       = wchar_t;
    using string_type      = wstring;
    using string_view_type = wstring_view;
    using size_type        = size_t;
    using pointer          = typename string_type::pointer;
    using const_pointer    = typename string_type::const_pointer;
    using reference        = typename string_type::reference;
    using const_reference  = typename string_type::const_reference;

    using iterator       = typename string_type::iterator;
    using const_iterator = typename string_type::const_iterator;

    using reverse_iterator       = typename string_type::reverse_iterator;
    using const_reverse_iterator = typename string_type::const_reverse_iterator;

    path()            = default;
    path(const path&) = default;
    path(path&&)      = default;
    ~path()           = default;

    path(const value_type* const _Path);
    path(const string_view_type _Path);
    path(const string_type& _Path);

    // assigns a new path
    path& assign(path&& _Path);
    path& assign(const path& _Path);
    path& assign(const value_type* const _Path);
    path& assign(const string_view_type _Path);
    path& assign(const string_type& _Path);

    path& operator=(path&& _Path);
    path& operator=(const path& _Path);
    path& operator=(const value_type* const _Path);
    path& operator=(const string_view_type _Path);
    path& operator=(const string_type& _Path);

    // appends an additional path
    path& append(path&& _Path);
    path& append(const path& _Path);
    path& append(const value_type _Ch);
    path& append(const value_type* const _Path);
    path& append(const string_view_type _Path);
    path& append(const string_type& _Path);

    path& operator+=(path&& _Path);
    path& operator+=(const path& _Path);
    path& operator+=(const value_type _Ch);
    path& operator+=(const value_type* const _Path);
    path& operator+=(const string_view_type _Path);
    path& operator+=(const string_type& _Path);

    // clears the current path
    void clear() noexcept;

    // checks if the current path is empty
    _NODISCARD bool empty() const noexcept;

    // extracts drive from the current path (if possible)
    _NODISCARD path drive() const noexcept;

    // extracts extension from the current path (if possible)
    _NODISCARD path extension() const noexcept;

    // extracts filename from the current path (if possible)
    _NODISCARD path filename() const noexcept;

    // removes unnecessary slashes from the current path and converts it to the Windows standard
    path& fix() noexcept;

    // checks if the current path has drive
    _NODISCARD bool has_drive() const noexcept;
    
    // checks if the current path has extension
    _NODISCARD bool has_extension() const noexcept;
    
    // checks if the current path has filename
    _NODISCARD bool has_filename() const noexcept;
    
    // checks if the current path has root directory
    _NODISCARD bool has_root_directory() const noexcept;
    
    // checks if the current path is absolute
    _NODISCARD bool is_absolute() const noexcept;
    
    // checks if the current path is relative
    _NODISCARD bool is_relative() const noexcept;

    // converts the current path to the Windows standard
    path& make_preferred() noexcept;

    // removes extension from the current path (if possible)
    path& remove_extension() noexcept;
    
    // removes filename from the current path (if possible)
    path& remove_filename() noexcept;
    
    // replaces extension in the current path (if possible)
    path& replace_extension(const path& _Replacement);
    
    // replaces filename in the current path (if possible)
    path& replace_filename(const path& _Replacement);

    // tries to resize the current path
    _NODISCARD bool resize(const size_type _New_size, const value_type _Ch = value_type{}) noexcept;

    // returns path max size
    _NODISCARD size_type max_size() const noexcept;

    // returns the current path size
    _NODISCARD size_type size() const noexcept;

    // returns the current path as a non-mutable C-style string
    _NODISCARD const_pointer c_str() const noexcept;

    // returns the current path as a mutable C-style string
    _NODISCARD pointer data() noexcept;

    // returns the current path as a view
    _NODISCARD string_view_type view() const noexcept;

    // returns the current path
    _NODISCARD const string_type& str() const noexcept;

    // returns mutable iterator with the first element of the path
    _NODISCARD iterator begin() noexcept;

    // returns non-mutable iterator with the first element of the path
    _NODISCARD const_iterator begin() const noexcept;

    // returns mutable iterator with the last element of the path
    _NODISCARD iterator end() noexcept;

    // returns non-mutable iterator with the last element of the path
    _NODISCARD const_iterator end() const noexcept;

    // returns mutable reverse iterator with the first element of the path
    _NODISCARD reverse_iterator rbegin() noexcept;

    // return non-mutable reverse iterator with the first element of the path
    _NODISCARD const_reverse_iterator rbegin() const noexcept;

    // returns mutable reverse iterator with the last element of the path
    _NODISCARD reverse_iterator rend() noexcept;

    // returns non-mutable reverse iterator with the last element of the path
    _NODISCARD const_reverse_iterator rend() const noexcept;

    // returns non-mutable iterator with the first element of the path
    _NODISCARD const_iterator cbegin() const noexcept;

    // returns non-mutable iterator with the last element of the path
    _NODISCARD const_iterator cend() const noexcept;

    // returns non-mutable reverse iterator with the first element of the path
    _NODISCARD const_reverse_iterator crbegin() const noexcept;

    // returns non-mutable reverse iterator with the last element of the path
    _NODISCARD const_reverse_iterator crend() const noexcept;

private:
    // returns position of the last slash
    _NODISCARD size_type _Find_last_slash() const noexcept;

    static constexpr size_type _Npos = static_cast<size_type>(-1);

#ifdef _MSC_VER
#pragma warning(push, 1)
#pragma warning(disable : 4251) // C4251: std::wstring requires dll-interface
#endif // _MSC_VER
    string_type _Mystr;
#ifdef _MSC_VER
#pragma warning(pop)
#endif // _MSC_VER
};

// FUNCTION operator==
_SDSDLL_API _NODISCARD bool operator==(const path& _Left, const path& _Right) noexcept;
_SDSDLL_API _NODISCARD bool operator==(const path& _Left, const wchar_t* const _Right) noexcept;
_SDSDLL_API _NODISCARD bool operator==(const wchar_t* const _Left, const path& _Right) noexcept;
_SDSDLL_API _NODISCARD bool operator==(const path& _Left, const wstring_view _Right) noexcept;
_SDSDLL_API _NODISCARD bool operator==(const wstring_view _Left, const path& _Right) noexcept;
_SDSDLL_API _NODISCARD bool operator==(const path& _Left, const wstring& _Right) noexcept;
_SDSDLL_API _NODISCARD bool operator==(const wstring& _Left, const path& _Right) noexcept;

// FUNCTION operator!= 
_SDSDLL_API _NODISCARD bool operator!=(const path& _Left, const path& _Right) noexcept;
_SDSDLL_API _NODISCARD bool operator!=(const path& _Left, const wchar_t* const _Right) noexcept;
_SDSDLL_API _NODISCARD bool operator!=(const wchar_t* const _Left, const path& _Right) noexcept;
_SDSDLL_API _NODISCARD bool operator!=(const path& _Left, const wstring_view _Right) noexcept;
_SDSDLL_API _NODISCARD bool operator!=(const wstring_view _Left, const path& _Right) noexcept;
_SDSDLL_API _NODISCARD bool operator!=(const path& _Left, const wstring& _Right) noexcept;
_SDSDLL_API _NODISCARD bool operator!=(const wstring& _Left, const path& _Right) noexcept;

// FUNCTION operator+
_SDSDLL_API _NODISCARD path operator+(const path& _Left, const path& _Right) noexcept;
_SDSDLL_API _NODISCARD path operator+(const path& _Left, const wchar_t _Right) noexcept;
_SDSDLL_API _NODISCARD path operator+(const wchar_t _Left, const path& _Right) noexcept;
_SDSDLL_API _NODISCARD path operator+(const path& _Left, const wchar_t* const _Right) noexcept;
_SDSDLL_API _NODISCARD path operator+(const wchar_t* const _Left, const path& _Right) noexcept;
_SDSDLL_API _NODISCARD path operator+(const path& _Left, const wstring_view _Right) noexcept;
_SDSDLL_API _NODISCARD path operator+(const wstring_view _Left, const path& _Right) noexcept;
_SDSDLL_API _NODISCARD path operator+(const path& _Left, const wstring& _Right) noexcept;
_SDSDLL_API _NODISCARD path operator+(const wstring& _Left, const path& _Right) noexcept;

// FUNCTION current_path
_SDSDLL_API _NODISCARD path current_path();
_SDSDLL_API _NODISCARD bool current_path(const path& _Path);

// ENUM CLASS path_base
enum class path_base : bool {
    directory,
    executable
};

// FUNCTION _Query_executable_path
extern _NODISCARD bool _Query_executable_path(_Sbo_buffer<wchar_t>& _Buf, size_t& _Buf_size) noexcept;
extern _NODISCARD path _Query_executable_path();

// FUNCTION make_path
_SDSDLL_API _NODISCARD path make_path(const path& _Path, const path_base _Base);
_SDSDLL_END

#endif // _SDSDLL_PREPROCESSOR_GUARD
#endif // _SDSDLL_FILESYSTEM_PATH_HPP_