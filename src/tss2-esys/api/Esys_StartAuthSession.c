/*******************************************************************************
 * Copyright 2017-2018, Fraunhofer SIT sponsored by Infineon Technologies AG
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
 ******************************************************************************/

#include "tss2_mu.h"
#include "tss2_sys.h"
#ifndef TSS2_API_VERSION_1_2_1_108
#error Version missmatch among TSS2 header files !
#endif /* TSS2_API_VERSION_1_2_1_108 */
#include "esys_types.h"
#include "tss2_esys.h"
#include "esys_iutil.h"
#include "esys_mu.h"
#include "tss2_sys.h"
#define LOGMODULE esys
#include "util/log.h"

/** Store command parameters inside the ESYS_CONTEXT for use during _finish */
static void store_input_parameters (
    ESYS_CONTEXT *esysContext,
    ESYS_TR tpmKey,
    ESYS_TR bind,
    const TPM2B_NONCE *nonceCaller,
    TPM2_SE sessionType,
    const TPMT_SYM_DEF *symmetric,
    TPMI_ALG_HASH authHash)
{
    esysContext->in.StartAuthSession.tpmKey = tpmKey;
    esysContext->in.StartAuthSession.bind = bind;
    esysContext->in.StartAuthSession.sessionType = sessionType;
    esysContext->in.StartAuthSession.authHash = authHash;
    if (nonceCaller == NULL) {
        esysContext->in.StartAuthSession.nonceCaller = NULL;
    } else {
        esysContext->in.StartAuthSession.nonceCallerData = *nonceCaller;
        esysContext->in.StartAuthSession.nonceCaller =
            &esysContext->in.StartAuthSession.nonceCallerData;
    }
    if (symmetric == NULL) {
        esysContext->in.StartAuthSession.symmetric = NULL;
    } else {
        esysContext->in.StartAuthSession.symmetricData = *symmetric;
        esysContext->in.StartAuthSession.symmetric =
            &esysContext->in.StartAuthSession.symmetricData;
    }
}

/** One-Call function for TPM2_StartAuthSession
 *
 * This function invokes the TPM2_StartAuthSession command in a one-call
 * variant. This means the function will block until the TPM response is
 * available. All input parameters are const. The memory for non-simple output
 * parameters is allocated by the function implementation.
 *
 * @param[in,out] esysContext The ESYS_CONTEXT.
 * @param[in] tpmKey Input handle of type ESYS_TR for
 *     object with handle type TPMI_DH_OBJECT.
 * @param[in] bind Input handle of type ESYS_TR for
 *     object with handle type TPMI_DH_ENTITY.
 * @param[in] shandle1 First session handle.
 * @param[in] shandle2 Second session handle.
 * @param[in] shandle3 Third session handle.
 * @param[in] nonceCaller Input parameter of type TPM2B_NONCE.
 * @param[in] encryptedSalt Input parameter of type TPM2B_ENCRYPTED_SECRET.
 * @param[in] sessionType Input parameter of type TPM2_SE.
 * @param[in] symmetric Input parameter of type TPMT_SYM_DEF.
 * @param[in] authHash Input parameter of type TPMI_ALG_HASH.
 * @param[out] nonceTPM (callee-allocated) Output parameter
 *    of type TPM2B_NONCE. May be NULL if this value is not required.
 * @param[out] sessionHandle  ESYS_TR handle of ESYS resource for TPMI_SH_AUTH_SESSION.
 * @retval TSS2_RC_SUCCESS on success
 * @retval TSS2_RC_BAD_SEQUENCE if context is not ready for this function
 * \todo add further error RCs to documentation
 */
