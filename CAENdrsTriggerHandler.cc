
#include "CAENdrsTriggerHandler.h"
#include <iostream>

#include <CAENDigitizer.h>
#include <CAENDigitizerType.h>

using namespace std;


int CAENdrsTriggerHandler::wait_for_trigger( const int moreinfo)
{
  const int timeout = 1;

  if (_is_USB ==1) // do a polling loop for USB
    {
      int count=50000;
      unsigned int  ev = 0;
      int ret = CAEN_DGTZ_ReadRegister(_handle, CAEN_DGTZ_EVENT_STORED_ADD, &ev);
      while (!ev)
	{
	  ret = CAEN_DGTZ_ReadRegister(_handle, CAEN_DGTZ_EVENT_STORED_ADD, &ev);
	  if ( count-- <= 0) return 0;
	}
      cout << __LINE__ << "  " << __FILE__ << " register read  " << ret << " ev =" << ev  << endl;
      return _etype;
    }

  if (_is_USB == 2) // do a polling loop for CONET
    {
      int count=5000;
      unsigned int  ev = 0;
      int ret = CAEN_DGTZ_ReadRegister(_handle, CAEN_DGTZ_ACQ_STATUS_ADD, &ev);
      while (! ( ev & 0x8) )
	{
	  //cout << __LINE__ << "  " << __FILE__ << " register read  " << ret << " ev =0x" << hex << ev << dec << endl;
	  ret = CAEN_DGTZ_ReadRegister(_handle, CAEN_DGTZ_ACQ_STATUS_ADD, &ev);
	  if ( count-- <= 0) return 0;
	  usleep(100);
	}
      //cout << __LINE__ << "  " << __FILE__ << " register read  " << ret << " ev =" << ev  << endl;
      return _etype;
    }

  else
    {
      
      int ret = CAEN_DGTZ_IRQWait(_handle, timeout);

      if (ret == CAEN_DGTZ_Timeout)
	{
	  usleep(100);
	  
	  return 0;
	}
      
      if (ret != CAEN_DGTZ_Success)  
	{
	  //cout << __LINE__ << "  " << __FILE__ << " Error from IRQwait " << ret  << endl;
	  return 0;
	}

      return _etype;
    }
  
  return 0;
}
