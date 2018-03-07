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

#include "tcti/common.h"
#include "sapi/tpm20.h"
#include "tcti.h"
#define LOGMODULE tcti
#include "log/log.h"

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
tcti_send_checks (
    TSS2_TCTI_CONTEXT *tctiContext,
    const uint8_t *command_buffer)
{
    TSS2_TCTI_CONTEXT_INTEL *tcti_intel;
    TSS2_RC rc;

    tcti_intel = tcti_context_intel_cast (tctiContext);;
    rc = tcti_common_checks (tctiContext);
    if (rc != TSS2_RC_SUCCESS) {
        return rc;
    }
    if (command_buffer == NULL) {
        return TSS2_TCTI_RC_BAD_REFERENCE;
    }
    if (tcti_intel->previousStage == TCTI_STAGE_SEND_COMMAND) {
        return TSS2_TCTI_RC_BAD_SEQUENCE;
    }

    return TSS2_RC_SUCCESS;
}

TSS2_RC
tcti_receive_checks (
    TSS2_TCTI_CONTEXT *tctiContext,
    size_t *response_size,
    unsigned char *response_buffer)
{
    TSS2_TCTI_CONTEXT_INTEL *tcti_intel;
    TSS2_RC rc;

    tcti_intel = tcti_context_intel_cast (tctiContext);
    rc = tcti_common_checks (tctiContext);
    if (rc != TSS2_RC_SUCCESS) {
        return rc;
    }
    if (response_buffer == NULL || response_size == NULL) {
        return TSS2_TCTI_RC_BAD_REFERENCE;
    }
    if (tcti_intel->previousStage == TCTI_STAGE_RECEIVE_RESPONSE) {
        return TSS2_TCTI_RC_BAD_SEQUENCE;
    }

    return TSS2_RC_SUCCESS;
}

ssize_t
write_all (
    int fd,
    const uint8_t *buf,
    size_t size)
{
    ssize_t written = 0;
    size_t written_total = 0;

    do {
        LOG_DEBUG("writing %zu bytes starting at 0x%" PRIxPTR " to fd %d",
                  size - written_total,
                  (uintptr_t)buf + written_total,
                  fd);
        written = TEMP_RETRY (write (fd,
                                     (const char*)&buf [written_total],
                                     size - written_total));
        if (written >= 0) {
            LOG_DEBUG ("wrote %zd bytes to fd %d", written, fd);
            written_total += (size_t)written;
        } else {
            LOG_ERROR ("failed to write to fd %d: %s", fd, strerror (errno));
            return written_total;
        }
    } while (written_total < size);

    return (ssize_t)written_total;
}

TSS2_RC
tcti_make_sticky_not_implemented (
    TSS2_TCTI_CONTEXT *tctiContext,
    TPM2_HANDLE *handle,
    uint8_t sticky)
{
    return TSS2_TCTI_RC_NOT_IMPLEMENTED;
}
