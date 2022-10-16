// sudb.cpp

// Copyright (c) Mateusz Jandura. All rights reserved.
// SPDX-License-Identifier: Apache-2.0

#include <build/sdsdll_pch.hpp>
#include <extensions/sudb.hpp>
#if _SDSDLL_PREPROCESSOR_GUARD

_SDSDLL_BEGIN
// FUNCTION _Extract_sudb_header_from_bytes
_NODISCARD _Sudb_header_data _Extract_sudb_header_from_bytes(const byte_string& _Bytes) {
    static constexpr size_t _Header_size = 44; // 44-byte header
    if (_Bytes.size() < _Header_size) { // incomplete header
        return {0, 0, 0, 0};
    }

    _Sudb_header_data _Result;
    uint8_t _As_bytes[4]; // 4-byte integer in bytes
    memory_traits::copy(_Result._Signature, _Bytes.c_str(), 4);
    memory_traits::copy(_Result._Magic, _Bytes.c_str() + 4, 4);
    memory_traits::copy(_Result._Checksum, _Bytes.c_str() + 8, 32);
    memory_traits::copy(_As_bytes, _Bytes.c_str() + 40, 4);
    _Result._Count = _SDSDLL pack_integer<uint32_t>(_As_bytes);
    return _Result;
}

// FUNCTION _Sudb_header constructors/destructor
_Sudb_header::_Sudb_header() noexcept : _Mydata{0, 0, 0, 0} {}

_Sudb_header::_Sudb_header(const byte_string& _Bytes) : _Mydata(_Extract_sudb_header_from_bytes(_Bytes)) {}

_Sudb_header::~_Sudb_header() noexcept {}

// FUNCTION _Sudb_header::_To_string
_NODISCARD byte_string _Sudb_header::_To_string() const {
    using _Traits                        = string_traits<uint8_t, int>;
    static constexpr size_t _Header_size = 44; // 44-byte header
    const auto& _As_bytes                = _SDSDLL unpack_integer(_Mydata._Count);
    byte_string _Result(_Header_size, uint8_t{});
    _Traits::copy(_Result.data(), _Signature, 4);
    _Traits::copy(_Result.data() + 4, _Magic, 4);
    _Traits::copy(_Result.data() + 8, _Mydata._Checksum, 32);
    _Traits::copy(_Result.data() + 40, _As_bytes.data(), _As_bytes.size());
    return _Result;
}

// FUNCTION _Sudb_header::_Valid
_NODISCARD bool _Sudb_header::_Valid() const noexcept {
    using _Traits = string_traits<uint8_t, int>;
    return _Traits::compare(_Mydata._Signature, _Signature, 4) == 0
        && _Traits::compare(_Mydata._Magic, _Magic, 4) == 0;
}

// FUNCTION _Sudb_header::_Checksum
_NODISCARD const uint8_t* _Sudb_header::_Checksum() const noexcept {
    return _Mydata._Checksum;
}

void _Sudb_header::_Checksum(const uint8_t* const _New_checksum) noexcept {
    string_traits<uint8_t, int>::move(_Mydata._Checksum, _New_checksum, 32);
}

// FUNCTION _Sudb_header::_Entries
_NODISCARD const uint32_t _Sudb_header::_Entries() const noexcept {
    return _Mydata._Count;
}

// FUNCTION _Sudb_header::_Checksum
void _Sudb_header::_Entries(const uint32_t _New_count) noexcept {
    _Mydata._Count = _New_count;
}

// FUNCTION _Sudb_header::_Reset_cache
void _Sudb_header::_Reset_cache(const _Sudb_header_data& _New_data) noexcept {
    using _Traits  = string_traits<uint8_t, int>;
    _Mydata._Count = _New_data._Count;
    _Traits::copy(_Mydata._Signature, _New_data._Signature, 4);
    _Traits::copy(_Mydata._Magic, _New_data._Magic, 4);
    _Traits::copy(_Mydata._Checksum, _New_data._Checksum, 32);
}

// FUNCTION _Sudb_entries_loader copy constructor/destructor
_Sudb_entries_loader::_Sudb_entries_loader(file* const _File) noexcept : _Myfile(_File), _Myentry() {}

_Sudb_entries_loader::~_Sudb_entries_loader() noexcept {}

