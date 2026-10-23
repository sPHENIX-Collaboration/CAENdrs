
#include <iostream>

#include <daq_device_CAENdrs.h>
#include <string.h>

#include <CAENDigitizer.h>
#include <CAENDigitizerType.h>
#include <CAENdrs_handles.h>

#define VME_INTERRUPT_LEVEL      1
#define VME_INTERRUPT_STATUS_ID  0xAAAA
#define INTERRUPT_MODE           CAEN_DGTZ_IRQ_MODE_ROAK


using namespace std;

daq_device_CAENdrs::daq_device_CAENdrs(const int eventtype
				       , const int subeventid
				       , const int linknumber
				       , const int nodenumber
				       , const int trigger
				       , const int endpulse)
  : daq_device_CAENDigitizer (eventtype,subeventid,linknumber,nodenumber,trigger,endpulse)
{

  cout << "*************** opening Digitizer" << endl;
  _broken = CAEN_DGTZ_OpenDigitizer( CAEN_DGTZ_OpticalLink, _linknumber , _nodenumber, 0 ,&handle);
  if ( ! _broken) caen_register_handle(0, _linknumber, _nodenumber, handle);
  //  _broken = CAEN_DGTZ_OpenDigitizer( CAEN_DGTZ_USB, _linknumber , _nodenumber, 0 ,&handle);
  cout << "*************** " << _broken  << endl;


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
      cout << __LINE__ << "  " << __FILE__ << " registering triggerhandler " << endl;
      _th  = new CAENdrsTriggerHandler (handle, m_eventType, max_length(m_eventType),0);
      registerTriggerHandler ( _th);
    }
  else
    {
      _th = 0;
    }

}


daq_device_CAENdrs::~daq_device_CAENdrs()
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
  caen_unregister_handle(handle);
  CAEN_DGTZ_CloseDigitizer(handle);

}



int  daq_device_CAENdrs::init()
{
  
  if ( _broken ) 
    {
      
      return 0; //  we had a catastrophic failure
    } 

  // configure interrupts and trigger
  _broken |= CAEN_DGTZ_SetInterruptConfig( handle, CAEN_DGTZ_ENABLE,
                                           VME_INTERRUPT_LEVEL, VME_INTERRUPT_STATUS_ID,
                                           1, INTERRUPT_MODE);
  if ( _broken )
    {
      cout << __FILE__ << " " <<  __LINE__ << " Error: " << _broken << endl;
      return 0;
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
int  daq_device_CAENdrs::rearm(const int etype)
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

int daq_device_CAENdrs::endrun()
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

int daq_device_CAENdrs::max_length(const int etype) const
{
  if (etype != m_eventType) return 0;
  return  (14900);
}


void daq_device_CAENdrs::identify(std::ostream& os) const
{

  CAEN_DGTZ_BoardInfo_t       BoardInfo;


  if ( _broken) 
    {
      os << "CAEN Digitizer Event Type: " << m_eventType 
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
	 << " Subevent id: " << m_subeventid 
	 << " Firmware "     << BoardInfo.ROC_FirmwareRel << " / " << BoardInfo.AMC_FirmwareRel 
	 << " speed "  << getGS() <<  "GS"
	 << " delay "  << getDelay() <<  "% ";
      if (_trigger_handler) os << " *Trigger" ;
      if (_endpulse) os << " *endpulse" ;
      if (_warning) os << " **** warning - check setup parameters ****";
      os << endl;

    }
}



