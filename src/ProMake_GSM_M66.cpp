/*

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/
#include <ProMake_GSM_M66.h>
#include <HardwareSerial.h>
#include <Arduino.h>

#define __RESETPIN__ 7
#define __TOUTLOCALCOMS__ 500
#define __TOUTSHUTDOWN__ 5000
#define __TOUTMODEMCONFIGURATION__ 5000//equivalent to 30000 because of time in interrupt routine.
#define __TOUTAT__ 1000
#define __TOUTSMS__ 7000
#define __TOUTCALL__ 15000
#define __TOUTGPRS__ 10000
#define __NCLIENTS_MAX__ 3

//Constructor.
ProMake_GSM_M66::ProMake_GSM_M66(bool db) 
{
	theProMakeGsmM66ModemCore.setCommandCounter(1);
	socketsAccepted=0;
	theProMakeGsmM66ModemCore.registerUMProvider(this);
	theProvider=this;
}

//Response management.
void ProMake_GSM_M66::manageResponse(byte from, byte to)
{
	switch(theProMakeGsmM66ModemCore.getOngoingCommand())
	{
		case NONE:
			theProMakeGsmM66ModemCore.stream.cb.deleteToTheEnd(from);
			break;

	}
}


///////////////////////////////////////////////////////UNSOLICITED RESULT CODE (URC) FUNCTIONS///////////////////////////////////////////////////////////////////

//URC recognize.
bool ProMake_GSM_M66::recognizeUnsolicitedEvent(byte oldTail)
{

int nlength;
char auxLocate [15];
	//POWER DOWN.
	prepareAuxLocate(PSTR("POWER DOWN"), auxLocate);
	if(theProMakeGsmM66ModemCore.stream.cb.locate(auxLocate))
	{
		theProMakeGsmM66ModemCore.stream.cb.flush();
		return true;
	}
	
	
	return false;
}