// FUNCTION _Sudb_entries_loader::_Next
_NODISCARD bool _Sudb_entries_loader::_Next() {
    if (!_Myfile || !_Myfile->is_open()) {
        return false;
    }

    // Note: The first 8 bytes are the account name xxHash hash. The next 64 bytes are the
    //       password Argon2ID hash. The next 16 bytes are the unique salt. The last 64 bytes
    //       are the account ARC SHA-512 hash. We need to read 152 bytes to fill all the data.
    static constexpr size_t _Buf_size = 152; // always 152 bytes
    uint8_t _Buf[_Buf_size];
    size_t _Read = 0; // read bytes, must be initialized
    if (!_Myfile->read(_Buf, _Buf_size, _Buf_size, &_Read) || _Read != _Buf_size) {
        return false;
    }

    memory_traits::copy(_Myentry._Account, _Buf, 8);
    memory_traits::copy(_Myentry._Password, _Buf + 8, 64);
    memory_traits::copy(_Myentry._Salt, _Buf + 72, 16);
    memory_traits::copy(_Myentry._Arc, _Buf + 88, 64);
    return true;
}

// FUNCTION _Sudb_entries_loader::_Get
_NODISCARD const _Sudb_entry& _Sudb_entries_loader::_Get() const noexcept {
    return _Myentry;
}

// FUNCTION sudb_file copy constructor/destructor
sudb_file::sudb_file(const path& _Target)
    : _Myfile(_Target), _Myheader(), _Myentries(), _Myok(_Load_file()), _Mychanges(false) {}

sudb_file::~sudb_file() noexcept {
    (void) flush();
}

// FUNCTION sudb_file::_Load_header
_NODISCARD bool sudb_file::_Load_header() {
    if (!_Myfile.is_open()) {
        return false;
    }
    
    size_t _Buf_size = 44; // 44-byte header
    byte_string _Buf(_Buf_size, uint8_t{});
    if (!_Myfile.read(_Buf, _Buf_size, &_Buf_size) || _Buf_size != _Buf.size()) {
        return false;
    }

    _Myheader._Reset_cache(_Extract_sudb_header_from_bytes(_Buf));
    return _Myheader._Valid();
}

// FUNCTION sudb_file::_Load_entries
_NODISCARD bool sudb_file::_Load_entries() {
    // Note: We do not need to check here if the file is open, because this function
    //       is only called if the _Load_header() succeeded.
    uint32_t _Count = _Myheader._Entries();
    if (_Count == 0) { // no entries to load
        return true;
    }

    if (!_Myfile.seek(44, file::beg)) {
        return false;
    }

    _Sudb_entries_loader _Loader(_SDSDLL addressof(_Myfile));
    while (_Count-- > 0) {
        if (!_Loader._Next()) {
            _Myentries.clear();
            return false;
        }

        _Myentries.push_back(_Loader._Get());
    }

    return true;
}

// FUNCTION sudb_file::_Validate_checksum
_NODISCARD bool sudb_file::_Validate_checksum() noexcept {
    const byte_string_view _As_bytes(_Myheader._Checksum(), 32);
    return _As_bytes == _SDSDLL blake3_file(_Myfile, 40); // skip the first 40 bytes
}

// FUNCTION sudb_file::_Load_file
_NODISCARD bool sudb_file::_Load_file() noexcept {
    if (!_Load_header()) {
        return false;
    }

    if (_Validate_checksum()) {
        return _Load_entries();
    } else {
        return false;
    }
}

// FUNCTION sudb_file::_Find_entry_by_account_name
_NODISCARD size_t sudb_file::_Find_entry_by_account_name(
    const wchar_t* const _Name, const size_t _Size) const {
    if (!_Myok || _Myentries.empty()) {
        return static_cast<size_t>(-1);
    }

    const byte_string& _Hash = _SDSDLL xxhash(_Name, _Size);
    if (_Hash.empty()) { // failed to compute a hash
        return static_cast<size_t>(-1);
    }

    for (size_t _Idx = 0; _Idx < _Myentries.size(); ++_Idx) {
        if (memory_traits::compare(
            _Hash.c_str(), _Myentries[_Idx]._Account, _Hash.size()) == 0) { // entry found
            return _Idx;
        }
    }

    return static_cast<size_t>(-1);
}

