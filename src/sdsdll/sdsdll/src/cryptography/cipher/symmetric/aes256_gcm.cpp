// aes256_gcm.cpp

// Copyright (c) Mateusz Jandura. All rights reserved.
// SPDX-License-Identifier: Apache-2.0

#include <build/sdsdll_pch.hpp>
#include <cryptography/cipher/symmetric/aes256_gcm.hpp>
#if _SDSDLL_PREPROCESSOR_GUARD

_SDSDLL_BEGIN
// FUNCTION _Get_aes256_gcm_tag
_NODISCARD bool _Get_aes256_gcm_tag(_Cipher_context_proxy& _Proxy, uint8_t* const _Buf) noexcept {
    if (!_Proxy._Ctx || !_Buf) { // invalid context or buffer
        return false;
    }

    // Note: It is important to exclude the tag from the context, because
    //       with it the EVP_DecryptFinal_ex() will return 0 (failure).
    return ::EVP_CIPHER_CTX_ctrl(_Proxy._Ctx, EVP_CTRL_GCM_GET_TAG, 16, _Buf) != 0; // 16-byte tag
}

// FUNCTION _Set_aes256_gcm_tag
_NODISCARD bool _Set_aes256_gcm_tag(_Cipher_context_proxy& _Proxy, uint8_t* const _Tag) noexcept {
    if (!_Proxy._Ctx || !_Tag) { // invalid context or tag
        return false;
    }

    return ::EVP_CIPHER_CTX_ctrl(_Proxy._Ctx, EVP_CTRL_GCM_SET_TAG, 16, _Tag) != 0; // 16-byte tag
}

// FUNCTION TEMPLATE _Aes256_gcm_fn::operator()
template <class _Simple_types>
_NODISCARD constexpr bool _Aes256_gcm_fn<_Simple_types>::operator()(
    byte_type* const _Buf, size_type _Buf_size, const byte_type* const _Data, const size_type _Data_size,
    const key& _Key, const iv& _Iv, const cipher_task _Task) const noexcept {
    if (!_Buf || _Buf_size == 0) { // invalid buffer
        return false;
    }

    _Cipher_context_proxy _Proxy;
    if (!_Proxy._Ctx) { // failed to allocated the context
        return false;
    }

    if (_Task == cipher_task::encryption) { // try to encrypt the data
        if (::EVP_EncryptInit_ex(_Proxy._Ctx, ::EVP_aes_256_gcm(), nullptr, _Key.get(), _Iv.get()) == 0) {
            return false;
        }

        constexpr size_type _Tag_size = 16;
        _Buf_size                    -= _Tag_size; // skip storage for the tag
        if (::EVP_EncryptUpdate(_Proxy._Ctx, _Buf, reinterpret_cast<int*>(&_Buf_size),
            _Data, static_cast<int>(_Data_size)) == 0) {
            return false;
        }

        int _Bytes; // encrypted bytes (unused)
        if (::EVP_EncryptFinal_ex(_Proxy._Ctx, _Buf + _Buf_size, &_Bytes) == 0) {
            return false;
        }

        // Note: Not extracted AES-256 GCM tag will cause an error in the EVP_DecryptFinal_ex() function.
        //       The tag should be extracted and concated with the encrypted data.
        byte_type _Tag[_Tag_size];
        if (!_Get_aes256_gcm_tag(_Proxy, _Tag)) {
            return false;
        }

        // Note: Real buffer size is _Buf_size + _Tag_size so it is safe to
        //       move tag data to the _Buf + _Buf_size location.
        memory_traits::move(_Buf + _Buf_size, _Tag, _Tag_size * sizeof(byte_type));
    } else { // try to decrypt the data
        if (::EVP_DecryptInit_ex(_Proxy._Ctx, ::EVP_aes_256_gcm(), nullptr, _Key.get(), _Iv.get()) == 0) {
            return false;
        }

        constexpr size_type _Tag_size = 16;
        if (::EVP_DecryptUpdate(_Proxy._Ctx, _Buf, reinterpret_cast<int*>(&_Buf_size),
            _Data, static_cast<int>(_Data_size - _Tag_size)) == 0) {
            return false;
        }

        // Note: The tag must be set in the context and the data must not contains it,
        //       otherwise the EVP_DecryptFinal_ex() will return 0 (failure).
        byte_type _Tag[_Tag_size];
        memory_traits::move(_Tag, _Data + (_Data_size - _Tag_size), _Tag_size * sizeof(byte_type));
        if (!_Set_aes256_gcm_tag(_Proxy, _Tag)) {
            return false;
        }

        int _Bytes; // decrypted bytes (unused)
        if (::EVP_DecryptFinal_ex(_Proxy._Ctx, _Buf + _Buf_size, &_Bytes) == 0) {
            return false;
        }
    }
    
    return true;
}

