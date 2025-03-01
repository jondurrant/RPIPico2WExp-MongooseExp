/*
 * main.cpp
 *
 * LED Control of WS2812B LEDs on GP15
 *
 * Switch on GP02 to enter configuration Wizard mode.
 * Single push results in Amber LED display for update config Wizard
 * Long push results in RED LED display for Factory reset
 *
 * In Config Wizard Mode Provide a AP with Credentials below
 * Listen on port 80 IP 192.168.4.1
 * URL: http://mip/api/led?color=FFFFFF
 * Where FFFFFF is color to set the LED.
 *      Author: jondurrant
 */

#include <cstdio>
#include <pico/stdlib.h>
#include "pico/cyw43_arch.h"
#include "hardware/watchdog.h"
#include "FreeRTOS.h"
#include <task.h>

#include "LEDAgent.h"

extern "C"{
#include "dhcpserver.h"
#include "dnsserver.h"
}

#include "mongoose.h"
#include "NVSHTML.h"
#include "Wizard.h"

#define GP_WIZARD 2

#define AP_SSID "PICO2W"
#define AP_PWD "DRJONEA2025"
#define HTTP_URL "http://0.0.0.0:80"

#define WIFI_LEN 80
char wifiSSID[WIFI_LEN];
char wifiPWD[WIFI_LEN];

#define NVS_MAX_VAL 80
#define NVS_SSID "NVS_WIFI_SSID"
#define NVS_PWD "NVS_WIFI__PWD"
#define NVS_WIZARD_SSID "NVS_WIZARD_SSID"
#define NVS_WIZARD_PWD "NVS_WIZARD__PWD"
#define NVS_RED "NVS_LEDS1_RED"
#define NVS_GRN "NVS_LEDS2_GRN"
#define NVS_BLU "NVS_LEDS3_BLU"
#define NVS_SPEED "NVS_LEDS4_SPEED"
#define NVS_BIDIRECT "NVS_LEDS4_BI"
#define NVS_CONFIG "NVS_CONFIG"


#define BUF_LEN 2048
char buf[BUF_LEN];


/***
 * Setup defaults in the NVS if they are not already there
 * Used to do factory reset
 */
void setupDefaults(){
	NVSHTML * nvs = NVSHTML::getInstance();
	nvs_err_t res;

	if (!nvs->contains(NVS_SSID)){
		 nvs->set_str ( NVS_SSID,  "");
	}

	if (!nvs->contains(NVS_PWD)){
		 nvs->set_pwd ( NVS_PWD,  "");
	}


	if (!nvs->contains(NVS_WIZARD_SSID)){
		 nvs->set_str ( NVS_WIZARD_SSID,  AP_SSID);
	}

	if (!nvs->contains(NVS_WIZARD_PWD)){
		 nvs->set_pwd ( NVS_WIZARD_PWD,  AP_PWD);
	}


	if (!nvs->contains(NVS_RED)){
		 nvs->set_u8 ( NVS_RED,  0xFF);
	}


	if (!nvs->contains(NVS_GRN)){
		 nvs->set_u8 ( NVS_GRN,  0xFF);
	}

	if (!nvs->contains(NVS_BLU)){
		 nvs->set_u8 ( NVS_BLU,  0xFF);
	}

	if (!nvs->contains(NVS_SPEED)){
		 nvs->set_double ( NVS_SPEED , 1.0);
	}

	if (!nvs->contains(NVS_BIDIRECT)){
		 nvs->set_bool( NVS_BIDIRECT , true);
	}

	nvs->commit();

}

/***
 * Change the LEDs colour from the Mongoose form
 * @param hex
 * @param la
 */
void ledColor(char * hex, LEDAgent * la){
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
		la->setRGB(r, g, b);
		sprintf(hex, "%X",rgb);
	}
}

/***
 * Mongoose callback function
 * @param c
 * @param ev
 * @param ev_data
 */
