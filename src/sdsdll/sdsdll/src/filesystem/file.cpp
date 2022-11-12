// file.cpp

// Copyright (c) Mateusz Jandura. All rights reserved.
// SPDX-License-Identifier: Apache-2.0

#include <build/sdsdll_pch.hpp>
#include <filesystem/file.hpp>
#if _SDSDLL_PREPROCESSOR_GUARD

_SDSDLL_BEGIN
// FUNCTION _Clear_file
_NODISCARD bool _Clear_file(void* const _Handle) noexcept {
    return _Resize_file(_Handle, 0);
}

// FUNCTION _File_size
_NODISCARD bool _File_size(void* const _Handle, uintmax_t& _Size) noexcept {
    FILE_STANDARD_INFO _Info = {0};
    if (::GetFileInformationByHandleEx(
        _Handle, FileStandardInfo, _SDSDLL addressof(_Info), sizeof(FILE_STANDARD_INFO)) == 0) {
        return false;
    }

    _Size = static_cast<uintmax_t>(_Info.EndOfFile.QuadPart);
    return true;
}

// FUNCTION _Resize_file
_NODISCARD bool _Resize_file(void* const _Handle, const uintmax_t _New_size) noexcept {
    FILE_END_OF_FILE_INFO _Info = {0};
    _Info.EndOfFile.QuadPart    = static_cast<intmax_t>(_New_size);
    return ::SetFileInformationByHandle(
        _Handle, FileEndOfFileInfo, _SDSDLL addressof(_Info), sizeof(FILE_END_OF_FILE_INFO)) != 0;
}

// FUNCTION _Read_file
_NODISCARD bool _Read_file(
    void* const _Handle, void* const _Buf, const size_t _Count, size_t* const _Read) noexcept {
    if (_Read) { // check how many bytes have been read
        return ::ReadFile(
            _Handle, _Buf, static_cast<DWORD>(_Count), reinterpret_cast<DWORD*>(_Read), nullptr) != 0;
    } else { // skip information about read bytes
        return ::ReadFile(_Handle, _Buf, static_cast<DWORD>(_Count), nullptr, nullptr) != 0;
    }
}

// FUNCTION _Write_file
_NODISCARD bool _Write_file(
    void* const _Handle, const void* const _Bytes, const size_t _Count) noexcept {
    DWORD _Written; // written bytes
    return ::WriteFile(_Handle, _Bytes, static_cast<DWORD>(_Count), &_Written, nullptr) == 0 ?
        false : static_cast<size_t>(_Written) == _Count;
}

// FUNCTION clear_file
_NODISCARD bool clear_file(const path& _Target) {
    { // check if the target is resizable
        const file_status _Status(_Target);
        if (!_SDSDLL exists(_Status) || _SDSDLL is_directory(_Status, any_directory_type)) {
            return false;
        }
    }

    generic_handle_wrapper _Handle = _Open_file_handle(_Target, file_access::write,
        file_share::none, file_disposition::only_if_exists, file_attributes::normal, file_flags::none);
    if (!_Handle) { // invalid handle
        return false;
    }

    return _Clear_file(_Handle);
}

// FUNCTION create_file
_NODISCARD bool create_file(
    const path& _Path, file_attributes _Attrs, const file_flags _Flags, const file_perms _Perms) {
    { // check if the file can be create
        const file_status _Status(_Path);
        if (_SDSDLL exists(_Status)) { // path already exists, but it may be any file type
            if (_SDSDLL is_regular_file(_Status)) { // file already exists
                return false;
            }
        }
    }

    if (_Perms == file_perms::readonly) { // mask must contains read-only attribute
        _Attrs ^= file_attributes::readonly;
    }

    generic_handle_wrapper _Handle = _Open_file_handle(_Path, file_access::all,
        file_share::none, file_disposition::only_new, _Attrs, _Flags);
    return !!_Handle; // negate twice to access the operator bool()
}

