// time.cpp

// Copyright (c) Mateusz Jandura. All rights reserved.
// SPDX-License-Identifier: Apache-2.0

#include <build/sdsdll_pch.hpp>
#include <time_zone/time.hpp>
#if _SDSDLL_PREPROCESSOR_GUARD

_SDSDLL_BEGIN
// FUNCTION _Time_to_seconds
_NODISCARD uint32_t _Time_to_seconds(const undefined_time& _Time) noexcept {
    return (_Time.hour * 60 * 60) + (_Time.minute * 60) + _Time.second;
}

// FUNCTION _Time_from_seconds
_NODISCARD undefined_time _Time_from_seconds(uint32_t _Seconds) noexcept {
    static constexpr uint32_t _Max_seconds = 86399; // 23:59:59
    if (_Seconds > _Max_seconds) { // too many seconds
        return undefined_time{0, 0, 0};
    }
    
    if (_Seconds < 60) { // less than a minute
        return undefined_time{0, 0, static_cast<uint8_t>(_Seconds)};
    }

    undefined_time _Result;
    _Result.hour   = static_cast<uint8_t>(_Seconds / 3600);
    _Seconds      -= _Result.hour * 3600;
    _Result.minute = static_cast<uint8_t>(_Seconds / 60);
    _Seconds      -= _Result.minute * 60;
    _Result.second = static_cast<uint8_t>(_Seconds);
    return _Result;
}

// FUNCTION undefined_time constructors/destructor
undefined_time::undefined_time() noexcept : hour(0), minute(0), second(0) {}

undefined_time::undefined_time(const undefined_time& _Other) noexcept
    : hour(_Other.hour), minute(_Other.minute), second(_Other.second) {}

undefined_time::undefined_time(undefined_time&& _Other) noexcept
    : hour(_Other.hour), minute(_Other.minute), second(_Other.second) {}

undefined_time::undefined_time(
    const uint8_t _Hour, const uint8_t _Minute, const uint8_t _Second) noexcept
    : hour(_Hour), minute(_Minute), second(_Second) {}

undefined_time::~undefined_time() noexcept {}

// FUNCTION undefined_time::operator=
undefined_time& undefined_time::operator=(const undefined_time& _Other) noexcept {
    if (this != _SDSDLL addressof(_Other)) {
        hour   = _Other.hour;
        minute = _Other.minute;
        second = _Other.second;
    }

    return *this;
}

undefined_time& undefined_time::operator=(undefined_time&& _Other) noexcept {
    if (this != _SDSDLL addressof(_Other)) {
        hour   = _Other.hour;
        minute = _Other.minute;
        second = _Other.second;
    }

    return *this;
}

// FUNCTION undefined_time::valid
_NODISCARD bool undefined_time::valid() const noexcept {
    return (hour >= 0 && hour < 24) && (minute >= 0 && minute < 60)
        && (second >= 0 && second < 60);
}

// FUNCTION undefined_time::operator==
_NODISCARD bool undefined_time::operator==(const undefined_time& _Other) const noexcept {
    return hour == _Other.hour && minute == _Other.minute && second == _Other.second;
}

// FUNCTION undefined_time::operator!=
_NODISCARD bool undefined_time::operator!=(const undefined_time& _Other) const noexcept {
    return !(*this == _Other);
}

// FUNCTION undefined_time::operator>
_NODISCARD bool undefined_time::operator>(const undefined_time& _Other) const noexcept {
    return _Time_to_seconds(*this) > _Time_to_seconds(_Other);
}

// FUNCTION undefined_time::operator>=
_NODISCARD bool undefined_time::operator>=(const undefined_time& _Other) const noexcept {
    return _Time_to_seconds(*this) >= _Time_to_seconds(_Other);
}

// FUNCTION undefined_time::operator<
_NODISCARD bool undefined_time::operator<(const undefined_time& _Other) const noexcept {
    return _Time_to_seconds(*this) < _Time_to_seconds(_Other);
}

// FUNCTION undefined_time::operator<=
_NODISCARD bool undefined_time::operator<=(const undefined_time& _Other) const noexcept {
    return _Time_to_seconds(*this) <= _Time_to_seconds(_Other);
}

// FUNCTION utc_time constructors/destructor
utc_time::utc_time() noexcept : undefined_time() {}

utc_time::utc_time(const utc_time& _Other) noexcept : undefined_time(_Other) {}

utc_time::utc_time(utc_time&& _Other) noexcept : undefined_time(_STD move(_Other)) {}

utc_time::utc_time(const undefined_time& _Time) noexcept : undefined_time(_Time) {}

utc_time::utc_time(const uint8_t _Hour, const uint8_t _Minute, const uint8_t _Second) noexcept
    : undefined_time(_Hour, _Minute, _Second) {}

utc_time::~utc_time() noexcept {}

