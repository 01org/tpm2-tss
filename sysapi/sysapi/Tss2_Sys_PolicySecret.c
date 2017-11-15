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

#include "sapi/tpm20.h"
#include "sysapi_util.h"

TSS2_RC Tss2_Sys_PolicySecret_Prepare(
    TSS2_SYS_CONTEXT *sysContext,
    TPMI_DH_ENTITY authHandle,
    TPMI_SH_POLICY policySession,
    const TPM2B_NONCE	*nonceTPM,
    const TPM2B_DIGEST	*cpHashA,
    const TPM2B_NONCE	*policyRef,
    INT32 expiration)
{
    TSS2_RC rval;

    if (!sysContext)
        return TSS2_SYS_RC_BAD_REFERENCE;

    rval = CommonPreparePrologue(sysContext, TPM2_CC_PolicySecret);
    if (rval)
        return rval;

    rval = Tss2_MU_UINT32_Marshal(authHandle, SYS_CONTEXT->cmdBuffer,
                                  SYS_CONTEXT->maxCmdSize,
                                  &SYS_CONTEXT->nextData);
    if (rval)
        return rval;

    rval = Tss2_MU_UINT32_Marshal(policySession, SYS_CONTEXT->cmdBuffer,
                                  SYS_CONTEXT->maxCmdSize,
                                  &SYS_CONTEXT->nextData);
    if (rval)
        return rval;

    if (!nonceTPM) {
        SYS_CONTEXT->decryptNull = 1;

        rval = Tss2_MU_UINT16_Marshal(0, SYS_CONTEXT->cmdBuffer,
                                      SYS_CONTEXT->maxCmdSize,
                                      &SYS_CONTEXT->nextData);
    } else {

        rval = Tss2_MU_TPM2B_NONCE_Marshal(nonceTPM, SYS_CONTEXT->cmdBuffer,
                                           SYS_CONTEXT->maxCmdSize,
                                           &SYS_CONTEXT->nextData);
    }

    if (rval)
        return rval;

    if (!cpHashA)
        rval = Tss2_MU_UINT16_Marshal(0, SYS_CONTEXT->cmdBuffer,
                                      SYS_CONTEXT->maxCmdSize,
                                      &SYS_CONTEXT->nextData);
    else
        rval = Tss2_MU_TPM2B_DIGEST_Marshal(cpHashA, SYS_CONTEXT->cmdBuffer,
                                            SYS_CONTEXT->maxCmdSize,
                                            &SYS_CONTEXT->nextData);
    if (rval)
        return rval;

    if (!policyRef)
        rval = Tss2_MU_UINT16_Marshal(0, SYS_CONTEXT->cmdBuffer,
                                      SYS_CONTEXT->maxCmdSize,
                                      &SYS_CONTEXT->nextData);
    else
        rval = Tss2_MU_TPM2B_NONCE_Marshal(policyRef, SYS_CONTEXT->cmdBuffer,
                                           SYS_CONTEXT->maxCmdSize,
                                           &SYS_CONTEXT->nextData);
    if (rval)
        return rval;

    rval = Tss2_MU_UINT32_Marshal(expiration, SYS_CONTEXT->cmdBuffer,
                                  SYS_CONTEXT->maxCmdSize,
                                  &SYS_CONTEXT->nextData);
    if (rval)
        return rval;

    SYS_CONTEXT->decryptAllowed = 1;
    SYS_CONTEXT->encryptAllowed = 1;
    SYS_CONTEXT->authAllowed = 1;

    return CommonPrepareEpilogue(sysContext);
}

TSS2_RC Tss2_Sys_PolicySecret_Complete(
    TSS2_SYS_CONTEXT *sysContext,
    TPM2B_TIMEOUT *timeout,
    TPMT_TK_AUTH *policyTicket)
{
    TSS2_RC rval;

    if (!sysContext)
        return TSS2_SYS_RC_BAD_REFERENCE;

    rval = CommonComplete(sysContext);
    if (rval)
        return rval;

    rval = Tss2_MU_TPM2B_TIMEOUT_Unmarshal(SYS_CONTEXT->cmdBuffer,
                                           SYS_CONTEXT->maxCmdSize,
                                           &SYS_CONTEXT->nextData, timeout);
    if (rval)
        return rval;

    return Tss2_MU_TPMT_TK_AUTH_Unmarshal(SYS_CONTEXT->cmdBuffer,
                                          SYS_CONTEXT->maxCmdSize,
                                          &SYS_CONTEXT->nextData, policyTicket);
}

TSS2_RC Tss2_Sys_PolicySecret(
    TSS2_SYS_CONTEXT *sysContext,
    TPMI_DH_ENTITY authHandle,
    TPMI_SH_POLICY policySession,
    TSS2_SYS_CMD_AUTHS const *cmdAuthsArray,
    const TPM2B_NONCE	*nonceTPM,
    const TPM2B_DIGEST	*cpHashA,
    const TPM2B_NONCE	*policyRef,
    INT32 expiration,
    TPM2B_TIMEOUT *timeout,
    TPMT_TK_AUTH *policyTicket,
    TSS2_SYS_RSP_AUTHS *rspAuthsArray)
{
    TSS2_RC rval;

    rval = Tss2_Sys_PolicySecret_Prepare(sysContext, authHandle, policySession,
                                         nonceTPM, cpHashA, policyRef, expiration);
    if (rval)
        return rval;

    rval = CommonOneCall(sysContext, cmdAuthsArray, rspAuthsArray);
    if (rval)
        return rval;

    return Tss2_Sys_PolicySecret_Complete(sysContext, timeout, policyTicket);
}
