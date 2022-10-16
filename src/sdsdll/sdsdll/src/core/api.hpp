// api.hpp

// Copyright (c) Mateusz Jandura. All rights reserved.
// SPDX-License-Identifier: Apache-2.0

#pragma once
#ifndef _SDSDLL_CORE_API_HPP_
#define _SDSDLL_CORE_API_HPP_

// Macro to export/import (if possible) symbols from a DLL file.
#ifndef _MSC_VER
#define _SDSDLL_API
#else // ^^^ !_MSC_VER ^^^ / vvv _MSC_VER vvv
#ifdef SDSDLL_EXPORTS
#define _SDSDLL_API __declspec(dllexport)
#else // ^^^ SDSDLL_EXPORTS ^^^ / vvv !SDSDLL_EXPORTS vvv
#define _SDSDLL_API __declspec(dllimport)
#endif // SDSDLL_EXPORTS
#endif // _MSC_VER
#endif // _SDSDLL_CORE_API_HPP_