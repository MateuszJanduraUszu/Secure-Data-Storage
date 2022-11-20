// date.cpp

// Copyright (c) Mateusz Jandura. All rights reserved.
// SPDX-License-Identifier: Apache-2.0

#include <build/sdsdll_pch.hpp>
#include <time_zone/date.hpp>
#if _SDSDLL_PREPROCESSOR_GUARD

_SDSDLL_BEGIN
// FUNCTION _Get_days
_NODISCARD uint8_t _Get_days(const uint8_t _Month, const bool _Leap) noexcept {
    return _Leap ? _Day_table<true>[_Month - 1] : _Day_table<false>[_Month - 1];
}

// FUNCTION _Is_leap_year
_NODISCARD bool _Is_leap_year(const uint16_t _Year) noexcept {
    return (_Year % 4 == 0 && _Year % 100 != 0) || _Year % 400 == 0;
}

// FUNCTION undefined_date constructors/destructor
undefined_date::undefined_date() noexcept : day(0), month(0), year(0) {}

undefined_date::undefined_date(const undefined_date& _Other) noexcept
    : day(_Other.day), month(_Other.month), year(_Other.year) {}

undefined_date::undefined_date(undefined_date&& _Other) noexcept
    : day(_Other.day), month(_Other.month), year(_Other.year) {}

undefined_date::undefined_date(
    const uint8_t _Day, const uint8_t _Month, const uint16_t _Year) noexcept
    : day(_Day), month(_Month), year(_Year) {}

undefined_date::~undefined_date() noexcept {}

// FUNCTION undefined_date::operator=
undefined_date& undefined_date::operator=(const undefined_date& _Other) noexcept {
    if (this != _SDSDLL addressof(_Other)) {
        day   = _Other.day;
        month = _Other.month;
        year  = _Other.year;
    }

    return *this;
}

undefined_date& undefined_date::operator=(undefined_date&& _Other) noexcept {
    if (this != _SDSDLL addressof(_Other)) {
        day   = _Other.day;
        month = _Other.month;
        year  = _Other.year;
    }

    return *this;
}

// FUNCTION undefined_date::valid
_NODISCARD bool undefined_date::valid() const noexcept {
    if (year > 9999) { // must fit in 4 characters
        return false;
    }

    if (month < 1 || month > 12) { // must be at least 1 and at most 12
        return false;
    }

    return day > 0 && day <= _Get_days(month, _Is_leap_year(year));
}

// FUNCTION undefined_date::operator==
_NODISCARD bool undefined_date::operator==(const undefined_date& _Other) const noexcept {
    return day == _Other.day && month == _Other.month && year == _Other.year;
}

// FUNCTION undefined_date::operator!=
_NODISCARD bool undefined_date::operator!=(const undefined_date& _Other) const noexcept {
    return !(*this == _Other);
}

// FUNCTION undefined_date::operator>
_NODISCARD bool undefined_date::operator>(const undefined_date& _Other) const noexcept {
    if (year != _Other.year) { // months and days are irrelevant in this case
        return year > _Other.year;
    }

    return month != _Other.month ? month > _Other.month : day > _Other.day;
}

// FUNCTION undefined_date::operator>=
_NODISCARD bool undefined_date::operator>=(const undefined_date& _Other) const noexcept {
    return *this > _Other || *this == _Other;
}

// FUNCTION undefined_date::operator<
_NODISCARD bool undefined_date::operator<(const undefined_date& _Other) const noexcept {
    return *this == _Other ? false : !(*this > _Other);
}

// FUNCTION undefined_date::operator<=
_NODISCARD bool undefined_date::operator<=(const undefined_date& _Other) const noexcept {
    return *this < _Other || *this == _Other;
}

// FUNCTION gregorian_calendar constructors/destructor
gregorian_calendar::gregorian_calendar() noexcept : undefined_date() {}

gregorian_calendar::gregorian_calendar(const gregorian_calendar& _Other) noexcept
    : undefined_date(_Other) {}

gregorian_calendar::gregorian_calendar(gregorian_calendar&& _Other) noexcept
    : undefined_date(_STD move(_Other)) {}

