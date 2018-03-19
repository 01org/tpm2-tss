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
 *******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <stdint.h>

#include <setjmp.h>
#include <cmocka.h>

#include "tss2_esys.h"

#define LOGMODULE test
#include "util/log.h"

/*
 * Tests whether all ESAPI commands check context parameter for NULL.
 */

void
check_Startup(void **state)
{
    TSS2_RC r;

    r = Esys_Startup(NULL, 0);
    assert_int_equal(r, TSS2_ESYS_RC_BAD_REFERENCE);
}

void
check_Shutdown(void **state)
{
    TSS2_RC r;

    r = Esys_Shutdown(NULL, ESYS_TR_NONE, ESYS_TR_NONE, ESYS_TR_NONE, 0);
    assert_int_equal(r, TSS2_ESYS_RC_BAD_REFERENCE);
}

void
check_SelfTest(void **state)
{
    TSS2_RC r;

    r = Esys_SelfTest(NULL, ESYS_TR_NONE, ESYS_TR_NONE, ESYS_TR_NONE, 0);
    assert_int_equal(r, TSS2_ESYS_RC_BAD_REFERENCE);
}

void
check_IncrementalSelfTest(void **state)
{
    TSS2_RC r;

    r = Esys_IncrementalSelfTest(NULL,
                                 ESYS_TR_NONE,
                                 ESYS_TR_NONE, ESYS_TR_NONE, NULL, NULL);
    assert_int_equal(r, TSS2_ESYS_RC_BAD_REFERENCE);
}

void
check_GetTestResult(void **state)
{
    TSS2_RC r;

    r = Esys_GetTestResult(NULL,
                           ESYS_TR_NONE,
                           ESYS_TR_NONE, ESYS_TR_NONE, NULL, NULL);
    assert_int_equal(r, TSS2_ESYS_RC_BAD_REFERENCE);
}

void
check_StartAuthSession(void **state)
{
    TSS2_RC r;

    r = Esys_StartAuthSession(NULL,
                              0,
                              0,
                              ESYS_TR_NONE,
                              ESYS_TR_NONE,
                              ESYS_TR_NONE, NULL, 0, NULL, 0, NULL, NULL);
    assert_int_equal(r, TSS2_ESYS_RC_BAD_REFERENCE);
}

void
check_PolicyRestart(void **state)
{
    TSS2_RC r;

    r = Esys_PolicyRestart(NULL, 0, ESYS_TR_NONE, ESYS_TR_NONE, ESYS_TR_NONE);
    assert_int_equal(r, TSS2_ESYS_RC_BAD_REFERENCE);
}

