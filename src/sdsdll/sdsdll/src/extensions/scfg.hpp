// scfg.hpp

// Copyright (c) Mateusz Jandura. All rights reserved.
// SPDX-License-Identifier: Apache-2.0

#pragma once
#ifndef _SDSDLL_EXTENSIONS_SCFG_HPP_
#define _SDSDLL_EXTENSIONS_SCFG_HPP_
#include <core/defs.hpp>
#if _SDSDLL_PREPROCESSOR_GUARD
#include <array>
#include <core/api.hpp>
#include <core/optimization/sbo.hpp>
#include <core/traits/integer.hpp>
#include <core/traits/memory_traits.hpp>
#include <core/traits/string_traits.hpp>
#include <core/traits/type_traits.hpp>
#include <cryptography/cipher/symmetric/aes256_gcm.hpp>
#include <cryptography/cipher/symmetric/aes_key.hpp>
#include <cryptography/cipher/symmetric/iv.hpp>
#include <cryptography/hash/generic/blake3.hpp>
#include <cryptography/hash/generic/xxhash.hpp>
#include <cstddef>
#include <cstdint>
#include <filesystem/file.hpp>
#include <filesystem/path.hpp>
#include <filesystem/status.hpp>
#include <string>
#include <vector>

// STD types
using _STD array;
using _STD string;
using _STD wstring;
using _STD vector;

_SDSDLL_BEGIN
// STRUCT _Scfg_header_data
struct _Scfg_header_data {
    uint8_t _Signature[4]; // 4-byte signature
    uint8_t _Magic[4]; // 4-byte magic value
    uint8_t _Checksum[32]; // 32-byte BLAKE3 checksum
    uint32_t _Count; // entries count
};

// FUNCTION _Extract_scfg_header_from_bytes
extern _NODISCARD _Scfg_header_data _Extract_scfg_header_from_bytes(const byte_string& _Bytes);

// CLASS _Scfg_header
class _Scfg_header {
public:
    _Scfg_header() noexcept;
    ~_Scfg_header() noexcept;

    explicit _Scfg_header(const byte_string& _Bytes);

    _Scfg_header(const _Scfg_header&) = delete;
    _Scfg_header& operator=(const _Scfg_header&) = delete;

    // constructs the header from the data
    _NODISCARD byte_string _To_string() const;

    // checks if the signature and the magic are valid
    _NODISCARD bool _Valid() const noexcept;

    // returns a file checksum
    _NODISCARD const uint8_t* _Checksum() const noexcept;

    // changes a file checksum
    void _Checksum(const uint8_t* const _New_checksum) noexcept;

    // returns the number of entries
    _NODISCARD const uint32_t _Entries() const noexcept;

    // changes the number of entries
    void _Entries(const uint32_t _New_count) noexcept;

    // resets cached data
    void _Reset_cache(const _Scfg_header_data& _New_data) noexcept;

private:
    static constexpr uint8_t _Signature[] = {0x4D, 0x4A, 0x00, 0x00}; // correct signature
    static constexpr uint8_t _Magic[]     = {0x00, 0x5D, 0x5C, 0xF6}; // correct magic value

    _Scfg_header_data _Mydata;
};

// STRUCT _Scfg_entry
struct _Scfg_entry {
    uint8_t _Id[8]; // 8-byte entry ID xxHash hash
    wstring _Value; // decrypted entry value
};

// STRUCT _Scfg_security
struct _Scfg_security {
    aes_key<32> _Key;
    iv<12> _Iv;
};

// CLASS _Scfg_entries_loader
class _Scfg_entries_loader {
public:
    _Scfg_entries_loader(file* const _File, _Scfg_security* const _Security) noexcept;
    ~_Scfg_entries_loader() noexcept;

    _Scfg_entries_loader() = delete;
    _Scfg_entries_loader(const _Scfg_entries_loader&) = delete;
    _Scfg_entries_loader& operator=(const _Scfg_entries_loader&) = delete;

    // tries to load the next entry
    _NODISCARD bool _Next();

