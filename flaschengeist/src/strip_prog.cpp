#include "strip.h"


bool CLedStrip::prog0(prog_param_t &conf)
{
	const uint8_t id = 0;
	if(!this->_init)
		return this->_init;
	
	if((conf.old_prog != this->_prog) && (this->_prog == id))
	{
		this->ClearTo(RgbColor(90,35,5));
		conf.old_prog = id;
	}
	// do, well, nothing...
	delay(500);

	return true;
}

bool CLedStrip::prog1(prog_param_t &conf)
{
	const uint8_t id = 1;
	bool ret = false;
	if(!this->_init)
		return this->_init;

	// not started up to now
	if((conf.old_prog != this->_prog) && (this->_prog == id))
	{
		this->SetPixelColor(0, RgbColor(0,0,100)); // oben
		this->SetPixelColor(1, RgbColor(0,50,50)); // mitte
		this->SetPixelColor(2, RgbColor(50,0,50)); // unten
		conf.old_prog = id;
		ret = true;
	}

	// do stuff
	if(conf.ctr >= conf.max)
	{
		// rotate left only
		this->RotateLeft(1);
		// reset counter to zero (restart)
		conf.ctr = conf.min;
	}
	else
		conf.ctr++;

	return ret;	
}

bool CLedStrip::prog2(prog_param_t &conf)
{
	const uint8_t id = 2;
	bool ret = false;
	if(!this->_init)
		return this->_init;

	// not started up to now
	if((conf.old_prog != this->_prog) && (this->_prog == id))
	{
		this->SetPixelColor(0, RgbColor(0,0,100)); // oben
		this->SetPixelColor(1, RgbColor(0,50,50)); // mitte
		this->SetPixelColor(2, RgbColor(50,0,50)); // unten
		conf.old_prog = id;
		ret = true;
	}
	// rotate left only
	this->RotateLeft(1);

	return ret;	
}

bool CLedStrip::prog3(prog_param_t &conf)
{
	const uint8_t id = 3;
	bool ret = false;
	if(!this->_init)
		return this->_init;

	// not started up to now
	if((conf.old_prog != this->_prog) && (this->_prog == id))
	{
		this->SetPixelColor(0, RgbColor(0,0,100)); // oben
		this->SetPixelColor(1, RgbColor(0,50,50)); // mitte
		this->SetPixelColor(2, RgbColor(50,0,50)); // unten
		conf.old_prog = id;
		ret = true;
	}
	// rotate left only
	this->RotateLeft(1);

	return ret;	
}

bool CLedStrip::prog4(prog_param_t &conf)
{
	const uint8_t id = 4;
	bool ret = false;
	if(!this->_init)
		return this->_init;

	// not started up to now
	if((conf.old_prog != this->_prog) && (this->_prog == id))
	{
		this->SetPixelColor(0, RgbColor(0,0,100)); // oben
		this->SetPixelColor(1, RgbColor(0,50,50)); // mitte
		this->SetPixelColor(2, RgbColor(50,0,50)); // unten
		conf.old_prog = id;
		ret = true;
	}
	// rotate left only
	this->RotateLeft(1);

	return ret;	
}

bool CLedStrip::prog5(prog_param_t &conf)
{
	const uint8_t id = 5;
	bool ret = false;
	if(!this->_init)
		return this->_init;

	// not started up to now
	if((conf.old_prog != this->_prog) && (this->_prog == id))
	{
		this->SetPixelColor(0, RgbColor(0,0,100)); // oben
		this->SetPixelColor(1, RgbColor(0,50,50)); // mitte
		this->SetPixelColor(2, RgbColor(50,0,50)); // unten
		conf.old_prog = id;
		ret = true;
	}
	// rotate left only
	this->RotateLeft(1);

	return ret;	
}

