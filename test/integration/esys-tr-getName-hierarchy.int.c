/* SPDX-License-Identifier: BSD-2 */
/*******************************************************************************
 * Copyright 2017-2018, Fraunhofer SIT sponsored by Infineon Technologies AG All
 * rights reserved.
 *******************************************************************************/
#include <stdlib.h>

#include "tss2_mu.h"
#include "tss2_esys.h"

#include "esys_iutil.h"
#define LOGMODULE test
#include "util/log.h"

/*
 * This tests the Esys_TR_FromTPMPublic and Esys_TR_GetName functions by
 * creating an NV Index and then attempting to retrieve an ESYS_TR object for
 * it. Then we call Esys_TR_GetName to see if the correct public name has been
 * retrieved.
 */

int
test_invoke_esapi(ESYS_CONTEXT * ectx)
{
    uint32_t r = 0;

    TPM2B_NAME name1, *name2;
    size_t offset = 0;

    r = Tss2_MU_TPM2_HANDLE_Marshal(TPM2_RH_OWNER, &name1.name[0],
                                    sizeof(name1.name), &offset);
    goto_if_error(r, "Marshaling name", error);
    name1.size = offset;

    r = Esys_TR_GetName(ectx, ESYS_TR_RH_OWNER, &name2);
    goto_if_error(r, "TR get name", error);

    if (name1.size != name2->size ||
        memcmp(&name1.name[0], &name2->name[0], name1.size) != 0)
    {
        free(name2);
        LOG_ERROR("Names mismatch between NV_GetPublic and TR_GetName");
        return 1;
    }

    free(name2);

    return 0;

 error:
    return 1;
}
