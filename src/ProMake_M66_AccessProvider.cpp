#include "ProMake_M66_AccessProvider.h"
#include "ProMake_GSM_Modem.h"
#include <ProMake_debug.h>

#define __TOUTSHUTDOWN__ 5000
#define __TOUTMODEMCONFIGURATION__ 20000
#define __TOUTAT__ 1000

ProMakeM66AccessProvider::ProMakeM66AccessProvider(ProMake_GSM_Modem *Modem, bool debug) : ProMakeGsmProviderBase(Modem)
{
  //_theProMakeGsmModem->setDebug(debug);
  _theProMakeGsmModem->registerUMProvider(this);
  retryCnt = 0;
}

ProMake_GSM_NetworkStatus_t ProMakeM66AccessProvider::getStatus() { return _theProMakeGsmModem->getStatus(); };

ProMake_GSM_NetworkStatus_t ProMakeM66AccessProvider::begin(int8_t pwrPin, char *pin, bool restart, bool synchronous)
{
  // If asked for modem restart, restart
  PROMAKE_LOGDEBUG("Powering ON ...");
  pinMode(pwrPin, OUTPUT);
  // GSM POWER ON
  digitalWrite(pwrPin, LOW);
  delay(1000);
  digitalWrite(pwrPin, HIGH);
  delay(10000);

  _theProMakeGsmModem->setStatus(NET_STATUS_IDLE);
  PROMAKE_LOGDEBUG("Powered ON");

  //
  _pin = pin;
  _theProMakeGsmModem->openCommand(this, MODEMCONFIG);
  _theProMakeGsmModem->setStatus(NET_STATUS_CONNECTING);
  ModemConfigurationContinue();

  if (synchronous)
  {
    // if we shorten this delay, the command fails
    unsigned long timeOut = millis();
    while (((millis() - timeOut) < __TOUTMODEMCONFIGURATION__) & (ready() == 0))
      delay(100);
  }
  return _theProMakeGsmModem->getStatus();
}

void ProMakeM66AccessProvider::manageResponse(byte from, byte to)
{
  switch (_theProMakeGsmModem->getOngoingCommand())
  {
  case MODEMCONFIG:
    ModemConfigurationContinue();
    break;
  case ALIVETEST:
    isModemAliveContinue();
    break;
  case SIGNALQUALITY:
    GetSignalQualityContinue();
  }
}
// Initial configuration continue function.
void ProMakeM66AccessProvider::ModemConfigurationContinue()
{
  bool resp;
  // 1: Send AT
  // 2: Wait AT OK and SetPin or CGREG
  int ct = _theProMakeGsmModem->getCommandCounter();
  if (ct == 1)
  {
    _theProMakeGsmModem->genericCommand_rqc("AT");
    _theProMakeGsmModem->takeMilliseconds();
    _theProMakeGsmModem->setCommandCounter(2);
  }
  else if (ct == 2)
  {
    // Wait for AT - OK.
    if (_theProMakeGsmModem->genericParse_rsp(resp))
    {
      if (resp)
      {
        // OK received
        PROMAKE_LOGDEBUG("Modem is responding");
        if (_pin && (_pin[0] != 0))
        {
          _theProMakeGsmModem->genericCommand_rqc("AT+CPIN=", false);
          _theProMakeGsmModem->print(_pin);
          _theProMakeGsmModem->print('\r');
          _theProMakeGsmModem->setCommandCounter(3);
        }
        else
        {
          PROMAKE_LOGDEBUG("AT+CGREG?");
          _theProMakeGsmModem->setCommandCounter(4);
          _theProMakeGsmModem->takeMilliseconds();
          retryCnt = 10;
          _theProMakeGsmModem->genericCommand_rqc("AT+CGREG?");
        }
      }
      else
      {
        if (_theProMakeGsmModem->takeMilliseconds() > __TOUTMODEMCONFIGURATION__)
        {
          PROMAKE_LOGDEBUG("Error - No Response from modem!");
          _theProMakeGsmModem->closeCommand(CMD_UNEXP);
        }
      }
    }
  }
  else if (ct == 3)
  {
    if (_theProMakeGsmModem->genericParse_rsp(resp))
    {
      if (resp)
      {
        _theProMakeGsmModem->setCommandCounter(4);
        _theProMakeGsmModem->takeMilliseconds();
        delay(2000);
        retryCnt = 10;
        _theProMakeGsmModem->genericCommand_rqc("AT+CGREG?");
      }
      else
        _theProMakeGsmModem->closeCommand(CMD_UNEXP);
    }
  }
  else if (ct == 4)
  {
    if (_theProMakeGsmModem->genericParse_rsp(resp, "+CGREG: 0,1", "+CGREG: 0,5"))
    {
      if (resp)
      {
        _theProMakeGsmModem->setCommandCounter(5);
        _theProMakeGsmModem->genericCommand_rqc("AT+CMGF=1");
      }
      else
      {
        // If not, launch command again
        _theProMakeGsmModem->genericParse_rsp(resp, "+CGREG: 0,2");
        if (resp)
        {
          PROMAKE_LOGDEBUG("Modem is searching");
          delay(2000);
          _theProMakeGsmModem->takeMilliseconds();
          _theProMakeGsmModem->genericCommand_rqc("AT+CGREG?");
        }
        else
        {
          _theProMakeGsmModem->genericParse_rsp(resp, "+CGREG: 0,0");
          if (resp)
          {
            PROMAKE_LOGDEBUG("register failed");
            _theProMakeGsmModem->closeCommand(CMD_UNEXP);
          }
          else if (_theProMakeGsmModem->takeMilliseconds() > __TOUTMODEMCONFIGURATION__ || retryCnt < 0)
          {
            PROMAKE_LOGDEBUG("register timed out");
            _theProMakeGsmModem->closeCommand(CMD_UNEXP);
          }
          else
          {
            retryCnt--;
            delay(2000);
            _theProMakeGsmModem->takeMilliseconds();
            //_theProMakeGsmModem->genericCommand_rqc("AT+CGREG?");
          }
        }
      }
    }
  }
  else if (ct == 5)
  {
    // 7: Wait Calling Line Id OK
    if (_theProMakeGsmModem->genericParse_rsp(resp))
    {
      // Echo off
      _theProMakeGsmModem->setCommandCounter(7);
      _theProMakeGsmModem->genericCommand_rqc("ATE0");
    }
  }
  else if (ct == 7)
  {
    // 9: Wait ATCOLP OK
    if (_theProMakeGsmModem->genericParse_rsp(resp))
    {
      if (resp)
      {
        _theProMakeGsmModem->setStatus(NET_STATUS_GSM_READY);
        _theProMakeGsmModem->closeCommand(CMD_OK);
      }
      else if (_theProMakeGsmModem->takeMilliseconds() > __TOUTMODEMCONFIGURATION__)
      {
        _theProMakeGsmModem->closeCommand(CMD_UNEXP);
      }
    }
  }
}

