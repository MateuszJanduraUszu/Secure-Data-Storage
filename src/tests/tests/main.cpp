// main.cpp

// Copyright (c) Mateusz Jandura. All rights reserved.
// SPDX-License-Identifier: Apache-2.0

#include <Windows.h>
#include <gtest/gtest.h>
#include <unit/cryptography/hash/generic/blake3.hpp>
#include <unit/cryptography/hash/generic/sha512.hpp>
#include <unit/cryptography/hash/generic/xxhash.hpp>

int main() {
    ::testing::InitGoogleTest();
    return ::RUN_ALL_TESTS();
}