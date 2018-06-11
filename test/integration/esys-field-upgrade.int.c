/* SPDX-License-Identifier: BSD-2 */
/*******************************************************************************
 * Copyright 2017-2018, Fraunhofer SIT sponsored by Infineon Technologies AG
 * All rights reserved.
 *******************************************************************************/

#include "tss2_esys.h"

#include "esys_iutil.h"
#define LOGMODULE test
#include "util/log.h"

/* Test the ESAPI function Esys_FieldUpgradeStart and   Esys_FieldUpgradeData */
int
test_invoke_esapi(ESYS_CONTEXT * esys_context)
{
    uint32_t r = 0;

    TPM2B_MAX_BUFFER fuData = {
        .size = 20,
        .buffer = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10,
                   11, 12, 13, 14, 15, 16, 17, 18, 19, 20}
    };
    TPMT_HA *nextDigest;
    TPMT_HA *firstDigest;

    r = Esys_FieldUpgradeData(
        esys_context,
        ESYS_TR_NONE,
        ESYS_TR_NONE,
        ESYS_TR_NONE,
        &fuData,
        &nextDigest,
        &firstDigest);
    if (r == TPM2_RC_COMMAND_CODE) {
        LOG_INFO("Command TPM2_FieldUpgradeData not supported by TPM.");
        r = 77; /* Skip */
        goto error;
    }

    goto_if_error(r, "Error: FieldUpgradeData", error);

    /* TODO test has to be adapted if FieldUpgrade commands are available */
    /*
    ESYS_TR authorization_handle = ESYS_TR_NONE;
    ESYS_TR keyHandle_handle = ESYS_TR_NONE;
    TPM2B_DIGEST fuDigest;
    TPMT_SIGNATURE manifestSignature;

    r = Esys_FieldUpgradeStart(
        esys_context,
        authorization_handle,
        keyHandle_handle,
        ESYS_TR_PASSWORD,
        ESYS_TR_NONE,
        ESYS_TR_NONE,
        &fuDigest,
        &manifestSignature);
    goto_if_error(r, "Error: FieldUpgradeStart", error);
    */
    return 0;

 error:
    return r;
}
