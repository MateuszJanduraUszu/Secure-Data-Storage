// tpm.hpp

// Copyright (c) Mateusz Jandura. All rights reserved.
// SPDX-License-Identifier: Apache-2.0

#pragma once
#ifndef _SDSDLL_HARDWARE_TPM_HPP_
#define _SDSDLL_HARDWARE_TPM_HPP_
#include <core/defs.hpp>
#if _SDSDLL_PREPROCESSOR_GUARD
#include <core/api.hpp>
#include <core/defs.hpp>
#include <core/traits/type_traits.hpp>
#include <cstdint>
#include <encoding/any.hpp>
#include <string>
#include <tbs.h>
#include <type_traits>

// STD types
using _STD basic_string;

_SDSDLL_BEGIN
// FUNCTION _Get_tpm_info
extern _NODISCARD TPM_DEVICE_INFO _Get_tpm_info() noexcept;

// CLASS TEMPLATE tpm
template <class _Elem>
class tpm;

// CLASS TEMPLATE tpm_interface_type
template <class _Elem>
class _SDSDLL_API tpm_interface_type {
private:
    static_assert(is_any_of_v<_Elem, char, wchar_t>, "Requires a UTF-8/Unicode element type.");

public:
    using char_type   = _Elem;
    using string_type = basic_string<_Elem>;

    enum type : unsigned char {
        none,
        mmio,
        trustzone,
        hardware,
        emulator,
        spb
    };

    constexpr tpm_interface_type() noexcept;
    constexpr tpm_interface_type(const tpm_interface_type& _Other) noexcept;
    constexpr tpm_interface_type(tpm_interface_type&& _Other) noexcept;
    _CONSTEXPR20 ~tpm_interface_type() noexcept;

    constexpr tpm_interface_type& operator=(const tpm_interface_type& _Other) noexcept;
    constexpr tpm_interface_type& operator=(tpm_interface_type&& _Other) noexcept;

    _NODISCARD constexpr operator const type() const noexcept;

    // returns the current type
    _NODISCARD constexpr const type get() const noexcept;

    // returns the current type as a string
    _NODISCARD constexpr string_type to_string() const noexcept;

private:
    friend tpm<_Elem>;

    explicit constexpr tpm_interface_type(const type _Type) noexcept;

    type _Myval;
};

// CLASS TEMPLATE tpm_version
template <class _Elem>
class _SDSDLL_API tpm_version {
private:
    static_assert(is_any_of_v<_Elem, char, wchar_t>, "Requires a UTF-8/Unicode element type.");

public:
    using char_type   = _Elem;
    using string_type = basic_string<_Elem>;

    enum version : unsigned char {
        none,
        older, // TPM 1.2
        newest // TPM 2.0
    };

    constexpr tpm_version() noexcept;
    constexpr tpm_version(const tpm_version& _Other) noexcept;
    constexpr tpm_version(tpm_version&& _Other) noexcept;
    _CONSTEXPR20 ~tpm_version() noexcept;

    constexpr tpm_version& operator=(const tpm_version& _Other) noexcept;
    constexpr tpm_version& operator=(tpm_version&& _Other) noexcept;

    _NODISCARD constexpr operator const version() const noexcept;

    // returns the current version
    _NODISCARD constexpr const version get() const noexcept;

    // returns the current version as a string
    _NODISCARD constexpr string_type to_string() const noexcept;

private:
    friend tpm<_Elem>;

    explicit constexpr tpm_version(const version _Ver) noexcept;

    version _Myval;
};

// CLASS TEMPLATE tpm
template <class _Elem>
class _SDSDLL_API tpm { // informations about the TPM (Trusted Platform Module)
public:
    using char_type      = _Elem;
    using interface_type = tpm_interface_type<_Elem>;
    using version        = tpm_version<_Elem>;

    tpm() = delete;
    tpm(const tpm&) = delete;
    tpm& operator=(const tpm&) = delete;

    // checks if the TPM is available
    _NODISCARD static constexpr const bool available() noexcept;

    // returns the type of the TPM interface
    _NODISCARD static constexpr const interface_type& get_interface() noexcept;

    // returns the TPM version
    _NODISCARD static constexpr const version& get_version() noexcept;

private:
    struct _Cached_data {
        bool _Available; // true if the TPM is available
        interface_type _Interface_type; // TPM interface type
        version _Version; // TPM version (1.2 or 2.0)

        constexpr _Cached_data() noexcept;
    };

    static _Cached_data _Mydata;
};
_SDSDLL_END

#endif // _SDSDLL_PREPROCESSOR_GUARD
#endif // _SDSDLL_HARDWARE_TPM_HPP_