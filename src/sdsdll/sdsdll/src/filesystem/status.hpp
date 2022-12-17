// status.hpp

// Copyright (c) Mateusz Jandura. All rights reserved.
// SPDX-License-Identifier: Apache-2.0

#pragma once
#ifndef _SDSDLL_FILESYSTEM_STATUS_HPP_
#define _SDSDLL_FILESYSTEM_STATUS_HPP_
#include <core/defs.hpp>
#if _SDSDLL_PREPROCESSOR_GUARD
#include <core/api.hpp>
#include <core/optimization/sbo.hpp>
#include <core/traits/type_traits.hpp>
#include <cstddef>
#include <cstdint>
#include <fileapi.h>
#include <filesystem/path.hpp>
#include <ioapiset.h>
#include <minwindef.h>
#include <system/handle/generic_handle.hpp>
#include <winioctl.h>

_SDSDLL_BEGIN
// ENUM CLASS file_access
enum class file_access : unsigned long {
    none  = 0,
    read  = 0x8000'0000, // GENERIC_READ
    write = 0x4000'0000, // GENERIC_WRITE
    all   = 0x1000'0000 // GENERIC_ALL
};

_BIT_OPS(file_access)

// ENUM CLASS file_attributes
enum class file_attributes : unsigned long {
    none          = 0,
    readonly      = 0x0001, // FILE_ATTRIBUTE_READONLY
    hidden        = 0x0002, // FILE_ATTRIBUTE_HIDDEN
    system        = 0x0004, // FILE_ATTRIBUTE_SYSTEM
    directory     = 0x0010, // FILE_ATTRIBUTE_DIRECTORY
    archive       = 0x0020, // FILE_ATTRIBUTE_ARCHIVE
    normal        = 0x0080, // FILE_ATTRIBUTE_NORMAL
    reparse_point = 0x0400, // FILE_ATTRIBUTE_REPARSE_POINT
    compressed    = 0x0800, // FILE_ATTRIBUTE_COMPRESSED
    encrypted     = 0x4000, // FILE_ATTRIBUTE_ENCRYPTED
    unknown       = static_cast<unsigned long>(-1) // INVALID_FILE_ATTRIBUTES
};

_BIT_OPS(file_attributes)

// ENUM CLASS file_disposition
enum class file_disposition : unsigned long {
    none           = 0,
    only_new       = 1, // CREATE_NEW
    force_create   = 2, // CREATE_ALWAYS
    only_if_exists = 3, // OPEN_EXISTING
    force_open     = 4 // OPEN_ALWAYS
};

// ENUM CLASS file_flags
enum class file_flags : unsigned long {
    none                = 0,
    first_pipe_instance = 0x0008'0000, // FILE_FLAG_FIRST_PIPE_INSTANCE
    open_no_recall      = 0x0010'0000, // FILE_FLAG_OPEN_NO_RECALL
    open_reparse_point  = 0x0020'0000, // FILE_FLAG_OPEN_REPARSE_POINT
    session_aware       = 0x0080'0000, // FILE_FLAG_SESSION_AWARE
    posix_semantics     = 0x0100'0000, // FILE_FLAG_POSIX_SEMANTICS
    backup_semantics    = 0x0200'0000, // FILE_FLAG_BACKUP_SEMANTICS
    delete_on_close     = 0x0400'0000, // FILE_FLAG_DELETE_ON_CLOSE
    sequential_scan     = 0x0800'0000, // FILE_FLAG_SEQUENTIAL_SCAN
    random_access       = 0x1000'0000, // FILE_FLAG_RANDOM_ACCESS
    no_buffering        = 0x2000'0000, // FILE_FLAG_NO_BUFFERING
    overlapped          = 0x4000'0000, // FILE_FLAG_OVERLAPPED
    write_through       = 0x8000'0000 // FILE_FLAG_WRITE_THROUGH
};

_BIT_OPS(file_flags)

// ENUM CLASS file_perms
enum class file_perms : unsigned long {
    none,
    readonly,
    all,
    unknown
};

// ENUM CLASS file_reparse_tag
enum class file_reparse_tag : unsigned long {
    none        = 0,
    mount_point = 0xA000'0003, // IO_REPARSE_TAG_MOUNT_POINT
    symlink     = 0xA000'000C // IO_REPARSE_TAG_SYMLINK
};

// ENUM CLASS file_share
enum class file_share : unsigned long {
    none   = 0,
    read   = 0x0001, // FILE_SHARE_READ
    write  = 0x0002, // FILE_SHARE_WRITE
    remove = 0x0004, // FILE_SHARE_DELETE
    all    = read | write | remove
};

_BIT_OPS(file_share)

// ENUM CLASS file_type
enum class file_type {
    none,
    not_found,
    directory,
    regular,

    // there could be also block, character, FIFO and socket,
    // but Windows doesn't support them
    symlink,
    junction
};

// ENUM CLASS symlink_flags
enum class symlink_flags : unsigned long {
    file               = 0,
    directory          = 0x1, // SYMBOLIC_LINK_DIRECTORY
    allow_unprivileged = 0x2 // SYMBOLIC_LINK_ALLOW_UNPRIVILEGED_CREATE
};

