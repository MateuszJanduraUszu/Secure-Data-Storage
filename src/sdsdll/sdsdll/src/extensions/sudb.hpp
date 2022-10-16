// sudb.hpp

// Copyright (c) Mateusz Jandura. All rights reserved.
// SPDX-License-Identifier: Apache-2.0

#pragma once
#ifndef _SDSDLL_EXTENSIONS_SUDB_HPP_
#define _SDSDLL_EXTENSIONS_SUDB_HPP_
#include <core/defs.hpp>
#if _SDSDLL_PREPROCESSOR_GUARD
#include <array>
#include <core/api.hpp>
#include <core/optimization/sbo.hpp>
#include <core/optimization/string_view.hpp>
#include <core/traits/integer.hpp>
#include <core/traits/memory_traits.hpp>
#include <core/traits/string_traits.hpp>
#include <core/traits/type_traits.hpp>
#include <cryptography/hash/generic/blake3.hpp>
#include <cryptography/hash/generic/sha512.hpp>
#include <cryptography/hash/generic/xxhash.hpp>
#include <cryptography/hash/password/argon2id.hpp>
#include <cryptography/random/salt.hpp>
#include <cstddef>
#include <cstdint>
#include <filesystem/file.hpp>
#include <filesystem/path.hpp>
#include <filesystem/status.hpp>
#include <openssl/rand.h>
#include <recovery/arc.hpp>
#include <string>
#include <vector>

// STD types
using _STD array;
using _STD string;
using _STD wstring;
using _STD vector;

_SDSDLL_BEGIN
// STRUCT _Sudb_header_data
struct _Sudb_header_data {
    uint8_t _Signature[4]; // 4-byte signature
    uint8_t _Magic[4]; // 4-byte magic value
    uint8_t _Checksum[32]; // 32-byte BLAKE3 checksum
    uint32_t _Count; // entries count
};

// FUNCTION _Extract_sudb_header_from_bytes
extern _NODISCARD _Sudb_header_data _Extract_sudb_header_from_bytes(const byte_string& _Bytes);

// CLASS _Sudb_header
class _Sudb_header {
public:
    _Sudb_header() noexcept;
    ~_Sudb_header() noexcept;

    explicit _Sudb_header(const byte_string& _Bytes);

    _Sudb_header(const _Sudb_header&) = delete;
    _Sudb_header& operator=(const _Sudb_header&) = delete;

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
    void _Reset_cache(const _Sudb_header_data& _New_data) noexcept;

private:
    static constexpr uint8_t _Signature[] = {0x4D, 0x4A, 0x00, 0x00}; // correct signature
    static constexpr uint8_t _Magic[]     = {0x00, 0x5D, 0x50, 0xDB}; // correct magic value

    _Sudb_header_data _Mydata;
};

// STRUCT _Sudb_entry
struct _Sudb_entry {
    uint8_t _Account[8]; // 8-byte xxHash account name
    uint8_t _Password[64]; // 64-byte Argon2id password
    uint8_t _Salt[16]; // 16-byte unique salt
    uint8_t _Arc[64]; // 64-byte SHA-512 Account Recovery Code
};

// CLASS _Sudb_entries_loader
class _Sudb_entries_loader {
public:
    _Sudb_entries_loader(file* const _File) noexcept;
    ~_Sudb_entries_loader() noexcept;

    _Sudb_entries_loader() = delete;
    _Sudb_entries_loader(const _Sudb_entries_loader&) = delete;
    _Sudb_entries_loader& operator=(const _Sudb_entries_loader&) = delete;

    // tries to load the next entry
    _NODISCARD bool _Next();

    // return the last loaded entry
    _NODISCARD const _Sudb_entry& _Get() const noexcept;

private:
    file* const _Myfile;
    _Sudb_entry _Myentry;
};

// CLASS sudb_file
class _SDSDLL_API sudb_file { // manages SUDB file reading/writing
private:
    using _Unique_salt = salt<_Argon2id_default_engine<wchar_t>>;

public:
    explicit sudb_file(const path& _Target);
    ~sudb_file() noexcept;

    sudb_file() = delete;
    sudb_file(const sudb_file&) = delete;
    sudb_file& operator=(const sudb_file&) = delete;

    // creates a new file or clears an existing file and writes a header into it
    _NODISCARD static bool make_storage(const path& _Target);

    // checks if everything is ok
    _NODISCARD const bool ok() const noexcept;

