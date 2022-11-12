// status.cpp

// Copyright (c) Mateusz Jandura. All rights reserved.
// SPDX-License-Identifier: Apache-2.0

#include <build/sdsdll_pch.hpp>
#include <filesystem/status.hpp>
#if _SDSDLL_PREPROCESSOR_GUARD

_SDSDLL_BEGIN
// FUNCTION _Open_file_handle
_NODISCARD void* _Open_file_handle(const path& _Target, const file_access _Access, const file_share _Share,
    const file_disposition _Disp, const file_attributes _Attrs, const file_flags _Flags) noexcept {
    return ::CreateFileW(_Target.c_str(), static_cast<DWORD>(_Access),
        static_cast<DWORD>(_Share), nullptr, static_cast<DWORD>(_Disp),
            static_cast<DWORD>(_Attrs) | static_cast<DWORD>(_Flags), nullptr);
}

// FUNCTION file_status constructors
file_status::file_status() : _Mypath(), _Myattrs(file_attributes::unknown),
    _Myperms(file_perms::unknown), _Mytype(file_type::not_found) {}

file_status::file_status(const path& _Target) : _Mypath(_Target) {
    _Refresh();
}

file_status::file_status(const path& _Target, const file_attributes _Attrs, const file_perms _Perms,
    const file_type _Type) : _Mypath(_Target), _Myattrs(_Attrs), _Myperms(_Perms), _Mytype(_Type) {}

// FUNCTION file_status::_Refresh
void file_status::_Refresh() {
    _Myattrs = file_attributes{::GetFileAttributesW(_Mypath.c_str())};

    // Note: The GetFileAttributesW() will return INVALID_FILE_ATTRIBUTES (file_attributes::unknown)
    //       if the target not found. Don't use GetLastError() to check if the target exists,
    //       because some functions will return ERROR_PATH_NOT_FOUND and other ERROR_FILE_NOT_FOUND
    //       when they cannot find the target. By the way, ERROR_PATH_NOT_FOUND and ERROR_FILE_NOT_FOUND
    //       does not always mean an error.
    if (_Myattrs == file_attributes::unknown) { // target not found
        _Myattrs = file_attributes::none;
        _Myperms = file_perms::none;
        _Mytype  = file_type::not_found;
        return;
    }

    if (_Has_bits(_Myattrs, file_attributes::readonly)) { // read-only permissions
        _Myperms = file_perms::readonly;
    } else { // all permissions
        _Myperms = file_perms::all;
    }

    if (_Has_bits(_Myattrs, file_attributes::reparse_point)) {
        static constexpr size_t _Buf_size = 16 * 1024 / sizeof(wchar_t); // non-bytes size
        _Sbo_buffer<wchar_t> _Buf(_Buf_size);
        if (_Buf._Empty()) { // allocation failed
            _Reset();
            return;
        }

        wchar_t* _Raw_buf              = _Buf._Get();
        _Reparse_data_buffer& _Rep_buf = reinterpret_cast<_Reparse_data_buffer&>(_Raw_buf);
        DWORD _Bytes; // returned bytes
        {
            generic_handle_wrapper _Handle = _Open_file_handle(_Mypath, file_access::all,
                file_share::read, file_disposition::only_if_exists, file_attributes::none,
                    file_flags::backup_semantics | file_flags::open_reparse_point);
            if (!_Handle) { // failed to get a handle
                _Reset();
                return;
            }

            if (::DeviceIoControl(_Handle, FSCTL_GET_REPARSE_POINT, nullptr, 0, _SDSDLL addressof(_Rep_buf),
                static_cast<DWORD>(_Buf._Size() * sizeof(wchar_t)), &_Bytes, nullptr) == 0) {
                _Reset();
                return;
            }
        }

        switch (static_cast<file_reparse_tag>(_Rep_buf._Reparse_tag)) {
        case file_reparse_tag::mount_point:
            _Mytype = file_type::junction;
            return;
        case file_reparse_tag::symlink:
            _Mytype = file_type::symlink;
            return;
        default:
            break;
        }
    }

    if (_Has_bits(_Myattrs, file_attributes::directory)) {
        _Mytype = file_type::directory;
    } else {
        _Mytype = file_type::regular;
    }
}

// FUNCTION file_status::_Reset
void file_status::_Reset() noexcept {
    _Myattrs = file_attributes::none;
    _Myperms = file_perms::none;
    _Mytype  = file_type::none;
}

