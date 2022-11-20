// time.hpp

// Copyright (c) Mateusz Jandura. All rights reserved.
// SPDX-License-Identifier: Apache-2.0

#pragma once
#ifndef _SDSDLL_TIME_ZONE_TIME_HPP_
#define _SDSDLL_TIME_ZONE_TIME_HPP_
#include <core/defs.hpp>
#if _SDSDLL_PREPROCESSOR_GUARD
#include <core/api.hpp>
#include <core/traits/type_traits.hpp>
#include <cstdint>
#include <minwinbase.h>
#include <string>
#include <sysinfoapi.h>
#include <type_traits>

// STD types
using _STD basic_string;
using _STD string;
using _STD wstring;

_SDSDLL_BEGIN
// CLASS undefined_time
class undefined_time;

// FUNCTION _Time_to_seconds
extern _NODISCARD uint32_t _Time_to_seconds(const undefined_time& _Time) noexcept;

// FUNCTION _Time_from_seconds
extern _NODISCARD undefined_time _Time_from_seconds(const uint32_t _Seconds) noexcept;

// CLASS undefined_time
class _SDSDLL_API undefined_time {
public:
    undefined_time() noexcept;
    undefined_time(const undefined_time& _Other) noexcept;
    undefined_time(undefined_time&& _Other) noexcept;
    ~undefined_time() noexcept;

    undefined_time& operator=(const undefined_time& _Other) noexcept;
    undefined_time& operator=(undefined_time&& _Other) noexcept;

    explicit undefined_time(const uint8_t _Hour, const uint8_t _Minute, const uint8_t _Second) noexcept;

    // checks if the time is valid
    _NODISCARD bool valid() const noexcept;

    // checks if the time is equal
    _NODISCARD bool operator==(const undefined_time& _Other) const noexcept;
    
    // checks if the time is not equal
    _NODISCARD bool operator!=(const undefined_time& _Other) const noexcept;

    // checks if the time is greater
    _NODISCARD bool operator>(const undefined_time& _Other) const noexcept;

    // checks if the time is greater or equal
    _NODISCARD bool operator>=(const undefined_time& _Other) const noexcept;

    // checks if the time is less
    _NODISCARD bool operator<(const undefined_time& _Other) const noexcept;

    // checks if the time is less or equal
    _NODISCARD bool operator<=(const undefined_time& _Other) const noexcept;

    uint8_t hour;
    uint8_t minute;
    uint8_t second;
};

// CLASS utc_time
class _SDSDLL_API utc_time : public undefined_time { // Universal Time Coordinated (UTC) time
public:
    utc_time() noexcept;
    utc_time(const utc_time& _Other) noexcept;
    utc_time(utc_time&& _Other) noexcept;
    ~utc_time() noexcept;

    utc_time& operator=(const utc_time& _Other) noexcept;
    utc_time& operator=(utc_time&& _Other) noexcept;

    explicit utc_time(const undefined_time& _Time) noexcept;
    explicit utc_time(const uint8_t _Hour, const uint8_t _Minute, const uint8_t _Second) noexcept;

    // returns the current UTC time
    _NODISCARD static utc_time current() noexcept;
};

// CLASS local_time
class _SDSDLL_API local_time : public undefined_time { // local time zone time
public:
    local_time() noexcept;
    local_time(const local_time& _Other) noexcept;
    local_time(local_time&& _Other) noexcept;
    ~local_time() noexcept;

    local_time& operator=(const local_time& _Other) noexcept;
    local_time& operator=(local_time&& _Other) noexcept;

    explicit local_time(const undefined_time& _Time) noexcept;
    explicit local_time(const uint8_t _Hour, const uint8_t _Minute, const uint8_t _Second) noexcept;

    // returns the current local time
    _NODISCARD static local_time current() noexcept;
};

// CLASS TEMPLATE _Time_formatter
template <class _Elem>
class _Time_formatter { // formats undefined_time into hh:mm:ss format
public:
    using _Value_type = basic_string<_Elem>;

    explicit _Time_formatter(const undefined_time& _Time) noexcept;

    _Time_formatter() = delete;
    _Time_formatter(const _Time_formatter&) = delete;
    _Time_formatter& operator=(const _Time_formatter&) = delete;

    // returns formatted time
    _NODISCARD const _Value_type& _Get() const noexcept;

private:
    // appends time number and delimiter if needs
    void _Append_number(const uint8_t _Num, const bool _Delim) noexcept;

    _Value_type _Mystr;
};

// FUNCTION TEMPLATE format_time
template <class _Elem>
_SDSDLL_API _NODISCARD constexpr basic_string<_Elem> format_time(const undefined_time& _Time);

// FUNCTION time_difference
_SDSDLL_API _NODISCARD undefined_time time_difference(
    const undefined_time& _Left, const undefined_time& _Right) noexcept;
_SDSDLL_END

#endif // _SDSDLL_PREPROCESSOR_GUARD
#endif // _SDSDLL_TIME_ZONE_TIME_HPP_