TSS2_RC
Esys_StartAuthSession(
    ESYS_CONTEXT *esysContext,
    ESYS_TR tpmKey,
    ESYS_TR bind,
    ESYS_TR shandle1,
    ESYS_TR shandle2,
    ESYS_TR shandle3,
    const TPM2B_NONCE *nonceCaller,
    TPM2_SE sessionType,
    const TPMT_SYM_DEF *symmetric,
    TPMI_ALG_HASH authHash,
    ESYS_TR *sessionHandle,
    TPM2B_NONCE **nonceTPM)
{
    TSS2_RC r = TSS2_RC_SUCCESS;

    r = Esys_StartAuthSession_async(esysContext,
                tpmKey,
                bind,
                shandle1,
                shandle2,
                shandle3,
                nonceCaller,
                sessionType,
                symmetric,
                authHash);
    return_if_error(r, "Error in async function");

    /* Set the timeout to indefinite for now, since we want _finish to block */
    int32_t timeouttmp = esysContext->timeout;
    esysContext->timeout = -1;
    /*
     * Now we call the finish function, until return code is not equal to
     * from TSS2_BASE_RC_TRY_AGAIN.
     * Note that the finish function may return TSS2_RC_TRY_AGAIN, even if we
     * have set the timeout to -1. This occurs for example if the TPM requests
     * a retransmission of the command via TPM2_RC_YIELDED.
     */
    do {
        r = Esys_StartAuthSession_finish(esysContext,
                sessionHandle,
                nonceTPM);
        /* This is just debug information about the reattempt to finish the
           command */
        if ((r & ~TSS2_RC_LAYER_MASK) == TSS2_BASE_RC_TRY_AGAIN)
            LOG_DEBUG("A layer below returned TRY_AGAIN: %" PRIx32
                      " => resubmitting command", r);
    } while ((r & ~TSS2_RC_LAYER_MASK) == TSS2_BASE_RC_TRY_AGAIN);

    /* Restore the timeout value to the original value */
    esysContext->timeout = timeouttmp;
    return_if_error(r, "Esys Finish");

    return TSS2_RC_SUCCESS;
}

/** Asynchronous function for TPM2_StartAuthSession
 *
 * This function invokes the TPM2_StartAuthSession command in a asynchronous
 * variant. This means the function will return as soon as the command has been
 * sent downwards the stack to the TPM. All input parameters are const.
 * In order to retrieve the TPM's response call Esys_StartAuthSession_finish.
 *
 * @param[in,out] esysContext The ESYS_CONTEXT.
 * @param[in] tpmKey Input handle of type ESYS_TR for
 *     object with handle type TPMI_DH_OBJECT.
 * @param[in] bind Input handle of type ESYS_TR for
 *     object with handle type TPMI_DH_ENTITY.
 * @param[in] shandle1 First session handle.
 * @param[in] shandle2 Second session handle.
 * @param[in] shandle3 Third session handle.
 * @param[in] nonceCaller Input parameter of type TPM2B_NONCE.
 * @param[in] encryptedSalt Input parameter of type TPM2B_ENCRYPTED_SECRET.
 * @param[in] sessionType Input parameter of type TPM2_SE.
 * @param[in] symmetric Input parameter of type TPMT_SYM_DEF.
 * @param[in] authHash Input parameter of type TPMI_ALG_HASH.
 * @retval TSS2_RC_SUCCESS on success
 * @retval TSS2_RC_BAD_SEQUENCE if context is not ready for this function
 * \todo add further error RCs to documentation
 */
