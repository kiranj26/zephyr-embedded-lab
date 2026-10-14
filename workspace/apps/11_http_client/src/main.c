/*
 * Lesson 11 — Wi-Fi + HTTP GET
 *   1. cp src/secrets.h.example src/secrets.h  (fill it in; it's git-ignored)
 *   2. wifi_init(); wifi_connect(WIFI_SSID, WIFI_PSK); wifi_wait_for_ip_addr();
 *   3. DNS: zsock_getaddrinfo("example.com", "80", &hints, &res)   (hints: AF_INET, SOCK_STREAM)
 *   4. zsock_socket / zsock_connect / zsock_send("GET / HTTP/1.1\r\nHost: example.com\r\n\r\n")
 *   5. zsock_recv loop until 0; print; zsock_close; zsock_freeaddrinfo
 *   Challenge: replace 4–5 with http_client_req() (CONFIG_HTTP_CLIENT) and a response callback.
 */
#include <zephyr/kernel.h>
#include <zephyr/net/socket.h>
#include "wifi.h"

#if __has_include("secrets.h")
#include "secrets.h"
#else
#warning "src/secrets.h missing - copy secrets.h.example"
#define WIFI_SSID ""
#define WIFI_PSK  ""
#endif

int main(void)
{
	printk("Lesson 11 starter (SSID configured: %s)\r\n", WIFI_SSID[0] ? "yes" : "no");
	/* TODO */
	return 0;
}
