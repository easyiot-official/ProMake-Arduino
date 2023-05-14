#include "ProMake_Relay_2ch.h"

#include <Arduino.h>

ProMake_Relay_2ch::ProMake_Relay_2ch(){
	m_relay1_pin = -1;
	m_relay2_pin = -1;
}

ProMake_Relay_2ch::~ProMake_Relay_2ch(){
	//Serial.println("~ProMake_Relay_2ch");
}

void ProMake_Relay_2ch::init(enuProMakeSlot slot){
	switch(slot){
		case ARDUINO_SHIELD_SLOT1:
		case ARDUINO_NANO_KIT_SLOT1:
			m_relay1_pin = 5;
			m_relay2_pin = 10;
		break;
		case ARDUINO_SHIELD_SLOT2:
		case ARDUINO_NANO_KIT_SLOT2:
			m_relay1_pin = 6;
			m_relay2_pin = 9;
		break;
		case ARDUINO_NANO_KIT_SLOT3:
			m_relay1_pin = 2;
			m_relay2_pin = 14;
			PROMAKE_LOGERROR("You need to change the state of "INT-SEL" jumper to use Relay 2CH Module in slot 3");
		break;
		default:
			PROMAKE_LOGERROR("INVALID slot for Relay 2CH Module");
    		while(true);
		break;
	}
	pinMode(m_relay1_pin,OUTPUT);
	pinMode(m_relay2_pin,OUTPUT);
}

void ProMake_Relay_2ch::SetRelay1State(bool on){
	digitalWrite(m_relay1_pin, on ? HIGH : LOW);
}
void ProMake_Relay_2ch::SetRelay2State(bool on){
    digitalWrite(m_relay2_pin, on ? HIGH : LOW);
}