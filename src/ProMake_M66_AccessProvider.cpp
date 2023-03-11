#include "ProMake_M66_AccessProvider.h"
#include "ProMake_M66_Modem.h"

#define __TOUTSHUTDOWN__ 5000
#define __TOUTMODEMCONFIGURATION__ 5000//equivalent to 30000 because of time in interrupt routine.
#define __TOUTAT__ 1000

ProMakeM66AccessProvider::ProMakeM66AccessProvider(ProMake_M66_Modem *Modem, bool debug) : _theProMakeM66Modem(Modem)
{
    //_theProMakeM66Modem->setDebug(debug);
    retryCnt=0;
}

ProMake_GSM_NetworkStatus_t ProMakeM66AccessProvider::getStatus() { return _theProMakeM66Modem->getStatus(); };

ProMake_GSM_NetworkStatus_t ProMakeM66AccessProvider::begin(char *pin, bool restart, bool synchronous)
{
    // If asked for modem restart, restart

    //
    _theProMakeM66Modem->setParam(pin);
    _theProMakeM66Modem->openCommand(this, MODEMCONFIG);
    _theProMakeM66Modem->setStatus(CONNECTING);

    // _theProMakeM66Modem->m_serial.println("ATE0");
    ModemConfigurationContinue();
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
                if (_theProMakeM66Modem->getParam() && (_theProMakeM66Modem->getParam()[0] != 0))
                {
                    _theProMakeM66Modem->genericCommand_rqc("AT+CPIN=");
                    _theProMakeM66Modem->setCommandCounter(3);
                    _theProMakeM66Modem->genericCommand_rqc(_theProMakeM66Modem->getParam());
                }
                else
                {
                    // DEBUG
                    // Serial.println("AT+CGREG?");
                    _theProMakeM66Modem->setCommandCounter(4);
                    _theProMakeM66Modem->takeMilliseconds();
                    retryCnt=5;
                    _theProMakeM66Modem->genericCommand_rqc("AT+CGREG?");
                }
            }
            else
            {
                Serial.println("init failed ...");
                _theProMakeM66Modem->closeCommand(CMD_ERROR);
            }
        }
    }
    else if(ct==3)
  {
    if(_theProMakeM66Modem->genericParse_rsp(resp))
      {
      if(resp)
      {
        _theProMakeM66Modem->setCommandCounter(4);
        _theProMakeM66Modem->takeMilliseconds();
        retryCnt=5;
        _theProMakeM66Modem->genericCommand_rqc("AT+CGREG?");
      }
      else _theProMakeM66Modem->closeCommand(CMD_ERROR);
      }
  }
  else if(ct==4)
  {
    if(_theProMakeM66Modem->genericParse_rsp(resp, "+CGREG: 0,1", "+CGREG: 0,5"))
    {
      if(resp)
      {
        _theProMakeM66Modem->setCommandCounter(5);
        _theProMakeM66Modem->genericCommand_rqc("AT+IFC=1,1");
      }
      else
      {
        // If not, launch command again
        if(_theProMakeM66Modem->takeMilliseconds() > __TOUTMODEMCONFIGURATION__ || retryCnt<0)
        {
          _theProMakeM66Modem->closeCommand(CMD_ERROR);
        }
        else 
        {
          retryCnt--;
          _theProMakeM66Modem->genericCommand_rqc("AT+CGREG?");
        }
      }
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