TSS2_RC
Esys_StartAuthSession_async(
    ESYS_CONTEXT *esysContext,
    ESYS_TR tpmKey,
    ESYS_TR bind,
    ESYS_TR shandle1,
    ESYS_TR shandle2,
    ESYS_TR shandle3,
    const TPM2B_NONCE *nonceCaller,
    TPM2_SE sessionType,
    const TPMT_SYM_DEF *symmetric,
    TPMI_ALG_HASH authHash)
{
    TSS2_RC r = TSS2_RC_SUCCESS;
    TPM2B_ENCRYPTED_SECRET encryptedSaltAux = {0};
    const TPM2B_ENCRYPTED_SECRET *encryptedSalt = &encryptedSaltAux;
    TSS2L_SYS_AUTH_COMMAND auths = { 0 };
    RSRC_NODE_T *tpmKeyNode;
    RSRC_NODE_T *bindNode;

    if (esysContext == NULL) {
        LOG_ERROR("esyscontext is NULL.");
        return TSS2_ESYS_RC_BAD_REFERENCE;
    }
    r = iesys_check_sequence_async(esysContext);
    if (r != TSS2_RC_SUCCESS)
        return r;
    r = check_session_feasability(shandle1, shandle2, shandle3, 0);
    return_if_error(r, "Check session usage");

    store_input_parameters(esysContext, tpmKey, bind,
                nonceCaller,
                sessionType,
                symmetric,
                authHash);
    r = esys_GetResourceObject(esysContext, tpmKey, &tpmKeyNode);
    if (r != TPM2_RC_SUCCESS)
        return r;
    r = esys_GetResourceObject(esysContext, bind, &bindNode);
    if (r != TPM2_RC_SUCCESS)
        return r;
    size_t keyHash_size = 0;
    size_t authHash_size = 0;
    TSS2_RC r2;
    r2 = iesys_compute_encrypted_salt(esysContext, tpmKeyNode,
                                      &encryptedSaltAux);
    return_if_error(r2, "Error in parameter encryption.");

    esysContext->in.StartAuthSession.encryptedSalt = &encryptedSaltAux;
    if (nonceCaller == NULL) {
        r2 = iesys_crypto_hash_get_digest_size(authHash,&authHash_size);
        if (r2 != TSS2_RC_SUCCESS) {
            LOG_ERROR("Error: initialize auth session (%x).", r2);
            return r2;
        }
        r2 = iesys_crypto_random2b(&esysContext->in.StartAuthSession.nonceCallerData,
                                   authHash_size);
        if (r2 != TSS2_RC_SUCCESS) {
            LOG_ERROR("Error: initialize auth session (%x).", r2);
            return r2;
        }
        esysContext->in.StartAuthSession.nonceCaller
           = &esysContext->in.StartAuthSession.nonceCallerData;
        nonceCaller = esysContext->in.StartAuthSession.nonceCaller;
    }

    r = Tss2_Sys_StartAuthSession_Prepare(esysContext->sys,
                (tpmKeyNode == NULL) ? TPM2_RH_NULL : tpmKeyNode->rsrc.handle,
                (bindNode == NULL) ? TPM2_RH_NULL : bindNode->rsrc.handle,
                nonceCaller,
                encryptedSalt,
                sessionType,
                symmetric,
                authHash);
    if (r != TSS2_RC_SUCCESS) {
        LOG_ERROR("Error async StartAuthSession");
        return r;
    }
    r = init_session_tab(esysContext, shandle1, shandle2, shandle3);
    return_if_error(r, "Initialize session resources");

    iesys_compute_session_value(esysContext->session_tab[0], NULL, NULL);
    iesys_compute_session_value(esysContext->session_tab[1], NULL, NULL);
    iesys_compute_session_value(esysContext->session_tab[2], NULL, NULL);
    r = iesys_gen_auths(esysContext, tpmKeyNode, bindNode, NULL, &auths);
    return_if_error(r, "Error in computation of auth values");

    esysContext->authsCount = auths.count;
    r = Tss2_Sys_SetCmdAuths(esysContext->sys, &auths);
    if (r != TSS2_RC_SUCCESS) {
        return r;
    }

    r = Tss2_Sys_ExecuteAsync(esysContext->sys);
    return_if_error(r, "Finish (Execute Async)");

    esysContext->state = _ESYS_STATE_SENT;

    return r;
}

/** Asynchronous finish function for TPM2_StartAuthSession
 *
 * This function returns the results of a TPM2_StartAuthSession command
 * invoked via Esys_StartAuthSession_finish. All non-simple output parameters
 * are allocated by the function's implementation. NULL can be passed for every
 * output parameter if the value is not required.
 *
 * @param[in,out] esysContext The ESYS_CONTEXT.
 * @param[out] nonceTPM (callee-allocated) Output parameter
 *    of type TPM2B_NONCE. May be NULL if this value is not required.
 * @param[out] sessionHandle  ESYS_TR handle of ESYS resource for TPMI_SH_AUTH_SESSION.
 * @retval TSS2_RC_SUCCESS on success
 * @retval TSS2_RC_BAD_SEQUENCE if context is not ready for this function.
 * \todo add further error RCs to documentation
 */
