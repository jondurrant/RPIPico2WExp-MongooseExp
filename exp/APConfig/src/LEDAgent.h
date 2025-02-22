/*
 * LEDAgent.h
 *
 *  Created on: 8 Jan 2025
 *      Author: jondurrant
 */

#ifndef EXP_LEDANIM_SRC_LEDAGENT_H_
#define EXP_LEDANIM_SRC_LEDAGENT_H_

#include "pico/stdlib.h"
#include "Agent.h"

#include <PicoLed.hpp>

#define LED_PIN 15
#define LED_LENGTH 11

class LEDAgent : public Agent {
public:
	LEDAgent();
	virtual ~LEDAgent();

	void setRGB(uint8_t red, uint8_t green, uint8_t blue);

	/***
	 * Speed between 0 and 1.0
	 * @param speed
	 */
	void setSpeed(float speed);

protected:
	/***
	 * Task main run loop
	 */
	virtual void run();

	/***
	 * Get the static depth required in words
	 * @return - words
	 */
	virtual configSTACK_DEPTH_TYPE getMaxStackSize();


private:

	PicoLed::PicoLedController ledStrip  =  PicoLed::addLeds<PicoLed::WS2812B>(pio0, 0, LED_PIN, LED_LENGTH, PicoLed::FORMAT_GRB);

	uint8_t xRed = 0xFF;
	uint8_t xGrn = 0xFF;
	uint8_t xBlu = 0xFF;

	uint8_t xDelay = 100;
};

#endif /* EXP_LEDANIM_SRC_LEDAGENT_H_ */
