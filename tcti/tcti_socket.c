/***********************************************************************
 * Copyright (c) 2015 - 2017 Intel Corporation
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
 **********************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#include "sapi/tpm20.h"
#include "sapi/tss2_mu.h"
#include "tcti/tcti_socket.h"
#include "sysapi_util.h"
#include "common/debug.h"
#include "tcti.h"
#include "logging.h"
#include "sockets.h"
#include "tss2_endian.h"

static TSS2_RC tctiRecvBytes (
    TSS2_TCTI_CONTEXT *tctiContext,
    SOCKET sock,
    unsigned char *data,
    int len
    )
{
    TSS2_RC result = 0;
    result = recvBytes (sock, data, len);
    if ((INT32)result == SOCKET_ERROR) {
        TCTI_LOG (tctiContext,
                  NO_PREFIX,
                  "In recvBytes, recv failed (socket: 0x%x) with error: %d\n",
                  sock,
                  WSAGetLastError ());
        return TSS2_TCTI_RC_IO_ERROR;
    }
#ifdef DEBUG_SOCKETS
    TCTI_LOG (tctiContext,
              NO_PREFIX,
              "Receive Bytes from socket #0x%x: \n",
              sock);
    TCTI_LOG_BUFFER (tctiContext, NO_PREFIX, data, len);
#endif

    return TSS2_RC_SUCCESS;
}

static TSS2_RC tctiSendBytes (
    TSS2_TCTI_CONTEXT *tctiContext,
    SOCKET sock,
    const unsigned char *data,
    int len
    )
{
    TSS2_RC ret = TSS2_RC_SUCCESS;

#ifdef DEBUG_SOCKETS
    TCTI_LOG (tctiContext, NO_PREFIX, "Send Bytes to socket #0x%x: \n", sock);
    TCTI_LOG_BUFFER (tctiContext, NO_PREFIX, (UINT8 *)data, len);
#endif

    ret = sendBytes (sock, data, len);
    if (ret != TSS2_RC_SUCCESS) {
        TCTI_LOG (tctiContext,
                  NO_PREFIX,
                  "In recvBytes, recv failed (socket: 0x%x) with error: %d\n",
                  sock,
                  WSAGetLastError ());
    }
    return ret;
}

TSS2_RC SendSessionEndSocketTcti (
    TSS2_TCTI_CONTEXT *tctiContext,
    UINT8 tpmCmdServer
    )
{
    TSS2_TCTI_CONTEXT_INTEL *tcti_intel = tcti_context_intel_cast (tctiContext);
    /* Value for "send command" to MS simulator */
    uint8_t buffer [4] = { 0, };
    SOCKET sock;
    TSS2_RC rval = TSS2_RC_SUCCESS;

    if (tpmCmdServer) {
        sock = tcti_intel->tpmSock;
    } else {
        sock = tcti_intel->otherSock;
    }

    rval = Tss2_MU_UINT32_Marshal (TPM_SESSION_END,
                                   buffer,
                                   sizeof (buffer),
                                   NULL);
    if (rval == TSS2_RC_SUCCESS) {
        return rval;
    }
    rval = tctiSendBytes (tctiContext, sock, (char unsigned *)buffer, 4);

    return( rval );
}

