// shared_lock.cpp

// Copyright (c) Mateusz Jandura. All rights reserved.
// SPDX-License-Identifier: Apache-2.0

#include <build/sdsdll_pch.hpp>
#include <system/execution/shared_lock.hpp>
#if _SDSDLL_PREPROCESSOR_GUARD

_SDSDLL_BEGIN
// FUNCTION shared_lock constructor/destructor
shared_lock::shared_lock() noexcept : _Myhandle() {
    ::InitializeSRWLock(_SDSDLL addressof(_Myhandle));
}

shared_lock::~shared_lock() noexcept {}

// FUNCTION shared_lock::lock_exclusive
void shared_lock::lock_exclusive() noexcept {
    ::AcquireSRWLockExclusive(_SDSDLL addressof(_Myhandle));
}

// FUNCTION shared_lock::unlock_exclusive
void shared_lock::unlock_exclusive() noexcept {
    ::ReleaseSRWLockExclusive(_SDSDLL addressof(_Myhandle));
}

// FUNCTION shared_lock::lock_shared
void shared_lock::lock_shared() noexcept {
    ::AcquireSRWLockShared(_SDSDLL addressof(_Myhandle));
}

// FUNCTION shared_lock::unlock_shared
void shared_lock::unlock_shared() noexcept {
    ::ReleaseSRWLockShared(_SDSDLL addressof(_Myhandle));
}

// FUNCTION shared_lock_guard copy constructor/destructor
shared_lock_guard::shared_lock_guard(shared_lock& _Lock) noexcept : _Mylock(_Lock) {
    _Mylock.lock_shared();
}

shared_lock_guard::~shared_lock_guard() noexcept {
    _Mylock.unlock_shared();
}

// FUNCTION exclusive_lock_guard copy constructor/destructor
exclusive_lock_guard::exclusive_lock_guard(shared_lock& _Lock) noexcept : _Mylock(_Lock) {
    _Mylock.lock_exclusive();
}

exclusive_lock_guard::~exclusive_lock_guard() noexcept {
    _Mylock.unlock_exclusive();
}
_SDSDLL_END

#endif // _SDSDLL_PREPROCESSOR_GUARD