// FUNCTION sudb_file::_Find_entry_by_arc
_NODISCARD size_t sudb_file::_Find_entry_by_arc(const arc& _Arc) const {
    if (!_Myok || _Myentries.empty()) {
        return static_cast<size_t>(-1);
    }

    const byte_string& _Hash = _SDSDLL sha512(_Arc.to_string());
    if (_Hash.empty()) { // failed to compute a hash
        return static_cast<size_t>(-1);
    }

    for (size_t _Idx = 0; _Idx < _Myentries.size(); ++_Idx) {
        if (memory_traits::compare(_Hash.c_str(), _Myentries[_Idx]._Arc, _Hash.size()) == 0) { // entry found
            return _Idx;
        }
    }

    return static_cast<size_t>(-1);
}

// FUNCTION sudb_file::_Is_unique_arc
_NODISCARD bool sudb_file::_Is_unique_arc(const arc& _Arc) const noexcept {
    for (const _Sudb_entry& _Entry : _Myentries) {
        if (memory_traits::compare(_Entry._Arc, _Arc.get(), arc::size) == 0) { // entry found
            return false;
        }
    }

    return true;
}

// FUNCTION sudb_file::_Is_unique_salt
_NODISCARD bool sudb_file::_Is_unique_salt(const _Unique_salt& _Salt) const noexcept {
    for (const _Sudb_entry& _Entry : _Myentries) {
        if (memory_traits::compare(_Entry._Salt, _Salt.get(), _Unique_salt::size) == 0) {
            return false;
        }
    }
    
    return true;
}

// FUNCTION sudb_file::_Generate_unique_arc
_NODISCARD arc sudb_file::_Generate_unique_arc() const noexcept {
    arc _Result;
    do { // keep doing this until the ARC is unique
        _Result = _SDSDLL make_arc();
    } while (!_Is_unique_arc(_Result));

    return _Result;
}

// FUNCTION sudb_file::_Generate_unique_salt
_NODISCARD typename sudb_file::_Unique_salt sudb_file::_Generate_unique_salt() const noexcept {
    _Unique_salt _Result;
    do { // keep doing this until the salt is unique
        ::RAND_bytes(_Result.get(), static_cast<int>(_Unique_salt::size));
    } while (!_Is_unique_salt(_Result));
    
    return _Result;
}

// FUNCTION sudb_file::_Write_entry
_NODISCARD bool sudb_file::_Write_entry(const _Sudb_entry& _Entry) {
    byte_string _Data(152, uint8_t{});
    memory_traits::copy(_Data.data(), _Entry._Account, 8);
    memory_traits::copy(_Data.data() + 8, _Entry._Password, 64);
    memory_traits::copy(_Data.data() + 72, _Entry._Salt, 16);
    memory_traits::copy(_Data.data() + 88, _Entry._Arc, 64);
    return _Myfile.write(_Data);
}

// FUNCTION sudb_file::_Flush_buffers
_NODISCARD bool sudb_file::_Flush_buffers() {
    // Note: The first step is to write the entries count (4-byte integer in bytes).
    //       Try to write it after the file checksum (40-byte offset).
    if (!_Myfile.resize(40)) {
        return false;
    }

    const auto& _Count = _SDSDLL unpack_integer(static_cast<uint32_t>(_Myentries.size()));
    if (!_Myfile.write(_Count.data(), _Count.size())) {
        return false;
    }

    // Note: The second step is to write all entries. The first 8 bytes are the account name
    //       xxHash hash. The next 64 bytes are the password Argon2id hash. The next 16 bytes
    //       are the unique salt. The last 64-bytes are the ARC SHA-512 hash. Try to write it
    //       after the entries count (44-byte offset).
    for (const _Sudb_entry& _Entry : _Myentries) {
        if (!_Write_entry(_Entry)) {
            return false;
        }
    }

    // Note: The last step is to write the file checksum. Try to write it after
    //       the file signature and magic value (8-byte offset).
    const byte_string& _Checksum = _SDSDLL blake3_file(_Myfile, 40); // skip the first 40 bytes
    if (_Checksum.empty()) { // failed to compute a checksum
        return false;
    }

    if (!_Myfile.seek(8)) {
        return false;
    }

    return _Myfile.write(_Checksum.c_str(), _Checksum.size());
}

