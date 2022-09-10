#ifndef __PROMAKE_H__
#define __PROMAKE_H__



#ifdef PROMAKE_DEBUG_OUTPUT
  #define PROMAKE_LIB_DEBUG_OUTPUT      PROMAKE_DEBUG_OUTPUT
  #warning Use Serial for debug
#else
  #define PROMAKE_LIB_DEBUG_OUTPUT      Serial
#endif

// Change _PROMAKE_LOGLEVEL_ to set tracing and logging verbosity
// 0: DISABLED: no logging
// 1: ERROR: errors
// 2: WARN: errors and warnings
// 3: INFO: errors, warnings and informational (default)
// 4: DEBUG: errors, warnings, informational and debug

#ifndef _PROMAKE_LOGLEVEL_
  #define _PROMAKE_LOGLEVEL_       0
#endif

#define PROMAKE_DEBUG_PRINT        PROMAKE_LIB_DEBUG_OUTPUT.print
#define PROMAKE_DEBUG_PRINTLN      PROMAKE_LIB_DEBUG_OUTPUT.println

const char AT_LIB_MARK[] = "[ProMake] ";
const char AT_LIB_SP[]   = " ";

#define PROMAKE_DEBUG_PRINT_MARK   PROMAKE_DEBUG_PRINT(AT_LIB_MARK)
#define PROMAKE_DEBUG_PRINT_SP     PROMAKE_DEBUG_PRINT(AT_LIB_SP)

#if _PROMAKE_LOGLEVEL_ > 3
//////////////////////////////////////////////

#define PROMAKE_LOGERROR(x)    if(_ESP_AT_LIB_LOGLEVEL_>0) { PROMAKE_DEBUG_PRINT_MARK; PROMAKE_DEBUG_PRINTLN(x); }
#define PROMAKE_LOGERROR1(x,y) if(_ESP_AT_LIB_LOGLEVEL_>0) { PROMAKE_DEBUG_PRINT_MARK; PROMAKE_DEBUG_PRINT(x); PROMAKE_DEBUG_PRINT_SP; PROMAKE_DEBUG_PRINTLN(y); }

//////////////////////////////////////////////

#define PROMAKE_LOGWARN(x)     if(_ESP_AT_LIB_LOGLEVEL_>1) { PROMAKE_DEBUG_PRINT_MARK; PROMAKE_DEBUG_PRINTLN(x); }
#define PROMAKE_LOGWARN1(x,y)  if(_ESP_AT_LIB_LOGLEVEL_>1) { PROMAKE_DEBUG_PRINT_MARK; PROMAKE_DEBUG_PRINT(x); PROMAKE_DEBUG_PRINT_SP; PROMAKE_DEBUG_PRINTLN(y); }
#define PROMAKE_LOGINFO(x)     if(_ESP_AT_LIB_LOGLEVEL_>2) { PROMAKE_DEBUG_PRINT_MARK; PROMAKE_DEBUG_PRINTLN(x); }
#define PROMAKE_LOGINFO1(x,y)  if(_ESP_AT_LIB_LOGLEVEL_>2) { PROMAKE_DEBUG_PRINT_MARK; PROMAKE_DEBUG_PRINT(x); PROMAKE_DEBUG_PRINT_SP; PROMAKE_DEBUG_PRINTLN(y); }

//////////////////////////////////////////////

#define PROMAKE_LOGDEBUG(x)      if(_ESP_AT_LIB_LOGLEVEL_>3) { PROMAKE_DEBUG_PRINTLN(x); }
#define PROMAKE_LOGDEBUG0(x)     if(_ESP_AT_LIB_LOGLEVEL_>3) { PROMAKE_DEBUG_PRINT(x); }
#define PROMAKE_LOGDEBUG1(x,y)   if(_ESP_AT_LIB_LOGLEVEL_>3) { PROMAKE_DEBUG_PRINT(x); PROMAKE_DEBUG_PRINT_SP; PROMAKE_DEBUG_PRINTLN(y); }
#define PROMAKE_LOGDEBUG2(x,y,z) if(_ESP_AT_LIB_LOGLEVEL_>3) { PROMAKE_DEBUG_PRINT(x); PROMAKE_DEBUG_PRINT_SP; PROMAKE_DEBUG_PRINT(y); PROMAKE_DEBUG_PRINT_SP; PROMAKE_DEBUG_PRINTLN(z); }

#else if  _PROMAKE_LOGLEVEL_ > 2

#else if  _PROMAKE_LOGLEVEL_ > 1

#else
//////////////////////////////////////////////

#define PROMAKE_LOGERROR(x)    ;
#define PROMAKE_LOGERROR1(x,y) ;

//////////////////////////////////////////////

#define PROMAKE_LOGWARN(x)     ;
#define PROMAKE_LOGWARN1(x,y)  ;
#define PROMAKE_LOGINFO(x)     ;
#define PROMAKE_LOGINFO1(x,y)  ;

//////////////////////////////////////////////

#define PROMAKE_LOGDEBUG(x)      ;
#define PROMAKE_LOGDEBUG0(x)     ;
#define PROMAKE_LOGDEBUG1(x,y)   ;
#define PROMAKE_LOGDEBUG2(x,y,z) ;
#endif

enum
{
	ARDUINO_SHIELD_SLOT1,
	ARDUINO_SHIELD_SLOT2,
	ARDUINO_NANO_KIT_SLOT1,
	ARDUINO_NANO_KIT_SLOT2,
	ARDUINO_NANO_KIT_SLOT3
}enuProMakeSlot;

#endif