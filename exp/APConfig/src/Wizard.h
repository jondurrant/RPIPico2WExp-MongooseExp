/*
 * Wizard.h
 *
 *  Created on: 22 Feb 2025
 *      Author: jondurrant
 */

#ifndef EXP_APCONFIG_SRC_WIZARD_H_
#define EXP_APCONFIG_SRC_WIZARD_H_

extern "C"{
#include "dhcpserver.h"
#include "dnsserver.h"
}

class Wizard {
public:
	Wizard();
	virtual ~Wizard();

	void startAP(const char *ssid, const char *pwd);

	void run(char *buf, size_t bufLen);

	static void reboot();


private:

	static const char ConfigSaved[];

	static void mgCB(struct mg_connection *c, int ev, void *ev_data);



	dhcp_server_t xDhcpServer;
	dns_server_t xDnsServer;
};

#endif /* EXP_APCONFIG_SRC_WIZARD_H_ */