// FUNCTION sudb_file::make_storage
_NODISCARD bool sudb_file::make_storage(const path& _Target) {
    file _File;
    if (_SDSDLL exists(_Target)) { // use an existing file
        if (!_File.open(_Target)) {
            return false;
        }

        if (!_File.clear()) { // file must be empty
            return false;
        }
    } else { // create a new file
        if (!_SDSDLL create_file(_Target)) {
            return false;
        }

        if (!_File.open(_Target)) {
            return false;
        }
    }

    _Sudb_header _Header;
    const byte_string& _Checksum = _SDSDLL blake3("\x00\x00\x00\x00", 4); // 4-byte integer in bytes
    _Header._Checksum(_Checksum.c_str());
    return _File.write(_Header._To_string());
}

// FUNCTION sudb_file::ok
_NODISCARD const bool sudb_file::ok() const noexcept {
    return _Myok;
}

// FUNCTION sudb_file::refresh
void sudb_file::refresh() noexcept {
    _Myok = _Load_file();
}

// FUNCTION sudb_file::flush
_NODISCARD bool sudb_file::flush() noexcept {
    if (!_Myok) {
        return false;
    }
    
    if (!_Mychanges) { // nothing has changed
        return true;
    }

    if (_Flush_buffers()) {
        _Mychanges = false; // reset changes
        return true;
    } else {
        return false;
    }
}

// FUNCTION sudb_file::has_entry
_NODISCARD bool sudb_file::has_entry(const wchar_t* const _Account) const {
    if (!_Myok || _Myentries.empty()) {
        return false;
    }

    using _Traits = string_traits<wchar_t, size_t>;
    return _Find_entry_by_account_name(_Account, _Traits::length(_Account)) != static_cast<size_t>(-1);
}

_NODISCARD bool sudb_file::has_entry(const wstring_view _Account) const {
    if (!_Myok || _Myentries.empty()) {
        return false;
    }

    return _Find_entry_by_account_name(_Account.data(), _Account.size()) != static_cast<size_t>(-1);
}

_NODISCARD bool sudb_file::has_entry(const wstring& _Account) const {
    if (!_Myok || _Myentries.empty()) {
        return false;
    }

    return _Find_entry_by_account_name(_Account.c_str(), _Account.size()) != static_cast<size_t>(-1);
}

_NODISCARD bool sudb_file::has_entry(const arc& _Arc) const {
    if (!_Myok || _Myentries.empty()) {
        return false;
    }

    return _Find_entry_by_arc(_Arc) != static_cast<size_t>(-1);
}

// FUNCTION sudb_file::compare_passwords
_NODISCARD bool sudb_file::compare_passwords(
    const wchar_t* const _Account, const wchar_t* const _Password) const {
    if (!_Myok || _Myentries.empty()) {
        return false;
    }

    using _Traits     = string_traits<wchar_t, size_t>;
    const size_t _Pos = _Find_entry_by_account_name(_Account, _Traits::length(_Account));
    if (_Pos == static_cast<size_t>(-1)) { // entry not found
        return false;
    }

    const _Unique_salt _Salt(_Myentries[_Pos]._Salt);
    const byte_string& _Hash = _SDSDLL argon2id(_Password, _Salt);
    if (_Hash.empty()) { // failed to compute a hash
        return false;
    }

    return memory_traits::compare(_Myentries[_Pos]._Password, _Hash.c_str(), _Hash.size()) == 0;
}

_NODISCARD bool sudb_file::compare_passwords(
    const wstring_view _Account, const wstring_view _Password) const {
    if (!_Myok || _Myentries.empty()) {
        return false;
    }

    const size_t _Pos = _Find_entry_by_account_name(_Account.data(), _Account.size());
    if (_Pos == static_cast<size_t>(-1)) { // entry not found
        return false;
    }

    const _Unique_salt _Salt(_Myentries[_Pos]._Salt);
    const byte_string& _Hash = _SDSDLL argon2id(_Password, _Salt);
    if (_Hash.empty()) { // failed to compute a hash
        return false;
    }

    return memory_traits::compare(_Myentries[_Pos]._Password, _Hash.c_str(), _Hash.size()) == 0;
}

