/*
 * main.cpp
 *
 * LED Control
 * Connect to WIFI AP defined by WIFI_SSID and WIFI_PASS (set in CMake)
 * Listen on port 80
 * URL: http://mip/api/led?color=FFFFFF
 * Where FFFFFF is color to set the LED.
 *  Created on: 7-Jan-2025
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


void ledColor(char * hex){
	int rgb;
	if (hex[0] == '#'){
		rgb = (int)strtol(&hex[1], NULL, 16);
	} else {
		rgb = (int)strtol(hex, NULL, 16);
	}
	if (errno == 0){
		uint8_t b = rgb & 0xff;
		uint8_t g = (rgb & 0xff00) >> 8;
		uint8_t r = (rgb & 0xff0000) >> 16;
		ledStrip.fill( PicoLed::RGB(r, g, b) );
		ledStrip.show();
		sprintf(hex, "%X",rgb);
	}
}


static void fn(struct mg_connection *c, int ev, void *ev_data) {

	if (ev == MG_EV_HTTP_MSG){
		struct mg_http_message *hm = (struct mg_http_message *) ev_data;  // Parsed HTTP request
		if (mg_match(hm->uri, mg_str("/api/led"), NULL)) {              // REST API call?
			char colorHex[80];
			if (mg_http_get_var(&hm->query, "color", colorHex, sizeof(colorHex)) > 0){
				printf("Colour was %s\n", colorHex);
				ledColor(colorHex);
				mg_http_reply(c, 200, "", "{%m:%m}\n", MG_ESC("status"), MG_ESC(colorHex));
			} else {
				mg_http_reply(c, 200, "", "{%m:%m}\n", MG_ESC("status"), MG_ESC("ERROR"));
			}
		}  else  if (mg_match(hm->uri, mg_str("/hello"), NULL)){
			mg_http_reply(c, 200, "Content-Type: application/json\r\n", "{\"result\": %m}", MG_ESC("Hello World"));
		} else {
			struct mg_http_serve_opts opts = {
			        .root_dir = "/web",
			        .fs = &mg_fs_packed
			      };
			    mg_http_serve_dir(c, hm, &opts);
		}
		c->recv.len = 0;     // Clean receive buffer
		c->is_draining = 1;  // Close this connection when the response is sent;
	}

}

static void mongoose(void *args) {
  struct mg_mgr mgr;        // Initialise Mongoose event manager
  mg_mgr_init(&mgr);        // and attach it to the interface
  mg_log_set(MG_LL_DEBUG);  // Set log level

  cyw43_arch_init();
  cyw43_arch_enable_sta_mode();
  cyw43_arch_wifi_connect_blocking(WIFI_SSID, WIFI_PASS, CYW43_AUTH_WPA2_AES_PSK);

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


