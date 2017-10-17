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

TPM_RC Tss2_Sys_FieldUpgradeData_Prepare(
    TSS2_SYS_CONTEXT *sysContext,
    TPM2B_MAX_BUFFER *fuData)
{
    TPM_RC rval;

    if (!sysContext)
        return TSS2_SYS_RC_BAD_REFERENCE;

    rval = CommonPreparePrologue(sysContext, TPM_CC_FieldUpgradeData);
    if (rval)
        return rval;

    rval = Tss2_MU_TPM2B_MAX_BUFFER_Marshal(fuData,
                                            SYS_CONTEXT->tpmInBuffPtr,
                                            SYS_CONTEXT->maxCommandSize,
                                            &SYS_CONTEXT->nextData);
    if (rval)
        return rval;

    SYS_CONTEXT->decryptAllowed = 1;
    SYS_CONTEXT->encryptAllowed = 0;
    SYS_CONTEXT->authAllowed = 1;

    return CommonPrepareEpilogue(sysContext);
}

TPM_RC Tss2_Sys_FieldUpgradeData_Complete(
    TSS2_SYS_CONTEXT *sysContext,
    TPMT_HA *nextDigest,
    TPMT_HA *firstDigest)
{
    TPM_RC rval;

    if (!sysContext)
        return TSS2_SYS_RC_BAD_REFERENCE;

    rval = CommonComplete(sysContext);
    if (rval)
        return rval;

    rval = Tss2_MU_TPMT_HA_Unmarshal(SYS_CONTEXT->tpmInBuffPtr,
                                     SYS_CONTEXT->maxCommandSize,
                                     &SYS_CONTEXT->nextData,
                                     nextDigest);
    if (rval)
        return rval;

    return Tss2_MU_TPMT_HA_Unmarshal(SYS_CONTEXT->tpmInBuffPtr,
                                     SYS_CONTEXT->maxCommandSize,
                                     &SYS_CONTEXT->nextData,
                                     firstDigest);
}

TPM_RC Tss2_Sys_FieldUpgradeData(
    TSS2_SYS_CONTEXT *sysContext,
    TSS2_SYS_CMD_AUTHS const *cmdAuthsArray,
    TPM2B_MAX_BUFFER *fuData,
    TPMT_HA *nextDigest,
    TPMT_HA *firstDigest,
    TSS2_SYS_RSP_AUTHS *rspAuthsArray)
{
    TSS2_RC rval;

    rval = Tss2_Sys_FieldUpgradeData_Prepare(sysContext, fuData);
    if (rval)
        return rval;

    rval = CommonOneCall(sysContext, cmdAuthsArray, rspAuthsArray);
    if (rval)
        return rval;

    return Tss2_Sys_FieldUpgradeData_Complete(sysContext, nextDigest,
                                              firstDigest);
}
