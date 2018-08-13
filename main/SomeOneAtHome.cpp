/*
 * SomeOneAtHome.cpp
 *
 *  Created on: 25.10.2017
 *      Author: Xiaoqiao
 */
#include "esp_log.h"

#include "lwip/mem.h"
#include "lwip/ip.h"
#include "lwip/raw.h"
#include "lwip/icmp.h"
#include "lwip/netif.h"
#include "lwip/sys.h"
#include "lwip/timers.h"
#include "lwip/inet_chksum.h"

#include "lwip/sockets.h"
#include "lwip/inet.h"
#include "Common.h"

#define TAG "SomeOneAtHome"

static u16_t ping_seq_num = 1;
static u32_t ping_id = 1;
void ping_prepare_echo(struct icmp_echo_hdr *iecho, u16_t len) {
	size_t i;
	size_t data_len = len - sizeof(struct icmp_echo_hdr);

	ICMPH_TYPE_SET(iecho, ICMP_ECHO);
	ICMPH_CODE_SET(iecho, 0);
	iecho->chksum = 0;
	iecho->id = ++ping_id;
	iecho->seqno = lwip_htons(++ping_seq_num);

	/* fill the additional data buffer with some data */
	for (i = 0; i < data_len; i++) {
		((char*) iecho)[sizeof(struct icmp_echo_hdr) + i] = (char) i;
	}

	iecho->chksum = inet_chksum(iecho, len);
}

bool ping_recv(int s) {
	char buf[64];
	int len;
	struct sockaddr_in from;
	struct ip_hdr *iphdr;
	struct icmp_echo_hdr *iecho;
	int fromlen = sizeof(from);

	while ((len = lwip_recvfrom(s, buf, sizeof(buf), 0,
			(struct sockaddr*) &from, (socklen_t*) &fromlen)) > 0) {
		if (len
				>= (int) (sizeof(struct ip_hdr) + sizeof(struct icmp_echo_hdr))) {
			if (from.sin_family != AF_INET) {
				ESP_LOGI(TAG, "ping: invalid sin_family\n");
			} else {
				ip4_addr_t fromaddr;
				inet_addr_to_ipaddr(&fromaddr, &from.sin_addr);
				ESP_LOGI(TAG, "ping: recv %s", ip4addr_ntoa(&fromaddr));

				iphdr = (struct ip_hdr *) buf;
				iecho = (struct icmp_echo_hdr *) (buf + (IPH_HL(iphdr) * 4));
				if ((iecho->id == ping_id)
						&& (iecho->seqno == lwip_htons(ping_seq_num))) {

					return true;
				} else {
					ESP_LOGI(TAG, "ping: drop\n");
				}
			}
		}
		fromlen = sizeof(from);
	}
	if (len == 0) {
		ESP_LOGE(TAG, "ping: rec 0");
		return false;
	}
	return false;
}

err_t ping_send(int s, ip_addr_t *addr) {
	int err;
	struct icmp_echo_hdr *iecho;
	struct sockaddr_in to;
	size_t ping_size = sizeof(struct icmp_echo_hdr) + 32;

	iecho = (struct icmp_echo_hdr *) mem_malloc((mem_size_t) ping_size);
	if (!iecho) {
		return ERR_MEM;
	}
	ping_prepare_echo(iecho, (u16_t) ping_size);
	to.sin_len = sizeof(to);
	to.sin_family = AF_INET;
	inet_addr_from_ipaddr(&to.sin_addr, ip_2_ip4(addr));
	err = lwip_sendto(s, iecho, ping_size, 0, (struct sockaddr*) &to,
			sizeof(to));
	mem_free(iecho);
	return (err ? ERR_OK : ERR_VAL);
}

int PingSocket = 0;
int64_t lastCheckSomeOneAtHome = 0;
bool pingRecived = false;
bool SomeOneAtHome() {
	int ret;
	ip_addr_t ping_target;

	struct timeval timeout;
	timeout.tv_sec = 1;
	timeout.tv_usec = 1000;
	if (PingSocket == 0) {
		if ((PingSocket = lwip_socket(AF_INET, SOCK_RAW, IP_PROTO_ICMP)) < 0) {
			ESP_LOGE(TAG, "failed to create socket");
			return false;
		}

		ret = lwip_setsockopt(PingSocket, SOL_SOCKET, SO_RCVTIMEO, &timeout,
				sizeof(timeout));
		if (ret < 0) {
			ESP_LOGE(TAG, "lwip_setsockopt failed to create socket");
		}
	}
	if((lastCheckSomeOneAtHome + 5000) < hmMillis())
	{
		lastCheckSomeOneAtHome =  hmMillis();
		pingRecived = false;
		IP4_ADDR(ip_2_ip4(&ping_target), 192, 168, 0, 72); // Xiaoqiao
		if (ping_send(PingSocket, &ping_target) == ERR_OK) {
			ESP_LOGI(TAG, "ping: send %s", ipaddr_ntoa(&ping_target));
			pingRecived = ping_recv(PingSocket);
		}
		if (!pingRecived) {
			IP4_ADDR(ip_2_ip4(&ping_target), 192, 168, 0, 86); //jayde
			if (ping_send(PingSocket, &ping_target) == ERR_OK) {
				ESP_LOGI(TAG, "ping: send %s", ipaddr_ntoa(&ping_target));
				ping_recv(PingSocket);
			}
		}
	}
	return pingRecived;
}
