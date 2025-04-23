/*
 * Wizard.cpp
 *
 *  Created on: 22 Feb 2025
 *      Author: jondurrant
 */

#include "Wizard.h"
#include "hardware/watchdog.h"
#include <pico/stdlib.h>
#include "pico/cyw43_arch.h"

#include "mongoose.h"
#include "NVSHTML.h"

typedef struct{
	char *buf;
	size_t bufLen;
} WizardBuffer;

 const char Wizard::ConfigSaved[] = "<html><body>"
			"<h1>Config Saved</h1>"
			"</body></html>";

#define HTTP_URL "http://0.0.0.0:80"

Wizard::Wizard() {
	// TODO Auto-generated constructor stub

}

Wizard::~Wizard() {
	// TODO Auto-generated destructor stub
}


void Wizard::startAP(const char *ssid, const char *pwd){
	cyw43_arch_enable_ap_mode (
		ssid,  
		pwd,  
		CYW43_AUTH_WPA2_AES_PSK
	);

	ip4_addr_t mask;
	ip4_addr_t gw;
	IP4_ADDR(ip_2_ip4(&gw), 192, 168, 4, 1);
	IP4_ADDR(ip_2_ip4(&mask), 255, 255, 255, 0);

	// Start the dhcp server
	dhcp_server_init(&xDhcpServer, &gw, &mask);

	// Start the dns server
	dns_server_init(&xDnsServer, &gw);
}

void Wizard::run(char *buf, size_t bufLen){
	struct mg_mgr mgr;        // Initialise Mongoose event manager
	 mg_mgr_init(&mgr);        // and attach it to the interface
	 mg_log_set(MG_LL_DEBUG);  // Set log level


	WizardBuffer wb;
	wb.buf = buf;
	wb.bufLen = bufLen;
	MG_INFO(("Initialising application..."));
	mg_http_listen(&mgr, HTTP_URL, Wizard::mgCB, &wb);

	MG_INFO(("Starting event loop"));
	for (;;) {
		mg_mgr_poll(&mgr, 10);
	}

}

void Wizard::mgCB(struct mg_connection *c, int ev, void *ev_data){
	if (ev == MG_EV_HTTP_MSG){
			struct mg_http_message *hm = (struct mg_http_message *) ev_data;  // Parsed HTTP request
			if (mg_match(hm->uri, mg_str("/setConfig"), NULL)){
				NVSHTML::getInstance()->updateQuery(&hm->query);
				mg_http_reply(c, 200,
						"Content-Type: text/html\r\n",
						Wizard::ConfigSaved
						);
				Wizard::reboot();
			} else {
				WizardBuffer *wb = (WizardBuffer*) c->fn_data;
				NVSHTML::getInstance()->toHTML(wb->buf,wb->bufLen);
				mg_http_reply(c, 200,
						"Content-Type: text/html\r\n",
						wb->buf
						);
			}
			c->recv.len = 0;     // Clean receive buffer
			c->is_draining = 1;  // Close this connection when the response is sent;
		}
}

void Wizard::reboot(){
	watchdog_enable(1000, 1);
}
