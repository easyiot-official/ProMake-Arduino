#ifndef PROMAKE_RELAY_2CH_H_INCLUDED
#define PROMAKE_RELAY_2CH_H_INCLUDED

#include <ProMake.h>

class ProMake_Relay_2ch{
public:
	ProMake_Relay_2ch();
	~ProMake_Relay_2ch();
	void init(enuProMakeSlot slot);
	void SetRelay1State(bool on);
	void SetRelay2State(bool on);
private:
	int m_relay1_pin;
	int m_relay2_pin;

};

#endif