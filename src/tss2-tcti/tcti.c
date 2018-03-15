/*
 * Copyright (c) 2015 - 2018 Intel Corporation
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
 */

#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "tpm20.h"
#include "tcti.h"
#define LOGMODULE tcti
#include "util/log.h"

TSS2_RC
tcti_common_checks (
    TSS2_TCTI_CONTEXT *tcti_context)
{
    if (tcti_context == NULL) {
        return TSS2_TCTI_RC_BAD_REFERENCE;
    }
    if (TSS2_TCTI_MAGIC (tcti_context) != TCTI_MAGIC ||
        TSS2_TCTI_VERSION (tcti_context) != TCTI_VERSION) {
        return TSS2_TCTI_RC_BAD_CONTEXT;
    }

    return TSS2_RC_SUCCESS;
}

TSS2_RC
tcti_transmit_checks (
    TSS2_TCTI_CONTEXT_INTEL *tcti_intel,
    const uint8_t *command_buffer)
{
    TSS2_RC rc;

    rc = tcti_common_checks (tcti_context_base_cast (tcti_intel));
    if (rc != TSS2_RC_SUCCESS) {
        return rc;
    }
    if (tcti_intel->state != TCTI_STATE_TRANSMIT) {
        return TSS2_TCTI_RC_BAD_SEQUENCE;
    }
    if (command_buffer == NULL) {
        return TSS2_TCTI_RC_BAD_REFERENCE;
    }

    return TSS2_RC_SUCCESS;
}

TSS2_RC
tcti_receive_checks (
    TSS2_TCTI_CONTEXT_INTEL *tcti_intel,
    size_t *response_size,
    unsigned char *response_buffer)
{
    TSS2_RC rc;

    rc = tcti_common_checks (tcti_context_base_cast (tcti_intel));
    if (rc != TSS2_RC_SUCCESS) {
        return rc;
    }
    if (tcti_intel->state != TCTI_STATE_RECEIVE) {
        return TSS2_TCTI_RC_BAD_SEQUENCE;
    }
    if (response_size == NULL) {
        return TSS2_TCTI_RC_BAD_REFERENCE;
    }

    return TSS2_RC_SUCCESS;
}


TSS2_RC
tcti_make_sticky_not_implemented (
    TSS2_TCTI_CONTEXT *tctiContext,
    TPM2_HANDLE *handle,
    uint8_t sticky)
{
    return TSS2_TCTI_RC_NOT_IMPLEMENTED;
}

TSS2_RC
parse_header (
    const uint8_t *buf,
    tpm_header_t *header)
{
    TSS2_RC rc;
    size_t offset = 0;

    LOG_TRACE ("Parsing header from buffer: 0x%" PRIxPTR, (uintptr_t)buf);
    rc = Tss2_MU_TPM2_ST_Unmarshal (buf,
                                    TPM_HEADER_SIZE,
                                    &offset,
                                    &header->tag);
    if (rc != TSS2_RC_SUCCESS) {
        LOG_ERROR ("Failed to unmarshal tag.");
        return rc;
    }
    rc = Tss2_MU_UINT32_Unmarshal (buf,
                                   TPM_HEADER_SIZE,
                                   &offset,
                                   &header->size);
    if (rc != TSS2_RC_SUCCESS) {
        LOG_ERROR ("Failed to unmarshal command size.");
        return rc;
    }
    rc = Tss2_MU_UINT32_Unmarshal (buf,
                                   TPM_HEADER_SIZE,
                                   &offset,
                                   &header->code);
    if (rc != TSS2_RC_SUCCESS) {
        LOG_ERROR ("Failed to unmarshal command code.");
    }
    return rc;
}