TSS2_RC SocketSendTpmCommand(
    TSS2_TCTI_CONTEXT *tctiContext,
    size_t command_size,
    uint8_t *command_buffer
    )
{
    TSS2_TCTI_CONTEXT_INTEL *tcti_intel = tcti_context_intel_cast (tctiContext);
    UINT32 tpmSendCommand;
    UINT32 cnt, cnt1;
    UINT8 locality;
    TSS2_RC rval = TSS2_RC_SUCCESS;
    size_t offset;

#ifdef DEBUG
    UINT32 commandCode;
    printf_type rmPrefix;
#endif
    rval = tcti_send_checks (tctiContext, command_buffer);
    if (rval != TSS2_RC_SUCCESS) {
        return rval;
    }

#ifdef DEBUG
    if (tcti_intel->status.rmDebugPrefix == 1) {
        rmPrefix = RM_PREFIX;
    } else {
        rmPrefix = NO_PREFIX;
    }

    if (tcti_intel->status.debugMsgEnabled == 1) {
        TCTI_LOG (tctiContext, rmPrefix, "");
        offset = sizeof (TPM2_ST) + sizeof (UINT32);
        rval = Tss2_MU_TPM2_CC_Unmarshal (command_buffer,
                                         command_size,
                                         &offset,
                                         &commandCode);

#ifdef DEBUG_SOCKETS
        TCTI_LOG (tctiContext,
                  NO_PREFIX,
                  "Command sent on socket #0x%x: %s\n",
                  TCTI_CONTEXT_INTEL->tpmSock,
                  strTpmCommandCode (commandCode));

#else
        TCTI_LOG (tctiContext,
                  NO_PREFIX,
                  "Cmd sent: %s\n",
                  strTpmCommandCode (commandCode));
#endif
    }
#endif
    /*
     * Size TPM 1.2 and TPM 2.0 headers overlap exactly, we can use
     * either 1.2 or 2.0 header to get the size.
     */
    offset = sizeof (TPM2_ST);
    rval = Tss2_MU_UINT32_Unmarshal (command_buffer,
                                     command_size,
                                     &offset,
                                     &cnt);

    /* Send TPM2_SEND_COMMAND */
    rval = Tss2_MU_UINT32_Marshal (MS_SIM_TPM_SEND_COMMAND,
                           (uint8_t*)&tpmSendCommand,
                           sizeof (tpmSendCommand),
                           NULL);  /* Value for "send command" to MS simulator. */
    rval = tctiSendBytes (tctiContext,
                          tcti_intel->tpmSock,
                          (unsigned char *)&tpmSendCommand,
                          4);
    if (rval != TSS2_RC_SUCCESS) {
        return rval;
    }

    /* Send the locality */
    locality = (UINT8)tcti_intel->status.locality;
    rval = tctiSendBytes (tctiContext,
                          tcti_intel->tpmSock,
                          (unsigned char *)&locality,
                          1);
    if (rval != TSS2_RC_SUCCESS) {
        return rval;
    }

#ifdef DEBUG
    if (tcti_intel->status.debugMsgEnabled == 1) {
        TCTI_LOG (tctiContext,
                  rmPrefix,
                  "Locality = %d",
                  tcti_intel->status.locality);
    }
#endif

    /* Send number of bytes. */
    cnt1 = cnt;
    cnt = HOST_TO_BE_32(cnt);
    rval = tctiSendBytes (tctiContext,
                          tcti_intel->tpmSock,
                          (unsigned char *)&cnt,
                          4);
    if (rval != TSS2_RC_SUCCESS) {
        return rval;
    }

    /* Send the TPM command buffer */
    rval = tctiSendBytes (tctiContext,
                          tcti_intel->tpmSock,
                          (unsigned char *)command_buffer,
                          cnt1);
    if (rval != TSS2_RC_SUCCESS) {
        return rval;
    }

#ifdef DEBUG
    if (tcti_intel->status.debugMsgEnabled == 1) {
        DEBUG_PRINT_BUFFER (rmPrefix, command_buffer, cnt1);
    }
#endif
    tcti_intel->status.commandSent = 1;

    tcti_intel->previousStage = TCTI_STAGE_SEND_COMMAND;
    tcti_intel->status.tagReceived = 0;
    tcti_intel->status.responseSizeReceived = 0;
    tcti_intel->status.protocolResponseSizeReceived = 0;

    return rval;
}

TSS2_RC SocketCancel(
    TSS2_TCTI_CONTEXT *tctiContext
    )
{
    TSS2_TCTI_CONTEXT_INTEL *tcti_intel = tcti_context_intel_cast (tctiContext);
    TSS2_RC rc;

    rc = tcti_common_checks (tctiContext);
    if (rc != TSS2_RC_SUCCESS) {
        return rc;
    } else if (tcti_intel->status.commandSent != 1) {
        return TSS2_TCTI_RC_BAD_SEQUENCE;
    } else {
        return PlatformCommand (tctiContext, MS_SIM_CANCEL_ON);
    }
}

TSS2_RC SocketSetLocality(
    TSS2_TCTI_CONTEXT *tctiContext,
    uint8_t locality
    )
{
    TSS2_TCTI_CONTEXT_INTEL *tcti_intel = tcti_context_intel_cast (tctiContext);
    TSS2_RC rc;

    rc = tcti_common_checks (tctiContext);
    if (rc != TSS2_RC_SUCCESS) {
        return rc;
    }
    if (tcti_intel->status.commandSent == 1) {
        return TSS2_TCTI_RC_BAD_SEQUENCE;
    }

    tcti_intel->status.locality = locality;

    return TSS2_RC_SUCCESS;
}