// FUNCTION delete_file
_NODISCARD bool delete_file(const path& _Target) {
    { // check if deletion is possible or necessary
        const file_status _Status(_Target);
        if (!_SDSDLL exists(_Status)) { // file not found, do nothing
            return true;
        }

        if (_SDSDLL is_directory(_Status, any_directory_type)) { // bad target
            return false;
        }
    }

    return ::DeleteFileW(_Target.c_str()) != 0;
}

// FUNCTION file_size
_NODISCARD unequivocal_result<uintmax_t> file_size(const path& _Target) {
    { // check if it is possible to get the file size
        const file_status _Status(_Target);
        if (!_SDSDLL exists(_Status) || _SDSDLL is_directory(_Status, any_directory_type)) {
            return {0, false};
        }
    }

    generic_handle_wrapper _Handle = _Open_file_handle(_Target, file_access::read,
        file_share::read, file_disposition::only_if_exists, file_attributes::normal, file_flags::none);
    if (!_Handle) {
        return {0, false};
    }

    unequivocal_result<uintmax_t> _Result;
    _Result.success = _File_size(_Handle, _Result.value);
    return _Result;
}

// FUNCTION is_empty_file
_NODISCARD unequivocal_result<bool> is_empty_file(const path& _Target) {
    const auto& _Size = _SDSDLL file_size(_Target);
    if (_Size.success) {
        return {_Size.value == 0, true};
    } else {
        return {false, false};
    }
}

// FUNCTION resize_file
_NODISCARD bool resize_file(const path& _Target, const uintmax_t _New_size) {
    { // check if the target is resizable
        const file_status _Status(_Target);
        if (!_SDSDLL exists(_Status) || _SDSDLL is_directory(_Status, any_directory_type)) {
            return false;
        }
    }

    generic_handle_wrapper _Handle = _Open_file_handle(_Target, file_access::write,
        file_share::none, file_disposition::only_if_exists, file_attributes::normal, file_flags::none);
    if (!_Handle) {
        return false;
    }

    return _Resize_file(_Handle, _New_size);
}

// FUNCTION _Filepos constructors/destructor
_Filepos::_Filepos() noexcept : _Myval{0}, _Myhandle(nullptr), _Mysize(0) {}

_Filepos::_Filepos(void* const _Handle) noexcept : _Myval{0}, _Myhandle(_Handle), _Mysize(0) {
    if (_Myhandle) { // handle ok, check file size
        (void) _File_size(_Myhandle, _Mysize);
    }
}

_Filepos::~_Filepos() noexcept {}

// FUNCTION _Filepos::_Submit_changes
void _Filepos::_Submit_changes() const noexcept {
    if (_Myhandle) {
        ::SetFilePointerEx(_Myhandle, _Myval, nullptr, FILE_BEGIN);
    }
}

// FUNCTION _Filepos::operator++
_Filepos& _Filepos::operator++() noexcept {
    if (_Myhandle && !_Reached_eof()) {
        ++_Myval.QuadPart;
        _Submit_changes();
    }

    return *this;
}

// FUNCTION _Filepos::operator+=
_Filepos& _Filepos::operator+=(uintmax_t _Off) noexcept {
    if (_Myhandle && !_Reached_eof()) {
        const uintmax_t _Quad_part = static_cast<uintmax_t>(_Myval.QuadPart);
        if (_Quad_part + _Off > _Mysize) {
            _Off = _Mysize - _Quad_part;
        }

        _Myval.QuadPart += static_cast<intmax_t>(_Off);
        _Submit_changes();
    }

    return *this;
}

// FUNCTION _Filepos::operator--
_Filepos& _Filepos::operator--() noexcept {
    if (_Myhandle && _Myval.QuadPart > 0) {
        --_Myval.QuadPart;
        _Submit_changes();
    }

    return *this;
}

// FUNCTION _Filepos::operator-=
_Filepos& _Filepos::operator-=(uintmax_t _Off) noexcept {
    if (_Myhandle && _Myval.QuadPart > 0) {
        if (_Myval.QuadPart - _Off < 0) {
            _Myval.QuadPart = 0;
        } else {
            _Myval.QuadPart -= static_cast<intmax_t>(_Off);
        }

        _Submit_changes();
    }

    return *this;
}

