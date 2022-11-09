// shared_lock.hpp

// Copyright (c) Mateusz Jandura. All rights reserved.
// SPDX-License-Identifier: Apache-2.0

#pragma once
#ifndef _SDSDLL_SYSTEM_EXECUTION_SHARED_LOCK_HPP_
#define _SDSDLL_SYSTEM_EXECUTION_SHARED_LOCK_HPP_
#include <core/defs.hpp>
#if _SDSDLL_PREPROCESSOR_GUARD
#include <core/api.hpp>
#include <core/traits/type_traits.hpp>
#include <synchapi.h>

_SDSDLL_BEGIN
// CLASS shared_lock
class _SDSDLL_API shared_lock { // non-copyable shared/exclusive lock object
public:
    shared_lock() noexcept;
    ~shared_lock() noexcept;

    shared_lock(const shared_lock&) = delete;
    shared_lock& operator=(const shared_lock&) = delete;

    // locks the code segment (exclusive mode)
    void lock_exclusive() noexcept;

    // unlocks the code segment (exclusive mode)
    void unlock_exclusive() noexcept;

    // locks the code segment (shared mode)
    void lock_shared() noexcept;

    // unlocks the code segment (shared mode)
    void unlock_shared() noexcept;

private:
    SRWLOCK _Myhandle;
};

// CLASS shared_lock_guard
class _SDSDLL_API shared_lock_guard { // locks the code segment for multiple threads
public:
    shared_lock_guard(shared_lock& _Lock) noexcept;
    ~shared_lock_guard() noexcept;

    shared_lock_guard() = delete;
    shared_lock_guard(const shared_lock_guard&) = delete;
    shared_lock_guard& operator=(const shared_lock_guard&) = delete;

private:
    shared_lock& _Mylock;
};

// CLASS exclusive_lock_guard
class _SDSDLL_API exclusive_lock_guard { // locks the code segment for exactly one thread
public:
    exclusive_lock_guard(shared_lock& _Lock) noexcept;
    ~exclusive_lock_guard() noexcept;

    exclusive_lock_guard() = delete;
    exclusive_lock_guard(const exclusive_lock_guard&) = delete;
    exclusive_lock_guard& operator=(const exclusive_lock_guard&) = delete;

private:
    shared_lock& _Mylock;
};
_SDSDLL_END

#endif // _SDSDLL_PREPROCESSOR_GUARD
#endif // _SDSDLL_SYSTEM_EXECUTION_SHARED_LOCK_HPP_