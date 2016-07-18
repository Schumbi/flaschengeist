#include <Arduino.h>

#include "led.h"

static const uint8_t ON  = 1;
static const uint8_t OFF = 0;


// Implementation of Led Class

CLed_base::CLed_base(const uint8_t pin)
{
	this->pin = pin;
	pinMode(this->pin, OUTPUT);
	digitalWrite(this->pin, OFF);
}

CLed_base::~CLed_base()
{
	digitalWrite(this->pin, OFF);
}

//###############################

CLed::CLed(const uint8_t pin)
	: CLed_base::CLed_base(pin)
{
	this->state = false;
}

// change power level of LED
void CLed::power(bool state)
{
	// store state
	this->state = state;
	// set state
	if(state)
		digitalWrite(this->pin, ON);
	else
		digitalWrite(this->pin, OFF);
}
// Activate LED
void CLed::switch_on()
{
	this->power(true);
}

// deactivate LED
void CLed::switch_off()
{
	this->power(false);
}

bool CLed::isOn()
{
	return this->state;
}

//###############################

CLed_fade::CLed_fade(const uint8_t pin, const brightness_t &prog)
	: CLed(pin)
{
	this->setUp(prog);
}

void CLed_fade::setUp(const brightness_t &prog)
{
	this->brightness.max = prog.max;
	this->brightness.min = prog.min;
	this->brightness.val = prog.val;
	this->brightness.fadeAmount = prog.fadeAmount;
}

void CLed_fade::fade()
{
	int check = 0;
	// wenn die LED angeschlatet ist
	if(this->isOn())
	{
		analogWrite(this->pin, this->brightness.val);

		brightness_t &b = this->brightness;

		// avoid overflows
		check = b.val + b.fadeAmount;
		if(check > 255) b.fadeAmount = 255 - b.val;
		if(check < 0)   b.fadeAmount =   0 + b.val;

		b.val = b.val + b.fadeAmount;

		if(b.val <= b.min || b.val >= b.max)
		{
			// reverse direction
			b.fadeAmount = -b.fadeAmount;
		}
	}
	// nothing will happen, if LED is switched off
}