// FUNCTION _Filepos::operator*
_NODISCARD uintmax_t _Filepos::operator*() const noexcept {
    return _Myhandle ? static_cast<uintmax_t>(_Myval.QuadPart) : 0;
}

// FUNCTION _Filepos::_Change_handle
void _Filepos::_Change_handle(void* const _New_handle) noexcept {
    _Myhandle = _New_handle;
    _Myval    = {0};
    _Mysize   = 0;
    if (_Myhandle) { // handle ok, check file size
        (void) _File_size(_Myhandle, _Mysize);
    }
}

// FUNCTION _Filepos::_Check_file_size
_NODISCARD bool _Filepos::_Check_file_size() noexcept {
    return _Myhandle ? _File_size(_Myhandle, _Mysize) : false;
}

// FUNCTION _Filepos::_Set_file_size
void _Filepos::_Set_file_size(const uintmax_t _Size) noexcept {
    _Mysize = _Size;
}

// FUNCTION _Filepos::_Get_file_size
_NODISCARD const uintmax_t _Filepos::_Get_file_size() const noexcept {
    return _Mysize;
}

// FUNCTION _Filepos::_Reached_eof
_NODISCARD bool _Filepos::_Reached_eof() const noexcept {
    return static_cast<uintmax_t>(_Myval.QuadPart) == _Mysize;
}

// FUNCTION _Filepos::_Set_pos
_NODISCARD bool _Filepos::_Set_pos(const uintmax_t _Off, const bool _Submit) noexcept {
    if (!_Myhandle) { // no file is open
        return false;
    }

    if (_Off > _Mysize) { // position out of range
        return false;
    }

    _Myval.QuadPart = static_cast<intmax_t>(_Off);
    if (_Submit) {
        _Submit_changes();
    }

    return true;
}

// FUNCTION file constructors/destructor
file::file() noexcept : _Myhandle(nullptr), _Mypos() {}

file::file(const path& _Target, const file_access _Access, const file_share _Share,
    const file_disposition _Disp, const file_attributes _Attrs, const file_flags _Flags)
    : _Myhandle(_Open_file_handle(_Target, _Access, _Share, _Disp, _Attrs, _Flags)), _Mypos(_Myhandle) {}

file::~file() noexcept {}

// FUNCTION file::_Go_forward
void file::_Go_forward(const size_type _Count) noexcept {
#ifdef _M_X64
    (void) _Mypos._Set_pos(*_Mypos + _Count, false);
#else // ^^^ _M_X64 ^^^ / vvv _M_IX86 vvv
    (void) _Mypos._Set_pos(*_Mypos + static_cast<uintmax_t>(_Count), false);
#endif // _M_X64
}

// FUNCTION file::open
_NODISCARD bool file::open(
    const path& _Target, const file_access _Access, const file_share _Share,
    const file_disposition _Disp, const file_attributes _Attrs, const file_flags _Flags) noexcept {
    if (_Myhandle) { // some file is already open
        return false;
    }

    _Myhandle = _Open_file_handle(_Target, _Access, _Share, _Disp, _Attrs, _Flags);
    if (_Myhandle) {
        _Mypos._Change_handle(_Myhandle);
        return true;
    } else {
        return false;
    }
}

// FUNCTION file::is_open
_NODISCARD bool file::is_open() const noexcept {
    return _Myhandle.good();
}

// FUNCTION file::close
void file::close() noexcept {
    _Myhandle.close();
}

// FUNCTION file::seek
_NODISCARD bool file::seek(const pos_type _New_pos) noexcept {
    if (!is_open()) { // no file is open
        return false;
    }

    return _Mypos._Set_pos(_New_pos);
}

