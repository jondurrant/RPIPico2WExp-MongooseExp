/*
 * main.cpp
 *
 * Hello World
 * Connect to WIFI AP defined by WIFI_SSID and WIFI_PASS (set in CMake)
 * Listen on port 80
 * Answer all HTTP requests with Hello World in JSON
 *  Created on: 7-Jan-2025
 *      Author: jondurrant
 */

#include <cstdio>
#include <pico/stdlib.h>

#include "mongoose.h"

#define HTTP_URL "http://0.0.0.0:80"


char wifiSSID[80];
char wifiPASS[80];

void main_setcreds(char **ssid, char **pass) {
	strcpy(wifiSSID, WIFI_SSID);
	strcpy(wifiPASS, WIFI_PASS);
  *ssid = wifiSSID;
  *pass = wifiPASS;
}

static void blink_timer(void *arg) {
  (void) arg;
  cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, !cyw43_arch_gpio_get(CYW43_WL_GPIO_LED_PIN));
}

static void fn(struct mg_connection *c, int ev, void *ev_data) {

	if (ev == MG_EV_HTTP_MSG){

		mg_http_reply(c, 200, "Content-Type: application/json\r\n", "{\"result\": %m}", MG_ESC("Hello World"));
		c->recv.len = 0;     // Clean receive buffer
		c->is_draining = 1;  // Close this connection when the response is sent;
	}

}

int main(){


	stdio_init_all();
	sleep_ms(1000);

	printf("Go\n");


	struct mg_mgr mgr;        // Initialise Mongoose event manager
	mg_mgr_init(&mgr);        // and attach it to the interface
	mg_log_set(MG_LL_DEBUG);  // Set log level

	MG_INFO(("Initialising application..."));
	mg_http_listen(&mgr, HTTP_URL, fn, NULL);

	mg_timer_add(&mgr, 1000, MG_TIMER_REPEAT, blink_timer, &mgr);

	MG_INFO(("Starting event loop"));
	for (;;) {
		mg_mgr_poll(&mgr, 10);
	}


	for (;;){
		printf("Stuff\n");
		sleep_ms(3000);
	}



}


