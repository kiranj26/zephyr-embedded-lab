#ifndef WIFI_H_
#define WIFI_H_

void wifi_init(void);
int wifi_connect(const char *ssid, const char *psk);
void wifi_wait_for_ip_addr(void);
int wifi_disconnect(void);

#endif /* WIFI_H_ */
