/* SPDX-License-Identifier: BSD-2-Clause */
/*******************************************************************************
 * Copyright 2017, Fraunhofer SIT sponsored by Infineon Technologies AG
 * All rights reserved.
 *******************************************************************************/
#ifndef     TCTI_INTERFACE_H
#define     TCTI_INTERFACE_H

#include "tss2_tpm2_types.h"
#include "tss2_tcti.h"

TSS2_RC tctildr_get_default(TSS2_TCTI_CONTEXT ** tcticontext, void **data);
void tctildr_finalize_data(void **data);

#endif