_NODISCARD bool file::seek(const off_type _Off, const seekdir _Seekdir) noexcept {
    if (!is_open()) { // no file is open
        return false;
    }

    switch (_Seekdir) {
    case beg:
        return _Mypos._Set_pos(static_cast<pos_type>(_Off));
    case cur:
        if (_Off >= 0) { // go forward
            return _Mypos._Set_pos(*_Mypos + static_cast<pos_type>(_Off));
        } else { // go back
            return _Mypos._Set_pos(*_Mypos - static_cast<pos_type>(_CSTD abs(_Off)));
        }
    case end:
        if (_Off < 0) { // must be non-negative
            return false;
        }

        return _Mypos._Set_pos(_Mypos._Get_file_size() - static_cast<pos_type>(_Off));
    default: // unknown direction
        return false;
    }
}

// FUNCTION file::tell
_NODISCARD typename file::pos_type file::tell() const noexcept {
    return *_Mypos;
}

// FUNCTION file::eof
_NODISCARD bool file::eof() const noexcept {
    return _Mypos._Reached_eof();
}

// FUNCTION file::clear
_NODISCARD bool file::clear() noexcept {
    if (!is_open()) { // no file is open
        return false;
    }

    if (_Clear_file(_Myhandle)) { // file cleared, reset the position
        _Mypos._Set_file_size(0);
        return _Mypos._Set_pos(0);
    } else {
        return false;
    }
}

// FUNCTION file::resize
_NODISCARD bool file::resize(const uintmax_t _New_size) noexcept {
    if (!is_open()) { // no file is open
        return false;
    }

    if (_Resize_file(_Myhandle, _New_size)) { // file resized, reset the position
        _Mypos._Set_file_size(_New_size);
        return _Mypos._Set_pos(_New_size);
    } else {
        return false;
    }
}

// FUNCTION file::get
_NODISCARD bool file::get(char& _Buf) noexcept {
    if (!is_open()) { // no file is open
        return false;
    }

    if (!_Mypos._Reached_eof()) { // at least 1 byte is still available
        if (!_Read_file(_Myhandle, &_Buf, 1)) {
            return false;
        }

        _Go_forward(1);
        return true;
    } else { // no bytes to read
        return false;
    }
}

_NODISCARD bool file::get(uint8_t& _Buf) noexcept {
    if (!is_open()) { // no file is open
        return false;
    }

    if (!_Mypos._Reached_eof()) { // at least 1 byte is still available
        if (!_Read_file(_Myhandle, &_Buf, 1)) {
            return false;
        }

        _Go_forward(1);
        return true;
    } else { // no bytes to read
        return false;
    }
}

// FUNCTION file::put
_NODISCARD bool file::put(const char _Ch) noexcept {
    if (!is_open()) { // no file is open
        return false;
    }

    if (_Write_file(_Myhandle, &_Ch, 1)) {
        _Go_forward(1);
        return true;
    } else {
        return false;
    }
}

_NODISCARD bool file::put(const uint8_t _Ch) noexcept {
    if (!is_open()) { // no file is open
        return false;
    }

    if (_Write_file(_Myhandle, &_Ch, 1)) {
        _Go_forward(1);
        return true;
    } else {
        return false;
    }
}

// FUNCTION file::read
_NODISCARD bool file::read(
    char* const _Buf, const size_type _Buf_size, size_type _Count, size_type* const _Read) noexcept {
    if (!is_open()) { // no file is open
        return false;
    }

    if (_Count == 0) { // do nothing
        return true;
    }

    if (_Count > _Buf_size) { // buffer overflow possible, break
        return false;
    }

    // read as many bytes as possible
    _Count = (_STD min)(static_cast<size_t>(_Mypos._Get_file_size() - *_Mypos), _Count);
    if (_Read_file(_Myhandle, _Buf, _Count, _Read)) {
        _Go_forward(_Count);
        return true;
    } else {
        return false;
    }
}

_NODISCARD bool file::read(string& _Buf, size_type _Count, size_type* const _Read) noexcept {
    return read(_Buf.data(), _Buf.size(), _Count, _Read);
}

_NODISCARD bool file::read(
    uint8_t* const _Buf, const size_type _Buf_size, size_type _Count, size_type* const _Read) noexcept {
    if (!is_open()) { // no file is open
        return false;
    }

    if (_Count == 0) { // do nothing
        return true;
    }

    if (_Count > _Buf_size) { // buffer overflow possible, break
        return false;
    }

    // read as many bytes as possible
    _Count = (_STD min)(static_cast<size_t>(_Mypos._Get_file_size() - *_Mypos), _Count);
    if (_Read_file(_Myhandle, _Buf, _Count, _Read)) {
        _Go_forward(_Count);
        return true;
    } else {
        return false;
    }
}

