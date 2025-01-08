/*
 * LEDAgent.cpp
 *
 *  Created on: 8 Jan 2025
 *      Author: jondurrant
 */

#include "LEDAgent.h"

LEDAgent::LEDAgent() {
	// TODO Auto-generated constructor stub

}

LEDAgent::~LEDAgent() {
	// TODO Auto-generated destructor stub
}


void LEDAgent::setRGB(uint8_t red, uint8_t green, uint8_t blue){
	xRed = red;
	xGrn = green;
	xBlu = blue;
}


void LEDAgent::run(){

	for (;;){

		for (int i=0; i < LED_LENGTH; i++){
			ledStrip.clear();
			ledStrip.setPixelColor(i,  PicoLed::RGB(xRed, xGrn, xBlu));
			ledStrip.show();
			vTaskDelay(100);
		}
		for (int i=0; i < LED_LENGTH; i++){
			ledStrip.clear();
			ledStrip.setPixelColor(LED_LENGTH - i - 1,  PicoLed::RGB(xRed, xGrn, xBlu));
			ledStrip.show();
			vTaskDelay(100);
		}
	}
}

configSTACK_DEPTH_TYPE LEDAgent::getMaxStackSize(){
	return 1024;
}