_NODISCARD bool sudb_file::compare_passwords(const wstring& _Account, const wstring& _Password) const {
    if (!_Myok || _Myentries.empty()) {
        return false;
    }

    const size_t _Pos = _Find_entry_by_account_name(_Account.c_str(), _Account.size());
    if (_Pos == static_cast<size_t>(-1)) { // entry not found
        return false;
    }

    const _Unique_salt _Salt(_Myentries[_Pos]._Salt);
    const byte_string& _Hash = _SDSDLL argon2id(_Password, _Salt);
    if (_Hash.empty()) { // failed to compute a hash
        return false;
    }

    return memory_traits::compare(_Myentries[_Pos]._Password, _Hash.c_str(), _Hash.size()) == 0;
}

// FUNCTION sudb_file::compare_arcs
_NODISCARD bool sudb_file::compare_arcs(const wchar_t* const _Account, const arc& _Arc) const {
    if (!_Myok || _Myentries.empty()) {
        return false;
    }

    using _Traits     = string_traits<wchar_t, size_t>;
    const size_t _Pos = _Find_entry_by_account_name(_Account, _Traits::length(_Account));
    if (_Pos == static_cast<size_t>(-1)) { // entry not found
        return false;
    }

    const byte_string& _Hash = _SDSDLL sha512(_Arc.to_string());
    if (_Hash.empty()) { // failed to compute a hash
        return false;
    }

    return memory_traits::compare(_Myentries[_Pos]._Arc, _Hash.c_str(), _Hash.size()) == 0;
}

_NODISCARD bool sudb_file::compare_arcs(const wstring_view _Account, const arc& _Arc) const {
    if (!_Myok || _Myentries.empty()) {
        return false;
    }

    const size_t _Pos = _Find_entry_by_account_name(_Account.data(), _Account.size());
    if (_Pos == static_cast<size_t>(-1)) { // entry not found
        return false;
    }

    const byte_string& _Hash = _SDSDLL sha512(_Arc.to_string());
    if (_Hash.empty()) { // failed to compute a hash
        return false;
    }

    return memory_traits::compare(_Myentries[_Pos]._Arc, _Hash.c_str(), _Hash.size()) == 0;
}

_NODISCARD bool sudb_file::compare_arcs(const wstring& _Account, const arc& _Arc) const {
    if (!_Myok || _Myentries.empty()) {
        return false;
    }

    const size_t _Pos = _Find_entry_by_account_name(_Account.c_str(), _Account.size());
    if (_Pos == static_cast<size_t>(-1)) { // entry not found
        return false;
    }

    const byte_string& _Hash = _SDSDLL sha512(_Arc.to_string());
    if (_Hash.empty()) { // failed to compute a hash
        return false;
    }

    return memory_traits::compare(_Myentries[_Pos]._Arc, _Hash.c_str(), _Hash.size()) == 0;
}

// FUNCTION sudb_file::modify_entry_account_name
_NODISCARD bool sudb_file::modify_entry_account_name(
    const wchar_t* const _Account, const wchar_t* const _New_name) {
    if (!_Myok || _Myentries.empty()) {
        return false;
    }

    using _Traits     = string_traits<wchar_t, size_t>;
    const size_t _Pos = _Find_entry_by_account_name(_Account, _Traits::length(_Account));
    if (_Pos == static_cast<size_t>(-1)) { // entry not found
        return false;
    }

    const byte_string& _Hash = _SDSDLL xxhash(_New_name);
    if (_Hash.empty()) { // failed to compute a hash
        return false;
    }

    if (memory_traits::compare(_Myentries[_Pos]._Account, _Hash.c_str(), _Hash.size()) != 0) {
        memory_traits::copy(_Myentries[_Pos]._Account, _Hash.c_str(), _Hash.size());
        _Mychanges = true; // save changes
    }

    return true;
}

_NODISCARD bool sudb_file::modify_entry_account_name(
    const wstring_view _Account, const wstring_view _New_name) {
    if (!_Myok || _Myentries.empty()) {
        return false;
    }

    const size_t _Pos = _Find_entry_by_account_name(_Account.data(), _Account.size());
    if (_Pos == static_cast<size_t>(-1)) { // entry not found
        return false;
    }

    const byte_string& _Hash = _SDSDLL xxhash(_New_name);
    if (_Hash.empty()) { // failed to compute a hash
        return false;
    }

    if (memory_traits::compare(_Myentries[_Pos]._Account, _Hash.c_str(), _Hash.size()) != 0) {
        memory_traits::copy(_Myentries[_Pos]._Account, _Hash.c_str(), _Hash.size());
        _Mychanges = true; // save changes
    }

    return true;
}

