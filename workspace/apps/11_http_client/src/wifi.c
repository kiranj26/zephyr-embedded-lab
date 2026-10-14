/*
 * Lesson 11 — write the Wi-Fi helper yourself.
 *   - two net_mgmt_event_callback structs (Wi-Fi connect/disconnect, IPv4 address added)
 *   - two semaphores (K_SEM_DEFINE(sem, 0, 1)) given from the callbacks
 *   - wifi_connect(): fill struct wifi_connect_req_params (ssid, psk, WIFI_SECURITY_TYPE_PSK,
 *       WIFI_FREQ_BAND_UNKNOWN, WIFI_CHANNEL_ANY, WIFI_MFP_OPTIONAL) -> net_mgmt(NET_REQUEST_WIFI_CONNECT, ...)
 *       then k_sem_take(&sem_wifi, K_FOREVER)
 *   - wifi_wait_for_ip_addr(): wait on sem_ipv4, then print status (NET_REQUEST_WIFI_IFACE_STATUS)
 * Improvement over the course: return errors, use timeouts instead of K_FOREVER, reconnect on disconnect.
 */
#include <errno.h>
#include <zephyr/kernel.h>
#include <zephyr/net/wifi_mgmt.h>
#include "wifi.h"

void wifi_init(void) { /* TODO */ }
int wifi_connect(const char *ssid, const char *psk) { ARG_UNUSED(ssid); ARG_UNUSED(psk); return -ENOSYS; }
void wifi_wait_for_ip_addr(void) { /* TODO */ }
int wifi_disconnect(void) { return -ENOSYS; }
