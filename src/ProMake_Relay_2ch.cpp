#include "ProMake_Relay_2ch.h"

ProMake_Relay_2ch::ProMake_Relay_2ch(enuProMakeSlot slot){
	if(slot==ARDUINO_SHIELD_SLOT1){
		
	}else{
		PROMAKE_LOGERROR("This Slot is NOT a Good slot for Relay Module")
	}
}