_NODISCARD bool sudb_file::modify_entry_account_name(const wstring& _Account, const wstring& _New_name) {
    if (!_Myok || _Myentries.empty()) {
        return false;
    }

    const size_t _Pos = _Find_entry_by_account_name(_Account.c_str(), _Account.size());
    if (_Pos == static_cast<size_t>(-1)) { // entry not found
        return false;
    }

    const byte_string& _Hash = _SDSDLL xxhash(_New_name);
    if (_Hash.empty()) { // failed to compute a hash
        return false;
    }

    if (memory_traits::compare(_Myentries[_Pos]._Account, _Hash.c_str(), _Hash.size()) != 0) {
        memory_traits::copy(_Myentries[_Pos]._Account, _Hash.c_str(), _Hash.size());
        _Mychanges = true; // save changes
    }

    return true;
}

// FUNCTION sudb_file::modify_entry_password
_NODISCARD bool sudb_file::modify_entry_password(
    const wchar_t* const _Account, const wchar_t* const _New_password) {
    if (!_Myok || _Myentries.empty()) {
        return false;
    }

    using _Traits     = string_traits<wchar_t, size_t>;
    const size_t _Pos = _Find_entry_by_account_name(_Account, _Traits::length(_Account));
    if (_Pos == static_cast<size_t>(-1)) { // entry not found
        return false;
    }

    const _Unique_salt _Salt(_Myentries[_Pos]._Salt);
    const byte_string& _Hash = _SDSDLL argon2id(_New_password, _Salt);
    if (_Hash.empty()) { // failed to compute a hash
        return false;
    }

    if (memory_traits::compare(_Myentries[_Pos]._Password, _Hash.c_str(), _Hash.size()) != 0) {
        memory_traits::copy(_Myentries[_Pos]._Password, _Hash.c_str(), _Hash.size());
        _Mychanges = true; // save changes
    }

    return true;
}

_NODISCARD bool sudb_file::modify_entry_password(
    const wstring_view _Account, const wstring_view _New_password) {
    if (!_Myok || _Myentries.empty()) {
        return false;
    }

    const size_t _Pos = _Find_entry_by_account_name(_Account.data(), _Account.size());
    if (_Pos == static_cast<size_t>(-1)) { // entry not found
        return false;
    }

    const _Unique_salt _Salt(_Myentries[_Pos]._Salt);
    const byte_string& _Hash = _SDSDLL argon2id(_New_password, _Salt);
    if (_Hash.empty()) { // failed to compute a hash
        return false;
    }

    if (memory_traits::compare(_Myentries[_Pos]._Password, _Hash.c_str(), _Hash.size()) != 0) {
        memory_traits::copy(_Myentries[_Pos]._Password, _Hash.c_str(), _Hash.size());
        _Mychanges = true; // save changes
    }

    return true;
}

_NODISCARD bool sudb_file::modify_entry_password(const wstring& _Account, const wstring& _New_password) {
    if (!_Myok || _Myentries.empty()) {
        return false;
    }

    const size_t _Pos = _Find_entry_by_account_name(_Account.c_str(), _Account.size());
    if (_Pos == static_cast<size_t>(-1)) { // entry not found
        return false;
    }

    const _Unique_salt _Salt(_Myentries[_Pos]._Salt);
    const byte_string& _Hash = _SDSDLL argon2id(_New_password, _Salt);
    if (_Hash.empty()) { // failed to compute a hash
        return false;
    }

    if (memory_traits::compare(_Myentries[_Pos]._Password, _Hash.c_str(), _Hash.size()) != 0) {
        memory_traits::copy(_Myentries[_Pos]._Password, _Hash.c_str(), _Hash.size());
        _Mychanges = true; // save changes
    }

    return true;
}