// Alive Test main function.
ProMake_GSM_CommandError_t ProMakeM66AccessProvider::isAccessAlive()
{
  _theProMakeGsmModem->setCommandError(CMD_ONGOING);
  _theProMakeGsmModem->setCommandCounter(1);
  _theProMakeGsmModem->openCommand(this, ALIVETEST);
  isModemAliveContinue();
  return _theProMakeGsmModem->getCommandError();
}

// Alive Test continue function.
void ProMakeM66AccessProvider::isModemAliveContinue()
{
  /*
bool rsp;
switch _theProMakeGsmModem->getCommandCounter()) {
    case 1:
    _theProMakeGsmModem->genericCommand_rq(_command_AT);
    _theProMakeGsmModem->setCommandCounter(2);
      break;
  case 2:
    if(_theProMakeGsmModem->genericParse_rsp(rsp))
    {
      if (rsp) _theProMakeGsmModem->closeCommand(1);
      else _theProMakeGsmModem->closeCommand(3);
    }
      break;
  }
  */
}

ProMake_GSM_CommandError_t ProMakeM66AccessProvider::getSignalQuality(char *rssi, char *ber)
{
  _rssi = rssi;
  _ber = ber;
  _rssi[0] = 0;
  _ber[0] = 0;

  _theProMakeGsmModem->setCommandError(CMD_ONGOING);
  _theProMakeGsmModem->setCommandCounter(1);
  _theProMakeGsmModem->openCommand(this, SIGNALQUALITY);
  GetSignalQualityContinue();

  unsigned long timeOut = millis();
  while (((millis() - timeOut) < 5000) & (ready() == 0));

  return _theProMakeGsmModem->getCommandError();
}

void ProMakeM66AccessProvider::GetSignalQualityContinue()
{

  bool rsp;
  switch (_theProMakeGsmModem->getCommandCounter())
  {
  case 1:
    _theProMakeGsmModem->genericCommand_rqc("AT+CSQ");
    _theProMakeGsmModem->setCommandCounter(2);
    break;
  case 2:
    if (_theProMakeGsmModem->genericParse_rsp(rsp, "+CSQ: "))
    {
      if (rsp)
      {
        parseCSQ_available(rsp);
        _theProMakeGsmModem->closeCommand(CMD_OK);
      }
      else
        _theProMakeGsmModem->closeCommand(CMD_UNEXP);
    }
    break;
  }
}

bool ProMakeM66AccessProvider::parseCSQ_available(bool &rsp)
{
  char c;
  char quality[50] = "";
  int i = 0;
  if (!(_theProMakeGsmModem->theBuffer().chopUntil("+CSQ: ", true)))
    rsp = false;
  else
    rsp = true;

  while (((c = _theProMakeGsmModem->theBuffer().read()) != 0) & (i < 6))
  {
    if ((c < 58) & (c > 47) || c == ',')
    {
      quality[i] = c;
      i++;
    }
  }
  quality[i] = 0;

  char *res_tok = strtok(quality, ",");
  strcpy(_rssi, res_tok);
  res_tok = strtok(NULL, ",");
  strcpy(_ber, res_tok);
  return true;
}

///////////////////////////////////////////////////////UNSOLICITED RESULT CODE (URC) FUNCTIONS///////////////////////////////////////////////////////////////////

// URC recognize.
bool ProMakeM66AccessProvider::recognizeUnsolicitedEvent(byte oldTail)
{

  if (_theProMakeGsmModem->theBuffer().locate("NORMAL POWER DOWN"))
  {
    //_theProMakeGsmModem->theBuffer().flush();
    //delay(5000);
    return true;
  }

  return false;
}