static void fn(struct mg_connection *c, int ev, void *ev_data) {

	if (ev == MG_EV_HTTP_MSG){
		struct mg_http_message *hm = (struct mg_http_message *) ev_data;  // Parsed HTTP request
		if (mg_match(hm->uri, mg_str("/api/led"), NULL)) {              // REST API call?
			char colorHex[80];
			if (mg_http_get_var(&hm->query, "color", colorHex, sizeof(colorHex)) > 0){
				printf("Colour was %s\n", colorHex);
				LEDAgent * la = (LEDAgent *) c->fn_data;
				ledColor(colorHex, la);
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

/***
 * Start a Mongoose thread
 * May be either the Config Wizard or normal operations
 * @param args
 */
static void mongoose(void *args) {
	char nvsVals[NVS_MAX_VAL];
	struct mg_mgr mgr;        // Initialise Mongoose event manager
	mg_mgr_init(&mgr);        // and attach it to the interface
	mg_log_set(MG_LL_DEBUG);  // Set log level

	//IF FACTORY RESET HELD DOWN
	NVSHTML * nvs;
	if (gpio_get(GP_WIZARD) == 0){
		nvs = NVSHTML::getInstance(true);
	} else {
		nvs = NVSHTML::getInstance();
	}

	//Initialise defaults if needed
	setupDefaults();

	//Init Wifi Controller
	cyw43_arch_init();

	//Start the LED display
	LEDAgent ledAgent;
	ledAgent.start("LED", configMAX_PRIORITIES-2);

	//Check if we should run the Config Wizard
	//Do this if the NVS_SSID is not set or
	//if the NVS_CONFIG key is in the NVS
	size_t len = NVS_MAX_VAL;
	nvs->get_str ( NVS_SSID,  nvsVals, &len);
	if ((len <2)  ||  nvs->contains(NVS_CONFIG) ){
		printf("Starting Config Wizard\n");
		size_t wlen = WIFI_LEN;
		nvs->get_str ( NVS_WIZARD_SSID,   wifiSSID, &wlen);
		wlen = WIFI_LEN;
		nvs->get_pwd ( NVS_WIZARD_PWD,   wifiPWD, &wlen);
		nvs->erase_key(NVS_CONFIG);
		printf("Local AP %s:%s\n", wifiSSID, wifiPWD);

		ledAgent.setSpeed(0.0);
		//If no NVS_SSDI then we are in factory reset mode
		if ((len < 2)) {
			ledAgent.setRGB(0xff, 0x00, 0x00);
		} else {
			ledAgent.setRGB(0xff, 0xff, 0x00);
		}
		Wizard wz;
		wz.startAP(wifiSSID,  wifiPWD);
		wz.run( buf,  BUF_LEN);
		//Will never get here
		for(;;){
			//NOP
		}
	}

	//Normal Operation!

	//Set LED colour from NVS Config
	uint8_t r,g,b;
	nvs->get_u8(NVS_RED, &r);
	nvs->get_u8(NVS_GRN, &g);
	nvs->get_u8(NVS_BLU, &b);
	ledAgent.setRGB(r, g,b);

	//Set LED speed from NVS Config
	double sp;
	nvs->get_double(NVS_SPEED, &sp);
	ledAgent.setSpeed(sp);

	//Join Wifi using credentials in NVS
	cyw43_arch_enable_sta_mode();
	size_t wlen = WIFI_LEN;
	nvs->get_str ( NVS_SSID,   wifiSSID, &wlen);
	wlen = WIFI_LEN;
	nvs->get_pwd ( NVS_PWD,   wifiPWD, &wlen);
	//printf("Join AP %s:%s\n", wifiSSID, wifiPWD);
	cyw43_arch_wifi_connect_blocking(wifiSSID, wifiPWD, CYW43_AUTH_WPA2_AES_PSK);
	char *s = ipaddr_ntoa(netif_ip4_addr(&cyw43_state.netif[0]));
	printf("IP: %s\n", s);

  //Run Mongoose
  MG_INFO(("Initialising application..."));
  mg_http_listen(&mgr, HTTP_URL, fn, &ledAgent);

  MG_INFO(("Starting event loop"));
  for (;;) {
    mg_mgr_poll(&mgr, 10);

    //Poll the wizard switch and if needed reboot into config mode
    if (gpio_get(GP_WIZARD) == 0){
    	nvs->set_bool(NVS_CONFIG, true);
    	nvs->commit();
    	watchdog_enable(1, 1);
    }
  }


}



int main(){
   //Setup the Config Wizard Switch
   gpio_init(GP_WIZARD);
   gpio_set_dir(GP_WIZARD, GPIO_IN);
   gpio_pull_up(GP_WIZARD);

	stdio_init_all();
	sleep_ms(1000);

	printf("Go\n");


	//Add mongoose task and run scheduler
	xTaskCreate(mongoose, "mongoose", 1024*8, 0, configMAX_PRIORITIES - 1, NULL);
	vTaskStartScheduler();

	for (;;){
		//Never reached
	}



}


