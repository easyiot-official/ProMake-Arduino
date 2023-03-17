#include "ProMake_GSM_ProviderBase.h"
#include "ProMake_M66_Modem.h"

ProMakeGsmProviderBase::ProMakeGsmProviderBase(ProMake_M66_Modem *Modem) : _theProMakeM66Modem(Modem)
{
}

// Returns 0 if last command is still executing
// 1 if success
// >1 if error
int ProMakeGsmProviderBase::ready()
{
	//_theProMakeM66Modem->manageReceivedData();

	return _theProMakeM66Modem->getCommandError();
};