_NODISCARD bool file::read(byte_string& _Buf, size_type _Count, size_type* const _Read) noexcept {
    return read(_Buf.data(), _Buf.size(), _Count, _Read);
}

// FUNCTION file::write
_NODISCARD bool file::write(const char* const _Data, const size_type _Count) noexcept {
    if (!is_open()) { // no file is open
        return false;
    }

    if (_Count == 0) { // empty string, do nothing
        return true;
    }

    if (_Write_file(_Myhandle, _Data, _Count)) {
        _Go_forward(_Count);
        return _Mypos._Check_file_size(); // the file may have grown, check it
    } else {
        return false;
    }
}

_NODISCARD bool file::write(const string_view _Data) noexcept {
    return write(_Data.data(), _Data.size());
}

_NODISCARD bool file::write(const string& _Data) {
    return write(_Data.c_str(), _Data.size());
}

_NODISCARD bool file::write(const uint8_t* const _Data, const size_type _Count) noexcept {
    if (!is_open()) { // no file is open
        return false;
    }

    if (_Count == 0) { // empty string, do nothing
        return true;
    }

    if (_Write_file(_Myhandle, _Data, _Count)) {
        _Go_forward(_Count);
        return _Mypos._Check_file_size(); // the file may have grown, check it
    } else {
        return false;
    }
}

_NODISCARD bool file::write(const byte_string_view _Data) noexcept {
    return write(_Data.data(), _Data.size());
}

_NODISCARD bool file::write(const byte_string& _Data) {
    return write(_Data.c_str(), _Data.size());
}

// FUNCTION read_utf16_string_from_file
_NODISCARD bool read_utf16_string_from_file(
    file& _File, wchar_t* const _Buf, const size_t _Buf_size, size_t _Count) noexcept {
    if (!_File.is_open()) { // no file is open
        return false;
    }

    char _Ch; // first byte from a chunk
    size_t _Chunk_size; // chunk size in bytes (1 UTF-16 character)
    string _Chunk; // chunk in bytes (1 UTF-16 character)
    wstring _Result;
    while (_Count-- > 0) {
        if (!_File.get(_Ch)) {
            return false;
        }

        _Chunk_size = _Get_utf8_chunk_size(_Ch);
        _Chunk.resize(_Chunk_size);
        _Chunk[0] = _Ch; // do not skip the first byte
        if (!_File.read(_Chunk.data() + 1, _Chunk_size - 1, _Chunk_size - 1)) {
            return false;
        }

        _Result += utf16_string::from_utf8(_Chunk).str();
    }

    _Result.copy(_Buf, _Count);
    return true;
}

_NODISCARD bool read_utf16_string_from_file(file& _File, wstring& _Buf, size_t _Count) noexcept {
    return _SDSDLL read_utf16_string_from_file(_File, _Buf.data(), _Buf.size(), _Count);
}

// FUNCTION write_utf16_string_to_file
_NODISCARD bool write_utf16_string_to_file(
    file& _File, const wchar_t* const _Data, const size_t _Count) noexcept {
    if (!_File.is_open()) { // no file is open
        return false;
    }

    const utf8_string& _Bytes = utf8_string::from_utf16(_Data, _Count);
    return _File.write(_Bytes.c_str(), _Bytes.size());
}

_NODISCARD bool write_utf16_string_to_file(file& _File, const wstring_view _Data) noexcept {
    return _SDSDLL write_utf16_string_to_file(_File, _Data.data(), _Data.size());
}

_NODISCARD bool write_utf16_string_to_file(file& _File, const wstring& _Data) {
    return _SDSDLL write_utf16_string_to_file(_File, _Data.c_str(), _Data.size());
}
_SDSDLL_END

#endif // _SDSDLL_PREPROCESSOR_GUARD