// FUNCTION sudb_file::modify_entry_arc
_NODISCARD bool sudb_file::modify_entry_arc(const wchar_t* const _Account) noexcept {
    if (!_Myok || _Myentries.empty()) {
        return false;
    }

    using _Traits     = string_traits<wchar_t, size_t>;
    const size_t _Pos = _Find_entry_by_account_name(_Account, _Traits::length(_Account));
    if (_Pos == static_cast<size_t>(-1)) { // entry not found
        return false;
    }

    const arc& _Arc          = _Generate_unique_arc();
    const byte_string& _Hash = _SDSDLL sha512(_Arc.to_string());
    if (_Hash.empty()) { // failed to compute a hash
        return false;
    }

    memory_traits::copy(_Myentries[_Pos]._Arc, _Hash.c_str(), _Hash.size());
    _Mychanges = true; // save changes
    return true;
}

_NODISCARD bool sudb_file::modify_entry_arc(const wstring_view _Account) noexcept {
    if (!_Myok || _Myentries.empty()) {
        return false;
    }

    const size_t _Pos = _Find_entry_by_account_name(_Account.data(), _Account.size());
    if (_Pos == static_cast<size_t>(-1)) { // entry not found
        return false;
    }

    const arc& _Arc = _Generate_unique_arc();
    memory_traits::copy(_Myentries[_Pos]._Arc, _Arc.get(), _Arc.size);
    _Mychanges = true; // save changes
    return true;
}

_NODISCARD bool sudb_file::modify_entry_arc(const wstring& _Account) {
    if (!_Myok || _Myentries.empty()) {
        return false;
    }

    const size_t _Pos = _Find_entry_by_account_name(_Account.c_str(), _Account.size());
    if (_Pos == static_cast<size_t>(-1)) { // entry not found
        return false;
    }

    const arc& _Arc          = _Generate_unique_arc();
    const byte_string& _Hash = _SDSDLL sha512(_Arc.to_string());
    if (_Hash.empty()) { // failed to compute a hash
        return false;
    }

    memory_traits::copy(_Myentries[_Pos]._Arc, _Hash.c_str(), _Hash.size());
    _Mychanges = true; // save changes
    return true;
}

// FUNCTION sudb_file::append_entry
_NODISCARD bool sudb_file::append_entry(
    const wchar_t* const _Account, const wchar_t* const _Password, arc* const _Arc) {
    if (has_entry(_Account) || _Myentries.size() >= 0xFFFF'FFFF) {
        return false;
    }

    _Sudb_entry _Entry;
    { // hash an account name
        const byte_string& _Hash = _SDSDLL xxhash(_Account);
        if (_Hash.empty()) { // failed to compute a hash
            return false;
        }

        memory_traits::copy(_Entry._Account, _Hash.c_str(), _Hash.size());
    }

    { // generate a unique salt and hash a password 
        const _Unique_salt _Salt = _Generate_unique_salt();
        const byte_string& _Hash = _SDSDLL argon2id(_Password, _Salt);
        if (_Hash.empty()) { // failed to compute a hash
            return false;
        }

        memory_traits::copy(_Entry._Password, _Hash.c_str(), _Hash.size());
        memory_traits::copy(_Entry._Salt, _Salt.get(), _Unique_salt::size);
    }

    { // hash a unique ARC
        const arc& _Unique_arc   = _Generate_unique_arc();
        const byte_string& _Hash = _SDSDLL sha512(_Unique_arc.to_string());
        if (_Hash.empty()) { // failed to compute a hash
            return false;
        }
    
        memory_traits::copy(_Entry._Arc, _Hash.c_str(), _Hash.size());
        if (_Arc) { // save a new ARC
            *_Arc = _Unique_arc;
        }
    }
    
    _Myentries.push_back(_Entry);
    _Mychanges = true; // save changes
    return true;
}

