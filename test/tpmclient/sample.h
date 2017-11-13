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

#ifndef SAMPLE_H
#define SAMPLE_H

#ifdef __cplusplus
extern "C" {
#endif


#include "sapi/tss2_tpm2_types.h"
#include "tpmclient.h"
#include <stdio.h>
#include <stdlib.h>
#include "syscontext.h"
#include "common/debug.h"

extern TSS2_TCTI_CONTEXT *resMgrTctiContext;
extern TSS2_ABI_VERSION abiVersion;

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

// Add this to application-specific error codes so they don't overlap
// with TSS ones which may be re-used for app level errors.
#define APP_RC_OFFSET 0x100

// These are app specific error codes, so they have
// APP_RC_OFFSET added.
#define TSS2_APP_RC_PASSED                      (APP_RC_PASSED + APP_RC_OFFSET + TSS2_APP_ERROR_LEVEL)
#define TSS2_APP_RC_GET_NAME_FAILED             (APP_RC_GET_NAME_FAILED + APP_RC_OFFSET + TSS2_APP_ERROR_LEVEL)
#define TSS2_APP_RC_CREATE_SESSION_KEY_FAILED   (APP_RC_CREATE_SESSION_KEY_FAILED + APP_RC_OFFSET + TSS2_APP_ERROR_LEVEL)
#define TSS2_APP_RC_SESSION_SLOT_NOT_FOUND      (APP_RC_SESSION_SLOT_NOT_FOUND + APP_RC_OFFSET + TSS2_APP_ERROR_LEVEL)
#define TSS2_APP_RC_BAD_ALGORITHM               (APP_RC_BAD_ALGORITHM + APP_RC_OFFSET + TSS2_APP_ERROR_LEVEL)
#define TSS2_APP_RC_SYS_CONTEXT_CREATE_FAILED   (APP_RC_SYS_CONTEXT_CREATE_FAILED + APP_RC_OFFSET + TSS2_APP_ERROR_LEVEL)
#define TSS2_APP_RC_GET_SESSION_STRUCT_FAILED   (APP_RC_GET_SESSION_STRUCT_FAILED + APP_RC_OFFSET + TSS2_APP_ERROR_LEVEL)
#define TSS2_APP_RC_GET_SESSION_ALG_ID_FAILED   (APP_RC_GET_SESSION_ALG_ID_FAILED + APP_RC_OFFSET + TSS2_APP_ERROR_LEVEL)
#define TSS2_APP_RC_INIT_SYS_CONTEXT_FAILED     (APP_RC_INIT_SYS_CONTEXT_FAILED + APP_RC_OFFSET + TSS2_APP_ERROR_LEVEL)
#define TSS2_APP_RC_TEARDOWN_SYS_CONTEXT_FAILED (APP_RC_TEARDOWN_SYS_CONTEXT_FAILED + APP_RC_OFFSET + TSS2_APP_ERROR_LEVEL)
#define TSS2_APP_RC_BAD_LOCALITY                (APP_RC_BAD_LOCALITY + APP_RC_OFFSET + TSS2_APP_ERROR_LEVEL)

#define TPM_HT_NO_HANDLE 0xfc000000
#define TPM_RC_NO_RESPONSE 0xffffffff

#define MAX_NUM_SESSIONS MAX_ACTIVE_SESSIONS
#define MAX_NUM_ENTITIES 100

#define APPLICATION_ERROR( errCode ) \
    ( TSS2_APP_ERROR_LEVEL + errCode )

#define APPLICATION_HMAC_ERROR(i) \
    ( TSS2_APP_ERROR_LEVEL + TPM_RC_S + TPM_RC_AUTH_FAIL + ( (i ) << 8 ) )

typedef struct {
    // Inputs to StartAuthSession; these need to be saved
    // so that HMACs can be calculated.
    TPMI_DH_OBJECT tpmKey;
    TPMI_DH_ENTITY bind;
    TPM2B_ENCRYPTED_SECRET encryptedSalt;
    TPM2B_MAX_BUFFER salt;
    TPM_SE sessionType;
    TPMT_SYM_DEF symmetric;
    TPMI_ALG_HASH authHash;

    // Outputs from StartAuthSession; these also need
    // to be saved for calculating HMACs and
    // other session related functions.
    TPMI_SH_AUTH_SESSION sessionHandle;
    TPM2B_NONCE nonceTPM;

    // Internal state for the session
    TPM2B_DIGEST sessionKey;
    TPM2B_DIGEST authValueBind;     // authValue of bind object
    TPM2B_NONCE nonceNewer;
    TPM2B_NONCE nonceOlder;
    TPM2B_NONCE nonceTpmDecrypt;
    TPM2B_NONCE nonceTpmEncrypt;
    TPM2B_NAME name;                // Name of the object the session handle
                                    // points to.  Used for computing HMAC for
                                    // any HMAC sessions present.
                                    //
    void *hmacPtr;                  // Pointer to HMAC field in the marshalled
                                    // data stream for the session.
                                    // This allows the function to calculate
                                    // and fill in the HMAC after marshalling
                                    // of all the inputs.
                                    //
                                    // This is only used if the session is an
                                    // HMAC session.
                                    //
    UINT8 nvNameChanged;            // Used for some special case code
                                    // dealing with the NV written state.
} SESSION;

//
// Structure used to maintain entity data.  Right now it just
// consists of handles/authValue pairs.
//
typedef struct{
    TPM_HANDLE entityHandle;
    TPM2B_AUTH entityAuth;
    UINT8 nvNameChanged;
} ENTITY;

void InitEntities();
TSS2_RC AddEntity( TPM_HANDLE entityHandle, TPM2B_AUTH *auth );
TSS2_RC DeleteEntity( TPM_HANDLE entityHandle );
TSS2_RC GetEntityAuth( TPM_HANDLE entityHandle, TPM2B_AUTH *auth );
TSS2_RC GetEntity( TPM_HANDLE entityHandle, ENTITY **entity );
TSS2_RC GetSessionStruct( TPMI_SH_AUTH_SESSION authHandle, SESSION **pSession );
TSS2_RC GetSessionAlgId( TPMI_SH_AUTH_SESSION authHandle, TPMI_ALG_HASH *sessionAlgId );
TSS2_RC EndAuthSession( SESSION *session );
TSS2_RC ComputeCommandHmacs( TSS2_SYS_CONTEXT *sysContext, TPM_HANDLE handle1,
    TPM_HANDLE handle2, TSS2_SYS_CMD_AUTHS *pSessionsData,
    TSS2_RC sessionCmdRval );

extern INT16 sessionEntriesUsed;

extern void InitSessionsTable();

extern UINT32 ( *ComputeSessionHmacPtr )(
    TSS2_SYS_CONTEXT *sysContext,
    TPMS_AUTH_COMMAND *cmdAuth,          // Pointer to session input struct
    TPM_HANDLE entityHandle,             // Used to determine if we're accessing a different
                                         // resource than the bound resoure.
    TSS2_RC responseCode,                 // Response code for the command, 0xffff for "none" is
                                         // used to indicate that no response code is present
                                         // (used for calculating command HMACs vs response HMACs).
    TPM_HANDLE handle1,                  // First handle == 0xff000000 indicates no handle
    TPM_HANDLE handle2,                  // Second handle == 0xff000000 indicates no handle
    TPMA_SESSION sessionAttributes,      // Current session attributes
    TPM2B_DIGEST *result,                // Where the result hash is saved.
    TSS2_RC sessionCmdRval
    );


extern TSS2_RC CheckResponseHMACs( TSS2_SYS_CONTEXT *sysContext,
    TSS2_RC responseCode,
    TSS2_SYS_CMD_AUTHS *pSessionsDataIn, TPM_HANDLE handle1, TPM_HANDLE handle2,
    TSS2_SYS_RSP_AUTHS *pSessionsDataOut );

TSS2_RC StartAuthSessionWithParams( SESSION **session, TPMI_DH_OBJECT tpmKey, TPM2B_MAX_BUFFER *salt,
    TPMI_DH_ENTITY bind, TPM2B_AUTH *bindAuth, TPM2B_NONCE *nonceCaller, TPM2B_ENCRYPTED_SECRET *encryptedSalt,
    TPM_SE sessionType, TPMT_SYM_DEF *symmetric, TPMI_ALG_HASH algId, TSS2_TCTI_CONTEXT *tctiContext );

//
// Used by upper layer code to save and update entity data
// (authValue, specifically) at creation and use time.
//
// NOTE: this really needs to be turned into a linked list
// with add, find, and remove entries, instead of a fixed
// length array.
//
ENTITY entities[MAX_NUM_ENTITIES+1];

//
// This function calculates the session HMAC
//
UINT32 TpmComputeSessionHmac(
    TSS2_SYS_CONTEXT *sysContext,
    TPMS_AUTH_COMMAND *pSessionDataIn, // Pointer to session input struct
    TPM_HANDLE entityHandle,             // Used to determine if we're accessing a different
                                         // resource than the bound resoure.
    TSS2_RC responseCode,                 // Response code for the command, 0xffff for "none" is
                                         // used to indicate that no response code is present
                                         // (used for calculating command HMACs vs response HMACs).
    TPM_HANDLE handle1,                  // First handle == 0xff000000 indicates no handle
    TPM_HANDLE handle2,                  // Second handle == 0xff000000 indicates no handle
    TPMA_SESSION sessionAttributes,      // Current session attributes
    TPM2B_DIGEST *result,                // Where the result hash is saved.
    TSS2_RC sessionCmdRval
    );

TSS2_RC TpmCalcPHash( TSS2_SYS_CONTEXT *sysContext, TPM_HANDLE handle1,
    TPM_HANDLE handle2, TPMI_ALG_HASH authHash, TSS2_RC responseCode, TPM2B_DIGEST *pHash );

void PrintSizedBuffer( TPM2B *sizedBuffer );

void InitNullSession( TPMS_AUTH_COMMAND *nullSessionData );

TSS2_RC LoadExternalHMACKey( TPMI_ALG_HASH hashAlg, TPM2B *key, TPM_HANDLE *keyHandle, TPM2B_NAME *keyName );

UINT16 CopySizedByteBuffer( TPM2B *dest, TPM2B *src );

TSS2_RC EncryptCommandParam( SESSION *session, TPM2B_MAX_BUFFER *encryptedData, TPM2B_MAX_BUFFER *clearData, TPM2B_AUTH *authValue );

TSS2_RC DecryptResponseParam( SESSION *session, TPM2B_MAX_BUFFER *clearData, TPM2B_MAX_BUFFER *encryptedData, TPM2B_AUTH *authValue );

TSS2_RC KDFa( TPMI_ALG_HASH hashAlg, TPM2B *key, char *label, TPM2B *contextU, TPM2B *contextV,
    UINT16 bits, TPM2B_MAX_BUFFER *resultKey );

UINT32 TpmHashSequence( TPMI_ALG_HASH hashAlg, UINT8 numBuffers, TPM2B_DIGEST *bufferList, TPM2B_DIGEST *result );

void CatSizedByteBuffer( TPM2B *dest, TPM2B *src );

void RollNonces( SESSION *session, TPM2B_NONCE *newNonce  );

TSS2_RC SetLocality( TSS2_SYS_CONTEXT *sysContext, UINT8 locality );

TSS2_RC TpmHmac( TPMI_ALG_HASH hashAlg, TPM2B *key,TPM2B **bufferList, TPM2B_DIGEST *result );

UINT32 TpmHash( TPMI_ALG_HASH hashAlg, UINT16 size, BYTE *data, TPM2B_DIGEST *result );

UINT32 TpmHandleToName( TPM_HANDLE handle, TPM2B_NAME *name );

int TpmClientPrintf( UINT8 type, const char *format, ...);

#ifdef __cplusplus
}
#endif


#endif