// FUNCTION TEMPLATE _Aes256_gcm_default_engine copy constructor/destructor
template <class _Elem>
constexpr _Aes256_gcm_default_engine<_Elem>::_Aes256_gcm_default_engine(
    const _Elem* const _Data, const size_type _Size, const key& _Key, const iv& _Iv,
    const cipher_task _Task) noexcept : _Mypair(_Ebco_default_init{}) {
    if _CONSTEXPR_IF (is_same_v<_Elem, char>) {
        const size_type _Buf_size = _Calculate_buffer_size(_Size, _Task);
        if (_Mypair._Val1._Resize(_Buf_size)) { // buffer allocated, complete the task
            _Release_on_failure(_Mypair._Val1, _Mypair._Get_val2()(_Mypair._Val1._Get(), _Buf_size,
                reinterpret_cast<const byte_type*>(_Data), _Size, _Key, _Iv, _Task));
        }
    } else if _CONSTEXPR_IF (is_same_v<_Elem, uint8_t>) {
        const size_type _Buf_size = _Calculate_buffer_size(_Size, _Task);
        if (_Mypair._Val1._Resize(_Buf_size)) { // buffer allocated, complete the task
            _Release_on_failure(_Mypair._Val1, _Mypair._Get_val2()(
                _Mypair._Val1._Get(), _Buf_size, _Data, _Size, _Key, _Iv, _Task));
        }
    } else {
        const utf8_string& _Narrow = utf8_string::from_utf16(_Data, _Size);
        const size_type _Buf_size  = _Calculate_buffer_size(_Narrow.size(), _Task);
        if (_Mypair._Val1._Resize(_Buf_size)) { // buffer allocated, complete the task
            _Release_on_failure(_Mypair._Val1, _Mypair._Get_val2()(_Mypair._Val1._Get(), _Buf_size,
                reinterpret_cast<const byte_type*>(_Narrow.c_str()), _Narrow.size(), _Key, _Iv, _Task));
        }
    }
}

template <class _Elem>
_CONSTEXPR20 _Aes256_gcm_default_engine<_Elem>::~_Aes256_gcm_default_engine() noexcept {}

// FUNCTION TEMPLATE _Aes256_gcm_default_engine::_Calculate_buffer_size
template <class _Elem>
_NODISCARD constexpr typename _Aes256_gcm_default_engine<_Elem>::size_type
    _Aes256_gcm_default_engine<_Elem>::_Calculate_buffer_size(
    const size_type _Data_size, const cipher_task _Task) const noexcept {
    // encryption adds 16-byte tag and decryption removes it
    return _Task == cipher_task::encryption ? _Data_size + 16 : _Data_size - 16;
}

// FUNCTION TEMPLATE _Aes256_gcm_default_engine::bytes
template <class _Elem>
_NODISCARD constexpr typename _Aes256_gcm_default_engine<_Elem>::byte_string
    _Aes256_gcm_default_engine<_Elem>::bytes() const {
    return !_Mypair._Val1._Empty() ?
        byte_string{_Mypair._Val1._Get(), _Mypair._Val1._Size()} : byte_string{};
}

// FUNCTION TEMPLATE decrypt_aes256_gcm
template <class _Elem, class _Eng>
_NODISCARD constexpr basic_string<_Elem> decrypt_aes256_gcm(const uint8_t* const _Data,
    const size_t _Count, const aes_key<32>& _Key, const iv<16>& _Iv) {
    static_assert(is_any_of_v<_Elem, char, wchar_t>, "Requires UTF-8/UTF-16 element type.");
    static_assert(is_same_v<uint8_t, typename _Eng::char_type>, "Incompatible type.");
    static_assert(is_same_v<size_t, typename _Eng::size_type>, "Incompatible type.");
    if (_Count == 0) { // empty string, do nothing
        return basic_string<_Elem>{};
    }

    const byte_string& _Bytes = _Eng(_Data, _Count, _Key, _Iv, cipher_task::decryption).bytes();
    if _CONSTEXPR_IF (is_same_v<_Elem, char>) {
        return string{reinterpret_cast<const char*>(_Bytes.c_str()), _Bytes.size()};
    } else {
        const utf16_string& _Wide = utf16_string::from_utf8(
            reinterpret_cast<const char*>(_Bytes.c_str()), _Bytes.size());
        return _Wide.str();
    }
}

template _SDSDLL_API _NODISCARD string decrypt_aes256_gcm(
    const uint8_t* const, const size_t, const aes_key<32>&, const iv<16>&);
template _SDSDLL_API _NODISCARD wstring decrypt_aes256_gcm(
    const uint8_t* const, const size_t, const aes_key<32>&, const iv<16>&);

template <class _Elem, class _Eng>
_NODISCARD constexpr basic_string<_Elem> decrypt_aes256_gcm(
    const uint8_t* const _Data, const aes_key<32>& _Key, const iv<16>& _Iv) {
    using _Size_t = typename _Eng::size_type;
    using _Traits = string_traits<uint8_t, _Size_t>;
    return _SDSDLL decrypt_aes256_gcm<_Elem, _Eng>(_Data, _Traits::length(_Data), _Key, _Iv);
}

