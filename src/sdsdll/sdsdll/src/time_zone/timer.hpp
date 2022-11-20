// timer.hpp

// Copyright (c) Mateusz Jandura. All rights reserved.
// SPDX-License-Identifier: Apache-2.0

#pragma once
#ifndef _SDSDLL_TIME_ZONE_TIMER_HPP_
#define _SDSDLL_TIME_ZONE_TIMER_HPP_
#include <core/defs.hpp>
#if _SDSDLL_PREPROCESSOR_GUARD
#include <core/api.hpp>
#include <cstdint>
#include <synchapi.h>
#include <sysinfoapi.h>

_SDSDLL_BEGIN
// FUNCTION _Get_tick_count
extern _NODISCARD uintmax_t _Get_tick_count() noexcept;

// CLASS timer
class _SDSDLL_API timer {
public:
    timer() noexcept;
    ~timer() noexcept;

    timer(const timer&) = delete;
    timer& operator=(const timer&) = delete;

    // calculates the difference between the last saved time and the current (in ticks)
    _NODISCARD uintmax_t elapsed_time() const noexcept;

    // restarts the timer
    void restart() noexcept;

private:
    uintmax_t _Myticks;
};

// ENUM CLASS time_format
enum class time_format : unsigned char {
    nanoseconds,
    microseconds,
    milliseconds,
    seconds
};

// STRUCT _Time_cvt_base
struct _Time_cvt_base {
    // converts any time format to the nanoseconds
    _NODISCARD static uintmax_t _As_nano(const uintmax_t _Value, const time_format _Old_fmt) noexcept;

    // converts any time format to the microseconds
    _NODISCARD static uintmax_t _As_micro(const uintmax_t _Value, const time_format _Old_fmt) noexcept;

    // converts any time format to the milliseconds
    _NODISCARD static uintmax_t _As_milli(const uintmax_t _Value, const time_format _Old_fmt) noexcept;

    // converts any time format to the seconds
    _NODISCARD static uintmax_t _As_sec(const uintmax_t _Value, const time_format _Old_fmt) noexcept;
};

// FUNCTION time_cast
_SDSDLL_API _NODISCARD uintmax_t time_cast(
    const uintmax_t _Value, const time_format _Old_fmt, const time_format _New_fmt) noexcept;

// FUNCTION sleep_for
_SDSDLL_API void sleep_for(uintmax_t _Duration, const time_format _Fmt = time_format::seconds) noexcept;
_SDSDLL_END

#endif // _SDSDLL_PREPROCESSOR_GUARD
#endif // _SDSDLL_TIME_ZONE_TIMER_HPP_