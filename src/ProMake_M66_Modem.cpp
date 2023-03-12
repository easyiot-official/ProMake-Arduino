
#include "ProMake_M66_Modem.h"

ProMake_M66_Modem::ProMake_M66_Modem(HardwareSerial &serial, bool debug) : m_serial(serial), cb(this)
{
  _commandError = CMD_OK;
  _commandCounter = 0;
  _ongoingCommand = NONE;

  for (int i = 0; i < UMPROVIDERS; i++)
    _UMProvider[i] = 0;
}

void ProMake_M66_Modem::registerUMProvider(ProMakeGsmProviderBase *provider)
{
  for (int i = 0; i < UMPROVIDERS; i++)
  {
    if (_UMProvider[i] == 0)
    {
      _UMProvider[i] = provider;
      break;
    }
  }
}

void ProMake_M66_Modem::unRegisterUMProvider(ProMakeGsmProviderBase *provider)
{
  for (int i = 0; i < UMPROVIDERS; i++)
  {
    if (_UMProvider[i] == provider)
    {
      _UMProvider[i] = 0;
      break;
    }
  }
}

unsigned long ProMake_M66_Modem::takeMilliseconds()
{
  unsigned long now=millis();
  unsigned long delta;
  delta=now-_milliseconds;
  _milliseconds=now;
  return delta;
}

void ProMake_M66_Modem::openCommand(ProMakeGsmProviderBase *provider, ProMake_GSM_commandType_e c)
{
  _activeProvider = provider;
  _commandError = CMD_ONGOING;
  _commandCounter = 1;
  _ongoingCommand = c;
  //_dataInBufferFrom=0;
  //_dataInBufferTo=0;
};

void ProMake_M66_Modem::closeCommand(ProMake_GSM_CommandError_t code)
{
  // If we were configuring the modem,
  // and there's been an error
  // we don't know exactly where we are
  if ((code != CMD_OK) && (_ongoingCommand == MODEMCONFIG))
    _status = ERROR;

  _commandError = code;
  _ongoingCommand = NONE;
  _activeProvider = 0;
  _commandCounter = 1;
}

// Response parse.
bool ProMake_M66_Modem::genericParse_rsp(bool &rsp, const char *string, const char *string2)
{
  if ((string == 0) && (string2 == 0))
    string = "OK";

  rsp = cb.locate(string);

  if ((!rsp) && (string2 != 0))
    rsp = cb.locate(string2);

  return true;
}

// Generic command (const string).
void ProMake_M66_Modem::genericCommand_rqc(const char *str, bool addCR)
{
  cb.flush();
  m_serial.print(str);
  Serial.println(str);
  if (addCR)
    m_serial.print("\r");
}

void ProMake_M66_Modem::recv()
{
  while (m_serial.available())
  {
    cb.write(m_serial.read());
  }
  parseResp();
  return;
}

void ProMake_M66_Modem::parseResp()
{
  cb.debugBuffer();
  bool recognized = false;

  for (int i = 0; (i < UMPROVIDERS) && (!recognized); i++)
  {
    if (_UMProvider[i])
      recognized = _UMProvider[i]->recognizeUnsolicitedEvent(cb.getHead());
  }
  if ((!recognized) && (_activeProvider))
    _activeProvider->manageResponse(cb.getHead(), cb.getTail());
}


size_t ProMake_M66_Modem::write(uint8_t c)
{
	//if(_debug)
	//	GSM3CircularBuffer::printCharDebug(c);
  Serial.write(c);
	return m_serial.write(c);
}
int ProMake_M66_Modem::getICCID(char *iccid)
{
  return 0;
}
