#include "ProMake_M66_AccessProvider.h"
#include "ProMake_M66_Modem.h"

#define __TOUTSHUTDOWN__ 5000
#define __TOUTMODEMCONFIGURATION__ 5000 // equivalent to 30000 because of time in interrupt routine.
#define __TOUTAT__ 1000

ProMakeM66AccessProvider::ProMakeM66AccessProvider(ProMake_M66_Modem *Modem, bool debug) : ProMakeGsmProviderBase(Modem)
{
  //_theProMakeM66Modem->setDebug(debug);
  _theProMakeM66Modem->registerUMProvider(this);
  retryCnt = 0;
}

ProMake_GSM_NetworkStatus_t ProMakeM66AccessProvider::getStatus() { return _theProMakeM66Modem->getStatus(); };

ProMake_GSM_NetworkStatus_t ProMakeM66AccessProvider::begin(int8_t pwrPin, char *pin, bool restart, bool synchronous)
{
  // If asked for modem restart, restart
  Serial.println("Powering ON ...");
  pinMode(pwrPin, OUTPUT);
  // GSM POWER ON
  digitalWrite(pwrPin, LOW);
  delay(1000);
  digitalWrite(pwrPin, HIGH);
  delay(10000);

  _theProMakeM66Modem->setStatus(IDLE);
  Serial.println("Powered ON");
 
  //
  _pin = pin;
  _theProMakeM66Modem->openCommand(this, MODEMCONFIG);
  _theProMakeM66Modem->setStatus(CONNECTING);
  ModemConfigurationContinue();

  if (synchronous)
  {
    // if we shorten this delay, the command fails
    while (ready() == 0)
      delay(1000);
  }
  return _theProMakeM66Modem->getStatus();
}

void ProMakeM66AccessProvider::manageResponse(byte from, byte to)
{
  switch (_theProMakeM66Modem->getOngoingCommand())
  {
  case MODEMCONFIG:
    ModemConfigurationContinue();
    break;
  case ALIVETEST:
    isModemAliveContinue();
    break;
  }
}
// Initial configuration continue function.
void ProMakeM66AccessProvider::ModemConfigurationContinue()
{
  bool resp;
  // 1: Send AT
  // 2: Wait AT OK and SetPin or CGREG
  int ct = _theProMakeM66Modem->getCommandCounter();
  if (ct == 1)
  {
    _theProMakeM66Modem->setCommandCounter(2);
    _theProMakeM66Modem->genericCommand_rqc("AT");
  }
  else if (ct == 2)
  {
    // Wait for AT - OK.
    if (_theProMakeM66Modem->genericParse_rsp(resp))
    {
      if (resp)
      {
        // OK received
        Serial.println("Init done ...");
        if (_pin && (_pin[0] != 0))
        {
          _theProMakeM66Modem->genericCommand_rqc("AT+CPIN=", false);
          _theProMakeM66Modem->print(_pin);
          _theProMakeM66Modem->print('\r');
          _theProMakeM66Modem->setCommandCounter(3);
        }
        else
        {
          // DEBUG
          // Serial.println("AT+CGREG?");
          _theProMakeM66Modem->setCommandCounter(4);
          _theProMakeM66Modem->takeMilliseconds();
          retryCnt = 5;
          _theProMakeM66Modem->genericCommand_rqc("AT+CGREG?");
        }
      }
      else
      {
        Serial.println("init failed ...");
        _theProMakeM66Modem->closeCommand(CMD_UNEXP);
      }
    }
  }
  else if (ct == 3)
  {
    if (_theProMakeM66Modem->genericParse_rsp(resp))
    {
      if (resp)
      {
        _theProMakeM66Modem->setCommandCounter(4);
        _theProMakeM66Modem->takeMilliseconds();
        delay(2000);
        retryCnt = 5;
        _theProMakeM66Modem->genericCommand_rqc("AT+CGREG?");
      }
      else
        _theProMakeM66Modem->closeCommand(CMD_UNEXP);
    }
  }
  else if (ct == 4)
  {
    if (_theProMakeM66Modem->genericParse_rsp(resp, "+CGREG: 0,1", "+CGREG: 0,5"))
    {
      if (resp)
      {
        _theProMakeM66Modem->setCommandCounter(5);
        _theProMakeM66Modem->genericCommand_rqc("AT+CMGF=1");
      }
      else
      {
        // If not, launch command again
        if (_theProMakeM66Modem->takeMilliseconds() > __TOUTMODEMCONFIGURATION__ || retryCnt < 0)
        {
          _theProMakeM66Modem->closeCommand(CMD_UNEXP);
        }
        else
        {
          retryCnt--;
          delay(2000);
          _theProMakeM66Modem->genericCommand_rqc("AT+CGREG?");
        }
      }
    }
  }
  else if (ct == 5)
  {
    // 7: Wait Calling Line Id OK
    if (_theProMakeM66Modem->genericParse_rsp(resp))
    {
      // Echo off
      _theProMakeM66Modem->setCommandCounter(7);
      _theProMakeM66Modem->genericCommand_rqc("ATE0");
    }
  }
  else if (ct == 7)
  {
    // 9: Wait ATCOLP OK
    if (_theProMakeM66Modem->genericParse_rsp(resp))
    {
      if (resp)
      {
        _theProMakeM66Modem->setStatus(GSM_READY);
        _theProMakeM66Modem->closeCommand(CMD_OK);
      }
      else
        _theProMakeM66Modem->closeCommand(CMD_UNEXP);
    }
  }
}

// Alive Test main function.
int ProMakeM66AccessProvider::isAccessAlive()
{
  _theProMakeM66Modem->setCommandError(CMD_ONGOING);
  _theProMakeM66Modem->setCommandCounter(1);
  _theProMakeM66Modem->openCommand(this, ALIVETEST);
  isModemAliveContinue();
  return _theProMakeM66Modem->getCommandError();
}

// Alive Test continue function.
void ProMakeM66AccessProvider::isModemAliveContinue()
{
  /*
bool rsp;
switch _theProMakeM66Modem->getCommandCounter()) {
    case 1:
    _theProMakeM66Modem->genericCommand_rq(_command_AT);
    _theProMakeM66Modem->setCommandCounter(2);
      break;
  case 2:
    if(_theProMakeM66Modem->genericParse_rsp(rsp))
    {
      if (rsp) _theProMakeM66Modem->closeCommand(1);
      else _theProMakeM66Modem->closeCommand(3);
    }
      break;
  }
  */
}

///////////////////////////////////////////////////////UNSOLICITED RESULT CODE (URC) FUNCTIONS///////////////////////////////////////////////////////////////////

// URC recognize.
bool ProMakeM66AccessProvider::recognizeUnsolicitedEvent(byte oldTail)
{

  if (_theProMakeM66Modem->theBuffer().locate("POWER DOWN"))
  {
    Serial.println("ProMakeM66AccessProvider::recognizeUnsolicitedEvent");
    _theProMakeM66Modem->theBuffer().flush();
    return true;
  }

  return false;
}