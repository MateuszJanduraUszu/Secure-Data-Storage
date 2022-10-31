// scfg.cpp

// Copyright (c) Mateusz Jandura. All rights reserved.
// SPDX-License-Identifier: Apache-2.0

#include <build/sdsdll_pch.hpp>
#include <extensions/scfg.hpp>
#if _SDSDLL_PREPROCESSOR_GUARD

_SDSDLL_BEGIN
// FUNCTION _Extract_scfg_header_from_bytes
_NODISCARD _Scfg_header_data _Extract_scfg_header_from_bytes(const byte_string& _Bytes) {
    static constexpr size_t _Header_size = 44; // 44-byte header
    if (_Bytes.size() < _Header_size) { // incomplete header
        return {0, 0, 0, 0};
    }

    _Scfg_header_data _Result;
    uint8_t _As_bytes[4]; // 4-byte integer in bytes
    memory_traits::copy(_Result._Signature, _Bytes.c_str(), 4);
    memory_traits::copy(_Result._Magic, _Bytes.c_str() + 4, 4);
    memory_traits::copy(_Result._Checksum, _Bytes.c_str() + 8, 32);
    memory_traits::copy(_As_bytes, _Bytes.c_str() + 40, 4);
    _Result._Count = _SDSDLL pack_integer<uint32_t>(_As_bytes);
    return _Result;
}

// FUNCTION _Scfg_header constructors/destructor
_Scfg_header::_Scfg_header() noexcept : _Mydata{0, 0, 0, 0} {}

_Scfg_header::_Scfg_header(const byte_string& _Bytes) : _Mydata(_Extract_scfg_header_from_bytes(_Bytes)) {}

_Scfg_header::~_Scfg_header() noexcept {}