TSS2_RC
Esys_StartAuthSession_finish(
    ESYS_CONTEXT *esysContext,
    ESYS_TR *sessionHandle,
    TPM2B_NONCE **nonceTPM)
{
    TPM2B_NONCE *lnonceTPM = NULL;
    LOG_TRACE("complete");
    if (esysContext == NULL) {
        LOG_ERROR("esyscontext is NULL.");
        return TSS2_ESYS_RC_BAD_REFERENCE;
    }
    if (esysContext->state != _ESYS_STATE_SENT) {
        LOG_ERROR("Esys called in bad sequence.");
        return TSS2_ESYS_RC_BAD_SEQUENCE;
    }
    TSS2_RC r = TSS2_RC_SUCCESS;
    IESYS_RESOURCE *sessionHandleRsrc = NULL;
    RSRC_NODE_T *sessionHandleNode = NULL;

    if (sessionHandle == NULL) {
        LOG_ERROR("Handle sessionHandle may not be NULL");
        return TSS2_ESYS_RC_BAD_REFERENCE;
    }
    *sessionHandle = esysContext->esys_handle_cnt++;
    r = esys_CreateResourceObject(esysContext, *sessionHandle, &sessionHandleNode);
    if (r != TSS2_RC_SUCCESS)
        return r;

    lnonceTPM = calloc(sizeof(TPM2B_NONCE), 1);
    if (lnonceTPM == NULL) {
        goto_error(r, TSS2_ESYS_RC_MEMORY, "Out of memory", error_cleanup);
    }
    IESYS_RESOURCE *rsrc = &sessionHandleNode->rsrc;
    rsrc->misc.rsrc_session.sessionAttributes =
        TPMA_SESSION_CONTINUESESSION;
    rsrc->misc.rsrc_session.sessionType =
        esysContext->in.StartAuthSession.sessionType;
    rsrc->misc.rsrc_session.authHash =
        esysContext->in.StartAuthSession.authHash;
    rsrc->misc.rsrc_session.symmetric =
        *esysContext->in.StartAuthSession.symmetric;
    rsrc->misc.rsrc_session.nonceCaller =
        esysContext->in.StartAuthSession.nonceCallerData;
    r = Tss2_Sys_ExecuteFinish(esysContext->sys, esysContext->timeout);
    if ((r & ~TSS2_RC_LAYER_MASK) == TSS2_BASE_RC_TRY_AGAIN) {
        LOG_DEBUG("A layer below returned TRY_AGAIN: %" PRIx32, r);
        goto error_cleanup;
    }
    if (r == TPM2_RC_RETRY || r == TPM2_RC_TESTING || r == TPM2_RC_YIELDED) {
        LOG_DEBUG("TPM returned RETRY, TESTING or YIELDED, which triggers a "
            "resubmission: %" PRIx32, r);
        if (esysContext->submissionCount > _ESYS_MAX_SUMBISSIONS) {
            LOG_WARNING("Maximum number of resubmissions has been reached.");
            esysContext->state = _ESYS_STATE_ERRORRESPONSE;
            goto error_cleanup;
        }
        esysContext->state = _ESYS_STATE_RESUBMISSION;
        r = Esys_StartAuthSession_async(esysContext,
                esysContext->in.StartAuthSession.tpmKey,
                esysContext->in.StartAuthSession.bind,
                esysContext->session_type[0],
                esysContext->session_type[1],
                esysContext->session_type[2],
                esysContext->in.StartAuthSession.nonceCaller,
                esysContext->in.StartAuthSession.sessionType,
                esysContext->in.StartAuthSession.symmetric,
                esysContext->in.StartAuthSession.authHash);
        if (r != TSS2_RC_SUCCESS) {
            LOG_ERROR("Error attempting to resubmit");
            goto error_cleanup;
        }
        r = TSS2_ESYS_RC_TRY_AGAIN;
        goto error_cleanup;
    }
    if (r != TSS2_RC_SUCCESS) {
        LOG_ERROR("Error finish (ExecuteFinish) StartAuthSession");
        goto error_cleanup;
    }
    /*
     * Now the verification of the response (hmac check) and if necessary the
     * parameter decryption have to be done
     */
    r = iesys_check_response(esysContext);
    goto_if_error(r, "Error: check response",
                      error_cleanup);
    /*
     * After the verification of the response we call the complete function
     * to deliver the result.
     */
    r = Tss2_Sys_StartAuthSession_Complete(esysContext->sys,
                &sessionHandleNode->rsrc.handle,
                lnonceTPM);
    if (r != TSS2_RC_SUCCESS) {
        LOG_ERROR("Error finish (ExecuteFinish) StartAuthSession: %" PRIx32, r);
        esysContext->state = _ESYS_STATE_ERRORRESPONSE;
        goto error_cleanup;;
    }

    sessionHandleNode->rsrc.misc.rsrc_session.nonceTPM = *lnonceTPM;
    sessionHandleNode->rsrc.rsrcType = IESYSC_SESSION_RSRC;
    if (esysContext->in.StartAuthSession.bind != ESYS_TR_NONE || esysContext->salt.size > 0) {
        ESYS_TR bind = esysContext->in.StartAuthSession.bind;
        ESYS_TR tpmKey = esysContext->in.StartAuthSession.tpmKey;
        RSRC_NODE_T *bindNode;
        r = esys_GetResourceObject(esysContext, bind, &bindNode);
        goto_if_error(r, "get resource", error_cleanup);

        RSRC_NODE_T *tpmKeyNode;
        r = esys_GetResourceObject(esysContext, tpmKey, &tpmKeyNode);
        goto_if_error(r, "get resource", error_cleanup);

        size_t keyHash_size = 0;
        size_t authHash_size = 0;
        if (tpmKeyNode != NULL) {
            r = iesys_crypto_hash_get_digest_size(
                     tpmKeyNode->rsrc.misc.rsrc_key_pub.publicArea.nameAlg, &
                     keyHash_size);
            if (r != TSS2_RC_SUCCESS) {
                LOG_ERROR("Error: initialize auth session (%x).", r);
                return r;
            }
        }
        r = iesys_crypto_hash_get_digest_size(esysContext->in.StartAuthSession.
                                              authHash,&authHash_size);
        if (r != TSS2_RC_SUCCESS) {
            LOG_ERROR("Error: initialize auth session (%x).", r);
            return r;
        }
        /* compute session key */
        size_t secret_size = 0;
        if (tpmKey != ESYS_TR_NONE)
            secret_size += keyHash_size;
        if (bind != ESYS_TR_NONE && bindNode != NULL)
            secret_size += bindNode->auth.size;
        if (secret_size == 0) {
            return_error(TSS2_ESYS_RC_GENERAL_FAILURE,
                         "Invalid secret size (0).");
        }
        uint8_t *secret = malloc(secret_size);
        if (secret == NULL) {
            LOG_ERROR("Out of memory.");
            return TSS2_ESYS_RC_MEMORY;
         }
        if  (bind != ESYS_TR_NONE && bindNode != NULL
             && bindNode->auth.size > 0)
             memcpy(&secret[0], &bindNode->auth.buffer[0], bindNode->auth.size);
        if (tpmKey != ESYS_TR_NONE)
            memcpy(&secret[(bind == ESYS_TR_NONE || bindNode == NULL) ? 0
                               : bindNode->auth.size],
                           &esysContext->salt.buffer[0], keyHash_size);
        if (bind != ESYS_TR_NONE &&  bindNode != NULL)
            iesys_compute_bound_entity(&bindNode->rsrc.name,
                                       &bindNode->auth,
                                       &sessionHandleNode->rsrc.misc.rsrc_session.bound_entity);
        LOGBLOB_DEBUG(secret, secret_size, "ESYS Session Secret");
        r = iesys_crypto_KDFa(esysContext->in.StartAuthSession.authHash, secret,
                              secret_size, "ATH",
                               lnonceTPM, esysContext->in.StartAuthSession.nonceCaller,
                               authHash_size*8, NULL,
                     &sessionHandleNode->rsrc.misc.rsrc_session.sessionKey.buffer[0]);
        free(secret);
        return_if_error(r, "Error in KDFa computation.");

        sessionHandleNode->rsrc.misc.rsrc_session.sessionKey.size = authHash_size;
        LOGBLOB_DEBUG(&sessionHandleNode->rsrc.misc.rsrc_session.sessionKey
                      .buffer[0],
                      sessionHandleNode->rsrc.misc.rsrc_session.sessionKey.size,
                      "Session Key");
        return_if_error(r,"Error KDFa");

        sessionHandleNode->rsrc.misc.rsrc_session.sessionKey.size = authHash_size;
    }
    if (nonceTPM != NULL)
        *nonceTPM = lnonceTPM;
    else
        SAFE_FREE(lnonceTPM);

    esysContext->state = _ESYS_STATE_FINISHED;

    return r;

error_cleanup:
    Esys_TR_Close(esysContext, sessionHandle);
    SAFE_FREE(lnonceTPM);
    return r;
}