template _SDSDLL_API _NODISCARD string decrypt_aes256_gcm(
    const uint8_t* const, const aes_key<32>&, const iv<16>&);
template _SDSDLL_API _NODISCARD wstring decrypt_aes256_gcm(
    const uint8_t* const, const aes_key<32>&, const iv<16>&);

template <class _Elem, class _Eng>
_NODISCARD constexpr basic_string<_Elem> decrypt_aes256_gcm(
    const byte_string_view _Data, const aes_key<32>& _Key, const iv<16>& _Iv) {
    return _SDSDLL decrypt_aes256_gcm<_Elem, _Eng>(_Data.data(), _Data.size(), _Key, _Iv);
}

template _SDSDLL_API _NODISCARD string decrypt_aes256_gcm(
    const byte_string_view, const aes_key<32>&, const iv<16>&);
template _SDSDLL_API _NODISCARD wstring decrypt_aes256_gcm(
    const byte_string_view, const aes_key<32>&, const iv<16>&);

template <class _Elem, class _Eng>
_NODISCARD constexpr basic_string<_Elem> decrypt_aes256_gcm(
    const byte_string& _Data, const aes_key<32>& _Key, const iv<16>& _Iv) {
    return _SDSDLL decrypt_aes256_gcm<_Elem, _Eng>(_Data.c_str(), _Data.size(), _Key, _Iv);
}

template _SDSDLL_API _NODISCARD string decrypt_aes256_gcm(
    const byte_string&, const aes_key<32>&, const iv<16>&);
template _SDSDLL_API _NODISCARD wstring decrypt_aes256_gcm(
    const byte_string&, const aes_key<32>&, const iv<16>&);

// FUNCTION TEMPLATE encrypt_aes256_gcm
template <class _Elem, class _Eng>
_NODISCARD constexpr byte_string encrypt_aes256_gcm(
    const _Elem* const _Data, const size_t _Count, const aes_key<32>& _Key, const iv<16>& _Iv) {
    static_assert(is_same_v<_Elem, typename _Eng::char_type>, "Incompatible type.");
    static_assert(is_same_v<size_t, typename _Eng::size_type>, "Incompatible type.");
    if (_Count == 0) { // empty string, do nothing
        return byte_string{};
    }
    
    return _Eng(_Data, _Count, _Key, _Iv, cipher_task::encryption).bytes();
}

template _SDSDLL_API _NODISCARD byte_string encrypt_aes256_gcm(
    const char* const, const size_t, const aes_key<32>&, const iv<16>&);
template _SDSDLL_API _NODISCARD byte_string encrypt_aes256_gcm(
    const wchar_t* const, const size_t, const aes_key<32>&, const iv<16>&);

template <class _Elem, class _Eng>
_NODISCARD constexpr byte_string encrypt_aes256_gcm(
    const _Elem* const _Data, const aes_key<32>& _Key, const iv<16>& _Iv) {
    using _Size_t = typename _Eng::size_type;
    using _Traits = string_traits<_Elem, _Size_t>;
    return _SDSDLL encrypt_aes256_gcm<_Elem, _Eng>(_Data, _Traits::length(_Data), _Key, _Iv);
}

template _SDSDLL_API _NODISCARD byte_string encrypt_aes256_gcm(
    const char* const, const aes_key<32>&, const iv<16>&);
template _SDSDLL_API _NODISCARD byte_string encrypt_aes256_gcm(
    const wchar_t* const, const aes_key<32>&, const iv<16>&);

template <class _Elem, class _Eng>
_NODISCARD constexpr byte_string encrypt_aes256_gcm(
    const basic_string_view<_Elem> _Data, const aes_key<32>& _Key, const iv<16>& _Iv) {
    return _SDSDLL encrypt_aes256_gcm<_Elem, _Eng>(_Data.data(), _Data.size(), _Key, _Iv);
}

template _SDSDLL_API _NODISCARD byte_string encrypt_aes256_gcm(
    const string_view, const aes_key<32>&, const iv<16>&);
template _SDSDLL_API _NODISCARD byte_string encrypt_aes256_gcm(
    const wstring_view, const aes_key<32>&, const iv<16>&);

template <class _Elem, class _Eng>
_NODISCARD constexpr byte_string encrypt_aes256_gcm(
    const basic_string<_Elem>& _Data, const aes_key<32>& _Key, const iv<16>& _Iv) {
    return _SDSDLL encrypt_aes256_gcm<_Elem, _Eng>(_Data.c_str(), _Data.size(), _Key, _Iv);
}

template _SDSDLL_API _NODISCARD byte_string encrypt_aes256_gcm(
    const string&, const aes_key<32>&, const iv<16>&);
template _SDSDLL_API _NODISCARD byte_string encrypt_aes256_gcm(
    const wstring&, const aes_key<32>&, const iv<16>&);
_SDSDLL_END

#endif // _SDSDLL_PREPROCESSOR_GUARD