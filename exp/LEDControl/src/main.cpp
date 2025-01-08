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

#include "mongoose.h"
#include <PicoLed.hpp>

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

int main(){


	stdio_init_all();
	sleep_ms(1000);

	printf("Go\n");

	ledStrip.setBrightness(64);
	ledStrip.fill( PicoLed::RGB(100, 0, 0) );
	ledStrip.show();


	struct mg_mgr mgr;        // Initialise Mongoose event manager
	mg_mgr_init(&mgr);        // and attach it to the interface
	mg_log_set(MG_LL_DEBUG);  // Set log level

	MG_INFO(("Initialising application..."));
	mg_http_listen(&mgr, HTTP_URL, fn, NULL);

	MG_INFO(("Starting event loop"));
	for (;;) {
		mg_mgr_poll(&mgr, 10);
	}


	for (;;){
		printf("Stuff\n");
		sleep_ms(3000);
	}



}