_BIT_OPS(symlink_flags)

// STRUCT _Reparse_data_buffer
struct _Reparse_data_buffer { // _REPARSE_DATA_BUFFER structure
    unsigned long _Reparse_tag;
    uint16_t _Reparse_data_length;
    uint16_t _Reserved;
    union {
        struct {
            uint16_t _Substitute_name_offset;
            uint16_t _Substitute_name_length;
            uint16_t _Print_name_offset;
            uint16_t _Print_name_length;
            unsigned long _Flags;
            wchar_t _Path_buffer[1];
        } _Symbolic_link_reparse_buffer;

        struct {
            uint16_t _Substitute_name_offset;
            uint16_t _Substitute_name_length;
            uint16_t _Print_name_offset;
            uint16_t _Print_name_length;
            wchar_t _Path_buffer[1];
        } _Mount_point_reparse_buffer;

        struct {
            unsigned char _Data_buffer[1];
        } _Generic_reparse_buffer;
    };
};

// STRUCT _Reparse_mount_point_data_buffer
struct _Reparse_mount_point_data_buffer { // _REPARSE_MOUNTPOINT_DATA_BUFFER structure
    unsigned long _Reparse_tag;
    unsigned long _Reparse_data_length;
    uint16_t _Reserved;
    uint16_t _Reparse_target_length;
    uint16_t _Reparse_target_maximum_length;
    uint16_t _Reserved1;
    wchar_t _Reparse_target[1];
};

// FUNCTION _Open_file_handle
extern _NODISCARD void* _Open_file_handle(const path& _Target, const file_access _Access,
    const file_share _Share, const file_disposition _Disp, const file_attributes _Attrs,
    const file_flags _Flags) noexcept;

// CLASS directory_filter
class directory_filter;

// CLASS file_status
class _SDSDLL_API file_status {
private:
    friend directory_filter;

public:
    file_status(const file_status&) = default;
    file_status(file_status&&)      = default;
    ~file_status()                  = default;

    file_status();
    explicit file_status(const path& _Target);
    explicit file_status(
        const path& _Target, const file_attributes _Attrs, const file_perms _Perms, const file_type _Type);

    file_status& operator=(const file_status&) = delete;
    file_status& operator=(file_status&&) = delete;

    // returns the current target attributes
    _NODISCARD const file_attributes attributes() const noexcept;

    // returns the current target permissions
    _NODISCARD const file_perms perms() const noexcept;

    // changes the current path and refreshes informations
    void refresh(const path& _New_target);

    // returns the current target type
    _NODISCARD const file_type type() const noexcept;

private:
    // assigns the latest informations about a file/directory
    void _Refresh();

    // assigns the default values
    void _Reset() noexcept;

    path _Mypath;
    file_attributes _Myattrs;
    file_perms _Myperms;
    file_type _Mytype;
};

// FUNCTION change_attributes
_SDSDLL_API _NODISCARD bool change_attributes(
    const path& _Target, const file_attributes _New_attrs, const bool _Mask = true);

// FUNCTION change_perms
_SDSDLL_API _NODISCARD bool change_perms(const path& _Target, const file_perms _New_perms);

// FUNCTION exists
_SDSDLL_API _NODISCARD bool exists(const file_status _Status) noexcept;
_SDSDLL_API _NODISCARD bool exists(const path& _Target);

// STRUCT any_directory_type_t
struct any_directory_type_t {
    any_directory_type_t() = default;
};

// CONSTANT any_directory_type
inline constexpr any_directory_type_t any_directory_type{};

// FUNCTION is_directory
_SDSDLL_API _NODISCARD bool is_directory(const file_status _Status) noexcept;
_SDSDLL_API _NODISCARD bool is_directory(const path& _Target);
_SDSDLL_API _NODISCARD bool is_directory(const file_status _Status, any_directory_type_t) noexcept;
_SDSDLL_API _NODISCARD bool is_directory(const path& _Target, any_directory_type_t);

// FUNCTION is_junction
_SDSDLL_API _NODISCARD bool is_junction(const file_status _Status) noexcept;
_SDSDLL_API _NODISCARD bool is_junction(const path& _Target);

// FUNCTION is_other
_SDSDLL_API _NODISCARD bool is_other(const file_status _Status) noexcept;
_SDSDLL_API _NODISCARD bool is_other(const path& _Target);

// FUNCTION is_regular_file
_SDSDLL_API _NODISCARD bool is_regular_file(const file_status _Status) noexcept;
_SDSDLL_API _NODISCARD bool is_regular_file(const path& _Target);

// FUNCTION is_symlink
_SDSDLL_API _NODISCARD bool is_symlink(const file_status _Status) noexcept;
_SDSDLL_API _NODISCARD bool is_symlink(const path& _Target);
_SDSDLL_END

#endif // _SDSDLL_PREPROCESSOR_GUARD
#endif // _SDSDLL_FILESYSTEM_STATUS_HPP_