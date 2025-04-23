/*
 * main.cpp
 *
 * LED Control
 * Connect to WIFI AP defined by WIFI_SSID and WIFI_PASS (set in CMake)
 * Listen on port 80
 * URL: http://mip/api/led
 * POST: JSON of form {red: 100, grn: 100, blu:100 }
 *  Created on: 23-Apr-2025
 *      Author: jondurrant
 */

#include <cstdio>
#include <pico/stdlib.h>
#include "pico/cyw43_arch.h"
#include "FreeRTOS.h"
#include <task.h>

#include <PicoLed.hpp>

#include "mongoose.h"



#define HTTP_URL "http://0.0.0.0:80"

#define LED_PIN 15
#define LED_LENGTH 11

auto ledStrip = PicoLed::addLeds<PicoLed::WS2812B>(pio0, 0, LED_PIN, LED_LENGTH, PicoLed::FORMAT_GRB);


char wifiSSID[80];
char wifiPASS[80];


void ledColor(long red, long grn, long blu){
	ledStrip.fill( PicoLed::RGB(red, grn, blu) );
	ledStrip.show();
}


static void fn(struct mg_connection *c, int ev, void *ev_data) {

	if (ev == MG_EV_HTTP_MSG){
		struct mg_http_message *hm = (struct mg_http_message *) ev_data;  // Parsed HTTP request
		if (mg_match(hm->uri, mg_str("/api/led"), NULL)) {              // REST API call?

			printf("Body: %.*s\n",
					hm->body.len,
					hm->body.buf
					);

			long red = mg_json_get_long(hm->body, "$.red", -1);
			long grn = mg_json_get_long(hm->body, "$.grn", -1);
			long blu = mg_json_get_long(hm->body, "$.blu", -1);


			if (
					((red >= 0) && (red < 256)) &&
					((grn >= 0) && (grn < 256)) &&
					((blu >= 0) && (blu < 256))
				){
				ledColor(red, grn, blu);
				mg_http_reply(c, 200, "", "{%m:%m}\n", MG_ESC("status"), MG_ESC("OK"));
			} else {
				printf("Error red: %d, grn: %d, blu: %d\n", red, grn, blu);
				mg_http_reply(c, 200, "", "{%m:%m}\n", MG_ESC("status"), MG_ESC("ERROR"));
			}
		} else {
			printf("Unknown request\n");
		}
	}

}

static void mongoose(void *args) {
  struct mg_mgr mgr;        // Initialise Mongoose event manager
  mg_mgr_init(&mgr);        // and attach it to the interface
  mg_log_set(MG_LL_DEBUG);  // Set log level

  cyw43_arch_init();
  cyw43_arch_enable_sta_mode();
  int r =-1;
  uint8_t attempts = 0;
  while (r < 0){
    attempts++;
	r = cyw43_arch_wifi_connect_timeout_ms(
			WIFI_SSID,
			WIFI_PASS,
			CYW43_AUTH_WPA2_AES_PSK,
			60000);

	if (r){
		printf("Failed to join AP.\n");
		if (attempts >= 3){
			return ;
		}
		vTaskDelay(2000);
	}
  }


  printf("IP Address: %s\n",
		  ipaddr_ntoa(netif_ip4_addr(&cyw43_state.netif[0]))
		  );

  MG_INFO(("Initialising application..."));
  mg_http_listen(&mgr, HTTP_URL, fn, NULL);

  MG_INFO(("Starting event loop"));
  for (;;) {
    mg_mgr_poll(&mgr, 10);
  }

  (void) args;
}

int main(){


	stdio_init_all();
	sleep_ms(1000);

	printf("Go\n");

	ledStrip.setBrightness(64);
	ledStrip.fill( PicoLed::RGB(100, 0, 0) );
	ledStrip.show();

	xTaskCreate(mongoose, "mongoose", 1024*8, 0, configMAX_PRIORITIES - 1, NULL);

	  vTaskStartScheduler();  // This blocks



	for (;;){
		printf("Stuff\n");
		sleep_ms(3000);
	}



}


