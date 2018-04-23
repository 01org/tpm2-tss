/*
 * Copyright (c) 2017, Intel Corporation
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGE.
 */
#ifndef TEST_INTEGRATION_SAPI_UTIL_H
#define TEST_INTEGRATION_SAPI_UTIL_H

#include "tss2_tpm2_types.h"
#include "tss2_sys.h"
#include "util/tpm2b.h"

/*
 * This macro is like the GNU TEMP_FAILURE_RETRY macro for the
 * TPM2_RC_RETRY response code.
 */
#define TSS2_RETRY_EXP(expression)                         \
    ({                                                     \
        TSS2_RC __result = 0;                              \
        do {                                               \
            __result = (expression);                       \
        } while ((__result & 0x0000ffff) == TPM2_RC_RETRY); \
        __result;                                          \
    })
/*
 * tpm2b default initializers, these set the size to the max for the default
 * structure and zero's the data area.
 */
#define TPM2B_SIZE(type) (sizeof (type) - 2)
#define TPM2B_NAMED_INIT(type, field) \
    { \
        .size = TPM2B_SIZE (type), \
        .field = { 0 } \
    }
#define TPM2B_DIGEST_INIT TPM2B_NAMED_INIT (TPM2B_DIGEST, buffer)
#define TPM2B_NAME_INIT TPM2B_NAMED_INIT (TPM2B_NAME, name)
#define TPM2B_PRIVATE_INIT TPM2B_NAMED_INIT (TPM2B_PRIVATE, buffer)

#define TPM2B_MAX_BUFFER_INIT { .size = TPM2_MAX_DIGEST_BUFFER }
#define TPM2B_IV_INIT { .size = TPM2_MAX_SYM_BLOCK_SIZE }

#define BUFFER_SIZE(type, field) (sizeof((((type *)NULL)->t.field)))
#define TPM2B_TYPE_INIT(type, field) { .size = BUFFER_SIZE(type, field), }
/*
 * Use te provide SAPI context to create & load a primary key. The key will
 * be a 2048 bit (restricted decryption) RSA key. The associated symmetric
 * key is a 128 bit AES (CFB mode) key.
 */
TSS2_RC
create_primary_rsa_2048_aes_128_cfb (
    TSS2_SYS_CONTEXT *sapi_context,
    TPM2_HANDLE       *handle);
/*
 * This function creates a 128 bit symmetric AES key in cbc mode. This key will
 * be created as the child of the parameter 'handle_parent'. The handle for the
 * newly created AND loaded key is returned in the parameter 'handle'.
 */
TSS2_RC
create_aes_128_cfb (
    TSS2_SYS_CONTEXT *sapi_context,
    TPM2_HANDLE        handle_parent,
    TPM2_HANDLE       *handle);

/*
 * This function creates a RSA key of KEYEDHASH type.
 */
TSS2_RC
create_keyedhash_key (
    TSS2_SYS_CONTEXT *sapi_context,
    TPM2_HANDLE       handle_parent,
    TPM2_HANDLE      *handle);

/*
 * This function will decrypt or encrypt the 'data_in' buffer and return the
 * results in the 'data_out' parameter. Decrypt or encrypt is selected using
 * the 'decrypt' TPMI_YES_NO parameter. The key used for the operation is
 * provided in the 'handle' parameter.
 * Under the covers this function uses an IV of all zeros and so it can not
 * be used for streaming. It can only be used to encrypt or decrypt a single
 * buffer.
 */
TSS2_RC
tpm_encrypt_decrypt_cfb (
    TSS2_SYS_CONTEXT *sapi_context,
    TPMI_DH_OBJECT    handle,
    TPMI_YES_NO       decrypt,
    TPM2B_MAX_BUFFER *data_in,
    TPM2B_MAX_BUFFER *data_out);
/*
 * This is a convenience wrapper around the encrypt_decrypt_cfb function.
 */
TSS2_RC
tpm_encrypt_cfb (
    TSS2_SYS_CONTEXT *sapi_context,
    TPMI_DH_OBJECT    handle,
    TPM2B_MAX_BUFFER *data_in,
    TPM2B_MAX_BUFFER *data_out);
/*
 * This is a convenience wrapper around the encrypt_decrypt_cfb function.
 */
TSS2_RC
tpm_decrypt_cfb (
    TSS2_SYS_CONTEXT *sapi_context,
    TPMI_DH_OBJECT    handle,
    TPM2B_MAX_BUFFER *data_in,
    TPM2B_MAX_BUFFER *data_out);
/*
 * This function is identical to the encrypt_decrypt_cfb function but under
 * the covers it uses the EncryptDecrypt2 function instead of EncryptDecrypt.
 */
TSS2_RC
tpm_encrypt_decrypt_2_cfb (
    TSS2_SYS_CONTEXT *sapi_context,
    TPMI_DH_OBJECT    handle,
    TPMI_YES_NO       decrypt,
    TPM2B_MAX_BUFFER *data_in,
    TPM2B_MAX_BUFFER *data_out);
/*
 * This is a convenience wrapper around the encrypt_decrypt_2_cfb function.
 */
TSS2_RC
tpm_encrypt_2_cfb (
    TSS2_SYS_CONTEXT *sapi_context,
    TPMI_DH_OBJECT    handle,
    TPM2B_MAX_BUFFER *data_in,
    TPM2B_MAX_BUFFER *data_out);
/*
 * This is a convenience wrapper around the encrypt_decrypt_2_cfb function.
 */
TSS2_RC
tpm_decrypt_2_cfb (
    TSS2_SYS_CONTEXT *sapi_context,
    TPMI_DH_OBJECT    handle,
    TPM2B_MAX_BUFFER *data_in,
    TPM2B_MAX_BUFFER *data_out);

/*
 * This is a helper function for digest calculation.
 * alg can be TPM2_ALG_SHA1, TPM2_ALG_SHA256, TPM2_ALG_SHA384,
 * and TPM2_ALG_SHA512
 */
TSS2_RC
hash (
    TPM2_ALG_ID alg,
    const void *data,
    int size,
    TPM2B_DIGEST *out);

/*
 * This is a helper function for calculating HMAC.
 * alg can be TPM2_ALG_SHA1, TPM2_ALG_SHA256, TPM2_ALG_SHA384,
 * and TPM2_ALG_SHA512
 */
TSS2_RC
hmac(
    TPM2_ALG_ID alg,
    const void *key,
    int key_len,
    TPM2B_DIGEST **buffer_list,
    TPM2B_DIGEST *out);

/*
 * Returns digest size for a give hash alg
 */
UINT16
GetDigestSize(TPM2_ALG_ID hash);

TSS2_RC
CompareSizedByteBuffer(
        TPM2B *buffer1,
        TPM2B *buffer2);

TSS2_RC
ConcatSizedByteBuffer(
        TPM2B_MAX_BUFFER *result,
        TPM2B *buf);

void
CatSizedByteBuffer(
        TPM2B *dest,
        TPM2B *src);

UINT16
CopySizedByteBuffer(
        TPM2B *dest,
        TPM2B *src);

#endif /* TEST_INTEGRATION_SAPI_UTIL_H */