TSS2_RC SocketGetPollHandles(
    TSS2_TCTI_CONTEXT *tctiContext,
    TSS2_TCTI_POLL_HANDLE *handles,
    size_t *num_handles)
{
    return TSS2_TCTI_RC_NOT_IMPLEMENTED;
}

void SocketFinalize(
    TSS2_TCTI_CONTEXT *tctiContext
    )
{
    TSS2_TCTI_CONTEXT_INTEL *tcti_intel = tcti_context_intel_cast (tctiContext);
    TSS2_RC rc;

    rc = tcti_common_checks (tctiContext);
    if (rc != TSS2_RC_SUCCESS) {
        return;
    }

    SendSessionEndSocketTcti (tctiContext, 1);
    SendSessionEndSocketTcti (tctiContext, 0);

    CloseSockets (tcti_intel->otherSock, tcti_intel->tpmSock);
}

TSS2_RC SocketReceiveTpmResponse(
    TSS2_TCTI_CONTEXT *tctiContext,
    size_t *response_size,
    unsigned char *response_buffer,
    int32_t timeout
    )
{
    TSS2_TCTI_CONTEXT_INTEL *tcti_intel = tcti_context_intel_cast (tctiContext);
    UINT32 trash;
    TSS2_RC rval = TSS2_RC_SUCCESS;
    fd_set readFds;
    struct timeval tv, *tvPtr;
    int32_t timeoutMsecs = timeout % 1000;
    int iResult;
    unsigned char responseSizeDelta = 0;
    printf_type rmPrefix;

    rval = tcti_receive_checks (tctiContext, response_size, response_buffer);
    if (rval != TSS2_RC_SUCCESS) {
        goto retSocketReceiveTpmResponse;
    }

    if (tcti_intel->status.rmDebugPrefix == 1) {
        rmPrefix = RM_PREFIX;
    } else {
        rmPrefix = NO_PREFIX;
    }

    if (timeout == TSS2_TCTI_TIMEOUT_BLOCK) {
        tvPtr = 0;
    } else {
        tv.tv_sec = timeout / 1000;
        tv.tv_usec = timeoutMsecs * 1000;
        tvPtr = &tv;
    }

    FD_ZERO (&readFds);
    FD_SET (tcti_intel->tpmSock, &readFds);

    iResult = select (tcti_intel->tpmSock + 1, &readFds, 0, 0, tvPtr);
    if (iResult == 0) {
        TCTI_LOG (tctiContext,
                  rmPrefix,
                  "select failed due to timeout, socket #: 0x%x\n",
                  tcti_intel->tpmSock);
        rval = TSS2_TCTI_RC_TRY_AGAIN;
        goto retSocketReceiveTpmResponse;
    } else if (iResult == SOCKET_ERROR) {
        TCTI_LOG (tctiContext,
                  rmPrefix,
                  "select failed with socket error: %d\n",
                  WSAGetLastError ());
        rval = TSS2_TCTI_RC_IO_ERROR;
        goto retSocketReceiveTpmResponse;
    } else if (iResult != 1) {
        TCTI_LOG (tctiContext,
                  rmPrefix,
                  "select failed, read the wrong # of bytes: %d\n",
                  iResult);
        rval = TSS2_TCTI_RC_IO_ERROR;
        goto retSocketReceiveTpmResponse;
    }

    if (tcti_intel->status.protocolResponseSizeReceived != 1) {
        /* Receive the size of the response. */
        rval = tctiRecvBytes (tctiContext,
                              tcti_intel->tpmSock,
                              (unsigned char *)&tcti_intel->responseSize,
                              4);
        if (rval != TSS2_RC_SUCCESS) {
            goto retSocketReceiveTpmResponse;
        }

        tcti_intel->responseSize = BE_TO_HOST_32 (tcti_intel->responseSize);
        tcti_intel->status.protocolResponseSizeReceived = 1;
    }

    if (response_buffer == NULL) {
        *response_size = tcti_intel->responseSize;
        tcti_intel->status.protocolResponseSizeReceived = 1;
        goto retSocketReceiveTpmResponse;
    }

    if (*response_size < tcti_intel->responseSize) {
        *response_size = tcti_intel->responseSize;
        rval = TSS2_TCTI_RC_INSUFFICIENT_BUFFER;

        /* If possible, receive tag from TPM. */
        if (*response_size >= sizeof (TPM2_ST) &&
            tcti_intel->status.tagReceived == 0)
        {
            rval = tctiRecvBytes (tctiContext,
                                  tcti_intel->tpmSock,
                                  (unsigned char *)&tcti_intel->tag,
                                  2);
            if (rval != TSS2_RC_SUCCESS) {
                goto retSocketReceiveTpmResponse;
            } else {
                tcti_intel->status.tagReceived = 1;
            }
        }

        /* If possible, receive response size from TPM */
        if (*response_size >= (sizeof (TPM2_ST) + sizeof (TPM2_RC)) &&
            tcti_intel->status.responseSizeReceived == 0)
        {
            rval = tctiRecvBytes (tctiContext,
                                  tcti_intel->tpmSock,
                                  (unsigned char *)&tcti_intel->responseSize,
                                  4);
            if (rval != TSS2_RC_SUCCESS) {
                goto retSocketReceiveTpmResponse;
            } else {
                tcti_intel->responseSize = BE_TO_HOST_32 (tcti_intel->responseSize);
                tcti_intel->status.responseSizeReceived = 1;
            }
        }
    } else {
        if (tcti_intel->status.debugMsgEnabled == 1 &&
            tcti_intel->responseSize > 0)
        {
#ifdef DEBUG
            TCTI_LOG (tctiContext, rmPrefix, "Response Received: ");
#endif
#ifdef DEBUG_SOCKETS
            TCTI_LOG (tctiContext,
                      rmPrefix,
                      "from socket #0x%x:\n",
                      tcti_intel->tpmSock);
#endif
        }

        if (tcti_intel->status.tagReceived == 1) {
            *(TPM2_ST *)response_buffer = tcti_intel->tag;
            responseSizeDelta += sizeof (TPM2_ST);
            response_buffer += sizeof (TPM2_ST);
        }

        if (tcti_intel->status.responseSizeReceived == 1) {
            *(TPM2_RC *)response_buffer = HOST_TO_BE_32 (tcti_intel->responseSize);
            responseSizeDelta += sizeof (TPM2_RC);
            response_buffer += sizeof (TPM2_RC);
        }

        /* Receive the TPM response. */
        rval = tctiRecvBytes (tctiContext,
                              tcti_intel->tpmSock,
                              (unsigned char *)response_buffer,
                              tcti_intel->responseSize - responseSizeDelta);
        if (rval != TSS2_RC_SUCCESS) {
            goto retSocketReceiveTpmResponse;
        }

#ifdef DEBUG
        if (tcti_intel->status.debugMsgEnabled == 1) {
            DEBUG_PRINT_BUFFER (rmPrefix,
                                response_buffer,
                                tcti_intel->responseSize);
        }
#endif

        /* Receive the appended four bytes of 0's */
        rval = tctiRecvBytes (tctiContext,
                              tcti_intel->tpmSock,
                              (unsigned char *)&trash,
                              4);
        if (rval != TSS2_RC_SUCCESS) {
            goto retSocketReceiveTpmResponse;
        }
    }

    if (tcti_intel->responseSize < *response_size) {
        *response_size = tcti_intel->responseSize;
    }

    tcti_intel->status.commandSent = 0;

    /* Turn cancel off. */
    if (rval == TSS2_RC_SUCCESS) {
        rval = PlatformCommand (tctiContext, MS_SIM_CANCEL_OFF);
    } else {
        /* Ignore return value so earlier error code is preserved. */
        PlatformCommand (tctiContext, MS_SIM_CANCEL_OFF);
    }

retSocketReceiveTpmResponse:
    if (rval == TSS2_RC_SUCCESS && response_buffer != NULL) {
        tcti_intel->previousStage = TCTI_STAGE_RECEIVE_RESPONSE;
    }

    return rval;
}

