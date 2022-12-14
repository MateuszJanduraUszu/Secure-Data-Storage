// directory.hpp

// Copyright (c) Mateusz Jandura. All rights reserved.
// SPDX-License-Identifier: Apache-2.0

#pragma once
#ifndef _SDSDLL_FILESYSTEM_DIRECTORY_HPP_
#define _SDSDLL_FILESYSTEM_DIRECTORY_HPP_
#include <core/defs.hpp>
#if _SDSDLL_PREPROCESSOR_GUARD
#include <core/api.hpp>
#include <core/traits/memory_traits.hpp>
#include <core/traits/string_traits.hpp>
#include <core/traits/type_traits.hpp>
#include <cstddef>
#include <fileapi.h>
#include <filesystem/path.hpp>
#include <filesystem/status.hpp>
#include <minwinbase.h>
#include <string>
#include <system/handle/generic_handle.hpp>

// STD types
using _STD wstring;

_SDSDLL_BEGIN
// CLASS directory_entry
class _SDSDLL_API directory_entry {
public:
    directory_entry() noexcept;
    directory_entry(const directory_entry& _Other);
    directory_entry(directory_entry&& _Other);
    ~directory_entry() noexcept;

    explicit directory_entry(const path& _Target);

    directory_entry& operator=(const directory_entry&) = default;
    directory_entry& operator=(directory_entry&&)      = default;

    // assigns a new target
    void assign(const path& _New_target);

    // replaces the current filename
    void replace_filename(const path& _New_filename);

    // refreshes a cached data
    void refresh();

    // returns a full path to the current entry
    _NODISCARD operator const path&() const;
    
    // returns a full path to the current entry
    _NODISCARD const path& full_path() const;

    // checks if the current entry exists
    _NODISCARD bool exists() const noexcept;

    // checks if the current entry is a directory
    _NODISCARD bool is_directory() const noexcept;
    _NODISCARD bool is_directory(any_directory_type_t) const noexcept;

    // checks if the current entry is an unknown type
    _NODISCARD bool is_other() const noexcept;

    // checks if the current entry is a regular file
    _NODISCARD bool is_regular_file() const noexcept;

    // checks if the current entry is a symbolic link
    _NODISCARD bool is_symlink() const noexcept;

    // checks if the current entry is a junction
    _NODISCARD bool is_junction() const noexcept;

    // returns the current entry status
    _NODISCARD file_status status() const;

private:
    struct _Cached_data {
        file_attributes _Attrs;
        file_perms _Perms;
        file_type _Type;
    };

    _Cached_data _Mycache;
    path _Mypath;
};

// FUNCTION operator==
_SDSDLL_API _NODISCARD bool operator==(const directory_entry& _Left, const directory_entry& _Right);

// FUNCTION operator!=
_SDSDLL_API _NODISCARD bool operator!=(const directory_entry& _Left, const directory_entry& _Right);

// FUNCTION _Open_directory_handle
extern _NODISCARD void* _Open_directory_handle(const path& _Target) noexcept;

// FUNCTION _Close_directory_handle
extern void _Close_directory_handle(void*& _Handle) noexcept;

// FUNCTION _Should_skip_entry
extern _NODISCARD bool _Should_skip_entry(const wchar_t* const _Filename) noexcept;

// STRUCT _Filename_and_attributes
struct _Filename_and_attributes {
    wstring _Filename;
    DWORD _Attributes;
};

// FUNCTION _Advance_directory_iterator
extern _NODISCARD bool _Advance_directory_iterator(
    void* const _Handle, _Filename_and_attributes& _Data) noexcept;

// STRUCT _Dir_iter
struct _Dir_iter {
    void* _Handle = nullptr; // handle to the directory
    directory_entry _Entry; // current entry
    _Filename_and_attributes _Data; // current entry basic data
    path _Path; // path to the directory

    _Dir_iter()                 = default;
    _Dir_iter(const _Dir_iter&) = default;
    _Dir_iter(_Dir_iter&&)      = default;
    
    _Dir_iter(const path& _Target);
    ~_Dir_iter() noexcept;

    // tries to advance the iterator
    _NODISCARD bool _Advance();

    // assigns a new entry
    void _Assign_entry(const wstring& _Filename);
};

// CLASS directory_iterator
class _SDSDLL_API directory_iterator {
public:
    using value_type      = directory_entry;
    using difference_type = ptrdiff_t;
    using pointer         = const directory_entry*;
    using reference       = const directory_entry&;

    directory_iterator(const directory_iterator&) = default;
    directory_iterator(directory_iterator&&)      = default;
    ~directory_iterator()                         = default;
    
    directory_iterator() noexcept;
    explicit directory_iterator(const path& _Target);

    directory_iterator& operator=(const directory_iterator&) = default;
    directory_iterator& operator=(directory_iterator&&)      = default;

    // returns the current entry as an non-mutable reference
    const directory_entry& operator*() const;

    // returns the current entry as an non-mutable pointer
    const directory_entry* operator->() const;

    // advances the iterator to the next entry
    _NODISCARD bool operator++();
    _NODISCARD bool increment();

private:
    _Dir_iter _Myimpl;
};
_SDSDLL_END

#endif // _SDSDLL_PREPROCESSOR_GUARD
#endif // _SDSDLL_FILESYSTEM_DIRECTORY_HPP_