// FUNCTION utc_time::operator=
utc_time& utc_time::operator=(const utc_time& _Other) noexcept {
    undefined_time::operator=(_Other);
    return *this;
}

utc_time& utc_time::operator=(utc_time&& _Other) noexcept {
    undefined_time::operator=(_STD move(_Other));
    return *this;
}

// FUNCTION utc_time::current
_NODISCARD utc_time utc_time::current() noexcept {
    SYSTEMTIME _Time;
    ::GetSystemTime(_SDSDLL addressof(_Time));
    return utc_time{static_cast<uint8_t>(_Time.wHour),
        static_cast<uint8_t>(_Time.wMinute), static_cast<uint8_t>(_Time.wSecond)};
}

// FUNCTION local_time constructors/destructor
local_time::local_time() noexcept : undefined_time() {}

local_time::local_time(const local_time& _Other) noexcept : undefined_time(_Other) {}

local_time::local_time(local_time&& _Other) noexcept : undefined_time(_STD move(_Other)) {}

local_time::local_time(const undefined_time& _Time) noexcept : undefined_time(_Time) {}

local_time::local_time(const uint8_t _Hour, const uint8_t _Minute, const uint8_t _Second) noexcept
    : undefined_time(_Hour, _Minute, _Second) {}

local_time::~local_time() noexcept {}

// FUNCTION local_time::operator=
local_time& local_time::operator=(const local_time& _Other) noexcept {
    undefined_time::operator=(_Other);
    return *this;
}

local_time& local_time::operator=(local_time&& _Other) noexcept {
    undefined_time::operator=(_STD move(_Other));
    return *this;
}

// FUNCTION local_time::current
_NODISCARD local_time local_time::current() noexcept {
    SYSTEMTIME _Time;
    ::GetLocalTime(_SDSDLL addressof(_Time));
    return local_time{static_cast<uint8_t>(_Time.wHour),
        static_cast<uint8_t>(_Time.wMinute), static_cast<uint8_t>(_Time.wSecond)};
}

// FUNCTION TEMPLATE _Time_formatter copy constructor
template <class _Elem>
_Time_formatter<_Elem>::_Time_formatter(const undefined_time& _Time) noexcept : _Mystr() {
    _Mystr.reserve(8); // reserve 8 elements (hh:mm:ss)
    _Append_number(_Time.hour, true);
    _Append_number(_Time.minute, true);
    _Append_number(_Time.second, false); // skip the delimiter after seconds
}

// FUNCTION TEMPLATE _Time_formatter::_Append_number
template <class _Elem>
void _Time_formatter<_Elem>::_Append_number(const uint8_t _Num, const bool _Delim) noexcept {
    // Note: Do not use the std::to_string() or anything like that,
    //       because we know that _Num is never greater than 59 (2 characters).
    if (_Num < 10) {
        _Mystr.push_back(static_cast<_Elem>('0'));
        _Mystr.push_back(static_cast<_Elem>(_Num + '0'));
    } else {
        const uint8_t _Tens = _Num / 10;
        _Mystr.push_back(static_cast<_Elem>(_Tens + '0'));
        _Mystr.push_back(static_cast<_Elem>(_Num - (_Tens * 10) + '0'));
    }

    if (_Delim) { // append delimiter
        _Mystr.push_back(static_cast<_Elem>(':'));
    }
}

// FUNCTION TEMPLATE _Time_formatter::_Get
template <class _Elem>
_NODISCARD const typename _Time_formatter<_Elem>::_Value_type&
    _Time_formatter<_Elem>::_Get() const noexcept {
    return _Mystr;
}

// FUNCTION TEMPLATE format_time
template <class _Elem>
_NODISCARD constexpr basic_string<_Elem> format_time(const undefined_time& _Time) {
    using _Formatter_t = _Time_formatter<_Elem>;
    using _Value_t     = typename _Formatter_t::_Value_type;
    if (!_Time.valid()) {
        return _Value_t{};
    }

    _Formatter_t _Formatter(_Time);
    return _Formatter._Get();
}

template _SDSDLL_API _NODISCARD string format_time(const undefined_time&);
template _SDSDLL_API _NODISCARD wstring format_time(const undefined_time&);

// FUNCTION time_difference
_NODISCARD undefined_time time_difference(
    const undefined_time& _Left, const undefined_time& _Right) noexcept {
    const uint32_t _Left_secs  = _Time_to_seconds(_Left);
    const uint32_t _Right_secs = _Time_to_seconds(_Right);
    if (_Left_secs == _Right_secs) { // no difference
        return undefined_time{0, 0, 0};
    }

    return _Left_secs > _Right_secs
        ? _Time_from_seconds(_Left_secs - _Right_secs) : _Time_from_seconds(_Right_secs - _Left_secs);
}
_SDSDLL_END

#endif // _SDSDLL_PREPROCESSOR_GUARD