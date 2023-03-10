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

#ifndef __ProMake_GSM_M66_ModemCore__
#define __ProMake_GSM_M66_ModemCore__

#include <GSM/ProMake_GSM_SoftSerial.h>
#include <ProMake_GSM_M66_BaseProvider.h>
#include <GSM/ProMake_GSM_MobileAccessProvider.h>
#include <Print.h>

#define UMPROVIDERS 3

class ProMake_GSM_M66_ModemCore : public ProMake_GSM_SoftSerialMgr, public Print
{
	private:
		
		// Phone number, used when calling, sending SMS and reading calling numbers
		// Also PIN in modem configuration
		// Also APN
		// Also remote server
		char* phoneNumber;	
		
		// Working port. Port used in the ongoing command, while opening a server
		// Also for IP address length
		int port;

		// 0 = ongoing
		// 1 = OK
		// 2 = Error. Incorrect state
		// 3 = Unexpected modem message
		// 4 = OK but not available data. 
		uint8_t commandError;

		// Counts the steps by the command
		uint8_t commandCounter;
		
		// Presently ongoing command
		ProMake_GSM_commandType_e ongoingCommand;
		
		// Enable/disable debug
		bool _debug;
		byte _dataInBufferFrom;
		byte _dataInBufferTo;
		
		// This is the modem (known) status
		ProMake_GSM_NetworkStatus_t _status;
		
		ProMake_GSM_M66_BaseProvider* UMProvider[UMPROVIDERS];
		ProMake_GSM_M66_BaseProvider* activeProvider;
		
		// Private function for anage message
		void manageMsgNow(byte from, byte to);
		
		unsigned long milliseconds;

	public:
	
		/** Constructor */
		ProMake_GSM_M66_ModemCore();
		
		ProMake_GSM_SoftSerial gss; // Direct access to modem
		
		/** Get phone number
			@return phone number
		 */
		char *getPhoneNumber(){return phoneNumber;};
		
		/** Establish a new phone number
			@param n			Phone number
		 */
		void setPhoneNumber(char *n){phoneNumber=n;};

		/** Get port used
			@return port
		 */
		int getPort(){return port;};
		
		/** Establish a new port for use
			@param p			Port
		 */
		void setPort(int p){port=p;};

		/** Get command error
			@return command error
		 */
		uint8_t getCommandError(){return commandError;};
		
		/** Establish a command error
			@param n			Command error
		 */
		void setCommandError(uint8_t n){commandError=n;};
		
		/** Get command counter
			@return command counter
		 */
		uint8_t getCommandCounter(){return commandCounter;};
		
		/** Set command counter
			@param c			Initial value
		 */
		void setCommandCounter(uint8_t c){commandCounter=c;};

		/** Get ongoing command
			@return command
		 */
		ProMake_GSM_commandType_e getOngoingCommand(){return ongoingCommand;};
		
		/** Set ongoing command
			@param c			 New ongoing command
		 */
		void setOngoingCommand(ProMake_GSM_commandType_e c){ongoingCommand=c;};
		
		/** Open command
			@param activeProvider	Active provider
			@param c				Command for open
		 */
		void openCommand(ProMake_GSM_M66_BaseProvider* activeProvider, ProMake_GSM_commandType_e c);
		
		/** Close command
			@param code			Close code
		 */
		void closeCommand(int code);

		// These functions allow writing to the SoftwareSerial
		// If debug is set, dump to the console
		
		/** Write a character in serial
			@param c			Character
			@return size
		 */
		size_t write(uint8_t c);
		
		/** Write PGM
			@param str			Buffer for write
			@param CR			Carriadge return adding automatically
			@return size
		 */
		virtual size_t writePGM(PGM_P str, bool CR=true);
		
		/** Establish debug mode
			@param db			Boolean that indicates debug on or off
		 */
		void setDebug(bool db){_debug=db;};
		
		/** Generic response parser
			@param rsp			Returns true if expected response exists
			@param string		Substring expected in response
			@param string2		Second substring expected in response
			@return true if parsed correctly
		 */		
		bool genericParse_rsp(bool& rsp, const char* string=0, const char* string2=0);
		
		/** Generates a generic AT command request from PROGMEM buffer
			@param str			Buffer with AT command
			@param addCR		Carriadge return adding automatically
		 */
		void genericCommand_rq(PGM_P str, bool addCR=true);

		/** Generates a generic AT command request from a simple char buffer
			@param str			Buffer with AT command
			@param addCR		Carriadge return adding automatically
		 */
		void genericCommand_rqc(const char* str, bool addCR=true);		
		
		/** Returns the circular buffer
			@return circular buffer
		 */
		inline ProMake_GSM_CircularBuffer& theBuffer(){return gss.cb;};

		/** Establish a new network status
			@param status		Network status
		 */
		inline void setStatus(ProMake_GSM_NetworkStatus_t status) { _status = status; };
		
		/** Returns actual network status
			@return network status
		 */
		inline ProMake_GSM_NetworkStatus_t getStatus() { return _status; };
		
		/** Register provider as willing to receive unsolicited messages
			@param provider	Pointer to provider able to receive unsolicited messages
		*/
		void registerUMProvider(ProMake_GSM_M66_BaseProvider* provider);		
	
		/** unegister provider as willing to receive unsolicited messages
			@param provider	Pointer to provider able to receive unsolicited messages
		*/
		void unRegisterUMProvider(ProMake_GSM_M66_BaseProvider* provider);		

		
		/** Register a provider as "dialoguing" talking in facto with the modem
			@param provider	Pointer to provider receiving responses
		*/
		void registerActiveProvider(ProMake_GSM_M66_BaseProvider* provider){activeProvider=provider;};
		
		/** Needed to manage the SoftSerial. Receives the call when received data
			If _debugging, no code is called
		@param from Starting byte to read
		@param to Last byte to read
		*/
		void manageMsg(byte from, byte to);
		
		/** If _debugging, this call is assumed to be made out of interrupts
			Prints incoming info and calls manageMsgNow
		*/
		void manageReceivedData();

		/** Chronometer. Measure milliseconds from last call
			@return milliseconds from las time function was called
		*/		
		unsigned long takeMilliseconds();
		
		/** Delay for interrupts
			@param milliseconds		Delay time in milliseconds
		 */
		void delayInsideInterrupt(unsigned long milliseconds);

};

extern ProMake_GSM_M66_ModemCore theProMakeGsmM66ModemCore;

#endif