gregorian_calendar::gregorian_calendar(const undefined_date& _Date) noexcept
    : undefined_date(_Date) {}

gregorian_calendar::gregorian_calendar(
    const uint8_t _Day, const uint8_t _Month, const uint16_t _Year) noexcept
    : undefined_date(_Day, _Month, _Year) {}

gregorian_calendar::~gregorian_calendar() noexcept {}

// FUNCTION gregorian_calendar::operator=
gregorian_calendar& gregorian_calendar::operator=(const gregorian_calendar& _Other) noexcept {
    undefined_date::operator=(_Other);
    return *this;
}

gregorian_calendar& gregorian_calendar::operator=(gregorian_calendar&& _Other) noexcept {
    undefined_date::operator=(_STD move(_Other));
    return *this;
}

// FUNCTION gregorian_calendar::current
_NODISCARD gregorian_calendar gregorian_calendar::current() noexcept {
    SYSTEMTIME _Time;
    ::GetSystemTime(_SDSDLL addressof(_Time));
    return gregorian_calendar{
        static_cast<uint8_t>(_Time.wDay), static_cast<uint8_t>(_Time.wMonth), _Time.wYear};
}

// FUNCTION TEMPLATE _Date_formatter copy constructor
template <class _Elem>
_Date_formatter<_Elem>::_Date_formatter(
    const undefined_date& _Date, const date_format _Fmt) noexcept : _Mystr() {
    _Mystr.reserve(10); // dd.mm.yyyy, mm/dd/yyyy, dd/mm/yyyy or yyyy-mm-dd
    _Elem _Delim;
    switch (_Fmt) {
    case date_format::general: // format to dd.mm.yyyy
        _Delim = static_cast<_Elem>('.');
        _Append_number(_Date.day, _Delim, true);
        _Append_number(_Date.month, _Delim, true);
        _Append_number(_Date.year, _Delim, false);
        break;
    case date_format::american: // format to mm/dd/yyyy
        _Delim = static_cast<_Elem>('/');
        _Append_number(_Date.month, _Delim, true);
        _Append_number(_Date.day, _Delim, true);
        _Append_number(_Date.year, _Delim, false);
        break;
    case date_format::british: // format to dd/mm/yyyy
        _Delim = static_cast<_Elem>('/');
        _Append_number(_Date.day, _Delim, true);
        _Append_number(_Date.month, _Delim, true);
        _Append_number(_Date.year, _Delim, false);
        break;
    case date_format::iso8601: // format to yyyy-mm-dd
        _Delim = static_cast<_Elem>('-');
        _Append_number(_Date.year, _Delim, true);
        _Append_number(_Date.month, _Delim, true);
        _Append_number(_Date.day, _Delim, false);
        break;
    default: // unknown format, do nothing
        break;
    }
}

// FUNCTION TEMPLATE _Date_formatter::_Append_number
template <class _Elem>
void _Date_formatter<_Elem>::_Append_number(
    const uint8_t _Num, const _Elem _Delim, const bool _Use_delim) noexcept {
    // Note: Do not use the std::to_string() or anything like that,
    //       because we know that _Num is never greater than 31 (2 characters).
    if (_Num < 10) {
        _Mystr.push_back(static_cast<_Elem>('0'));
        _Mystr.push_back(static_cast<_Elem>(_Num + '0'));
    } else {
        const uint8_t _Tens = _Num / 10;
        _Mystr.push_back(static_cast<_Elem>(_Tens + '0'));
        _Mystr.push_back(static_cast<_Elem>(_Num - (_Tens * 10) + '0'));
    }

    if (_Use_delim) { // append the delimiter
        _Mystr.push_back(_Delim);
    }
}

