
#include <iostream>

#include <daq_device_CAEN_DT57xx.h>
#include <string.h>

#include <CAENDigitizer.h>
#include <CAENDigitizerType.h>

#define VME_INTERRUPT_LEVEL      1
#define VME_INTERRUPT_STATUS_ID  0xAAAA
#define INTERRUPT_MODE           CAEN_DGTZ_IRQ_MODE_ROAK


using namespace std;

daq_device_CAEN_DT57xx::daq_device_CAEN_DT57xx(const int eventtype
				       , const int subeventid
				       , const int CONET_Flag
				       , const int linknumber
				       , const int nodenumber
				       , const int trigger
				       , const int endpulse)
  : daq_device_CAENDigitizer (eventtype,subeventid,linknumber,nodenumber,trigger,endpulse)
{

  cout << eventtype << " "
       << subeventid << " " 
       << CONET_Flag << " " 
       << linknumber << " " 
       << nodenumber << " " 
       << trigger << endl;

    if (CONET_Flag)
    {
      _is_conet = 1;
      cout << "*************** opening Digitizer for conet with link number " << _linknumber << endl;
      //  _broken = CAEN_DGTZ_OpenDigitizer( CAEN_DGTZ_OpticalLink, _linknumber , _nodenumber, 0 ,&handle);
      _broken = CAEN_DGTZ_OpenDigitizer( CAEN_DGTZ_USB_A4818, _linknumber , _nodenumber, 0 ,&handle);
      cout << "*************** " << _broken  << endl;
    }
  else
    {
      _is_conet = 0;
      cout << "*************** opening Digitizer" << endl;
      //  _broken = CAEN_DGTZ_OpenDigitizer( CAEN_DGTZ_OpticalLink, _linknumber , _nodenumber, 0 ,&handle);
      _broken = CAEN_DGTZ_OpenDigitizer( CAEN_DGTZ_USB, _linknumber , _nodenumber, 0 ,&handle);
      cout << "*************** " << _broken  << endl;
    }
    

  _broken =  CAEN_DGTZ_Reset(handle);

  if ( _broken )
    {
      cout << " Error in CAEN_DGTZ_OpenDigitizer " << _broken << endl;
      return;
    }

  _trigger_handler=0;
  if (trigger)   _trigger_handler=1;
 
  if ( _trigger_handler )
    {
      if ( _is_conet)
	{
	  cout << __LINE__ << "  " << __FILE__ << " registering triggerhandler " << endl;
	  _th  = new CAENdrsTriggerHandler (handle, m_eventType, 2);
	}
      else
	{
	  cout << __LINE__ << "  " << __FILE__ << " registering triggerhandler " << endl;
	  _th  = new CAENdrsTriggerHandler (handle, m_eventType, 1);
	}
      registerTriggerHandler ( _th);
    }
  else
    {
      _th = 0;
    }

}


int daq_device_CAEN_DT57xx::max_length(const int etype) const
{
  if (etype != m_eventType) return 0;
  return  (14900/2);
}




daq_device_CAEN_DT57xx::~daq_device_CAEN_DT57xx()
{

  if (_th)
    {
      clearTriggerHandler();
      delete _th;
      _th = 0;
    }

  if ( _Event742) 
    {
      CAEN_DGTZ_FreeEvent(handle, (void**)&_Event742);
    }
  CAEN_DGTZ_CloseDigitizer(handle);

}



int  daq_device_CAEN_DT57xx::init()
{
  
  if ( _broken ) 
    {
      
      return 0; //  we had a catastrophic failure
    } 

  if ( ! _is_conet )
    {
      // configure interrupts and trigger
      _broken |= CAEN_DGTZ_SetInterruptConfig( handle, CAEN_DGTZ_ENABLE,
					       VME_INTERRUPT_LEVEL, VME_INTERRUPT_STATUS_ID,
					       1, INTERRUPT_MODE);
      if ( _broken )
	{
	  cout << __FILE__ << " " <<  __LINE__ << " Error: " << _broken << endl;
	  return 0;
	}
    }
  
  _broken = CAEN_DGTZ_SetAcquisitionMode(handle, CAEN_DGTZ_SW_CONTROLLED);
  if ( _broken )
    {
      cout << __FILE__ << " " <<  __LINE__ << " Error: " << _broken << endl;
      return 0;
    }


  if ( _Event742 )
    {
      _broken =  CAEN_DGTZ_AllocateEvent(handle, (void**)&_Event742);
      if ( _broken )
	{
	  cout << __FILE__ << " " <<  __LINE__ << " Error: " << _broken << endl;
	  return 0;
	}

    }

  // and we trigger rearm with our event type so it takes effect
  rearm (m_eventType);

  _broken = CAEN_DGTZ_SWStartAcquisition(handle);
  if ( _broken )
    {
      cout << __FILE__ << " " <<  __LINE__ << " Error: " << _broken << endl;
      return 0;
    }

  return 0;

}

// the rearm() function
int  daq_device_CAEN_DT57xx::rearm(const int etype)
{
  if ( _broken ) 
    {
      //      cout << __LINE__ << "  " << __FILE__ << " broken ";
      //      identify();
      return 0; //  we had a catastrophic failure
    }

  if (etype != m_eventType) return 0;

  return 0;
}

int daq_device_CAEN_DT57xx::endrun()
{
  if ( _broken ) 
    {
      return 0; //  we had a catastrophic failure
    }

  _broken = CAEN_DGTZ_SWStopAcquisition(handle);
  if ( _broken )
    {
      cout << __FILE__ << " " <<  __LINE__ << " Error: " << _broken << endl;
      
    }
  return _broken;

}


void daq_device_CAEN_DT57xx::identify(std::ostream& os) const
{

  CAEN_DGTZ_BoardInfo_t       BoardInfo;


  if ( _broken) 
    {
      os << "CAEN DT57xx Digitizer Event Type: " << m_eventType 
	 << " Subevent id: " << m_subeventid 
	 << " ** not functional ** " << endl;
    }
  else
    {
      
      int ret = CAEN_DGTZ_GetInfo(handle, &BoardInfo);
      if ( ret )
	{
	  cout << " Error in CAEN_DGTZ_GetInfo" << endl;
	  return;
	}
      
      os << "CAEN Digitizer Model " << BoardInfo.ModelName
	 << " Event Type: " << m_eventType 
	 << " Subevent id: " << m_subeventid; 
      if ( _is_conet) os << "reading via CONET";  
      os << " Firmware "     << BoardInfo.ROC_FirmwareRel << " / " << BoardInfo.AMC_FirmwareRel 
	 << " speed "  << getGS() <<  "GS"
	 << " delay "  << getDelay() <<  "% ";
      if (_trigger_handler) os << " *Trigger" ;
      if (_endpulse) os << " *endpulse" ;
      if (_warning) os << " **** warning - check setup parameters ****";
      os << endl;

    }
}



