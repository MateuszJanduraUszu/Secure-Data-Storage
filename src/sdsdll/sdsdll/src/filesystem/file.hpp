// file.hpp

// Copyright (c) Mateusz Jandura. All rights reserved.
// SPDX-License-Identifier: Apache-2.0

#pragma once
#ifndef _SDSDLL_FILESYSTEM_FILE_HPP_
#define _SDSDLL_FILESYSTEM_FILE_HPP_
#include <core/defs.hpp>
#if _SDSDLL_PREPROCESSOR_GUARD
#include <core/api.hpp>
#include <core/optimization/string_view.hpp>
#include <core/traits/string_traits.hpp>
#include <core/traits/type_traits.hpp>
#include <cstdint>
#include <cwchar>
#include <encoding/utf8.hpp>
#include <encoding/utf16.hpp>
#include <fileapi.h>
#include <filesystem/path.hpp>
#include <filesystem/status.hpp>
#include <minwinbase.h>
#include <string>
#include <system/handle/generic_handle.hpp>
#include <utility>
#include <WinBase.h>

// STD types
using _STD string;
using _STD wstring;

_SDSDLL_BEGIN
// FUNCTION _Clear_file
extern _NODISCARD bool _Clear_file(void* const _Handle) noexcept;

// FUNCTION _File_size
extern _NODISCARD bool _File_size(void* const _Handle, uintmax_t& _Size) noexcept;

// FUNCTION _Resize_file
extern _NODISCARD bool _Resize_file(void* const _Handle, const uintmax_t _New_size) noexcept;

// FUNCTION _Read_file
extern _NODISCARD bool _Read_file(
    void* const _Handle, void* const _Buf, const size_t _Count, size_t* const _Read = nullptr) noexcept;

// FUNCTION _Write_file
extern _NODISCARD bool _Write_file(
    void* const _Handle, const void* const _Bytes, const size_t _Count) noexcept;

// FUNCTION clear_file
_SDSDLL_API _NODISCARD bool clear_file(const path& _Target);

// FUNCTION create_file
_SDSDLL_API _NODISCARD bool create_file(
    const path& _Path, file_attributes _Attrs = file_attributes::normal,
    const file_flags _Flags = file_flags::none, const file_perms _Perms = file_perms::all);

// FUNCTION delete_file
_SDSDLL_API _NODISCARD bool delete_file(const path& _Target);

// FUNCTION file_size
_SDSDLL_API _NODISCARD unequivocal_result<uintmax_t> file_size(const path& _Target);

// FUNCTION is_empty_file
_SDSDLL_API _NODISCARD unequivocal_result<bool> is_empty_file(const path& _Target);

// FUNCTION resize_file
_SDSDLL_API _NODISCARD bool resize_file(const path& _Target, const uintmax_t _New_size);

// CLASS _Filepos
class _Filepos {
public:
    _Filepos() noexcept;
    _Filepos(void* const _Handle) noexcept;
    ~_Filepos() noexcept;

    _Filepos(const _Filepos&) = delete;
    _Filepos& operator=(const _Filepos&) = delete;

    // goes 1 byte forward
    _Filepos& operator++() noexcept;

    // goes _Off bytes forward
    _Filepos& operator+=(uintmax_t _Off) noexcept;

    // goes back 1 byte
    _Filepos& operator--() noexcept;

    // goes back _Off bytes
    _Filepos& operator-=(uintmax_t _Off) noexcept;
    
    // returns the current position
    _NODISCARD uintmax_t operator*() const noexcept;

    // changes the current handle
    void _Change_handle(void* const _New_handle) noexcept;

    // checks the current file size
    _NODISCARD bool _Check_file_size() noexcept;

    // changes the current file size
    void _Set_file_size(const uintmax_t _Size) noexcept;

    // returns the current file size
    _NODISCARD const uintmax_t _Get_file_size() const noexcept;

    // checks if the file has reached EOF
    _NODISCARD bool _Reached_eof() const noexcept;

    // tries to set a new position
    _NODISCARD bool _Set_pos(const uintmax_t _Off, const bool _Submit = true) noexcept;

private:
    // changes the current position
    void _Submit_changes() const noexcept;
    
    LARGE_INTEGER _Myval; // current position
    void* _Myhandle; // handle to the file
    uintmax_t _Mysize; // size of the file
};

// CLASS file
class _SDSDLL_API file { // manages a binary file
public:
    using char_type = char;
    using int_type  = int;
    using pos_type  = uintmax_t;
    using off_type  = intmax_t;
    using size_type = size_t;

    file() noexcept;
    ~file() noexcept;

