// sdsdll_pch.hpp

// Copyright (c) Mateusz Jandura. All rights reserved.
// SPDX-License-Identifier: Apache-2.0

#pragma once
#ifndef _SDSDLL_BUILD_SDSDLL_PCH_HPP_
#define _SDSDLL_BUILD_SDSDLL_PCH_HPP_

// pre-compiled headers
#include <build/sdsdll_framework.hpp>
#include <compression/lz4.hpp>
#include <compression/types.hpp>
#include <core/api.hpp>
#include <core/defs.hpp>
#include <core/debug/trace.hpp>
#include <core/memory/allocator.hpp>
#include <core/optimization/ebco.hpp>
#include <core/optimization/sbo.hpp>
#include <core/optimization/string_view.hpp>
#include <core/pattern/singleton.hpp>
#include <core/symbol/loader.hpp>
#include <core/traits/concepts.hpp>
#include <core/traits/integer.hpp>
#include <core/traits/memory_traits.hpp>
#include <core/traits/string_traits.hpp>
#include <core/traits/type_traits.hpp>
#include <cryptography/cipher/cipher_types.hpp>
#include <cryptography/cipher/symmetric/aes256_cbc.hpp>
#include <cryptography/cipher/symmetric/aes256_gcm.hpp>
#include <cryptography/cipher/symmetric/aes_key.hpp>
#include <cryptography/cipher/symmetric/iv.hpp>
#include <cryptography/hash/generic/blake3.hpp>
#include <cryptography/hash/generic/sha512.hpp>
#include <cryptography/hash/generic/xxhash.hpp>
#include <cryptography/hash/hash_types.hpp>
#include <cryptography/hash/password/argon2d.hpp>
#include <cryptography/hash/password/argon2i.hpp>
#include <cryptography/hash/password/argon2id.hpp>
#include <cryptography/hash/password/scrypt.hpp>
#include <cryptography/random/random.hpp>
#include <cryptography/random/salt.hpp>
#include <encoding/bom.hpp>
#include <encoding/utf16.hpp>
#include <encoding/utf8.hpp>
#include <extensions/scfg.hpp>
#include <extensions/sudb.hpp>
#include <filesystem/com.hpp>
#include <filesystem/directory.hpp>
#include <filesystem/file.hpp>
#include <filesystem/link.hpp>
#include <filesystem/path.hpp>
#include <filesystem/security.hpp>
#include <filesystem/shortcut.hpp>
#include <filesystem/status.hpp>
#include <recovery/arc.hpp>
#include <system/execution/process.hpp>
#include <system/handle/generic_handle.hpp>
#include <system/handle/handle_wrapper.hpp>
#include <system/handle/library_handle.hpp>
#include <system/handle/process_handle.hpp>
#include <system/power/power.hpp>
#include <system/registry/key.hpp>
#include <system/registry/manager.hpp>
#include <system/registry/view.hpp>
#endif // _SDSDLL_BUILD_SDSDLL_PCH_HPP_