    // return the last loaded entry
    _NODISCARD const _Scfg_entry& _Get() const noexcept;

private:
    file* const _Myfile;
    _Scfg_security* const _Mysec;
    _Scfg_entry _Myentry;
};

// CLASS scfg_file
class _SDSDLL_API scfg_file { // manages SCFG file reading/writing
public:
    scfg_file(const path& _Target, const aes_key<32>& _Key, const iv<12>& _Iv);
    ~scfg_file() noexcept;
    
    scfg_file() = delete;
    scfg_file(const scfg_file&) = delete;
    scfg_file& operator=(const scfg_file&) = delete;

    // creates a new file or clears an existing file and writes a header into it
    _NODISCARD static bool make_storage(const path& _Target);

    // checks if everything is ok
    _NODISCARD const bool ok() const noexcept;

    // changes the current AES-256 GCM key
    void set_key(const aes_key<32>& _New_key) noexcept;

    // changes the current IV
    void set_iv(const iv<12>& _New_iv) noexcept;

    // loads the file again
    void refresh() noexcept;

    // saves the changes
    _NODISCARD bool flush() noexcept;

    // checks if the storage has the selected entry
    _NODISCARD bool has_entry(const wchar_t* const _Id) const;
    _NODISCARD bool has_entry(const wstring_view _Id) const;
    _NODISCARD bool has_entry(const wstring& _Id) const;

    // returns the selected entry value
    _NODISCARD wstring query_entry(const wchar_t* const _Id) const;
    _NODISCARD wstring query_entry(const wstring_view _Id) const;
    _NODISCARD wstring query_entry(const wstring& _Id) const;

    // modifies the selected entry value
    _NODISCARD bool modify_entry(const wchar_t* const _Id, const wchar_t* const _New_value);
    _NODISCARD bool modify_entry(const wstring_view _Id, const wstring_view _New_value);
    _NODISCARD bool modify_entry(const wstring& _Id, const wstring& _New_value);

    // modifies the selected entry ID
    _NODISCARD bool modify_entry_id(const wchar_t* const _Id, const wchar_t* const _New_id);
    _NODISCARD bool modify_entry_id(const wstring_view _Id, const wstring_view _New_id);
    _NODISCARD bool modify_entry_id(const wstring& _Id, const wstring& _New_id);

    // appends a new entry
    _NODISCARD bool append_entry(const wchar_t* const _Id, const wchar_t* const _Value);
    _NODISCARD bool append_entry(const wstring_view _Id, const wstring_view _Value);
    _NODISCARD bool append_entry(const wstring& _Id, const wstring& _Value);

    // erases an existing entry
    void erase_entry(const wchar_t* const _Id);
    void erase_entry(const wstring_view _Id);
    void erase_entry(const wstring& _Id);

    // erases all entries
    void erase_all_entries() noexcept;

private:
    // loads the header from a file
    _NODISCARD bool _Load_header();

    // loads the entries from a file
    _NODISCARD bool _Load_entries();

    // validates the checksum from a file
    _NODISCARD bool _Validate_checksum() noexcept;

    // loads the file header and the entries
    _NODISCARD bool _Load_file() noexcept;

    // returns the selected entry position, -1 if not found
    _NODISCARD size_t _Find_entry(const wchar_t* const _Id, const size_t _Size) const;

    // writes a single entry
    _NODISCARD bool _Write_entry(const _Scfg_entry& _Entry);

    // saves changes into the file
    bool _Flush_buffers();

#ifdef _MSC_VER
#pragma warning(push, 1)
#pragma warning(disable : 4251) // C4251: file, _Scfg_header, std::vector and _Scfg_security
                                //        require dll-interface
#endif // _MSC_VER
    file _Myfile;
    _Scfg_header _Myheader;
    vector<_Scfg_entry> _Myentries;
    _Scfg_security _Mysec; // AES-256 GCM key and IV
    bool _Myok; // true if everything is ok
    bool _Mychanges; // true if any data has been changed
#ifdef _MSC_VER
#pragma warning(pop)
#endif // _MSC_VER
};
_SDSDLL_END

#endif // _SDSDLL_PREPROCESSOR_GUARD
#endif // _SDSDLL_EXTENSIONS_SCFG_HPP_