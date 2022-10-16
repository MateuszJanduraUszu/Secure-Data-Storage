// arc.cpp

// Copyright (c) Mateusz Jandura. All rights reserved.
// SPDX-License-Identifier: Apache-2.0

#include <build/sdsdll_pch.hpp>
#include <recovery/arc.hpp>
#if _SDSDLL_PREPROCESSOR_GUARD

_SDSDLL_BEGIN
// FUNCTION arc constructors/destructor
arc::arc() noexcept : _Mybase() {}

arc::arc(const arc& _Other) noexcept : _Mybase(_Other) {}

arc::arc(arc&& _Other) noexcept : _Mybase(_STD move(_Other)) {}

arc::arc(const value_type* const _Ptr) noexcept : _Mybase(_Ptr) {}

arc::~arc() noexcept {}

// FUNCTION arc::operator=
arc& arc::operator=(const arc& _Other) noexcept {
    _Mybase::operator=(_Other);
    return *this;
}

arc& arc::operator=(arc&& _Other) noexcept {
    _Mybase::operator=(_STD move(_Other));
    return *this;
}

arc& arc::operator=(const value_type* const _Ptr) noexcept {
    _Mybase::operator=(_Ptr);
    return *this;
}

// FUNCTION arc::to_string
_NODISCARD string_view arc::to_string() const noexcept {
    return string_view{reinterpret_cast<const char*>(get()), size};
}

// FUNCTION operator==
_NODISCARD bool operator==(const arc& _Left, const arc& _Right) noexcept {
    return memory_traits::compare(_Left.get(), _Right.get(), arc::size) == 0;
}

// FUNCTION operator!=
_NODISCARD bool operator!=(const arc& _Left, const arc& _Right) noexcept {
    return memory_traits::compare(_Left.get(), _Right.get(), arc::size) != 0;
}

// FUNCTION make_arc
_NODISCARD arc make_arc() {
    byte_string _Bytes;
    _Bytes.reserve(arc::size);
    for (uint8_t _Blocks = 0; _Blocks < 4; ++_Blocks) {
        _Bytes.append(_SDSDLL random_bytes(5, random_format::number | random_format::uppercase));
        if (_Blocks < 3) { // skip '-' after thel last block
            _Bytes.push_back(uint8_t{0x2D});
        }
    }

    return arc{_Bytes.c_str()};
}
_SDSDLL_END

#endif // _SDSDLL_PREPROCESSOR_GUARD