// FUNCTION _Scfg_header::_To_string
_NODISCARD byte_string _Scfg_header::_To_string() const {
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

// FUNCTION _Scfg_header::_Valid
_NODISCARD bool _Scfg_header::_Valid() const noexcept {
    using _Traits = string_traits<uint8_t, int>;
    return _Traits::compare(_Mydata._Signature, _Signature, 4) == 0
        && _Traits::compare(_Mydata._Magic, _Magic, 4) == 0;
}

// FUNCTION _Scfg_header::_Checksum
_NODISCARD const uint8_t* _Scfg_header::_Checksum() const noexcept {
    return _Mydata._Checksum;
}

void _Scfg_header::_Checksum(const uint8_t* const _New_checksum) noexcept {
    string_traits<uint8_t, int>::move(_Mydata._Checksum, _New_checksum, 32);
}

// FUNCTION _Scfg_header::_Entries
_NODISCARD const uint32_t _Scfg_header::_Entries() const noexcept {
    return _Mydata._Count;
}

void _Scfg_header::_Entries(const uint32_t _New_count) noexcept {
    _Mydata._Count = _New_count;
}

// FUNCTION _Scfg_header::_Reset_cache
void _Scfg_header::_Reset_cache(const _Scfg_header_data& _New_data) noexcept {
    using _Traits  = string_traits<uint8_t, int>;
    _Mydata._Count = _New_data._Count;
    _Traits::copy(_Mydata._Signature, _New_data._Signature, 4);
    _Traits::copy(_Mydata._Magic, _New_data._Magic, 4);
    _Traits::copy(_Mydata._Checksum, _New_data._Checksum, 32);
}

// FUNCTION _Scfg_entries_loader copy constructor/destructor
_Scfg_entries_loader::_Scfg_entries_loader(file* const _File, _Scfg_security* const _Security) noexcept
    : _Myfile(_File), _Mysec(_Security), _Myentry() {
    if (_Myfile && _Myfile->is_open()) { // skip the first 44 bytes (header)
        (void) _Myfile->seek(44);
    }
}

_Scfg_entries_loader::~_Scfg_entries_loader() noexcept {}

// FUNCTION _Scfg_entries_loader::_Next
_NODISCARD bool _Scfg_entries_loader::_Next() {
    if (!_Myfile || !_Myfile->is_open()) {
        return false;
    }

    static constexpr size_t _Count_and_hash_size = 10; // 2-byte integer + 8-byte hash
    uint8_t _Count_and_hash[_Count_and_hash_size];
    size_t _Read = 0; // read bytes, must be initialized
    if (!_Myfile->read(_Count_and_hash, _Count_and_hash_size, _Count_and_hash_size, &_Read)
        || _Read != _Count_and_hash_size) {
        return false;
    }

    memory_traits::copy(_Myentry._Id, _Count_and_hash + 2, 8);
    const uint16_t _Count = _SDSDLL pack_integer<uint16_t>({_Count_and_hash[0], _Count_and_hash[1]});
    
    // Note: The first 10 bytes are the encrypted value length and the hashed ID. The next
    //       n bytes are the encryped value. We must read the next n bytes and decrypt them.
    _Sbo_buffer<uint8_t> _Buf(static_cast<size_t>(_Count));
    if (_Buf._Empty()) { // allocation failed, break
        return false;
    }

    if (!_Myfile->read(_Buf._Get(), _Buf._Size(), _Buf._Size(), &_Read) || _Read != _Buf._Size()) {
        return false;
    }

    _Myentry._Value = _SDSDLL decrypt_aes256_gcm<wchar_t>(
        _Buf._Get(), _Buf._Size(), _Mysec->_Key, _Mysec->_Iv);
    return !_Myentry._Value.empty();
}

// FUNCTION _Scfg_entries_loader::_Get
_NODISCARD const _Scfg_entry& _Scfg_entries_loader::_Get() const noexcept {
    return _Myentry;
}

// FUNCTION scfg_file copy constructors/destructor
scfg_file::scfg_file(const path& _Target, const aes_key<32>& _Key, const iv<12>& _Iv)
    : _Myfile(_Target), _Myheader(), _Myentries(), _Mysec{_Key, _Iv},
    _Myok(_Load_file()), _Mychanges(false) {}

scfg_file::~scfg_file() noexcept {
    (void) flush();
}

// FUNCTION scfg_file::_Load_header
_NODISCARD bool scfg_file::_Load_header() {
    if (!_Myfile.is_open()) {
        return false;
    }
    
    size_t _Buf_size = 44; // 44-byte header
    byte_string _Buf(_Buf_size, uint8_t{});
    if (!_Myfile.read(_Buf, _Buf_size, &_Buf_size) || _Buf_size != _Buf.size()) {
        return false;
    }

    _Myheader._Reset_cache(_Extract_scfg_header_from_bytes(_Buf));
    return _Myheader._Valid();
}

// FUNCTION scfg_file::_Load_entries
_NODISCARD bool scfg_file::_Load_entries() {
    // Note: We do not need to check here if the file is open, because this function
    //       is only called if the _Load_header() succeeded.
    uint32_t _Count = _Myheader._Entries();
    if (_Count == 0) { // no entries to load
        return true;
    }

    if (!_Myfile.seek(44, file::beg)) {
        return false;
    }

    _Scfg_entries_loader _Loader(_SDSDLL addressof(_Myfile), _SDSDLL addressof(_Mysec));
    while (_Count-- > 0) {
        if (!_Loader._Next()) {
            _Myentries.clear();
            return false;
        }

        _Myentries.push_back(_Loader._Get());
    }

    return true;
}

// FUNCTION scfg_file::_Validate_checksum
_NODISCARD bool scfg_file::_Validate_checksum() noexcept {
    const byte_string_view _As_bytes(_Myheader._Checksum(), 32);
    return _As_bytes == _SDSDLL blake3_file(_Myfile, 40); // skip the first 40 bytes
}

// FUNCTION scfg_file::_Load_file
_NODISCARD bool scfg_file::_Load_file() noexcept {
    if (!_Load_header()) {
        return false;
    }

    if (_Validate_checksum()) {
        return _Load_entries();
    } else {
        return false;
    }
}

// FUNCTION scfg_file::_Find_entry
_NODISCARD size_t scfg_file::_Find_entry(const wchar_t* const _Id, const size_t _Size) const {
    if (!_Myok || _Myentries.empty()) {
        return static_cast<size_t>(-1);
    }
    
    const byte_string& _Hash = _SDSDLL xxhash(_Id, _Size);
    if (_Hash.empty()) { // failed to compute a hash
        return static_cast<size_t>(-1);
    }
    
    for (size_t _Idx = 0; _Idx < _Myentries.size(); ++_Idx) {
        if (_Hash == byte_string_view{_Myentries[_Idx]._Id, 8}) { // entry found
            return _Idx;
        }
    }

    return static_cast<size_t>(-1);
}

// FUNCTION scfg_file::_Write_entry
_NODISCARD bool scfg_file::_Write_entry(const _Scfg_entry& _Entry) {
    using _Traits              = string_traits<uint8_t, size_t>;
    const byte_string& _Cipher = _SDSDLL encrypt_aes256_gcm(_Entry._Value, _Mysec._Key, _Mysec._Iv);
    if (_Cipher.empty()) { // failed to compute a cipher
        return false;
    }

    const auto& _Len = _SDSDLL unpack_integer(static_cast<uint16_t>(_Cipher.size()));
    byte_string _Data(10 + _Cipher.size(), uint8_t{}); // 2-byte length + 8-byte hash + n-byte cipher
    _Traits::copy(_Data.data(), _Len.data(), _Len.size());
    _Traits::copy(_Data.data() + 2, _Entry._Id, 8);
    _Traits::copy(_Data.data() + 10, _Cipher.c_str(), _Cipher.size());
    return _Myfile.write(_Data);
}

// FUNCTION scfg_file::_Flush_buffers
bool scfg_file::_Flush_buffers() {
    // Note: The first step is to write the entries count (4-byte integer in bytes).
    //       Try to write it after the file checksum (40-byte offset).
    if (!_Myfile.resize(40)) {
        return false;
    }

    const auto& _Count = _SDSDLL unpack_integer(static_cast<uint32_t>(_Myentries.size()));
    if (!_Myfile.write(_Count.data(), _Count.size())) {
        return false;
    }

    // Note: The second step is to write all entries. The first 2 bytes are the length of the
    //       encrypted entry value. The next 8 bytes are the xxHash hash of the entry ID.
    //       The last n bytes are the encrypted entry value. Try to write it after the
    //       entries count (44-byte offset).
    for (const _Scfg_entry& _Entry : _Myentries) {
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

// FUNCTION scfg_file::make_storage
_NODISCARD bool scfg_file::make_storage(const path& _Target) {
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

    _Scfg_header _Header;
    const byte_string& _Checksum = _SDSDLL blake3("\x00\x00\x00\x00", 4); // 4-byte integer in bytes
    _Header._Checksum(_Checksum.c_str());
    return _File.write(_Header._To_string());
}

// FUNCTION scfg_file::ok
_NODISCARD const bool scfg_file::ok() const noexcept {
    return _Myok;
}

// FUNCTION scfg_file::set_key
void scfg_file::set_key(const aes_key<32>& _New_key) noexcept {
    _Mysec._Key = _New_key;
    if (_Myok) {
        _Mychanges = true; // encrypt data with a new key
    }
}

// FUNCTION scfg_file::set_iv
void scfg_file::set_iv(const iv<12>& _New_iv) noexcept {
    _Mysec._Iv = _New_iv;
    if (_Myok) {
        _Mychanges = true; // encrypt data with a new IV
    }
}

// FUNCTION scfg_file::refresh
void scfg_file::refresh() noexcept {
    _Myok = _Load_file();
}

// FUNCTION scfg_file::flush
_NODISCARD bool scfg_file::flush() noexcept {
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

// FUNCTION scfg_file::has_entry
_NODISCARD bool scfg_file::has_entry(const wchar_t* const _Id) const {
    if (!_Myok || _Myentries.empty()) {
        return false;
    }

    using _Traits = string_traits<wchar_t, size_t>;
    return _Find_entry(_Id, _Traits::length(_Id)) != static_cast<size_t>(-1);
}

_NODISCARD bool scfg_file::has_entry(const wstring_view _Id) const {
    if (!_Myok || _Myentries.empty()) {
        return false;
    }

    return _Find_entry(_Id.data(), _Id.size()) != static_cast<size_t>(-1);
}

_NODISCARD bool scfg_file::has_entry(const wstring& _Id) const {
    if (!_Myok || _Myentries.empty()) {
        return false;
    }

    return _Find_entry(_Id.c_str(), _Id.size()) != static_cast<size_t>(-1);
}

// FUNCTION scfg_file::query_entry
_NODISCARD wstring scfg_file::query_entry(const wchar_t* const _Id) const {
    if (!_Myok || _Myentries.empty()) {
        return wstring{};
    }

    using _Traits     = string_traits<wchar_t, size_t>;
    const size_t _Pos = _Find_entry(_Id, _Traits::length(_Id));
    return _Pos != static_cast<size_t>(-1) ? _Myentries[_Pos]._Value : wstring{};
}

_NODISCARD wstring scfg_file::query_entry(const wstring_view _Id) const {
    if (!_Myok || _Myentries.empty()) {
        return wstring{};
    }

    const size_t _Pos = _Find_entry(_Id.data(), _Id.size());
    return _Pos != static_cast<size_t>(-1) ? _Myentries[_Pos]._Value : wstring{};
}

_NODISCARD wstring scfg_file::query_entry(const wstring& _Id) const {
    if (!_Myok || _Myentries.empty()) {
        return wstring{};
    }

    const size_t _Pos = _Find_entry(_Id.c_str(), _Id.size());
    return _Pos != static_cast<size_t>(-1) ? _Myentries[_Pos]._Value : wstring{};
}

// FUNCTION scfg_file::modify_entry
_NODISCARD bool scfg_file::modify_entry(const wchar_t* const _Id, const wchar_t* const _New_value) {
    if (!_Myok || _Myentries.empty()) {
        return false;
    }

    using _Traits     = string_traits<wchar_t, size_t>;
    const size_t _Pos = _Find_entry(_Id, _Traits::length(_Id));
    if (_Pos == static_cast<size_t>(-1)) { // entry not found
        return false;
    }

    if (_Myentries[_Pos]._Value != _New_value) {
        _Myentries[_Pos]._Value = _New_value;
        _Mychanges              = true; // save changes
    }

    return true;
}

_NODISCARD bool scfg_file::modify_entry(const wstring_view _Id, const wstring_view _New_value) {
    if (!_Myok || _Myentries.empty()) {
        return false;
    }

    const size_t _Pos = _Find_entry(_Id.data(), _Id.size());
    if (_Pos == static_cast<size_t>(-1)) { // entry not found
        return false;
    }

    if (wstring_view{_Myentries[_Pos]._Value.c_str(), _Myentries[_Pos]._Value.size()}
        != _New_value) {
        _Myentries[_Pos]._Value = _New_value;
        _Mychanges              = true; // save changes
    }

    return true;
}

_NODISCARD bool scfg_file::modify_entry(const wstring& _Id, const wstring& _New_value) {
    if (!_Myok || _Myentries.empty()) {
        return false;
    }

    const size_t _Pos = _Find_entry(_Id.c_str(), _Id.size());
    if (_Pos == static_cast<size_t>(-1)) { // entry not found
        return false;
    }

    if (_Myentries[_Pos]._Value != _New_value) {
        _Myentries[_Pos]._Value = _New_value;
        _Mychanges              = true; // save changes
    }

    return true;
}

// FUNCTION scfg_file::modify_entry_id
_NODISCARD bool scfg_file::modify_entry_id(const wchar_t* const _Id, const wchar_t* const _New_id) {
    if (!_Myok || _Myentries.empty()) {
        return false;
    }

    using _Traits     = string_traits<wchar_t, size_t>;
    const size_t _Pos = _Find_entry(_Id, _Traits::length(_Id));
    if (_Pos == static_cast<size_t>(-1)) { // entry not found
        return false;
    }

    const byte_string& _Hash = _SDSDLL xxhash(_New_id);
    if (_Hash.empty()) { // failed to compute a hash
        return false;
    }

    if (_Hash.compare(0, 8, _Myentries[_Pos]._Id) != 0) {
        _Hash.copy(_Myentries[_Pos]._Id, 8);
        _Mychanges = true; // save changes
    }

    return true;
}

_NODISCARD bool scfg_file::modify_entry_id(const wstring_view _Id, const wstring_view _New_id) {
    if (!_Myok) {
        return false;
    }

    const size_t _Pos = _Find_entry(_Id.data(), _Id.size());
    if (_Pos == static_cast<size_t>(-1)) { // entry not found
        return false;
    }

    const byte_string& _Hash = _SDSDLL xxhash(_New_id);
    if (_Hash.empty()) { // failed to compute a hash
        return false;
    }

    if (_Hash.compare(0, 8, _Myentries[_Pos]._Id) != 0) {
        _Hash.copy(_Myentries[_Pos]._Id, 8);
        _Mychanges = true; // save changes
    }

    return true;
}

_NODISCARD bool scfg_file::modify_entry_id(const wstring& _Id, const wstring& _New_id) {
    if (!_Myok) {
        return false;
    }

    const size_t _Pos = _Find_entry(_Id.c_str(), _Id.size());
    if (_Pos == static_cast<size_t>(-1)) { // entry not found
        return false;
    }

    const byte_string& _Hash = _SDSDLL xxhash(_New_id);
    if (_Hash.empty()) { // failed to compute a hash
        return false;
    }

    if (_Hash.compare(0, 8, _Myentries[_Pos]._Id) != 0) {
        _Hash.copy(_Myentries[_Pos]._Id, 8);
        _Mychanges = true; // save changes
    }

    return true;
}

// FUNCTION scfg_file::append_entry
_NODISCARD bool scfg_file::append_entry(const wchar_t* const _Id, const wchar_t* const _Value) {
    if (has_entry(_Id) || _Myentries.size() >= 0xFFFF'FFFF) {
        return false;
    }

    const byte_string& _Hash = _SDSDLL xxhash(_Id);
    if (_Hash.empty()) { // failed to compute a hash
        return false;
    }

    _Scfg_entry _Entry;
    _Hash.copy(_Entry._Id, 8); // copy a 8-byte xxHash hash
    _Entry._Value = _Value;
    _Myentries.push_back(_Entry);
    _Mychanges = true; // save changes
    return true;
}

_NODISCARD bool scfg_file::append_entry(const wstring_view _Id, const wstring_view _Value) {
    if (has_entry(_Id) || _Myentries.size() >= 0xFFFF'FFFF) {
        return false;
    }

    const byte_string& _Hash = _SDSDLL xxhash(_Id);
    if (_Hash.empty()) { // failed to compute a hash
        return false;
    }

    _Scfg_entry _Entry;
    _Hash.copy(_Entry._Id, 8); // copy 8-byte xxHash hash
    _Entry._Value = _Value;
    _Myentries.push_back(_Entry);
    _Mychanges = true; // save changes
    return true;
}

_NODISCARD bool scfg_file::append_entry(const wstring& _Id, const wstring& _Value) {
    if (has_entry(_Id) || _Myentries.size() >= 0xFFFF'FFFF) {
        return false;
    }

    const byte_string& _Hash = _SDSDLL xxhash(_Id);
    if (_Hash.empty()) { // failed to compute a hash
        return false;
    }

    _Scfg_entry _Entry;
    _Hash.copy(_Entry._Id, 8); // copy 8-byte xxHash hash
    _Entry._Value = _Value;
    _Myentries.push_back(_Entry);
    _Mychanges = true; // save changes
    return true;
}

// FUNCTION scfg_file::erase_entry
void scfg_file::erase_entry(const wchar_t* const _Id) {
    if (!_Myok || _Myentries.empty()) {
        return;
    }

    using _Traits     = string_traits<wchar_t, size_t>;
    const size_t _Pos = _Find_entry(_Id, _Traits::length(_Id));
    if (_Pos != static_cast<size_t>(-1)) { // entry found, erase it
        _Myentries.erase(_Myentries.cbegin() + _Pos);
        _Mychanges = true; // save changes
    }
}

void scfg_file::erase_entry(const wstring_view _Id) {
    if (!_Myok || _Myentries.empty()) {
        return;
    }

    const size_t _Pos = _Find_entry(_Id.data(), _Id.size());
    if (_Pos != static_cast<size_t>(-1)) { // entry found, erase it
        _Myentries.erase(_Myentries.cbegin() + _Pos);
        _Mychanges = true; // save changes
    }
}

void scfg_file::erase_entry(const wstring& _Id) {
    if (!_Myok || _Myentries.empty()) {
        return;
    }

    const size_t _Pos = _Find_entry(_Id.c_str(), _Id.size());
    if (_Pos != static_cast<size_t>(-1)) { // entry found, erase it
        _Myentries.erase(_Myentries.cbegin() + _Pos);
        _Mychanges = true; // save changes
    }
}

// FUNCTION scfg_file::erase_all_entries
void scfg_file::erase_all_entries() noexcept {
    if (!_Myok || _Myentries.empty()) {
        return;
    }

    _Myentries.clear();
    _Mychanges = true; // save changes
}
_SDSDLL_END

#endif // _SDSDLL_PREPROCESSOR_GUARD