/***********************************************************************;
 * Copyright (c) 2015 - 2017, Intel Corporation
 * All rights reserved.
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
 ***********************************************************************/

#ifndef TSS2_SYSAPI_UTIL_H
#define TSS2_SYSAPI_UTIL_H

enum cmdStates {CMD_STAGE_INITIALIZE,
                CMD_STAGE_PREPARE,
                CMD_STAGE_SEND_COMMAND,
                CMD_STAGE_RECEIVE_RESPONSE,
                CMD_STAGE_ALL = 0xff };

#pragma pack(push, 1)
typedef struct _TPM20_Header_In {
  TPM_ST tag;
  UINT32 commandSize;
  UINT32 commandCode;
} TPM20_Header_In;

typedef struct _TPM20_Header_Out {
  TPM_ST tag;
  UINT32 responseSize;
  UINT32 responseCode;
} TPM20_Header_Out;

typedef struct _TPM20_ErrorResponse {
  TPM_ST tag;
  UINT32 responseSize;
  UINT32 responseCode;
} TPM20_ErrorResponse;
#pragma pack(pop)

typedef struct {
    TSS2_TCTI_CONTEXT *tctiContext;
    UINT8 *cmdBuffer;
    UINT32 maxCmdSize;
    TPM20_Header_Out rsp_header;

    //
    // These are set by system API and used by helper functions to calculate cpHash,
    // rpHash, and for auditing.
    //
    TPM_CC commandCode;
    UINT32 cpBufferUsedSize;
    UINT8 *cpBuffer;
    UINT32 *rspParamsSize;  // Points to response paramsSize.
    UINT32 rpBufferUsedSize;
    UINT8 *rpBuffer;
    UINT8 previousStage;            // Used to check for sequencing errors.
    UINT8 authsCount;
    UINT8 numResponseHandles;
    struct
    {
        UINT16 tpmVersionInfoValid:1;  // Identifies whether the TPM version info fields are valid; if not valid
                                      // this info can't be used for TPM version-specific workarounds.
        UINT16 decryptAllowed:1;  // Identifies whether this command supports an encrypted command parameter.
        UINT16 encryptAllowed:1;  // Identifies whether this command supports an encrypted response parameter.

        UINT16 decryptNull:1;     // Indicates that the decrypt param was NULL at _Prepare call.
        UINT16 authAllowed:1;

        // Following are used to support decrypt/encrypt sessions with one-call.
        UINT16 decryptSession:1; // If true, complex TPM2B's are not marshalled but instead treated as simple TPM2B's.
        UINT16 encryptSession:1; // If true, complex TPM2B's are not unmarshalled but instead treated as simple TPM2B's.
        UINT16 prepareCalledFromOneCall:1;    // Indicates that the _Prepare call was called from the one-call.
        UINT16 completeCalledFromOneCall:1;    // Indicates that the _Prepare call was called from the one-call.
    };

    // Used to maintain state of SAPI functions.

    // Placeholder for current rval.  This is a convenience and code size optimization for SAPI functions.
    // Marshalling functions check this and SAPI functions return it.
    TSS2_RC rval;

    /* Offset to next data in command/response buffer. */
    size_t nextData;
} _TSS2_SYS_CONTEXT_BLOB;

#define SYS_CONTEXT ((_TSS2_SYS_CONTEXT_BLOB *)sysContext)
#define SYS_RESP_HEADER ((TPM20_Header_Out *)(SYS_CONTEXT->cmdBuffer))
#define SYS_REQ_HEADER ((TPM20_Header_In *)(SYS_CONTEXT->cmdBuffer))

typedef struct {
    TPM_CC commandCode;
    int numCommandHandles;  // Num of handles that require authorization in
                            // command: used for virtualization and for
                            // parsing sessions following handles section.
    int numResponseHandles; // Num of handles that require authorization in
                            // in response: used for virtualization and for
                            // parsing sessions following handles section.
} COMMAND_HANDLES;

struct TSS2_SYS_CONTEXT;

#ifdef __cplusplus
extern "C" {
#endif
TPM_RC CopyCommandHeader(TSS2_SYS_CONTEXT *sysContext, TPM_CC commandCode);
UINT16 GetDigestSize( TPM_ALG_ID authHash );
UINT32 GetCommandSize( TSS2_SYS_CONTEXT *sysContext );

TPM_RC ConcatSizedByteBuffer( TPM2B_MAX_BUFFER *result, TPM2B *addBuffer );

void InitSysContextFields( TSS2_SYS_CONTEXT *sysContext );
void InitSysContextPtrs ( TSS2_SYS_CONTEXT *sysContext, size_t contextSize );

TSS2_RC CompleteChecks( TSS2_SYS_CONTEXT *sysContext );

TSS2_RC CommonComplete( TSS2_SYS_CONTEXT *sysContext );

TSS2_RC  CommonOneCallForNoResponseCmds(
    TSS2_SYS_CONTEXT *sysContext,
    TSS2_SYS_CMD_AUTHS const *cmdAuthsArray,
    TSS2_SYS_RSP_AUTHS *rspAuthsArray
    );

TSS2_RC CommonOneCall(
    TSS2_SYS_CONTEXT *sysContext,
    TSS2_SYS_CMD_AUTHS const *cmdAuthsArray,
    TSS2_SYS_RSP_AUTHS *rspAuthsArray
    );

TSS2_RC CommonPreparePrologue(
    TSS2_SYS_CONTEXT *sysContext,
    TPM_CC commandCode
    );

TSS2_RC CommonPrepareEpilogue(
    TSS2_SYS_CONTEXT *sysContext
    );

int GetNumCommandHandles( TPM_CC commandCode );
int GetNumResponseHandles( TPM_CC commandCode );

TSS2_SYS_CONTEXT *InitSysContext(
    UINT16 maxCommandSize,
    TSS2_TCTI_CONTEXT *tctiContext,
    TSS2_ABI_VERSION *abiVersion
 );

void TeardownSysContext( TSS2_SYS_CONTEXT **sysContext );

#ifdef __cplusplus
}
#endif
#endif  // TSS2_SYSAPI_UTIL_H