    // loads the file again
    void refresh() noexcept;

    // saves the changes
    _NODISCARD bool flush() noexcept;

    // checks if the storage has the selected entry
    _NODISCARD bool has_entry(const wchar_t* const _Account) const;
    _NODISCARD bool has_entry(const wstring_view _Account) const;
    _NODISCARD bool has_entry(const wstring& _Account) const;
    _NODISCARD bool has_entry(const arc& _Arc) const;

    // checks if the selected password is correct
    _NODISCARD bool compare_passwords(const wchar_t* const _Account, const wchar_t* const _Password) const;
    _NODISCARD bool compare_passwords(const wstring_view _Account, const wstring_view _Password) const;
    _NODISCARD bool compare_passwords(const wstring& _Account, const wstring& _Password) const;

    // checks if the selected ARC is correct
    _NODISCARD bool compare_arcs(const wchar_t* const _Account, const arc& _Arc) const;
    _NODISCARD bool compare_arcs(const wstring_view _Account, const arc& _Arc) const;
    _NODISCARD bool compare_arcs(const wstring& _Account, const arc& _Arc) const;

    // modifies the selected entry account name
    _NODISCARD bool modify_entry_account_name(const wchar_t* const _Account, const wchar_t* const _New_name);
    _NODISCARD bool modify_entry_account_name(const wstring_view _Account, const wstring_view _New_name);
    _NODISCARD bool modify_entry_account_name(const wstring& _Account, const wstring& _New_name);

    // modifies the selected entry password
    _NODISCARD bool modify_entry_password(const wchar_t* const _Account, const wchar_t* const _New_password);
    _NODISCARD bool modify_entry_password(const wstring_view _Account, const wstring_view _New_password);
    _NODISCARD bool modify_entry_password(const wstring& _Account, const wstring& _New_password);

    // modifies the selected entry ARC (generated automatically)
    _NODISCARD bool modify_entry_arc(const wchar_t* const _Account) noexcept;
    _NODISCARD bool modify_entry_arc(const wstring_view _Account) noexcept;
    _NODISCARD bool modify_entry_arc(const wstring& _Account);

    // appends a new entry
    _NODISCARD bool append_entry(
        const wchar_t* const _Account, const wchar_t* const _Password, arc* const _Arc = nullptr);
    _NODISCARD bool append_entry(
        const wstring_view _Account, const wstring_view _Password, arc* const _Arc = nullptr);
    _NODISCARD bool append_entry(
        const wstring& _Account, const wstring& _Password, arc* const _Arc = nullptr);

    // erases an existing entry
    void erase_entry(const wchar_t* const _Account);
    void erase_entry(const wstring_view _Account);
    void erase_entry(const wstring& _Account);

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

    // returns the selected entry position (-1 if not found), searches by account name
    _NODISCARD size_t _Find_entry_by_account_name(const wchar_t* const _Name, const size_t _Size) const;

    // returns the selected entry position (-1 if not found), searches by ARC
    _NODISCARD size_t _Find_entry_by_arc(const arc& _Arc) const;

    // checks if the selected ARC is unique
    _NODISCARD bool _Is_unique_arc(const arc& _Arc) const noexcept;

    // checks if the selected salt is unique
    _NODISCARD bool _Is_unique_salt(const _Unique_salt& _Salt) const noexcept;

    // generates a new ARC
    _NODISCARD arc _Generate_unique_arc() const noexcept;

    // generates a new salt
    _NODISCARD _Unique_salt _Generate_unique_salt() const noexcept;

    // writes a single entry
    _NODISCARD bool _Write_entry(const _Sudb_entry& _Entry);

    // saves changes into the file
    _NODISCARD bool _Flush_buffers();

#ifdef _MSC_VER
#pragma warning(push, 1)
#pragma warning(disable : 4251) // C4251: file, _Sudb_header and std::vector require dll-interface
#endif // _MSC_VER
    file _Myfile;
    _Sudb_header _Myheader;
    vector<_Sudb_entry> _Myentries;
    bool _Myok; // true if everything is ok
    bool _Mychanges; // true if any data has been changed
#ifdef _MSC_VER
#pragma warning(pop)
#endif // _MSC_VER
};
_SDSDLL_END

#endif // _SDSDLL_PREPROCESSOR_GUARD
#endif // _SDSDLL_EXTENSIONS_SUDB_HPP_