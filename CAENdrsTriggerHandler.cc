
#include "CAENdrsTriggerHandler.h"
#include <iostream>

#include <string.h>

#include <CAENDigitizer.h>
#include <CAENDigitizerType.h>

using namespace std;


int CAENdrsTriggerHandler::wait_for_trigger( const int moreinfo)
{

  //cout << __FILE__ << " " << __LINE__ << " entering wait_for_trigger" << endl;

  const int timeout = 1;

  uint32_t buffersize = 0;
  uint32_t count = 0;
  
  if (_is_USB) // do a polling loop
    {
      while ( buffersize == 0 && count++ < 30000) // we make 50K polls before we make an "idle cycle" 
	{
	  // Read data from the board 
	  int status = CAEN_DGTZ_ReadData(_handle, CAEN_DGTZ_SLAVE_TERMINATED_READOUT_MBLT, (char *) _buffer, &buffersize);
	  if ( status )
	    {
	      cout << __FILE__ << " " << __LINE__ << " read status error = " <<  status << endl;
	      return 0;
	    }
	}

      _current_size = buffersize;
      //cout << __FILE__ << " " << __LINE__ << " buffersize = " << buffersize ;
      if ( buffersize)
	{
	  //cout << __FILE__ << " " << __LINE__ << " returning etpye " << _etype << endl; ;
	  return _etype;
	}
      //cout << endl;
      return 0;
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

int CAENdrsTriggerHandler::fill_data(int * adr, const int size)
{
  if ( _current_size >= 4*size)
    {
      cout << __LINE__ << "  " << __FILE__ << "data size too large  " << _current_size << " max is " <<  4*size  << endl;
      return 0;
    }
  memcpy ( adr, _buffer, _current_size);
  return _current_size;
}
