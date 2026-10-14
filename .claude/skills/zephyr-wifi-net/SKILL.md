---
name: zephyr-wifi-net
description: ESP32-S3 Wi-Fi + networking on Zephyr - net_mgmt events, semaphores for async connect/DHCP, DNS (zsock_getaddrinfo), BSD sockets, the HTTP client subsystem, heap sizing, and secrets handling. Use for lesson 11 and any network feature.
---
# Wi-Fi and networking (Part 11)

- **Config:** `11_demo_wifi/prj.conf` plus the board conf (`ESP32_WIFI_STA_AUTO_DHCPV4=y`, `ESP_WIFI_HEAP_SYSTEM=y`, `HEAP_MEM_POOL_SIZE=51200`)
  and the overlay `&wifi { status = "okay"; };`. The challenge adds `CONFIG_HTTP_CLIENT=y`.
- **Flow:** `net_mgmt_init_event_callback` + `net_mgmt_add_event_callback` for `NET_EVENT_WIFI_CONNECT_RESULT` and `NET_EVENT_IPV4_ADDR_ADD`.
  Then `net_mgmt(NET_REQUEST_WIFI_CONNECT, iface, &params, sizeof(params))`. Block on semaphores given from the callbacks.
- **Then:** `zsock_getaddrinfo(host, "80", &hints, &res)`, `zsock_socket`, `zsock_connect`, then either `zsock_send`/`zsock_recv`
  or `http_client_req(sock, &req, timeout, NULL)` with a response callback (called per chunk: `HTTP_DATA_MORE` / `HTTP_DATA_FINAL`).
  Remember `zsock_freeaddrinfo(res)` and `zsock_close(sock)`.
- **Network rules:** 2.4 GHz only (no 5 GHz). WPA2-PSK works with `WIFI_SECURITY_TYPE_PSK`. WPA3-only networks won't. Port 80 is plain HTTP only.
- **Secrets:** `#include "secrets.h"` (git-ignored). Commit `secrets.h.example`.
- **Robustness extras:** handle `NET_EVENT_WIFI_DISCONNECT_RESULT` → reconnect with backoff. Check every return. Bound buffers.
