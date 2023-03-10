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
#ifndef __ProMake_GSM_M66__
#define __ProMake_GSM_M66__

#include <GSM/ProMake_GSM_MobileNetworkProvider.h>
#include <ProMake_GSM_M66_ModemCore.h>
#include <ProMake_GSM_M66_BaseProvider.h>
#include <Arduino.h>


class ProMake_GSM_M66 : public ProMake_GSM_MobileNetworkProvider, public ProMake_GSM_M66_BaseProvider
{
	// General code, for modem management
	private:
		
	// Code for SMS Service
	private:


		long commandMillis;
		bool commandSent;
		
		const char* pinConfig;		//PIN.
		char* accessPoint;			//APN.					
		char* userName;				//User.
		char* passw;				//Password.
		const char* remoteID;		//Server.
		
		char* dataSocket;			//Data socket.
		int local_Port;				//Local Port.
		char* local_IP;				//Local IP.
		int local_IP_Length;		//Local IP length.
				
		
		int socketDataSize; 		//Size of socket data to be read.
		int socketDataSizeWritten;  //Number of socket data written in buffer not to overflow the buffer
				
		int socketsAccepted;		//Status for remote clients accepted of closed.
			
	public:
	
		/** Constructor **/
		ProMake_GSM_M66(bool debug=false);

		/** Manages modem response
			@param from 		Initial byte of buffer
			@param to 			Final byte of buffer
		 */
		void manageResponse(byte from, byte to);
		
		/** Get last command status
			@return returns 0 if last command is still executing, 1 success, >1 error
		*/
		int ready(){return ProMake_GSM_M66_BaseProvider::ready();};
		
		/** Parse modem response
			@param rsp			Returns true if expected response exists
			@param string1		Substring expected in response
			@param string2		Second substring expected in response
			@return true if parsed successful
		*/
		bool genericParse_rsp2(bool& rsp, char* string1, char* string2);
		
		/** Recognize URC
			@param oldTail		
			@return true if successful
		*/		
		bool recognizeUnsolicitedEvent(byte oldTail);
		
		/** Receive answer
			@return true if successful
		*/		
		bool answerReceived();
		
		/** Receive socket
			@param id_socket	Socket ID
			@return true if successful
		*/		
		bool socketReceived(int id_socket);
		
		/** Update active ID sockets
			@param active		Active sockets
			@param ID			Id for update
		*/		
		void update_activeIDsockets (bool active, int ID);
		
		/** Assign ID to socket
			@param ID			Id to assign to socket
			@return true if successful
		*/		
		bool assignIDsocket (int& ID);
		
		/** Close data socket
			@return true if successful
		*/		
		bool closedDataSocket(); //Flag closed current data socket. 
		
		//bool writeIncomingCalls(char* bufferForCallerId) If isn't zero, doesn't wait calls
};

#endif