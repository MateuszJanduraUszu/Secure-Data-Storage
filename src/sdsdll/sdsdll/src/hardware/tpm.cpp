// tpm.cpp

// Copyright (c) Mateusz Jandura. All rights reserved.
// SPDX-License-Identifier: Apache-2.0

#include <build/sdsdll_pch.hpp>
#include <hardware/tpm.hpp>
#if _SDSDLL_PREPROCESSOR_GUARD

_SDSDLL_BEGIN
// FUNCTION _Get_tpm_info
_NODISCARD TPM_DEVICE_INFO _Get_tpm_info() noexcept {
    TPM_DEVICE_INFO _Result;
    ::Tbsi_GetDeviceInfo(sizeof(TPM_DEVICE_INFO), _SDSDLL addressof(_Result));
    return _Result;
}

// FUNCTION TEMPLATE tpm_interface_type constructors/destructor
template <class _Elem>
constexpr tpm_interface_type<_Elem>::tpm_interface_type() noexcept : _Myval(none) {}

template <class _Elem>
constexpr tpm_interface_type<_Elem>::tpm_interface_type(
    const tpm_interface_type& _Other) noexcept : _Myval(_Other._Myval) {}

template <class _Elem>
constexpr tpm_interface_type<_Elem>::tpm_interface_type(
    tpm_interface_type&& _Other) noexcept : _Myval(_Other._Myval) {}

template <class _Elem>
constexpr tpm_interface_type<_Elem>::tpm_interface_type(
    const type _Type) noexcept : _Myval(_Type) {}

template <class _Elem>
_CONSTEXPR20 tpm_interface_type<_Elem>::~tpm_interface_type() noexcept {}

// FUNCTION TEMPLATE tpm_interface_type::operator=
template <class _Elem>
constexpr tpm_interface_type<_Elem>& tpm_interface_type<_Elem>::operator=(
    const tpm_interface_type& _Other) noexcept {
    if (this != _SDSDLL addressof(_Other)) {
        _Myval = _Other._Myval;
    }

    return *this;
}

template <class _Elem>
constexpr tpm_interface_type<_Elem>& tpm_interface_type<_Elem>::operator=(
    tpm_interface_type&& _Other) noexcept {
    if (this != _SDSDLL addressof(_Other)) {
        _Myval = _Other._Myval;
    }

    return *this;
}

// FUNCTION TEMPLATE tpm_interface_type::operator type
template <class _Elem>
_NODISCARD constexpr tpm_interface_type<_Elem>::operator const type() const noexcept {
    return _Myval;
}

// FUNCTION TEMPLATE tpm_interface_type::get
template <class _Elem>
_NODISCARD constexpr const typename tpm_interface_type<_Elem>::type
    tpm_interface_type<_Elem>::get() const noexcept {
    return _Myval;
}

// FUNCTION TEMPLATE tpm_interface_type::to_string
template <class _Elem>
_NODISCARD constexpr typename tpm_interface_type<_Elem>::string_type
    tpm_interface_type<_Elem>::to_string() const noexcept {
    switch (_Myval) {
    case mmio:
        return _SDSDLL auto_string<_Elem>("MMIO", 4);
    case trustzone:
        return _SDSDLL auto_string<_Elem>("Trustzone", 9);
    case hardware:
        return _SDSDLL auto_string<_Elem>("Hardware", 8);
    case emulator:
        return _SDSDLL auto_string<_Elem>("Emulator", 8);
    case spb:
        return _SDSDLL auto_string<_Elem>("SPB", 3);
    default:
        return string_type{};
    }
}

template class _SDSDLL_API tpm_interface_type<char>;
template class _SDSDLL_API tpm_interface_type<wchar_t>;

// FUNCTION TEMPLATE tpm_version constructors/destructor
template <class _Elem>
constexpr tpm_version<_Elem>::tpm_version() noexcept : _Myval(none) {}

