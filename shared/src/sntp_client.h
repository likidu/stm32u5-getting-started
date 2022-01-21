/* Copyright (c) Microsoft Corporation.
   Licensed under the MIT License. */

#ifndef _SNTP_CLIENT_H
#define _SNTP_CLIENT_H

#include <tx_api.h>

//#define SAMPLE_SNTP_SERVER_ADDRESS      IP_ADDRESS(118, 190, 21, 209)

ULONG sntp_time_get();
UINT sntp_time(ULONG* unix_time);

UINT sntp_init();
UINT sntp_sync();

#endif
