/* Copyright (c) Microsoft Corporation.
   Licensed under the MIT License. */

#include "stm_networking.h"

#include "nx_api.h"
#include "nx_secure_tls_api.h"
#include "nxd_dhcp_client.h"
#include "nxd_dns.h"

#include "nx_driver_emw3080.h"

#include "sntp_client.h"

#define NETX_IP_STACK_SIZE 2048
#define NETX_IP_STACK_PRIORITY 1
#define NETX_PACKET_COUNT  20
#define NETX_PACKET_SIZE   1544 // Set the default value to 1544 since MXChip WiFi requires at least 1542
#define NETX_POOL_SIZE     ((NETX_PACKET_SIZE + sizeof(NX_PACKET)) * NETX_PACKET_COUNT)

#define NETX_IPV4_ADDRESS           IP_ADDRESS(0, 0, 0, 0)
#define NETX_IPV4_MASK              IP_ADDRESS(255, 255, 255, 0)

static UCHAR netx_ip_stack[NETX_IP_STACK_SIZE];
static UCHAR netx_ip_pool[NETX_POOL_SIZE];

#ifndef NETX_ARP_CACHE_SIZE
#define NETX_ARP_CACHE_SIZE (512)
#endif /* NETX_ARP_CACHE_SIZE  */

static ULONG netx_arp_cache[NETX_ARP_CACHE_SIZE / sizeof(ULONG)];

NX_IP nx_ip;
NX_PACKET_POOL nx_pool;
NX_DNS nx_dns_client;
NX_DHCP nx_dhcp;

static void print_address(CHAR* preable, ULONG address)
{
    printf("\t%s: %lu.%lu.%lu.%lu\r\n", preable, 
        (address >> 24),
        (address >> 16 & 0xFF),
        (address >> 8 & 0xFF),
        (address & 0xFF));
}

UINT dhcp_connect()
{
    UINT status;
    ULONG ip_status;
    ULONG ip_address      = 0;
    ULONG ip_mask         = 0;
    ULONG gateway_address = 0;

    printf("\r\nInitializing DHCP\r\n");

    // Create DHCP client
    nx_dhcp_create(&nx_dhcp, &nx_ip, "DHCP Client");

    // Start DHCP client
    nx_dhcp_start(&nx_dhcp);

    // Wait until IP address is resolved
    nx_ip_status_check(&nx_ip, NX_IP_ADDRESS_RESOLVED, &ip_status, NX_WAIT_FOREVER);

    // Get IP address
    if ((status = nx_ip_address_get(&nx_ip, &ip_address, &ip_mask)))
    {
        printf("ERROR: nx_ip_address_get (0x%08x)\r\n", status);
        return status;
    }

    // Get gateway address
    if ((status = nx_ip_gateway_address_get(&nx_ip, &gateway_address)))
    {
        printf("ERROR: nx_ip_gateway_address_get (0x%08x)\r\n", status);
        return status;
    }

    // Output IP address and gateway address
    print_address("IP address", ip_address);
    print_address("Mask", ip_mask);
    print_address("Gateway", gateway_address);

    printf("SUCCESS: DHCP initialized\r\n");

    return NX_SUCCESS;
}

UINT dns_connect()
{
    UINT status;
    ULONG dns_server_address[3];
    UINT dns_server_address_size = 12;

    printf("\r\nInitializing DNS client\r\n");

    if ((status = nx_dns_server_remove_all(&nx_dns_client)))
    {
        printf("ERROR: nx_dns_server_remove_all (0x%08x)\r\n", status);
        return status;
    }

    if ((status = nx_dhcp_interface_user_option_retrieve(
             &nx_dhcp, 0, NX_DHCP_OPTION_DNS_SVR, (UCHAR*)(dns_server_address), &dns_server_address_size)))
    {
        printf("ERROR: nx_dhcp_interface_user_option_retrieve (0x%08x)\r\n", status);
        return status;
    }

    // Add an IPv4 server address to the Client list
    if ((status = nx_dns_server_add(&nx_dns_client, dns_server_address[0])))
    {
        printf("ERROR: nx_dns_server_add (0x%08x)\r\n", status);
        return status;
    }

    // Output DNS Server address
    print_address("DNS address", dns_server_address[0]);

    printf("SUCCESS: DNS client initialized\r\n");

    return NX_SUCCESS;
}