template <class _Elem>
constexpr tpm_version<_Elem>::tpm_version(
    const tpm_version& _Other) noexcept : _Myval(_Other._Myval) {}

template <class _Elem>
constexpr tpm_version<_Elem>::tpm_version(
    tpm_version&& _Other) noexcept : _Myval(_Other._Myval) {}

template <class _Elem>
constexpr tpm_version<_Elem>::tpm_version(const version _Ver) noexcept : _Myval(_Ver) {}

template <class _Elem>
_CONSTEXPR20 tpm_version<_Elem>::~tpm_version() noexcept {}

// FUNCTION TEMPLATE tpm_version::operator=
template <class _Elem>
constexpr tpm_version<_Elem>& tpm_version<_Elem>::operator=(const tpm_version& _Other) noexcept {
    if (this != _SDSDLL addressof(_Other)) {
        _Myval = _Other._Myval;
    }

    return *this;
}

template <class _Elem>
constexpr tpm_version<_Elem>& tpm_version<_Elem>::operator=(tpm_version&& _Other) noexcept {
    if (this != _SDSDLL addressof(_Other)) {
        _Myval = _Other._Myval;
    }

    return *this;
}

// FUNCTION TEMPLATE tpm_version::operator version
template <class _Elem>
_NODISCARD constexpr tpm_version<_Elem>::operator const version() const noexcept {
    return _Myval;
}

// FUNCTION TEMPLATE tpm_version::get
template <class _Elem>
_NODISCARD constexpr const typename tpm_version<_Elem>::version
    tpm_version<_Elem>::get() const noexcept {
    return _Myval;
}

// FUNCTION TEMPLATE tpm_version::to_string
template <class _Elem>
_NODISCARD constexpr typename tpm_version<_Elem>::string_type
    tpm_version<_Elem>::to_string() const noexcept {
    switch (_Myval) {
    case older:
        return _SDSDLL auto_string<_Elem>("1.2", 3);
    case newest:
        return _SDSDLL auto_string<_Elem>("2.0", 3);
    default:
        return string_type{};
    }
}

template class _SDSDLL_API tpm_version<char>;
template class _SDSDLL_API tpm_version<wchar_t>;

// VARIABLE TEMPLATE tpm::_Mydata
template <class _Elem>
typename tpm<_Elem>::_Cached_data tpm<_Elem>::_Mydata{};

// FUNCTION TEMPLATE tpm::_Cached_data constructor
template <class _Elem>
constexpr tpm<_Elem>::_Cached_data::_Cached_data() noexcept {
    using _Interface_t           = typename interface_type::type;
    using _Version_t             = typename version::version;
    const TPM_DEVICE_INFO& _Info = _Get_tpm_info();
    _Available                   = _Info.tpmInterfaceType != 0 && _Info.tpmVersion != 0;
    _Interface_type              = interface_type{static_cast<_Interface_t>(_Info.tpmInterfaceType)};
    _Version                     = version{static_cast<_Version_t>(_Info.tpmVersion)};
}

// FUNCTION TEMPLATE tpm::available
template <class _Elem>
_NODISCARD constexpr const bool tpm<_Elem>::available() noexcept {
    return _Mydata._Available;
}

// FUNCTION TEMPLATE tpm::get_interface
template <class _Elem>
_NODISCARD constexpr const typename tpm<_Elem>::interface_type& tpm<_Elem>::get_interface() noexcept {
    return _Mydata._Interface_type;
}

// FUNCTION TEMPLATE tpm::get_version
template <class _Elem>
_NODISCARD constexpr const typename tpm<_Elem>::version& tpm<_Elem>::get_version() noexcept {
    return _Mydata._Version;
}

template class _SDSDLL_API tpm<char>;
template class _SDSDLL_API tpm<wchar_t>;
_SDSDLL_END

#endif // _SDSDLL_PREPROCESSOR_GUARD