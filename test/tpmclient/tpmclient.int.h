//**********************************************************************;
// Copyright (c) 2015, Intel Corporation
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// 1. Redistributions of source code must retain the above copyright notice,
// this list of conditions and the following disclaimer.
//
// 2. Redistributions in binary form must reproduce the above copyright notice,
// this list of conditions and the following disclaimer in the documentation
// and/or other materials provided with the distribution.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
// THE POSSIBILITY OF SUCH DAMAGE.
//**********************************************************************;

#ifndef TPMCLIENT_INT_H
#define TPMCLIENT_INT_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <uthash.h>

#include "tss2_tpm2_types.h"
#include "tss2_mu.h"
#include "tss2_sys.h"
#include "util/tpm2b.h"
#include "../integration/session-util.h"

extern TSS2_TCTI_CONTEXT *resMgrTctiContext;

enum TSS2_APP_RC_CODE
{
    APP_RC_PASSED,
    APP_RC_GET_NAME_FAILED,
    APP_RC_CREATE_SESSION_KEY_FAILED,
    APP_RC_SESSION_SLOT_NOT_FOUND,
    APP_RC_BAD_ALGORITHM,
    APP_RC_SYS_CONTEXT_CREATE_FAILED,
    APP_RC_GET_SESSION_STRUCT_FAILED,
    APP_RC_GET_SESSION_ALG_ID_FAILED,
    APP_RC_INIT_SYS_CONTEXT_FAILED,
    APP_RC_TEARDOWN_SYS_CONTEXT_FAILED,
    APP_RC_BAD_LOCALITY
};

/*
 * Definition of TSS2_RC values returned by application level stuff. We use
 * this "level" for errors returned by functions in the integration test
 * harness.
 */
#define TSS2_APP_RC_LAYER         TSS2_RC_LAYER(0x10)
#define TSS2_APP_ERROR(base_rc)   (TSS2_APP_RC_LAYER | base_rc)
#define TSS2_APP_RC_BAD_REFERENCE  TSS2_APP_ERROR (TSS2_BASE_RC_BAD_REFERENCE)

// Add this to application-specific error codes so they don't overlap
// with TSS ones which may be re-used for app level errors.
#define APP_RC_OFFSET 0xf800

// These are app specific error codes, so they have
// APP_RC_OFFSET added.
#define TSS2_APP_RC_PASSED                      (APP_RC_PASSED + APP_RC_OFFSET + TSS2_APP_RC_LAYER)
#define TSS2_APP_RC_GET_NAME_FAILED             (APP_RC_GET_NAME_FAILED + APP_RC_OFFSET + TSS2_APP_RC_LAYER)
#define TSS2_APP_RC_CREATE_SESSION_KEY_FAILED   (APP_RC_CREATE_SESSION_KEY_FAILED + APP_RC_OFFSET + TSS2_APP_RC_LAYER)
#define TSS2_APP_RC_SESSION_SLOT_NOT_FOUND      (APP_RC_SESSION_SLOT_NOT_FOUND + APP_RC_OFFSET + TSS2_APP_RC_LAYER)
#define TSS2_APP_RC_BAD_ALGORITHM               (APP_RC_BAD_ALGORITHM + APP_RC_OFFSET + TSS2_APP_RC_LAYER)
#define TSS2_APP_RC_SYS_CONTEXT_CREATE_FAILED   (APP_RC_SYS_CONTEXT_CREATE_FAILED + APP_RC_OFFSET + TSS2_APP_RC_LAYER)
#define TSS2_APP_RC_GET_SESSION_STRUCT_FAILED   (APP_RC_GET_SESSION_STRUCT_FAILED + APP_RC_OFFSET + TSS2_APP_RC_LAYER)
#define TSS2_APP_RC_GET_SESSION_ALG_ID_FAILED   (APP_RC_GET_SESSION_ALG_ID_FAILED + APP_RC_OFFSET + TSS2_APP_RC_LAYER)
#define TSS2_APP_RC_INIT_SYS_CONTEXT_FAILED     (APP_RC_INIT_SYS_CONTEXT_FAILED + APP_RC_OFFSET + TSS2_APP_RC_LAYER)
#define TSS2_APP_RC_TEARDOWN_SYS_CONTEXT_FAILED (APP_RC_TEARDOWN_SYS_CONTEXT_FAILED + APP_RC_OFFSET + TSS2_APP_RC_LAYER)
#define TSS2_APP_RC_BAD_LOCALITY                (APP_RC_BAD_LOCALITY + APP_RC_OFFSET + TSS2_APP_RC_LAYER)

#define TPM2_HT_NO_HANDLE 0xfc000000
#define TPM2_RC_NO_RESPONSE 0xffffffff

#define MAX_NUM_SESSIONS MAX_ACTIVE_SESSIONS

#define APPLICATION_ERROR( errCode ) \
    ( TSS2_APP_RC_LAYER + errCode )

#define APPLICATION_HMAC_ERROR(i) \
    ( TSS2_APP_RC_LAYER + TPM2_RC_S + TPM2_RC_AUTH_FAIL + ( (i ) << 8 ) )

TSS2_RC EncryptCommandParam(SESSION *session, TPM2B_MAX_BUFFER *encryptedData, TPM2B_MAX_BUFFER *clearData, TPM2B_AUTH *authValue );

TSS2_RC DecryptResponseParam(SESSION *session, TPM2B_MAX_BUFFER *clearData, TPM2B_MAX_BUFFER *encryptedData, TPM2B_AUTH *authValue );

#define INIT_SIMPLE_TPM2B_SIZE(type) (type).size = sizeof(type) - 2;

#define YES 1
#define NO 0

#endif
