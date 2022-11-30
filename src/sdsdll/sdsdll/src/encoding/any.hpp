// any.hpp

// Copyright (c) Mateusz Jandura. All rights reserved.
// SPDX-License-Identifier: Apache-2.0

#pragma once
#ifndef _SDSDLL_ENCODING_ANY_HPP_
#define _SDSDLL_ENCODING_ANY_HPP_
#include <core/defs.hpp>
#if _SDSDLL_PREPROCESSOR_GUARD
#include <core/api.hpp>
#include <core/defs.hpp>
#include <core/optimization/string_view.hpp>
#include <core/traits/concepts.hpp>
#include <core/traits/string_traits.hpp>
#include <cstddef>
#include <string>

// STD types
using _STD basic_string;
using _STD string;
using _STD wstring;

_SDSDLL_BEGIN
// FUNCTION TEMPLATE narrow_string
template <class _Elem>
_SDSDLL_API _NODISCARD constexpr string narrow_string(const _Elem* _Str, size_t _Count);

template <class _Elem>
_SDSDLL_API _NODISCARD constexpr string narrow_string(const _Elem* const _Str);

template <class _Elem>
_SDSDLL_API _NODISCARD constexpr string narrow_string(const basic_string_view<_Elem> _Str);

template <class _Elem>
_SDSDLL_API _NODISCARD constexpr string narrow_string(const basic_string<_Elem>& _Str);

// FUNCTION TEMPLATE widen_string
template <class _Elem>
_SDSDLL_API _NODISCARD constexpr wstring widen_string(const _Elem* _Str, size_t _Count);

template <class _Elem>
_SDSDLL_API _NODISCARD constexpr wstring widen_string(const _Elem* const _Str);

template <class _Elem>
_SDSDLL_API _NODISCARD constexpr wstring widen_string(const basic_string_view<_Elem> _Str);

template <class _Elem>
_SDSDLL_API _NODISCARD constexpr wstring widen_string(const basic_string<_Elem>& _Str);

// FUNCTION TEMPLATE auto_string
template <class _Elem1, class _Elem2>
_SDSDLL_API _NODISCARD constexpr basic_string<_Elem1> auto_string(const _Elem2* _Str, size_t _Count);

template <class _Elem1, class _Elem2>
_SDSDLL_API _NODISCARD constexpr basic_string<_Elem1> auto_string(const _Elem2* _Str);

template <class _Elem1, class _Elem2>
_SDSDLL_API _NODISCARD constexpr basic_string<_Elem1> auto_string(const basic_string_view<_Elem2> _Str);

template <class _Elem1, class _Elem2>
_SDSDLL_API _NODISCARD constexpr basic_string<_Elem1> auto_string(const basic_string<_Elem2>& _Str);
_SDSDLL_END

#endif // _SDSDLL_PREPROCESSOR_GUARD
#endif // _SDSDLL_ENCODING_ANY_HPP_