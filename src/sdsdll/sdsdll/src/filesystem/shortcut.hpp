// shortcut.hpp

// Copyright (c) Mateusz Jandura. All rights reserved.
// SPDX-License-Identifier: Apache-2.0

#pragma once
#ifndef _SDSDLL_FILESYSTEM_SHORTCUT_HPP_
#define _SDSDLL_FILESYSTEM_SHORTCUT_HPP_
#include <core/defs.hpp>
#if _SDSDLL_PREPROCESSOR_GUARD
#include <combaseapi.h>
#include <core/api.hpp>
#include <core/optimization/sbo.hpp>
#include <core/optimization/string_view.hpp>
#include <core/traits/type_traits.hpp>
#include <filesystem/file.hpp>
#include <filesystem/path.hpp>
#include <objbase.h>
#include <objidl.h>
#include <ShObjIdl_core.h>
#include <string>
#include <system/handle/com.hpp>

// STD types
using _STD wstring;

_SDSDLL_BEGIN
// STRUCT shortcut_icon
struct shortcut_icon {
    path path;
    int index;
};

// STRUCT _Shortcut_handle
struct _Shortcut_handle {
    path _Path;
    IShellLinkW* _Link;
    IPersistFile* _File;

    explicit _Shortcut_handle(const path& _Target);
    ~_Shortcut_handle() noexcept;

    _Shortcut_handle() = delete;
    _Shortcut_handle(const _Shortcut_handle&) = delete;
    _Shortcut_handle& operator=(const _Shortcut_handle&) = delete;

    // checks if all COM instances are valid
    _NODISCARD bool _Valid() const noexcept;

    // tries to save all changes
    _NODISCARD bool _Submit() noexcept;
};

// FUNCTION create_shortcut
_SDSDLL_API _NODISCARD bool create_shortcut(const path& _Target, const path& _Shortcut,
    const wchar_t* const _Description, const shortcut_icon& _Icon, const path& _Directory);
_SDSDLL_API _NODISCARD bool create_shortcut(const path& _Target, const path& _Shortcut,
    const wstring_view _Description, const shortcut_icon& _Icon, const path& _Directory);
_SDSDLL_API _NODISCARD bool create_shortcut(const path& _Target, const path& _Shortcut,
    const wstring& _Description, const shortcut_icon& _Icon, const path& _Directory);

// FUNCTION read_shortcut
_SDSDLL_API _NODISCARD path read_shortcut(const path& _Target);

// FUNCTION remove_shortcut
_SDSDLL_API _NODISCARD bool remove_shortcut(const path& _Target);

// FUNCTION change_shortcut_description
_SDSDLL_API _NODISCARD bool change_shortcut_description(
    const path& _Target, const wchar_t* const _New_description);
_SDSDLL_API _NODISCARD bool change_shortcut_description(
    const path& _Target, const wstring_view _New_description);
_SDSDLL_API _NODISCARD bool change_shortcut_description(
    const path& _Target, const wstring& _New_description);

// FUNCTION change_shortcut_icon
_SDSDLL_API _NODISCARD bool change_shortcut_icon(const path& _Target, const shortcut_icon& _New_icon);

// FUNCTION change_shortcut_target
_SDSDLL_API _NODISCARD bool change_shortcut_target(const path& _Target, const path& _New_target);

// FUNCTION change_shortcut_working_directory
_SDSDLL_API _NODISCARD bool change_shortcut_working_directory(
    const path& _Target, const path& _New_directory);
_SDSDLL_END

#endif // _SDSDLL_PREPROCESSOR_GUARD
#endif // _SDSDLL_FILESYSTEM_SHORTCUT_HPP_