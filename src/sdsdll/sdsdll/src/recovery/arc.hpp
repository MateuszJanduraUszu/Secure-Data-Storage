// arc.hpp

// Copyright (c) Mateusz Jandura. All rights reserved.
// SPDX-License-Identifier: Apache-2.0

#pragma once
#ifndef _SDSDLL_RECOVERY_ARC_HPP_
#define _SDSDLL_RECOVERY_ARC_HPP_
#include <core/defs.hpp>
#if _SDSDLL_PREPROCESSOR_GUARD
#include <array>
#include <core/api.hpp>
#include <core/container/bytes.hpp>
#include <core/optimization/string_view.hpp>
#include <core/traits/concepts.hpp>
#include <core/traits/memory_traits.hpp>
#include <core/traits/string_traits.hpp>
#include <core/traits/type_traits.hpp>
#include <cryptography/random/random.hpp>
#include <cstddef>
#include <cstdint>
#include <string>
#include <type_traits>

_SDSDLL_BEGIN
// CLASS arc
class _SDSDLL_API arc : public _Bytes_container<23> { // Account Recovery Code
private:
    using _Mybase = _Bytes_container<23>;

public:
    using value_type      = typename _Mybase::value_type;
    using size_type       = typename _Mybase::size_type;
    using difference_type = typename _Mybase::difference_type;
    using pointer         = typename _Mybase::pointer;
    using const_pointer   = typename _Mybase::const_pointer;

    arc() noexcept;
    arc(const arc& _Other) noexcept;
    arc(arc&& _Other) noexcept;
    ~arc() noexcept;

    explicit arc(const value_type* const _Ptr) noexcept;

    arc& operator=(const arc& _Other) noexcept;
    arc& operator=(arc&& _Other) noexcept;
    arc& operator=(const value_type* const _Ptr) noexcept;

    // returns the current ARC as a string view
    _NODISCARD string_view to_string() const noexcept;
};

// FUNCTION operator==
_SDSDLL_API _NODISCARD bool operator==(const arc& _Left, const arc& _Right) noexcept;

// FUNCTION operator!=
_SDSDLL_API _NODISCARD bool operator!=(const arc& _Left, const arc& _Right) noexcept;

// FUNCTION make_arc
_SDSDLL_API _NODISCARD arc make_arc();
_SDSDLL_END

#endif // _SDSDLL_PREPROCESSOR_GUARD
#endif // _SDSDLL_RECOVERY_ARC_HPP_