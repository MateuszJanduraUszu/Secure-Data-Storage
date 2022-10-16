// main.cpp

// Copyright (c) Mateusz Jandura. All rights reserved.
// SPDX-License-Identifier: Apache-2.0

#include <Windows.h>
#include <gtest/gtest.h>
#include <unit/cryptography/aes256_cbc.hpp>

int main(int, char**) {
    ::testing::InitGoogleTest();
    return ::RUN_ALL_TESTS();
}