#define HOSTNAME_LENGTH 200
#define PORT_LENGTH 4

/**
 * This function sends the Microsoft simulator the MS_SIM_POWER_ON and
 * MS_SIM_NV_ON commands using the PlatformCommand mechanism. Without
 * these the simulator will respond with zero sized buffer which causes
 * the TSS to freak out. Sending this command more than once is harmelss
 * so it's advisable to call this function as part of the TCTI context
 * initialization just to be sure.
 *
 * NOTE: The caller will still need to call Tss2_Sys_Startup. If they
 * don't, an error will be returned from each call till they do but
 * the error will at least be meaningful (TPM2_RC_INITIALIZE).
 */
static TSS2_RC InitializeMsTpm2Simulator(
    TSS2_TCTI_CONTEXT *tctiContext
    )
{
    TSS2_TCTI_CONTEXT_INTEL *tcti_intel = tcti_context_intel_cast (tctiContext);
    TSS2_RC rval;

    rval = PlatformCommand (tctiContext ,MS_SIM_POWER_ON);
    if (rval != TSS2_RC_SUCCESS) {
        CloseSockets (tcti_intel->otherSock, tcti_intel->tpmSock);
        return rval;
    }

    rval = PlatformCommand (tctiContext, MS_SIM_NV_ON);
    if (rval != TSS2_RC_SUCCESS) {
        CloseSockets (tcti_intel->otherSock, tcti_intel->tpmSock);
    }

    return rval;
}

