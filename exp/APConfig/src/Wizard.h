/*
 * Wizard.h
 *
 * A Config wizard that will configure the NVS Storage parameters via a web form
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

	/***
	 * Start local AP with the given SSID and Password
	 * @param ssid
	 * @param pwd
	 */
	void startAP(const char *ssid, const char *pwd);

	/***
	 * Start a configuration WebServer
	 * @param buf
	 * @param bufLen
	 */
	void run(char *buf, size_t bufLen);

	/***
	 * Reboot the device to trigger a clean restart
	 */
	static void reboot();


private:

	static const char ConfigSaved[];

	/***
	 * Mongoose callback to provide webpage
	 * @param c
	 * @param ev
	 * @param ev_data
	 */
	static void mgCB(struct mg_connection *c, int ev, void *ev_data);



	dhcp_server_t xDhcpServer;
	dns_server_t xDnsServer;
};

#endif /* EXP_APCONFIG_SRC_WIZARD_H_ */