// FUNCTION file_status::attributes
_NODISCARD const file_attributes file_status::attributes() const noexcept {
    return _Myattrs;
}

// FUNCTION file_status::perms
_NODISCARD const file_perms file_status::perms() const noexcept {
    return _Myperms;
}

// FUNCTION file_status::refresh
void file_status::refresh(const path& _New_target) {
    _Mypath.assign(_New_target);
    _Refresh();
}

// FUNCTION file_status::type
_NODISCARD const file_type file_status::type() const noexcept {
    return _Mytype;
}

// FUNCTION change_attributes
_NODISCARD bool change_attributes(
    const path& _Target, const file_attributes _New_attrs, const bool _Mask) {
    file_attributes _Old_attrs;
    {
        const file_status _Status(_Target);
        if (!_SDSDLL exists(_Status)) { // target not found
            return false;
        }

        _Old_attrs = _Status.attributes();
        if (_New_attrs == _Old_attrs) { // nothing has changed
            return true;
        }
    }

    if (_Mask) { // make mask from an old and a new attributes
        return ::SetFileAttributesW(
            _Target.c_str(), static_cast<DWORD>(_Old_attrs ^ _New_attrs)) != 0;
    } else { // discard an old attributes
        return ::SetFileAttributesW(_Target.c_str(), static_cast<DWORD>(_New_attrs)) != 0;
    }
}

// FUNCTION change_perms
_NODISCARD bool change_perms(const path& _Target, const file_perms _New_perms) {
    file_attributes _Old_attrs;
    {
        const file_status _Status(_Target);
        if (!_SDSDLL exists(_Status)) { // target not found
            return false;
        }

        if (_New_perms == _Status.perms()) { // nothing has changed
            return true;
        }

        _Old_attrs = _Status.attributes();
    }

    if (_New_perms == file_perms::readonly) { // permissions for reading
        if (!_Has_bits(_Old_attrs, file_attributes::readonly)) { // include read-only attribute
            _Old_attrs |= file_attributes::readonly;
        }
    } else { // all permissions
        if (_Has_bits(_Old_attrs, file_attributes::readonly)) { // exclude read-only attribute
            _Old_attrs &= ~file_attributes::readonly;
        }
    }

    return _SDSDLL change_attributes(_Target, _Old_attrs, false);
}

// FUNCTION exists
_NODISCARD bool exists(const file_status _Status) noexcept {
    switch (_Status.type()) {
    case file_type::none:
    case file_type::not_found:
        return false;
    default:
        return true;
    }
}

_NODISCARD bool exists(const path& _Target) {
    return _SDSDLL exists(file_status(_Target));
}

// FUNCTION is_directory
_NODISCARD bool is_directory(const file_status _Status) noexcept {
    return _Status.type() == file_type::directory;
}

_NODISCARD bool is_directory(const path& _Target) {
    return _SDSDLL is_directory(file_status(_Target));
}

_NODISCARD bool is_directory(const file_status _Status, any_directory_type_t) noexcept {
    return _Has_bits(_Status.attributes(), file_attributes::directory);
}

_NODISCARD bool is_directory(const path& _Target, any_directory_type_t) {
    return _SDSDLL is_directory(file_status(_Target), any_directory_type);
}

// FUNCTION is_junction
_NODISCARD bool is_junction(const file_status _Status) noexcept {
    return _Status.type() == file_type::junction;
}

_NODISCARD bool is_junction(const path& _Target) {
    return _SDSDLL is_junction(file_status(_Target));
}

// FUNCTION is_other
_NODISCARD bool is_other(const file_status _Status) noexcept {
    switch (_Status.type()) {
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

_NODISCARD bool is_other(const path& _Target) {
    return _SDSDLL is_other(file_status(_Target));
}

// FUNCTION is_regular_file
_NODISCARD bool is_regular_file(const file_status _Status) noexcept {
    return _Status.type() == file_type::regular;
}

_NODISCARD bool is_regular_file(const path& _Target) {
    return _SDSDLL is_regular_file(file_status(_Target));
}

// FUNCTION is_symlink
_NODISCARD bool is_symlink(const file_status _Status) noexcept {
    return _Status.type() == file_type::symlink;
}

_NODISCARD bool is_symlink(const path& _Target) {
    return _SDSDLL is_symlink(file_status(_Target));
}
_SDSDLL_END

#endif // _SDSDLL_PREPROCESSOR_GUARD