    file(const file&) = delete;
    file& operator=(const file&) = delete;

    explicit file(const path& _Target,
        const file_access _Access = file_access::all, const file_share _Share = file_share::none,
        const file_disposition _Disp = file_disposition::only_if_exists,
        const file_attributes _Attrs = file_attributes::normal,
        const file_flags _Flags = file_flags::none);

    enum seekdir {
        beg,
        end,
        cur
    };

    // tries to open a new file
    _NODISCARD bool open(const path& _Target,
        const file_access _Access = file_access::all, const file_share _Share = file_share::none,
        const file_disposition _Disp = file_disposition::only_if_exists,
        const file_attributes _Attrs = file_attributes::normal,
        const file_flags _Flags = file_flags::none) noexcept;

    // checks if any file is open
    _NODISCARD bool is_open() const noexcept;

    // closes the current file (if is open)
    void close() noexcept;

    // changes the current position
    _NODISCARD bool seek(const pos_type _New_pos) noexcept;
    _NODISCARD bool seek(const off_type _Off, const seekdir _Seekdir) noexcept;

    // returns the current position
    _NODISCARD pos_type tell() const noexcept;

    // checks if EOF has been reached
    _NODISCARD bool eof() const noexcept;

    // tries to clear the file
    _NODISCARD bool clear() noexcept;

    // tries to resize the file
    _NODISCARD bool resize(const uintmax_t _New_size) noexcept;

    // tries to read exactly 1 byte from the file
    _NODISCARD bool get(char& _Buf) noexcept;

    // tries to read exactly 1 raw byte from the file
    _NODISCARD bool get(uint8_t& _Buf) noexcept;

    // tries to write exactly 1 byte to the file
    _NODISCARD bool put(const char _Ch) noexcept;

    // tries to write exactly 1 raw byte to the file
    _NODISCARD bool put(const uint8_t _Ch) noexcept;

    // tries to read _Count bytes from the current file and store it in the _Buf
    _NODISCARD bool read(char* const _Buf, const size_type _Buf_size,
        size_type _Count, size_type* const _Read = nullptr) noexcept;
    _NODISCARD bool read(string& _Buf, size_type _Count, size_type* const _Read = nullptr) noexcept;

    // tries to read _Count raw bytes from the current file and store it in the _Buf
    _NODISCARD bool read(uint8_t* const _Buf, const size_type _Buf_size,
        size_type _Count, size_type* const _Read = nullptr) noexcept;
    _NODISCARD bool read(byte_string& _Buf, size_type _Count, size_type* const _Read = nullptr) noexcept;

    // tries to write _Count bytes from _Data to the file
    _NODISCARD bool write(const char* const _Data, const size_type _Count) noexcept;
    _NODISCARD bool write(const string_view _Data) noexcept;
    _NODISCARD bool write(const string& _Data);

    // tries to write _Count raw bytes from _Data to the file
    _NODISCARD bool write(const uint8_t* const _Data, const size_type _Count) noexcept;
    _NODISCARD bool write(const byte_string_view _Data) noexcept;
    _NODISCARD bool write(const byte_string& _Data);

private:
    // goes forward _Off bytes
    void _Go_forward(const size_type _Count) noexcept;

#ifdef _MSC_VER
#pragma warning(push, 1)
#pragma warning(disable : 4251) // C4251: generic_handle_wrapper and _Filepos require dll-interface
#endif // _MSC_VER
    generic_handle_wrapper _Myhandle;
    _Filepos _Mypos;
#ifdef _MSC_VER
#pragma warning(pop)
#endif // _MSC_VER
};

// FUNCTION read_utf16_string_from_file
_SDSDLL_API _NODISCARD bool read_utf16_string_from_file(
    file& _File, wchar_t* const _Buf, const size_t _Buf_size, size_t _Count) noexcept;
_SDSDLL_API _NODISCARD bool read_utf16_string_from_file(
    file& _File, wstring& _Buf, size_t _Count) noexcept;

// FUNCTION write_utf16_string_to_file
_SDSDLL_API _NODISCARD bool write_utf16_string_to_file(
    file& _File, const wchar_t* const _Data, const size_t _Count) noexcept;
_SDSDLL_API _NODISCARD bool write_utf16_string_to_file(file& _File, const wstring_view _Data) noexcept;
_SDSDLL_API _NODISCARD bool write_utf16_string_to_file(file& _File, const wstring& _Data);
_SDSDLL_END

#endif // _SDSDLL_PREPROCESSOR_GUARD
#endif // _SDSDLL_FILESYSTEM_FILE_HPP_