_NODISCARD bool sudb_file::append_entry(
    const wstring_view _Account, const wstring_view _Password, arc* const _Arc) {
    if (has_entry(_Account) || _Myentries.size() >= 0xFFFF'FFFF) {
        return false;
    }

    _Sudb_entry _Entry;
    { // hash an account name
        const byte_string& _Hash = _SDSDLL xxhash(_Account);
        if (_Hash.empty()) { // failed to compute a hash
            return false;
        }

        memory_traits::copy(_Entry._Account, _Hash.c_str(), _Hash.size());
    }

    { // generate a unique salt and hash a password 
        const _Unique_salt _Salt = _Generate_unique_salt();
        const byte_string& _Hash = _SDSDLL argon2id(_Password, _Salt);
        if (_Hash.empty()) { // failed to compute a hash
            return false;
        }

        memory_traits::copy(_Entry._Password, _Hash.c_str(), _Hash.size());
        memory_traits::copy(_Entry._Salt, _Salt.get(), _Unique_salt::size);
    }

    { // hash a unique ARC
        const arc& _Unique_arc   = _Generate_unique_arc();
        const byte_string& _Hash = _SDSDLL sha512(_Unique_arc.to_string());
        if (_Hash.empty()) { // failed to compute a hash
            return false;
        }
    
        memory_traits::copy(_Entry._Arc, _Hash.c_str(), _Hash.size());
        if (_Arc) { // save a new ARC
            *_Arc = _Unique_arc;
        }
    }
    
    _Myentries.push_back(_Entry);
    _Mychanges = true; // save changes
    return true;
}

_NODISCARD bool sudb_file::append_entry(
    const wstring& _Account, const wstring& _Password, arc* const _Arc) {
    if (has_entry(_Account) || _Myentries.size() >= 0xFFFF'FFFF) {
        return false;
    }

    _Sudb_entry _Entry;
    { // hash an account name
        const byte_string& _Hash = _SDSDLL xxhash(_Account);
        if (_Hash.empty()) { // failed to compute a hash
            return false;
        }

        memory_traits::copy(_Entry._Account, _Hash.c_str(), _Hash.size());
    }

    { // generate a unique salt and hash a password 
        const _Unique_salt _Salt = _Generate_unique_salt();
        const byte_string& _Hash = _SDSDLL argon2id(_Password, _Salt);
        if (_Hash.empty()) { // failed to compute a hash
            return false;
        }

        memory_traits::copy(_Entry._Password, _Hash.c_str(), _Hash.size());
        memory_traits::copy(_Entry._Salt, _Salt.get(), _Unique_salt::size);
    }

    { // hash a unique ARC
        const arc& _Unique_arc   = _Generate_unique_arc();
        const byte_string& _Hash = _SDSDLL sha512(_Unique_arc.to_string());
        if (_Hash.empty()) { // failed to compute a hash
            return false;
        }
    
        memory_traits::copy(_Entry._Arc, _Hash.c_str(), _Hash.size());
        if (_Arc) { // save a new ARC
            *_Arc = _Unique_arc;
        }
    }
    
    _Myentries.push_back(_Entry);
    _Mychanges = true; // save changes
    return true;
}

// FUNCTION sudb_file::erase_entry
void sudb_file::erase_entry(const wchar_t* const _Account) {
    if (!_Myok || _Myentries.empty()) {
        return;
    }

    using _Traits     = string_traits<wchar_t, size_t>;
    const size_t _Pos = _Find_entry_by_account_name(_Account, _Traits::length(_Account));
    if (_Pos != static_cast<size_t>(-1)) { // entry not found
        _Myentries.erase(_Myentries.cbegin() + _Pos);
        _Mychanges = true; // save changes
    }
}

void sudb_file::erase_entry(const wstring_view _Account) {
    if (!_Myok || _Myentries.empty()) {
        return;
    }

    const size_t _Pos = _Find_entry_by_account_name(_Account.data(), _Account.size());
    if (_Pos != static_cast<size_t>(-1)) { // entry not found
        _Myentries.erase(_Myentries.cbegin() + _Pos);
        _Mychanges = true; // save changes
    }
}

void sudb_file::erase_entry(const wstring& _Account) {
    if (!_Myok || _Myentries.empty()) {
        return;
    }

    const size_t _Pos = _Find_entry_by_account_name(_Account.c_str(), _Account.size());
    if (_Pos != static_cast<size_t>(-1)) { // entry not found
        _Myentries.erase(_Myentries.cbegin() + _Pos);
        _Mychanges = true; // save changes
    }
}

// FUNCTION sudb_file::erase_all_entries
void sudb_file::erase_all_entries() noexcept {
    if (!_Myok || _Myentries.empty()) {
        return;
    }

    _Myentries.clear();
    _Mychanges = true; // save changes
}
_SDSDLL_END

#endif // _SDSDLL_PREPROCESSOR_GUARD