UINT stm_network_init()
{
    UINT status;

    // Initialize the NetX system
    nx_system_initialize();

    // Create a packet pool
    if ((status = nx_packet_pool_create(
                  &nx_pool, "NetX Packet Pool", NETX_PACKET_SIZE, netx_ip_pool, NETX_POOL_SIZE)))
    {
        printf("ERROR: nx_packet_pool_create (0x%08x)\r\n", status);
    }

    // Create an IP instance
    else if ((status = nx_ip_create(&nx_ip,
                  "NetX IP Instance 0",
                  NETX_IPV4_ADDRESS,
                  NETX_IPV4_MASK,
                  &nx_pool,
                  nx_driver_emw3080_entry,
                  (UCHAR*)netx_ip_stack,
                  NETX_IP_STACK_SIZE,
                  NETX_IP_STACK_PRIORITY)))
    {
        nx_packet_pool_delete(&nx_pool);
        printf("ERROR: nx_ip_create (0x%08x)\r\n", status);
    }

    else if ((status = nx_arp_enable(&nx_ip, 
                    (VOID*)netx_arp_cache, 
                    sizeof(netx_arp_cache))))
    {
        nx_ip_delete(&nx_ip);
        nx_packet_pool_delete(&nx_pool);
        printf("ERROR: nx_arp_enable (0x%08x)\r\n", status);
    }

    // Enable TCP traffic
    else if ((status = nx_tcp_enable(&nx_ip)))
    {
        nx_ip_delete(&nx_ip);
        nx_packet_pool_delete(&nx_pool);
        printf("ERROR: nx_tcp_enable (0x%08x)\r\n", status);
    }

    // Enable UDP traffic
    else if ((status = nx_udp_enable(&nx_ip)))
    {
        nx_ip_delete(&nx_ip);
        nx_packet_pool_delete(&nx_pool);
        printf("ERROR: nx_udp_enable (0x%08x)\r\n", status);
    }

    else if ((status = nx_dns_create(&nx_dns_client, &nx_ip, (UCHAR*)"DNS Client")))
    {
        nx_ip_delete(&nx_ip);
        nx_packet_pool_delete(&nx_pool);
        printf("ERROR: nx_dns_create (0x%08x)\r\n", status);
    }

    // Use the packet pool here
#ifdef NX_DNS_CLIENT_USER_CREATE_PACKET_POOL
    else if ((status = nx_dns_packet_pool_set(&nx_dns_client, nx_ip.nx_ip_default_packet_pool)))
    {
        nx_dns_delete(&nx_dns_client);
        nx_ip_delete(&nx_ip);
        nx_packet_pool_delete(&nx_pool);
        printf("ERROR: nx_dns_packet_pool_set (%0x08)\r\n", status);
    }
#endif

    // Initialize the SNTP client
    else if ((status = sntp_init()))
    {
        printf("ERROR: Failed to init the SNTP client (0x%08x)\r\n", status);
    }

    // Initialize TLS
    else
    {
        nx_secure_tls_initialize();
    }

    return status;
}

UINT stm_network_connect()
{
    UINT status;

    // Fetch IP details
    if ((status = dhcp_connect()))
    {
        printf("ERROR: dhcp_connect\r\n");
    }

    // Create DNS
    else if ((status = dns_connect()))
    {
        printf("ERROR: dns_connect\r\n");
    }

    // Wait for an SNTP sync
    else if ((status = sntp_sync()))
    {
        printf("ERROR: Failed to sync SNTP time (0x%08x)\r\n", status);
    }

    return status;
}
