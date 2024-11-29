#include "ProMake_GSM_ProviderBase.h"
#include "ProMake_GSM_Modem.h"

ProMakeGsmProviderBase::ProMakeGsmProviderBase(ProMake_GSM_Modem *Modem) : _theProMakeGsmModem(Modem)
{
}

// Returns 0 if last command is still executing
// 1 if success
// >1 if error
int ProMakeGsmProviderBase::ready()
{
	//_theProMakeGsmModem->manageReceivedData();

	return _theProMakeGsmModem->getCommandError();
};