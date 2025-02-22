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
		if (xDelay == 0){
			ledStrip.fill(PicoLed::RGB(xRed, xGrn, xBlu));
			ledStrip.show();
			vTaskDelay(100);
		} else {
			for (int i=0; i < LED_LENGTH; i++){
				ledStrip.clear();
				ledStrip.setPixelColor(i,  PicoLed::RGB(xRed, xGrn, xBlu));
				ledStrip.show();
				vTaskDelay(xDelay);
			}
			for (int i=0; i < LED_LENGTH; i++){
				ledStrip.clear();
				ledStrip.setPixelColor(LED_LENGTH - i - 1,  PicoLed::RGB(xRed, xGrn, xBlu));
				ledStrip.show();
				vTaskDelay(xDelay);
			}
		}
	}
}

configSTACK_DEPTH_TYPE LEDAgent::getMaxStackSize(){
	return 1024;
}

void LEDAgent::setSpeed(float speed){
	if ((speed > 1.0) || (speed < 0.0)){
		return;
	}

	xDelay = (uint8_t)( 100.0* speed);
}

