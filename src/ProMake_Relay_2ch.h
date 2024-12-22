#ifndef PROMAKE_RELAY_2CH_H_INCLUDED
#define PROMAKE_RELAY_2CH_H_INCLUDED

#include <ProMake.h>
namespace ProMake
{

	class Relay_2ch
	{
	public:
		Relay_2ch();
		~Relay_2ch();
		void init(enuSlot slot);
		void SetRelay1State(bool on);
		void SetRelay2State(bool on);

	private:
		int m_relay1_pin;
		int m_relay2_pin;
	};

}
#endif