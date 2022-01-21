/* Copyright (c) Microsoft Corporation.
   Licensed under the MIT License. */
#include "nx_client.h"

#include <stdio.h>

#include "b_u585i_iot02a_env_sensors.h"

#include "nx_api.h"
#include "nx_azure_iot_hub_client.h"
#include "nx_azure_iot_json_reader.h"
#include "nx_azure_iot_provisioning_client.h"

#include "azure_iot_nx_client.h"

#include "azure_config.h"
#include "stm_networking.h"

#define IOT_MODEL_ID "dtmi:azurertos:devkit:gsgstml4s5;2"

#define TELEMETRY_HUMIDITY "humidity"

typedef enum TELEMETRY_STATE_ENUM
{
    TELEMETRY_STATE_DEFAULT,
    //TELEMETRY_STATE_MAGNETOMETER,
    //TELEMETRY_STATE_ACCELEROMETER,
    //TELEMETRY_STATE_GYROSCOPE,
    TELEMETRY_STATE_END
} TELEMETRY_STATE;

static AZURE_IOT_NX_CONTEXT azure_iot_nx_client;

static int32_t telemetry_interval = 10;

static UINT append_device_telemetry(NX_AZURE_IOT_JSON_WRITER* json_writer)
{
    float temperature;

    if (BSP_ENV_SENSOR_GetValue(0, ENV_TEMPERATURE, &temperature) != BSP_ERROR_NONE)
    {
        printf("ERROR: BSP_ENV_SENSOR_GetValue\r\n");
    }

    if (nx_azure_iot_json_writer_append_property_with_double_value(
            json_writer, (UCHAR*)TELEMETRY_HUMIDITY, sizeof(TELEMETRY_HUMIDITY) - 1, temperature, 2))
    {
        return NX_NOT_SUCCESSFUL;
    }

    return NX_AZURE_IOT_SUCCESS;
}

static void telemetry_cb(AZURE_IOT_NX_CONTEXT* nx_context)
{
    static TELEMETRY_STATE telemetry_state = TELEMETRY_STATE_DEFAULT;

    switch (telemetry_state)
    {
        case TELEMETRY_STATE_DEFAULT:
            azure_iot_nx_client_publish_telemetry(&azure_iot_nx_client, NULL, append_device_telemetry);
            break;

        default:
            break;
    }

    telemetry_state = (telemetry_state + 1) % TELEMETRY_STATE_END;
}

UINT azure_iot_nx_client_entry(
    NX_IP* ip_ptr, NX_PACKET_POOL* pool_ptr, NX_DNS* dns_ptr, UINT (*unix_time_callback)(ULONG* unix_time))
{
    UINT status;

    if ((status = azure_iot_nx_client_create(&azure_iot_nx_client,
             ip_ptr,
             pool_ptr,
             dns_ptr,
             unix_time_callback,
             IOT_MODEL_ID,
             sizeof(IOT_MODEL_ID) - 1)))
    {
        printf("ERROR: azure_iot_nx_client_create failed (0x%08x)\r\n", status);
        return status;
    }

    // Register the callbacks
    azure_iot_nx_client_register_timer_callback(&azure_iot_nx_client, telemetry_cb, telemetry_interval);

    // Setup authentication
#ifdef ENABLE_X509
    if ((status = azure_iot_nx_client_cert_set(&azure_iot_nx_client,
             (UCHAR*)iot_x509_device_cert,
             iot_x509_device_cert_len,
             (UCHAR*)iot_x509_private_key,
             iot_x509_private_key_len)))
    {
        printf("ERROR: azure_iot_nx_client_cert_set (0x%08x)\r\n", status);
        return status;
    }
#else
    if ((status = azure_iot_nx_client_sas_set(&azure_iot_nx_client, IOT_DEVICE_SAS_KEY)))
    {
        printf("ERROR: azure_iot_nx_client_sas_set (0x%08x)\r\n", status);
        return status;
    }
#endif

    // Enter the main loop
#ifdef ENABLE_DPS
    azure_iot_nx_client_dps_run(&azure_iot_nx_client, IOT_DPS_ID_SCOPE, IOT_DPS_REGISTRATION_ID, stm_network_connect);
#else
    azure_iot_nx_client_hub_run(&azure_iot_nx_client, IOT_HUB_HOSTNAME, IOT_HUB_DEVICE_ID, stm_network_connect);
#endif

    return NX_SUCCESS;
}