TSS2_RC InitSocketTcti (
    TSS2_TCTI_CONTEXT *tctiContext,
    size_t *contextSize,
    const TCTI_SOCKET_CONF *conf,
    const uint8_t serverSockets
    )
{
    TSS2_TCTI_CONTEXT_INTEL *tcti_intel = tcti_context_intel_cast (tctiContext);
    TSS2_RC rval = TSS2_RC_SUCCESS;
    SOCKET otherSock;
    SOCKET tpmSock;

    if (tctiContext == NULL && contextSize == NULL) {
        return TSS2_TCTI_RC_BAD_VALUE;
    } else if( tctiContext == NULL ) {
        *contextSize = sizeof (TSS2_TCTI_CONTEXT_INTEL);
        return TSS2_RC_SUCCESS;
    } else if( conf == NULL ) {
        return TSS2_TCTI_RC_BAD_VALUE;
    }

    TSS2_TCTI_MAGIC (tctiContext) = TCTI_MAGIC;
    TSS2_TCTI_VERSION (tctiContext) = TCTI_VERSION;
    TSS2_TCTI_TRANSMIT (tctiContext) = SocketSendTpmCommand;
    TSS2_TCTI_RECEIVE (tctiContext) = SocketReceiveTpmResponse;
    TSS2_TCTI_FINALIZE (tctiContext) = SocketFinalize;
    TSS2_TCTI_CANCEL (tctiContext) = SocketCancel;
    TSS2_TCTI_GET_POLL_HANDLES (tctiContext) = SocketGetPollHandles;
    TSS2_TCTI_SET_LOCALITY (tctiContext) = SocketSetLocality;
    tcti_intel->status.debugMsgEnabled = 0;
    tcti_intel->status.locality = 3;
    tcti_intel->status.commandSent = 0;
    tcti_intel->status.rmDebugPrefix = 0;
    tcti_intel->status.tagReceived = 0;
    tcti_intel->status.responseSizeReceived = 0;
    tcti_intel->status.protocolResponseSizeReceived = 0;
    tcti_intel->currentTctiContext = 0;
    tcti_intel->previousStage = TCTI_STAGE_INITIALIZE;
    TCTI_LOG_CALLBACK (tctiContext) = conf->logCallback;
    TCTI_LOG_BUFFER_CALLBACK (tctiContext) = conf->logBufferCallback;
    TCTI_LOG_DATA (tctiContext) = conf->logData;

    rval = (TSS2_RC) InitSockets (conf->hostname,
                                  conf->port,
                                  &otherSock,
                                  &tpmSock,
                                  TCTI_LOG_CALLBACK (tctiContext),
                                  TCTI_LOG_DATA (tctiContext));
    if (rval == TSS2_RC_SUCCESS) {
        tcti_intel->otherSock = otherSock;
        tcti_intel->tpmSock = tpmSock;
        rval = InitializeMsTpm2Simulator (tctiContext);
    } else {
        CloseSockets (otherSock, tpmSock);
    }

    return rval;
}