template <class _Elem>
void _Date_formatter<_Elem>::_Append_number(
    const uint16_t _Num, const _Elem _Delim, const bool _Use_delim) noexcept {
    if (_Num < 10) { // 3 zeros and 1 number
        _Mystr.append(3, static_cast<_Elem>('0'));
        _Mystr.push_back(static_cast<_Elem>(_Num + '0'));
    } else if (_Num < 100) { // 2 zeros and 2 numbers
        const uint16_t _Tens = _Num / 10;
        _Mystr.append(2, static_cast<_Elem>('0'));
        _Mystr.push_back(static_cast<_Elem>(_Tens + '0'));
        _Mystr.push_back(static_cast<_Elem>(_Num - (_Tens * 10) + '0'));
    } else if (_Num < 1000) { // 1 zero and 3 numbers
        const uint16_t _Hundreds = _Num / 100;
        const uint16_t _Tens     = (_Num - (_Hundreds * 100)) / 10;
        _Mystr.push_back(static_cast<_Elem>('0'));
        _Mystr.push_back(static_cast<_Elem>(_Hundreds + '0'));
        _Mystr.push_back(static_cast<_Elem>(_Tens + '0'));
        _Mystr.push_back(static_cast<_Elem>(_Num - ((_Hundreds * 100) + (_Tens * 10)) + '0'));
    } else { // 4 numbers
        const uint16_t _Thousands = _Num / 1000;
        const uint16_t _Hundreds  = (_Num - (_Thousands * 1000)) / 100;
        const uint16_t _Tens      = (_Num - ((_Thousands * 1000) + (_Hundreds * 100))) / 10;
        _Mystr.push_back(static_cast<_Elem>(_Thousands + '0'));
        _Mystr.push_back(static_cast<_Elem>(_Hundreds + '0'));
        _Mystr.push_back(static_cast<_Elem>(_Tens + '0'));
        _Mystr.push_back(static_cast<_Elem>(
            _Num - ((_Thousands * 1000) + (_Hundreds * 100) + (_Tens * 10)) + '0'));
    }

    if (_Use_delim) { // append the delimiter
        _Mystr.push_back(_Delim);
    }
}

// FUNCTION TEMPLATE _Date_formatter::_Get
template <class _Elem>
_NODISCARD const typename _Date_formatter<_Elem>::_Value_type&
    _Date_formatter<_Elem>::_Get() const noexcept {
    return _Mystr;
}

// FUNCTION TEMPLATE format_date
template <class _Elem>
_NODISCARD constexpr basic_string<_Elem> format_date(
    const undefined_date& _Date, const date_format _Fmt) {
    using _Formatter_t = _Date_formatter<_Elem>;
    using _Value_t     = typename _Formatter_t::_Value_type;
    if (!_Date.valid()) {
        return _Value_t{};
    }

    _Formatter_t _Formatter(_Date, _Fmt);
    return _Formatter._Get();
}

template _SDSDLL_API _NODISCARD string format_date(const undefined_date&, const date_format);
template _SDSDLL_API _NODISCARD wstring format_date(const undefined_date&, const date_format);

// FUNCTION date_difference
_NODISCARD date_difference_result date_difference(
    const undefined_date& _Left, const undefined_date& _Right) noexcept {
    if (_Left == _Right) {
        return date_difference_result{0, 0, 0};
    }
    
    const bool _Left_greater       = _Left > _Right;
    const undefined_date& _Greater = _Left_greater ? _Left : _Right;
    const undefined_date& _Less    = _Left_greater ? _Right : _Left;
    date_difference_result _Result = {0, 0, 0};
    if (_Greater.day >= _Less.day) {
        _Result.days += _Greater.day - _Less.day;
    } else {
        _Result.months = -1;
        _Result.days  += _Get_days(_Less.month, _Is_leap_year(_Greater.year)) - _Less.day;
        _Result.days  += _Greater.day;
    }

    if (_Greater.month >= _Less.month) {
        _Result.months += _Greater.month - _Less.month;
    } else {
        _Result.years   = -1;
        _Result.months += 12 - _Less.month;
        _Result.months += _Greater.month;
    }

    if (_Greater.year >= _Less.year) {
        _Result.years += _Greater.year - _Less.year;
    } else {
        _Result.years = 0;
    }

    return _Result;
}
_SDSDLL_END

#endif // _SDSDLL_PREPROCESSOR_GUARD