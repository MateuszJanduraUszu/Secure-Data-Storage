// date.hpp

// Copyright (c) Mateusz Jandura. All rights reserved.
// SPDX-License-Identifier: Apache-2.0

#pragma once
#ifndef _SDSDLL_TIME_ZONE_DATE_HPP_
#define _SDSDLL_TIME_ZONE_DATE_HPP_
#include <core/defs.hpp>
#if _SDSDLL_PREPROCESSOR_GUARD
#include <core/api.hpp>
#include <core/optimization/string_view.hpp>
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
// CONSTANT TEMPLATE _Day_table
template <bool _Leap>
_INLINE_VARIABLE constexpr uint8_t _Day_table[12] = {
    31, _Leap ? 29 : 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31
};

// FUNCTION _Get_days
extern _NODISCARD uint8_t _Get_days(const uint8_t _Month, const bool _Leap) noexcept;

// FUNCTION _Is_leap_yearnmo
extern _NODISCARD bool _Is_leap_year(const uint16_t _Year) noexcept;

// CLASS undefined_date
class _SDSDLL_API undefined_date {
public:
    undefined_date() noexcept;
    undefined_date(const undefined_date& _Other) noexcept;
    undefined_date(undefined_date&& _Other) noexcept;
    ~undefined_date() noexcept;

    undefined_date& operator=(const undefined_date& _Other) noexcept;
    undefined_date& operator=(undefined_date&& _Other) noexcept;

    explicit undefined_date(const uint8_t _Day, const uint8_t _Month, const uint16_t _Year) noexcept;

    // checks if the date is valid
    _NODISCARD bool valid() const noexcept;

    // checks if the date is equal
    _NODISCARD bool operator==(const undefined_date& _Other) const noexcept;

    // checks if the date is not equal
    _NODISCARD bool operator!=(const undefined_date& _Other) const noexcept;

    // checks if the date is greater
    _NODISCARD bool operator>(const undefined_date& _Other) const noexcept;

    // checks if the date is greater or equal
    _NODISCARD bool operator>=(const undefined_date& _Other) const noexcept;

    // checks if the date is less
    _NODISCARD bool operator<(const undefined_date& _Other) const noexcept;

    // checks if the date is less or equal
    _NODISCARD bool operator<=(const undefined_date& _Other) const noexcept;

    uint8_t day;
    uint8_t month;
    uint16_t year;
};

// CLASS gregorian_calendar
class _SDSDLL_API gregorian_calendar : public undefined_date { // Gregorian calendar (global date)
public:
    gregorian_calendar() noexcept;
    gregorian_calendar(const gregorian_calendar& _Other) noexcept;
    gregorian_calendar(gregorian_calendar&& _Other) noexcept;
    ~gregorian_calendar() noexcept;

    gregorian_calendar& operator=(const gregorian_calendar& _Other) noexcept;
    gregorian_calendar& operator=(gregorian_calendar&& _Other) noexcept;

    explicit gregorian_calendar(const undefined_date& _Time) noexcept;
    explicit gregorian_calendar(const uint8_t _Day, const uint8_t _Month, const uint16_t _Year) noexcept;

    // returns the current global date
    _NODISCARD static gregorian_calendar current() noexcept;
};

// ENUM CLASS date_format
enum class date_format : unsigned char {
    general,
    american,
    british,
    iso8601
};

// CLASS TEMPLATE _Date_formatter
template <class _Elem>
class _Date_formatter { // formats undefined_date into the selected date format
public:
    using _Value_type = basic_string<_Elem>;

    explicit _Date_formatter(const undefined_date& _Date, const date_format _Fmt) noexcept;

    _Date_formatter() = delete;
    _Date_formatter(const _Date_formatter&) = delete;
    _Date_formatter& operator=(const _Date_formatter&) = delete;

    // returns formatted date
    _NODISCARD const _Value_type& _Get() const noexcept;

private:
    // appends date number and delimiter if needs
    void _Append_number(const uint8_t _Num, const _Elem _Delim, const bool _Use_delim) noexcept;
    void _Append_number(const uint16_t _Num, const _Elem _Delim, const bool _Use_delim) noexcept;

    _Value_type _Mystr;
};

// FUNCTION TEMPLATE format_date
template <class _Elem>
_SDSDLL_API _NODISCARD constexpr basic_string<_Elem> format_date(
    const undefined_date& _Date, const date_format _Fmt = date_format::general);

// STRUCT date_difference_result
struct date_difference_result {
    uint8_t days;
    uint8_t months;
    uint16_t years;
};

// FUNCTION date_difference
_SDSDLL_API _NODISCARD date_difference_result date_difference(
    const undefined_date& _Left, const undefined_date& _Right) noexcept;
_SDSDLL_END

#endif // _SDSDLL_TIME_ZONE_DATE_HPP_
#endif // _SDSDLL_PREPROCESSOR_GUARD