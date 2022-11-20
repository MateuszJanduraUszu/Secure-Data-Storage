// timer.cpp

// Copyright (c) Mateusz Jandura. All rights reserved.
// SPDX-License-Identifier: Apache-2.0

#include <build/sdsdll_pch.hpp>
#include <time_zone/timer.hpp>
#if _SDSDLL_PREPROCESSOR_GUARD

_SDSDLL_BEGIN
// FUNCTION _Get_tick_count
_NODISCARD uintmax_t _Get_tick_count() noexcept {
#ifdef _M_X64
    return ::GetTickCount64();
#else // ^^^ _M_X64 ^^^ / vvv _M_IX86 vvv
    return static_cast<uintmax_t>(::GetTickCount64());
#endif // _M_X64
}

// FUNCTION timer constructor/destructor
timer::timer() noexcept {
    restart();
}

timer::~timer() noexcept {}

// FUNCTION timer::elapsed_time
_NODISCARD uintmax_t timer::elapsed_time() const noexcept {
    return _Get_tick_count() - _Myticks;
}

// FUNCTION timer::restart
void timer::restart() noexcept {
    _Myticks = _Get_tick_count();
}

// FUNTION _Time_cvt_base::_As_nano
_NODISCARD uintmax_t _Time_cvt_base::_As_nano(
    const uintmax_t _Value, const time_format _Old_fmt) noexcept {
    switch (_Old_fmt) {
    case time_format::microseconds:
        return _Value * 1'000;
    case time_format::milliseconds:
        return _Value * 1'000'000;
    case time_format::seconds:
        return _Value * 1'000'000'000;
    default: // the same or unknown format, do nothing
        return _Value;
    }
}

// FUNTION _Time_cvt_base::_As_micro
_NODISCARD uintmax_t _Time_cvt_base::_As_micro(
    const uintmax_t _Value, const time_format _Old_fmt) noexcept {
    switch (_Old_fmt) {
    case time_format::nanoseconds:
        return _Value / 1'000;
    case time_format::milliseconds:
        return _Value * 1'000;
    case time_format::seconds:
        return _Value * 1'000'000;
    default: // the same or unknown format, do nothing
        return _Value;
    }
}

// FUNTION _Time_cvt_base::_As_milli
_NODISCARD uintmax_t _Time_cvt_base::_As_milli(
    const uintmax_t _Value, const time_format _Old_fmt) noexcept {
    switch (_Old_fmt) {
    case time_format::nanoseconds:
        return _Value / 1'000'000;
    case time_format::microseconds:
        return _Value / 1'000;
    case time_format::seconds:
        return _Value * 1'000;
    default: // the same or unknown format, do nothing
        return _Value;
    }
}

// FUNTION _Time_cvt_base::_As_sec
_NODISCARD uintmax_t _Time_cvt_base::_As_sec(
    const uintmax_t _Value, const time_format _Old_fmt) noexcept {
    switch (_Old_fmt) {
    case time_format::nanoseconds:
        return _Value / 1'000'000'000;
    case time_format::microseconds:
        return _Value / 1'000'000;
    case time_format::milliseconds:
        return _Value / 1'000;
    default: // the same or unknown format, do nothing
        return _Value;
    }
}

// FUNCTION time_cast
_NODISCARD uintmax_t time_cast(
    const uintmax_t _Value, const time_format _Old_fmt, const time_format _New_fmt) noexcept {
    switch (_New_fmt) {
    case time_format::nanoseconds:
        return _Time_cvt_base::_As_nano(_Value, _Old_fmt);
    case time_format::microseconds:
        return _Time_cvt_base::_As_micro(_Value, _Old_fmt);
    case time_format::milliseconds:
        return _Time_cvt_base::_As_milli(_Value, _Old_fmt);
    case time_format::seconds:
        return _Time_cvt_base::_As_sec(_Value, _Old_fmt);
    default: // unknown format, do nothing
        return _Value;
    }
}

// FUNCTION sleep_for
void sleep_for(uintmax_t _Duration, const time_format _Fmt) noexcept {
    _Duration = _SDSDLL time_cast(_Duration, _Fmt, time_format::milliseconds);
    ::Sleep(static_cast<DWORD>(_Duration));
}
_SDSDLL_END

#endif // _SDSDLL_PREPROCESSOR_GUARD