void
check_Create(void **state)
{
    TSS2_RC r;

    r = Esys_Create(NULL,
                    0,
                    ESYS_TR_PASSWORD,
                    ESYS_TR_NONE,
                    ESYS_TR_NONE,
                    NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
    assert_int_equal(r, TSS2_ESYS_RC_BAD_REFERENCE);
}

void
check_Load(void **state)
{
    TSS2_RC r;

    r = Esys_Load(NULL,
                  0,
                  ESYS_TR_PASSWORD,
                  ESYS_TR_NONE, ESYS_TR_NONE, NULL, NULL, NULL);
    assert_int_equal(r, TSS2_ESYS_RC_BAD_REFERENCE);
}

void
check_LoadExternal(void **state)
{
    TSS2_RC r;

    r = Esys_LoadExternal(NULL,
                          ESYS_TR_NONE,
                          ESYS_TR_NONE, ESYS_TR_NONE, NULL, NULL, 0, NULL);
    assert_int_equal(r, TSS2_ESYS_RC_BAD_REFERENCE);
}

void
check_ReadPublic(void **state)
{
    TSS2_RC r;

    r = Esys_ReadPublic(NULL,
                        0,
                        ESYS_TR_NONE,
                        ESYS_TR_NONE, ESYS_TR_NONE, NULL, NULL, NULL);
    assert_int_equal(r, TSS2_ESYS_RC_BAD_REFERENCE);
}

void
check_ActivateCredential(void **state)
{
    TSS2_RC r;

    r = Esys_ActivateCredential(NULL,
                                0,
                                0,
                                ESYS_TR_PASSWORD,
                                ESYS_TR_PASSWORD,
                                ESYS_TR_NONE, NULL, NULL, NULL);
    assert_int_equal(r, TSS2_ESYS_RC_BAD_REFERENCE);
}

void
check_MakeCredential(void **state)
{
    TSS2_RC r;

    r = Esys_MakeCredential(NULL,
                            0,
                            ESYS_TR_NONE,
                            ESYS_TR_NONE, ESYS_TR_NONE, NULL, NULL, NULL, NULL);
    assert_int_equal(r, TSS2_ESYS_RC_BAD_REFERENCE);
}

void
check_Unseal(void **state)
{
    TSS2_RC r;

    r = Esys_Unseal(NULL,
                    0, ESYS_TR_PASSWORD, ESYS_TR_NONE, ESYS_TR_NONE, NULL);
    assert_int_equal(r, TSS2_ESYS_RC_BAD_REFERENCE);
}

void
check_ObjectChangeAuth(void **state)
{
    TSS2_RC r;

    r = Esys_ObjectChangeAuth(NULL,
                              0,
                              0,
                              ESYS_TR_PASSWORD,
                              ESYS_TR_NONE, ESYS_TR_NONE, NULL, NULL);
    assert_int_equal(r, TSS2_ESYS_RC_BAD_REFERENCE);
}

void
check_CreateLoaded(void **state)
{
    TSS2_RC r;

    r = Esys_CreateLoaded(NULL,
                          0,
                          ESYS_TR_PASSWORD,
                          ESYS_TR_NONE,
                          ESYS_TR_NONE, NULL, NULL, NULL, NULL, NULL);
    assert_int_equal(r, TSS2_ESYS_RC_BAD_REFERENCE);
}

void
check_Duplicate(void **state)
{
    TSS2_RC r;

    r = Esys_Duplicate(NULL,
                       0,
                       0,
                       ESYS_TR_PASSWORD,
                       ESYS_TR_NONE,
                       ESYS_TR_NONE, NULL, NULL, NULL, NULL, NULL);
    assert_int_equal(r, TSS2_ESYS_RC_BAD_REFERENCE);
}

void
check_Rewrap(void **state)
{
    TSS2_RC r;

    r = Esys_Rewrap(NULL,
                    0,
                    0,
                    ESYS_TR_PASSWORD,
                    ESYS_TR_NONE, ESYS_TR_NONE, NULL, NULL, NULL, NULL, NULL);
    assert_int_equal(r, TSS2_ESYS_RC_BAD_REFERENCE);
}

void
check_Import(void **state)
{
    TSS2_RC r;

    r = Esys_Import(NULL,
                    0,
                    ESYS_TR_PASSWORD,
                    ESYS_TR_NONE,
                    ESYS_TR_NONE, NULL, NULL, NULL, NULL, NULL, NULL);
    assert_int_equal(r, TSS2_ESYS_RC_BAD_REFERENCE);
}

void
check_RSA_Encrypt(void **state)
{
    TSS2_RC r;

    r = Esys_RSA_Encrypt(NULL,
                         0,
                         ESYS_TR_NONE,
                         ESYS_TR_NONE, ESYS_TR_NONE, NULL, NULL, NULL, NULL);
    assert_int_equal(r, TSS2_ESYS_RC_BAD_REFERENCE);
}

void
check_RSA_Decrypt(void **state)
{
    TSS2_RC r;

    r = Esys_RSA_Decrypt(NULL,
                         0,
                         ESYS_TR_PASSWORD,
                         ESYS_TR_NONE, ESYS_TR_NONE, NULL, NULL, NULL, NULL);
    assert_int_equal(r, TSS2_ESYS_RC_BAD_REFERENCE);
}

void
check_ECDH_KeyGen(void **state)
{
    TSS2_RC r;

    r = Esys_ECDH_KeyGen(NULL,
                         0,
                         ESYS_TR_NONE, ESYS_TR_NONE, ESYS_TR_NONE, NULL, NULL);
    assert_int_equal(r, TSS2_ESYS_RC_BAD_REFERENCE);
}

void
check_ECDH_ZGen(void **state)
{
    TSS2_RC r;

    r = Esys_ECDH_ZGen(NULL,
                       0,
                       ESYS_TR_PASSWORD,
                       ESYS_TR_NONE, ESYS_TR_NONE, NULL, NULL);
    assert_int_equal(r, TSS2_ESYS_RC_BAD_REFERENCE);
}

void
check_ECC_Parameters(void **state)
{
    TSS2_RC r;

    r = Esys_ECC_Parameters(NULL,
                            ESYS_TR_NONE, ESYS_TR_NONE, ESYS_TR_NONE, 0, NULL);
    assert_int_equal(r, TSS2_ESYS_RC_BAD_REFERENCE);
}

void
check_ZGen_2Phase(void **state)
{
    TSS2_RC r;

    r = Esys_ZGen_2Phase(NULL,
                         0,
                         ESYS_TR_PASSWORD,
                         ESYS_TR_NONE,
                         ESYS_TR_NONE, NULL, NULL, 0, 0, NULL, NULL);
    assert_int_equal(r, TSS2_ESYS_RC_BAD_REFERENCE);
}

void
check_EncryptDecrypt(void **state)
{
    TSS2_RC r;

    r = Esys_EncryptDecrypt(NULL,
                            0,
                            ESYS_TR_PASSWORD,
                            ESYS_TR_NONE,
                            ESYS_TR_NONE, 0, 0, NULL, NULL, NULL, NULL);
    assert_int_equal(r, TSS2_ESYS_RC_BAD_REFERENCE);
}

void
check_EncryptDecrypt2(void **state)
{
    TSS2_RC r;

    r = Esys_EncryptDecrypt2(NULL,
                             0,
                             ESYS_TR_PASSWORD,
                             ESYS_TR_NONE,
                             ESYS_TR_NONE, NULL, 0, 0, NULL, NULL, NULL);
    assert_int_equal(r, TSS2_ESYS_RC_BAD_REFERENCE);
}

void
check_Hash(void **state)
{
    TSS2_RC r;

    r = Esys_Hash(NULL,
                  ESYS_TR_NONE,
                  ESYS_TR_NONE, ESYS_TR_NONE, NULL, 0, 0, NULL, NULL);
    assert_int_equal(r, TSS2_ESYS_RC_BAD_REFERENCE);
}

void
check_HMAC(void **state)
{
    TSS2_RC r;

    r = Esys_HMAC(NULL,
                  0,
                  ESYS_TR_PASSWORD, ESYS_TR_NONE, ESYS_TR_NONE, NULL, 0, NULL);
    assert_int_equal(r, TSS2_ESYS_RC_BAD_REFERENCE);
}

void
check_GetRandom(void **state)
{
    TSS2_RC r;

    r = Esys_GetRandom(NULL, ESYS_TR_NONE, ESYS_TR_NONE, ESYS_TR_NONE, 0, NULL);
    assert_int_equal(r, TSS2_ESYS_RC_BAD_REFERENCE);
}

void
check_StirRandom(void **state)
{
    TSS2_RC r;

    r = Esys_StirRandom(NULL, ESYS_TR_NONE, ESYS_TR_NONE, ESYS_TR_NONE, NULL);
    assert_int_equal(r, TSS2_ESYS_RC_BAD_REFERENCE);
}

void
check_HMAC_Start(void **state)
{
    TSS2_RC r;

    r = Esys_HMAC_Start(NULL,
                        0,
                        ESYS_TR_PASSWORD,
                        ESYS_TR_NONE, ESYS_TR_NONE, NULL, 0, NULL);
    assert_int_equal(r, TSS2_ESYS_RC_BAD_REFERENCE);
}

void
check_HashSequenceStart(void **state)
{
    TSS2_RC r;

    r = Esys_HashSequenceStart(NULL,
                               ESYS_TR_NONE,
                               ESYS_TR_NONE, ESYS_TR_NONE, NULL, 0, NULL);
    assert_int_equal(r, TSS2_ESYS_RC_BAD_REFERENCE);
}

void
check_SequenceUpdate(void **state)
{
    TSS2_RC r;

    r = Esys_SequenceUpdate(NULL,
                            0,
                            ESYS_TR_PASSWORD, ESYS_TR_NONE, ESYS_TR_NONE, NULL);
    assert_int_equal(r, TSS2_ESYS_RC_BAD_REFERENCE);
}

void
check_SequenceComplete(void **state)
{
    TSS2_RC r;

    r = Esys_SequenceComplete(NULL,
                              0,
                              ESYS_TR_PASSWORD,
                              ESYS_TR_NONE, ESYS_TR_NONE, NULL, 0, NULL, NULL);
    assert_int_equal(r, TSS2_ESYS_RC_BAD_REFERENCE);
}

void
check_EventSequenceComplete(void **state)
{
    TSS2_RC r;

    r = Esys_EventSequenceComplete(NULL,
                                   0,
                                   0,
                                   ESYS_TR_PASSWORD,
                                   ESYS_TR_PASSWORD, ESYS_TR_NONE, NULL, NULL);
    assert_int_equal(r, TSS2_ESYS_RC_BAD_REFERENCE);
}

void
check_Certify(void **state)
{
    TSS2_RC r;

    r = Esys_Certify(NULL,
                     0,
                     0,
                     ESYS_TR_PASSWORD,
                     ESYS_TR_PASSWORD, ESYS_TR_NONE, NULL, NULL, NULL, NULL);
    assert_int_equal(r, TSS2_ESYS_RC_BAD_REFERENCE);
}

void
check_CertifyCreation(void **state)
{
    TSS2_RC r;

    r = Esys_CertifyCreation(NULL,
                             0,
                             0,
                             ESYS_TR_PASSWORD,
                             ESYS_TR_NONE,
                             ESYS_TR_NONE, NULL, NULL, NULL, NULL, NULL, NULL);
    assert_int_equal(r, TSS2_ESYS_RC_BAD_REFERENCE);
}

void
check_Quote(void **state)
{
    TSS2_RC r;

    r = Esys_Quote(NULL,
                   0,
                   ESYS_TR_PASSWORD,
                   ESYS_TR_NONE, ESYS_TR_NONE, NULL, NULL, NULL, NULL, NULL);
    assert_int_equal(r, TSS2_ESYS_RC_BAD_REFERENCE);
}

void
check_GetSessionAuditDigest(void **state)
{
    TSS2_RC r;

    r = Esys_GetSessionAuditDigest(NULL,
                                   0,
                                   0,
                                   0,
                                   ESYS_TR_PASSWORD,
                                   ESYS_TR_PASSWORD,
                                   ESYS_TR_NONE, NULL, NULL, NULL, NULL);
    assert_int_equal(r, TSS2_ESYS_RC_BAD_REFERENCE);
}

void
check_GetCommandAuditDigest(void **state)
{
    TSS2_RC r;

    r = Esys_GetCommandAuditDigest(NULL,
                                   0,
                                   0,
                                   ESYS_TR_PASSWORD,
                                   ESYS_TR_PASSWORD,
                                   ESYS_TR_NONE, NULL, NULL, NULL, NULL);
    assert_int_equal(r, TSS2_ESYS_RC_BAD_REFERENCE);
}

void
check_GetTime(void **state)
{
    TSS2_RC r;

    r = Esys_GetTime(NULL,
                     0,
                     0,
                     ESYS_TR_PASSWORD,
                     ESYS_TR_PASSWORD, ESYS_TR_NONE, NULL, NULL, NULL, NULL);
    assert_int_equal(r, TSS2_ESYS_RC_BAD_REFERENCE);
}

void
check_Commit(void **state)
{
    TSS2_RC r;

    r = Esys_Commit(NULL,
                    0,
                    ESYS_TR_PASSWORD,
                    ESYS_TR_NONE,
                    ESYS_TR_NONE, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
    assert_int_equal(r, TSS2_ESYS_RC_BAD_REFERENCE);
}

void
check_EC_Ephemeral(void **state)
{
    TSS2_RC r;

    r = Esys_EC_Ephemeral(NULL,
                          ESYS_TR_NONE,
                          ESYS_TR_NONE, ESYS_TR_NONE, 0, NULL, NULL);
    assert_int_equal(r, TSS2_ESYS_RC_BAD_REFERENCE);
}

void
check_VerifySignature(void **state)
{
    TSS2_RC r;

    r = Esys_VerifySignature(NULL,
                             0,
                             ESYS_TR_NONE,
                             ESYS_TR_NONE, ESYS_TR_NONE, NULL, NULL, NULL);
    assert_int_equal(r, TSS2_ESYS_RC_BAD_REFERENCE);
}

void
check_Sign(void **state)
{
    TSS2_RC r;

    r = Esys_Sign(NULL,
                  0,
                  ESYS_TR_PASSWORD,
                  ESYS_TR_NONE, ESYS_TR_NONE, NULL, NULL, NULL, NULL);
    assert_int_equal(r, TSS2_ESYS_RC_BAD_REFERENCE);
}

void
check_SetCommandCodeAuditStatus(void **state)
{
    TSS2_RC r;

    r = Esys_SetCommandCodeAuditStatus(NULL,
                                       0,
                                       ESYS_TR_PASSWORD,
                                       ESYS_TR_NONE,
                                       ESYS_TR_NONE, 0, NULL, NULL);
    assert_int_equal(r, TSS2_ESYS_RC_BAD_REFERENCE);
}

void
check_PCR_Extend(void **state)
{
    TSS2_RC r;

    r = Esys_PCR_Extend(NULL,
                        0, ESYS_TR_PASSWORD, ESYS_TR_NONE, ESYS_TR_NONE, NULL);
    assert_int_equal(r, TSS2_ESYS_RC_BAD_REFERENCE);
}

void
check_PCR_Event(void **state)
{
    TSS2_RC r;

    r = Esys_PCR_Event(NULL,
                       0,
                       ESYS_TR_PASSWORD,
                       ESYS_TR_NONE, ESYS_TR_NONE, NULL, NULL);
    assert_int_equal(r, TSS2_ESYS_RC_BAD_REFERENCE);
}

void
check_PCR_Read(void **state)
{
    TSS2_RC r;

    r = Esys_PCR_Read(NULL,
                      ESYS_TR_NONE,
                      ESYS_TR_NONE, ESYS_TR_NONE, NULL, NULL, NULL, NULL);
    assert_int_equal(r, TSS2_ESYS_RC_BAD_REFERENCE);
}

void
check_PCR_Allocate(void **state)
{
    TSS2_RC r;

    r = Esys_PCR_Allocate(NULL,
                          0,
                          ESYS_TR_PASSWORD,
                          ESYS_TR_NONE,
                          ESYS_TR_NONE, NULL, NULL, NULL, NULL, NULL);
    assert_int_equal(r, TSS2_ESYS_RC_BAD_REFERENCE);
}

void
check_PCR_SetAuthPolicy(void **state)
{
    TSS2_RC r;

    r = Esys_PCR_SetAuthPolicy(NULL,
                               0,
                               ESYS_TR_PASSWORD,
                               ESYS_TR_NONE, ESYS_TR_NONE, NULL, 0, 0);
    assert_int_equal(r, TSS2_ESYS_RC_BAD_REFERENCE);
}

void
check_PCR_SetAuthValue(void **state)
{
    TSS2_RC r;

    r = Esys_PCR_SetAuthValue(NULL,
                              0,
                              ESYS_TR_PASSWORD,
                              ESYS_TR_NONE, ESYS_TR_NONE, NULL);
    assert_int_equal(r, TSS2_ESYS_RC_BAD_REFERENCE);
}

void
check_PCR_Reset(void **state)
{
    TSS2_RC r;

    r = Esys_PCR_Reset(NULL, 0, ESYS_TR_PASSWORD, ESYS_TR_NONE, ESYS_TR_NONE);
    assert_int_equal(r, TSS2_ESYS_RC_BAD_REFERENCE);
}

void
check_PolicySigned(void **state)
{
    TSS2_RC r;

    r = Esys_PolicySigned(NULL,
                          0,
                          0,
                          ESYS_TR_NONE,
                          ESYS_TR_NONE,
                          ESYS_TR_NONE, NULL, NULL, NULL, 0, NULL, NULL, NULL);
    assert_int_equal(r, TSS2_ESYS_RC_BAD_REFERENCE);
}

void
check_PolicySecret(void **state)
{
    TSS2_RC r;

    r = Esys_PolicySecret(NULL,
                          0,
                          0,
                          ESYS_TR_PASSWORD,
                          ESYS_TR_NONE,
                          ESYS_TR_NONE, NULL, NULL, NULL, 0, NULL, NULL);
    assert_int_equal(r, TSS2_ESYS_RC_BAD_REFERENCE);
}

void
check_PolicyTicket(void **state)
{
    TSS2_RC r;

    r = Esys_PolicyTicket(NULL,
                          0,
                          ESYS_TR_NONE,
                          ESYS_TR_NONE,
                          ESYS_TR_NONE, NULL, NULL, NULL, NULL, NULL);
    assert_int_equal(r, TSS2_ESYS_RC_BAD_REFERENCE);
}

void
check_PolicyOR(void **state)
{
    TSS2_RC r;

    r = Esys_PolicyOR(NULL, 0, ESYS_TR_NONE, ESYS_TR_NONE, ESYS_TR_NONE, NULL);
    assert_int_equal(r, TSS2_ESYS_RC_BAD_REFERENCE);
}

void
check_PolicyPCR(void **state)
{
    TSS2_RC r;

    r = Esys_PolicyPCR(NULL,
                       0, ESYS_TR_NONE, ESYS_TR_NONE, ESYS_TR_NONE, NULL, NULL);
    assert_int_equal(r, TSS2_ESYS_RC_BAD_REFERENCE);
}

void
check_PolicyLocality(void **state)
{
    TSS2_RC r;

    r = Esys_PolicyLocality(NULL,
                            ESYS_TR_NONE, ESYS_TR_NONE, ESYS_TR_NONE, 0, 0);
    assert_int_equal(r, TSS2_ESYS_RC_BAD_REFERENCE);
}

void
check_PolicyNV(void **state)
{
    TSS2_RC r;

    r = Esys_PolicyNV(NULL,
                      0,
                      0,
                      0,
                      ESYS_TR_PASSWORD, ESYS_TR_NONE, ESYS_TR_NONE, NULL, 0, 0);
    assert_int_equal(r, TSS2_ESYS_RC_BAD_REFERENCE);
}

void
check_PolicyCounterTimer(void **state)
{
    TSS2_RC r;

    r = Esys_PolicyCounterTimer(NULL,
                                0,
                                ESYS_TR_NONE,
                                ESYS_TR_NONE, ESYS_TR_NONE, NULL, 0, 0);
    assert_int_equal(r, TSS2_ESYS_RC_BAD_REFERENCE);
}

void
check_PolicyCommandCode(void **state)
{
    TSS2_RC r;

    r = Esys_PolicyCommandCode(NULL,
                               0, ESYS_TR_NONE, ESYS_TR_NONE, ESYS_TR_NONE, 0);
    assert_int_equal(r, TSS2_ESYS_RC_BAD_REFERENCE);
}

void
check_PolicyPhysicalPresence(void **state)
{
    TSS2_RC r;

    r = Esys_PolicyPhysicalPresence(NULL,
                                    0,
                                    ESYS_TR_NONE, ESYS_TR_NONE, ESYS_TR_NONE);
    assert_int_equal(r, TSS2_ESYS_RC_BAD_REFERENCE);
}

void
check_PolicyCpHash(void **state)
{
    TSS2_RC r;

    r = Esys_PolicyCpHash(NULL,
                          ESYS_TR_NONE, ESYS_TR_NONE, ESYS_TR_NONE, 0, NULL);
    assert_int_equal(r, TSS2_ESYS_RC_BAD_REFERENCE);
}

void
check_PolicyNameHash(void **state)
{
    TSS2_RC r;

    r = Esys_PolicyNameHash(NULL,
                            ESYS_TR_NONE, ESYS_TR_NONE, ESYS_TR_NONE, 0, NULL);
    assert_int_equal(r, TSS2_ESYS_RC_BAD_REFERENCE);
}

void
check_PolicyDuplicationSelect(void **state)
{
    TSS2_RC r;

    r = Esys_PolicyDuplicationSelect(NULL,
                                     ESYS_TR_NONE,
                                     ESYS_TR_NONE,
                                     ESYS_TR_NONE, 0, NULL, NULL, 0);
    assert_int_equal(r, TSS2_ESYS_RC_BAD_REFERENCE);
}

void
check_PolicyAuthorize(void **state)
{
    TSS2_RC r;

    r = Esys_PolicyAuthorize(NULL,
                             0,
                             ESYS_TR_NONE,
                             ESYS_TR_NONE,
                             ESYS_TR_NONE, NULL, NULL, NULL, NULL);
    assert_int_equal(r, TSS2_ESYS_RC_BAD_REFERENCE);
}

void
check_PolicyAuthValue(void **state)
{
    TSS2_RC r;

    r = Esys_PolicyAuthValue(NULL, 0, ESYS_TR_NONE, ESYS_TR_NONE, ESYS_TR_NONE);
    assert_int_equal(r, TSS2_ESYS_RC_BAD_REFERENCE);
}

void
check_PolicyPassword(void **state)
{
    TSS2_RC r;

    r = Esys_PolicyPassword(NULL, 0, ESYS_TR_NONE, ESYS_TR_NONE, ESYS_TR_NONE);
    assert_int_equal(r, TSS2_ESYS_RC_BAD_REFERENCE);
}

void
check_PolicyGetDigest(void **state)
{
    TSS2_RC r;

    r = Esys_PolicyGetDigest(NULL,
                             0, ESYS_TR_NONE, ESYS_TR_NONE, ESYS_TR_NONE, NULL);
    assert_int_equal(r, TSS2_ESYS_RC_BAD_REFERENCE);
}

void
check_PolicyNvWritten(void **state)
{
    TSS2_RC r;

    r = Esys_PolicyNvWritten(NULL,
                             0, ESYS_TR_NONE, ESYS_TR_NONE, ESYS_TR_NONE, 0);
    assert_int_equal(r, TSS2_ESYS_RC_BAD_REFERENCE);
}

void
check_PolicyTemplate(void **state)
{
    TSS2_RC r;

    r = Esys_PolicyTemplate(NULL,
                            ESYS_TR_NONE, ESYS_TR_NONE, ESYS_TR_NONE, 0, NULL);
    assert_int_equal(r, TSS2_ESYS_RC_BAD_REFERENCE);
}

void
check_PolicyAuthorizeNV(void **state)
{
    TSS2_RC r;

    r = Esys_PolicyAuthorizeNV(NULL,
                               0,
                               0,
                               0, ESYS_TR_PASSWORD, ESYS_TR_NONE, ESYS_TR_NONE);
    assert_int_equal(r, TSS2_ESYS_RC_BAD_REFERENCE);
}

void
check_CreatePrimary(void **state)
{
    TSS2_RC r;

    r = Esys_CreatePrimary(NULL,
                           0,
                           ESYS_TR_PASSWORD,
                           ESYS_TR_NONE,
                           ESYS_TR_NONE,
                           NULL,
                           NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
    assert_int_equal(r, TSS2_ESYS_RC_BAD_REFERENCE);
}

void
check_HierarchyControl(void **state)
{
    TSS2_RC r;

    r = Esys_HierarchyControl(NULL,
                              0,
                              ESYS_TR_PASSWORD,
                              ESYS_TR_NONE, ESYS_TR_NONE, 0, 0);
    assert_int_equal(r, TSS2_ESYS_RC_BAD_REFERENCE);
}

void
check_SetPrimaryPolicy(void **state)
{
    TSS2_RC r;

    r = Esys_SetPrimaryPolicy(NULL,
                              0,
                              ESYS_TR_PASSWORD,
                              ESYS_TR_NONE, ESYS_TR_NONE, NULL, 0);
    assert_int_equal(r, TSS2_ESYS_RC_BAD_REFERENCE);
}

void
check_ChangePPS(void **state)
{
    TSS2_RC r;

    r = Esys_ChangePPS(NULL, 0, ESYS_TR_PASSWORD, ESYS_TR_NONE, ESYS_TR_NONE);
    assert_int_equal(r, TSS2_ESYS_RC_BAD_REFERENCE);
}

void
check_ChangeEPS(void **state)
{
    TSS2_RC r;

    r = Esys_ChangeEPS(NULL, 0, ESYS_TR_PASSWORD, ESYS_TR_NONE, ESYS_TR_NONE);
    assert_int_equal(r, TSS2_ESYS_RC_BAD_REFERENCE);
}

void
check_Clear(void **state)
{
    TSS2_RC r;

    r = Esys_Clear(NULL, 0, ESYS_TR_PASSWORD, ESYS_TR_NONE, ESYS_TR_NONE);
    assert_int_equal(r, TSS2_ESYS_RC_BAD_REFERENCE);
}

void
check_ClearControl(void **state)
{
    TSS2_RC r;

    r = Esys_ClearControl(NULL,
                          0, ESYS_TR_PASSWORD, ESYS_TR_NONE, ESYS_TR_NONE, 0);
    assert_int_equal(r, TSS2_ESYS_RC_BAD_REFERENCE);
}

void
check_HierarchyChangeAuth(void **state)
{
    TSS2_RC r;

    r = Esys_HierarchyChangeAuth(NULL,
                                 0,
                                 ESYS_TR_PASSWORD,
                                 ESYS_TR_NONE, ESYS_TR_NONE, NULL);
    assert_int_equal(r, TSS2_ESYS_RC_BAD_REFERENCE);
}

void
check_DictionaryAttackLockReset(void **state)
{
    TSS2_RC r;

    r = Esys_DictionaryAttackLockReset(NULL,
                                       0,
                                       ESYS_TR_PASSWORD,
                                       ESYS_TR_NONE, ESYS_TR_NONE);
    assert_int_equal(r, TSS2_ESYS_RC_BAD_REFERENCE);
}

void
check_DictionaryAttackParameters(void **state)
{
    TSS2_RC r;

    r = Esys_DictionaryAttackParameters(NULL,
                                        0,
                                        ESYS_TR_PASSWORD,
                                        ESYS_TR_NONE, ESYS_TR_NONE, 0, 0, 0);
    assert_int_equal(r, TSS2_ESYS_RC_BAD_REFERENCE);
}

void
check_PP_Commands(void **state)
{
    TSS2_RC r;

    r = Esys_PP_Commands(NULL,
                         0,
                         ESYS_TR_PASSWORD,
                         ESYS_TR_NONE, ESYS_TR_NONE, NULL, NULL);
    assert_int_equal(r, TSS2_ESYS_RC_BAD_REFERENCE);
}

void
check_SetAlgorithmSet(void **state)
{
    TSS2_RC r;

    r = Esys_SetAlgorithmSet(NULL,
                             0,
                             ESYS_TR_PASSWORD, ESYS_TR_NONE, ESYS_TR_NONE, 0);
    assert_int_equal(r, TSS2_ESYS_RC_BAD_REFERENCE);
}

void
check_FieldUpgradeStart(void **state)
{
    TSS2_RC r;

    r = Esys_FieldUpgradeStart(NULL,
                               0,
                               0,
                               ESYS_TR_PASSWORD,
                               ESYS_TR_NONE, ESYS_TR_NONE, NULL, NULL);
    assert_int_equal(r, TSS2_ESYS_RC_BAD_REFERENCE);
}

void
check_FieldUpgradeData(void **state)
{
    TSS2_RC r;

    r = Esys_FieldUpgradeData(NULL,
                              ESYS_TR_NONE,
                              ESYS_TR_NONE, ESYS_TR_NONE, NULL, NULL, NULL);
    assert_int_equal(r, TSS2_ESYS_RC_BAD_REFERENCE);
}

void
check_FirmwareRead(void **state)
{
    TSS2_RC r;

    r = Esys_FirmwareRead(NULL,
                          ESYS_TR_NONE, ESYS_TR_NONE, ESYS_TR_NONE, 0, NULL);
    assert_int_equal(r, TSS2_ESYS_RC_BAD_REFERENCE);
}

void
check_ContextSave(void **state)
{
    TSS2_RC r;

    r = Esys_ContextSave(NULL, 0, NULL);
    assert_int_equal(r, TSS2_ESYS_RC_BAD_REFERENCE);
}

void
check_ContextLoad(void **state)
{
    TSS2_RC r;

    r = Esys_ContextLoad(NULL, NULL, NULL);
    assert_int_equal(r, TSS2_ESYS_RC_BAD_REFERENCE);
}

void
check_FlushContext(void **state)
{
    TSS2_RC r;

    r = Esys_FlushContext(NULL, 0);
    assert_int_equal(r, TSS2_ESYS_RC_BAD_REFERENCE);
}

void
check_EvictControl(void **state)
{
    TSS2_RC r;

    r = Esys_EvictControl(NULL,
                          0,
                          0,
                          ESYS_TR_PASSWORD,
                          ESYS_TR_NONE, ESYS_TR_NONE, 0, NULL);
    assert_int_equal(r, TSS2_ESYS_RC_BAD_REFERENCE);
}

void
check_ReadClock(void **state)
{
    TSS2_RC r;

    r = Esys_ReadClock(NULL, ESYS_TR_NONE, ESYS_TR_NONE, ESYS_TR_NONE, NULL);
    assert_int_equal(r, TSS2_ESYS_RC_BAD_REFERENCE);
}

void
check_ClockSet(void **state)
{
    TSS2_RC r;

    r = Esys_ClockSet(NULL, 0, ESYS_TR_NONE, ESYS_TR_NONE, ESYS_TR_NONE, 0);
    assert_int_equal(r, TSS2_ESYS_RC_BAD_REFERENCE);
}

void
check_ClockRateAdjust(void **state)
{
    TSS2_RC r;

    r = Esys_ClockRateAdjust(NULL,
                             0,
                             ESYS_TR_PASSWORD, ESYS_TR_NONE, ESYS_TR_NONE, 0);
    assert_int_equal(r, TSS2_ESYS_RC_BAD_REFERENCE);
}

void
check_GetCapability(void **state)
{
    TSS2_RC r;

    r = Esys_GetCapability(NULL,
                           ESYS_TR_NONE,
                           ESYS_TR_NONE, ESYS_TR_NONE, 0, 0, 0, NULL, NULL);
    assert_int_equal(r, TSS2_ESYS_RC_BAD_REFERENCE);
}

void
check_TestParms(void **state)
{
    TSS2_RC r;

    r = Esys_TestParms(NULL, ESYS_TR_NONE, ESYS_TR_NONE, ESYS_TR_NONE, NULL);
    assert_int_equal(r, TSS2_ESYS_RC_BAD_REFERENCE);
}

void
check_NV_DefineSpace(void **state)
{
    TSS2_RC r;

    r = Esys_NV_DefineSpace(NULL,
                            0,
                            ESYS_TR_PASSWORD,
                            ESYS_TR_NONE, ESYS_TR_NONE, NULL, NULL, NULL);
    assert_int_equal(r, TSS2_ESYS_RC_BAD_REFERENCE);
}

void
check_NV_UndefineSpace(void **state)
{
    TSS2_RC r;

    r = Esys_NV_UndefineSpace(NULL,
                              0,
                              0, ESYS_TR_PASSWORD, ESYS_TR_NONE, ESYS_TR_NONE);
    assert_int_equal(r, TSS2_ESYS_RC_BAD_REFERENCE);
}

void
check_NV_UndefineSpaceSpecial(void **state)
{
    TSS2_RC r;

    r = Esys_NV_UndefineSpaceSpecial(NULL,
                                     0,
                                     0,
                                     ESYS_TR_PASSWORD,
                                     ESYS_TR_PASSWORD, ESYS_TR_NONE);
    assert_int_equal(r, TSS2_ESYS_RC_BAD_REFERENCE);
}

void
check_NV_ReadPublic(void **state)
{
    TSS2_RC r;

    r = Esys_NV_ReadPublic(NULL,
                           0,
                           ESYS_TR_NONE,
                           ESYS_TR_NONE, ESYS_TR_NONE, NULL, NULL);
    assert_int_equal(r, TSS2_ESYS_RC_BAD_REFERENCE);
}

void
check_NV_Write(void **state)
{
    TSS2_RC r;

    r = Esys_NV_Write(NULL,
                      0,
                      0, ESYS_TR_PASSWORD, ESYS_TR_NONE, ESYS_TR_NONE, NULL, 0);
    assert_int_equal(r, TSS2_ESYS_RC_BAD_REFERENCE);
}

void
check_NV_Increment(void **state)
{
    TSS2_RC r;

    r = Esys_NV_Increment(NULL,
                          0, 0, ESYS_TR_PASSWORD, ESYS_TR_NONE, ESYS_TR_NONE);
    assert_int_equal(r, TSS2_ESYS_RC_BAD_REFERENCE);
}

void
check_NV_Extend(void **state)
{
    TSS2_RC r;

    r = Esys_NV_Extend(NULL,
                       0,
                       0, ESYS_TR_PASSWORD, ESYS_TR_NONE, ESYS_TR_NONE, NULL);
    assert_int_equal(r, TSS2_ESYS_RC_BAD_REFERENCE);
}

void
check_NV_SetBits(void **state)
{
    TSS2_RC r;

    r = Esys_NV_SetBits(NULL,
                        0, 0, ESYS_TR_PASSWORD, ESYS_TR_NONE, ESYS_TR_NONE, 0);
    assert_int_equal(r, TSS2_ESYS_RC_BAD_REFERENCE);
}

void
check_NV_WriteLock(void **state)
{
    TSS2_RC r;

    r = Esys_NV_WriteLock(NULL,
                          0, 0, ESYS_TR_PASSWORD, ESYS_TR_NONE, ESYS_TR_NONE);
    assert_int_equal(r, TSS2_ESYS_RC_BAD_REFERENCE);
}

void
check_NV_GlobalWriteLock(void **state)
{
    TSS2_RC r;

    r = Esys_NV_GlobalWriteLock(NULL,
                                0,
                                ESYS_TR_PASSWORD, ESYS_TR_NONE, ESYS_TR_NONE);
    assert_int_equal(r, TSS2_ESYS_RC_BAD_REFERENCE);
}

void
check_NV_Read(void **state)
{
    TSS2_RC r;

    r = Esys_NV_Read(NULL,
                     0,
                     0,
                     ESYS_TR_PASSWORD, ESYS_TR_NONE, ESYS_TR_NONE, 0, 0, NULL);
    assert_int_equal(r, TSS2_ESYS_RC_BAD_REFERENCE);
}

void
check_NV_ReadLock(void **state)
{
    TSS2_RC r;

    r = Esys_NV_ReadLock(NULL,
                         0, 0, ESYS_TR_PASSWORD, ESYS_TR_NONE, ESYS_TR_NONE);
    assert_int_equal(r, TSS2_ESYS_RC_BAD_REFERENCE);
}

void
check_NV_ChangeAuth(void **state)
{
    TSS2_RC r;

    r = Esys_NV_ChangeAuth(NULL,
                           0,
                           ESYS_TR_PASSWORD, ESYS_TR_NONE, ESYS_TR_NONE, NULL);
    assert_int_equal(r, TSS2_ESYS_RC_BAD_REFERENCE);
}

void
check_NV_Certify(void **state)
{
    TSS2_RC r;

    r = Esys_NV_Certify(NULL,
                        0,
                        0,
                        0,
                        ESYS_TR_PASSWORD,
                        ESYS_TR_PASSWORD,
                        ESYS_TR_NONE, NULL, NULL, 0, 0, NULL, NULL);
    assert_int_equal(r, TSS2_ESYS_RC_BAD_REFERENCE);
}

void
check_Vendor_TCG_Test(void **state)
{
    TSS2_RC r;

    r = Esys_Vendor_TCG_Test(NULL,
                             ESYS_TR_NONE,
                             ESYS_TR_NONE, ESYS_TR_NONE, NULL, NULL);
    assert_int_equal(r, TSS2_ESYS_RC_BAD_REFERENCE);
}

int
main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(check_Startup),
        cmocka_unit_test(check_Shutdown),
        cmocka_unit_test(check_SelfTest),
        cmocka_unit_test(check_IncrementalSelfTest),
        cmocka_unit_test(check_GetTestResult),
        cmocka_unit_test(check_StartAuthSession),
        cmocka_unit_test(check_PolicyRestart),
        cmocka_unit_test(check_Create),
        cmocka_unit_test(check_Load),
        cmocka_unit_test(check_LoadExternal),
        cmocka_unit_test(check_ReadPublic),
        cmocka_unit_test(check_ActivateCredential),
        cmocka_unit_test(check_MakeCredential),
        cmocka_unit_test(check_Unseal),
        cmocka_unit_test(check_ObjectChangeAuth),
        cmocka_unit_test(check_CreateLoaded),
        cmocka_unit_test(check_Duplicate),
        cmocka_unit_test(check_Rewrap),
        cmocka_unit_test(check_Import),
        cmocka_unit_test(check_RSA_Encrypt),
        cmocka_unit_test(check_RSA_Decrypt),
        cmocka_unit_test(check_ECDH_KeyGen),
        cmocka_unit_test(check_ECDH_ZGen),
        cmocka_unit_test(check_ECC_Parameters),
        cmocka_unit_test(check_ZGen_2Phase),
        cmocka_unit_test(check_EncryptDecrypt),
        cmocka_unit_test(check_EncryptDecrypt2),
        cmocka_unit_test(check_Hash),
        cmocka_unit_test(check_HMAC),
        cmocka_unit_test(check_GetRandom),
        cmocka_unit_test(check_StirRandom),
        cmocka_unit_test(check_HMAC_Start),
        cmocka_unit_test(check_HashSequenceStart),
        cmocka_unit_test(check_SequenceUpdate),
        cmocka_unit_test(check_SequenceComplete),
        cmocka_unit_test(check_EventSequenceComplete),
        cmocka_unit_test(check_Certify),
        cmocka_unit_test(check_CertifyCreation),
        cmocka_unit_test(check_Quote),
        cmocka_unit_test(check_GetSessionAuditDigest),
        cmocka_unit_test(check_GetCommandAuditDigest),
        cmocka_unit_test(check_GetTime),
        cmocka_unit_test(check_Commit),
        cmocka_unit_test(check_EC_Ephemeral),
        cmocka_unit_test(check_VerifySignature),
        cmocka_unit_test(check_Sign),
        cmocka_unit_test(check_SetCommandCodeAuditStatus),
        cmocka_unit_test(check_PCR_Extend),
        cmocka_unit_test(check_PCR_Event),
        cmocka_unit_test(check_PCR_Read),
        cmocka_unit_test(check_PCR_Allocate),
        cmocka_unit_test(check_PCR_SetAuthPolicy),
        cmocka_unit_test(check_PCR_SetAuthValue),
        cmocka_unit_test(check_PCR_Reset),
        cmocka_unit_test(check_PolicySigned),
        cmocka_unit_test(check_PolicySecret),
        cmocka_unit_test(check_PolicyTicket),
        cmocka_unit_test(check_PolicyOR),
        cmocka_unit_test(check_PolicyPCR),
        cmocka_unit_test(check_PolicyLocality),
        cmocka_unit_test(check_PolicyNV),
        cmocka_unit_test(check_PolicyCounterTimer),
        cmocka_unit_test(check_PolicyCommandCode),
        cmocka_unit_test(check_PolicyPhysicalPresence),
        cmocka_unit_test(check_PolicyCpHash),
        cmocka_unit_test(check_PolicyNameHash),
        cmocka_unit_test(check_PolicyDuplicationSelect),
        cmocka_unit_test(check_PolicyAuthorize),
        cmocka_unit_test(check_PolicyAuthValue),
        cmocka_unit_test(check_PolicyPassword),
        cmocka_unit_test(check_PolicyGetDigest),
        cmocka_unit_test(check_PolicyNvWritten),
        cmocka_unit_test(check_PolicyTemplate),
        cmocka_unit_test(check_PolicyAuthorizeNV),
        cmocka_unit_test(check_CreatePrimary),
        cmocka_unit_test(check_HierarchyControl),
        cmocka_unit_test(check_SetPrimaryPolicy),
        cmocka_unit_test(check_ChangePPS),
        cmocka_unit_test(check_ChangeEPS),
        cmocka_unit_test(check_Clear),
        cmocka_unit_test(check_ClearControl),
        cmocka_unit_test(check_HierarchyChangeAuth),
        cmocka_unit_test(check_DictionaryAttackLockReset),
        cmocka_unit_test(check_DictionaryAttackParameters),
        cmocka_unit_test(check_PP_Commands),
        cmocka_unit_test(check_SetAlgorithmSet),
        cmocka_unit_test(check_FieldUpgradeStart),
        cmocka_unit_test(check_FieldUpgradeData),
        cmocka_unit_test(check_FirmwareRead),
        cmocka_unit_test(check_ContextSave),
        cmocka_unit_test(check_ContextLoad),
        cmocka_unit_test(check_FlushContext),
        cmocka_unit_test(check_EvictControl),
        cmocka_unit_test(check_ReadClock),
        cmocka_unit_test(check_ClockSet),
        cmocka_unit_test(check_ClockRateAdjust),
        cmocka_unit_test(check_GetCapability),
        cmocka_unit_test(check_TestParms),
        cmocka_unit_test(check_NV_DefineSpace),
        cmocka_unit_test(check_NV_UndefineSpace),
        cmocka_unit_test(check_NV_UndefineSpaceSpecial),
        cmocka_unit_test(check_NV_ReadPublic),
        cmocka_unit_test(check_NV_Write),
        cmocka_unit_test(check_NV_Increment),
        cmocka_unit_test(check_NV_Extend),
        cmocka_unit_test(check_NV_SetBits),
        cmocka_unit_test(check_NV_WriteLock),
        cmocka_unit_test(check_NV_GlobalWriteLock),
        cmocka_unit_test(check_NV_Read),
        cmocka_unit_test(check_NV_ReadLock),
        cmocka_unit_test(check_NV_ChangeAuth),
        cmocka_unit_test(check_NV_Certify),
        cmocka_unit_test(check_Vendor_TCG_Test),
    };
    return cmocka_run_group_tests(tests, NULL, NULL);
}
