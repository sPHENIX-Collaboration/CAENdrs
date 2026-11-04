
#include <iostream>

#include <daq_device_CAENPulse.h>
#include <string.h>

#include <CAENDigitizer.h>
#include <CAENDigitizerType.h>
#include <CAENdrs_handles.h>

#define VME_INTERRUPT_LEVEL      1
#define VME_INTERRUPT_STATUS_ID  0xAAAA
#define INTERRUPT_MODE           CAEN_DGTZ_IRQ_MODE_ROAK


using namespace std;

daq_device_CAENPulse::daq_device_CAENPulse(const int eventtype
					   , const int subeventid
					   , const int linknumber)

{

  m_eventType  = eventtype;

  _linknumber = linknumber;

  handle = 0;
  _own_handle = 0;

  _warning = 0;
  _broken = 0;

  // we don't open the digitizer here. A second open of a digitizer that a
  // readout device has open conflicts with it, so we look for that device's
  // handle when we need it, see get_handle().
}

int daq_device_CAENPulse::get_handle()
{
  if ( _own_handle) return 0;

  const int node = 0;
  if ( caen_find_handle(0, _linknumber, node, handle) ) return 0;

  // no readout device has it open, so we open it ourselves, and keep it
  _broken = CAEN_DGTZ_OpenDigitizer( CAEN_DGTZ_OpticalLink, _linknumber , node, 0 ,&handle);
  if ( _broken )
    {
      cout << " Error in CAEN_DGTZ_OpenDigitizer " << _broken << endl;
      return _broken;
    }
  _own_handle = 1;
  return 0;
}

daq_device_CAENPulse::~daq_device_CAENPulse()
{


  if ( _own_handle) CAEN_DGTZ_CloseDigitizer(handle);

}



// the put_data function

int daq_device_CAENPulse::put_data(const int etype, int * adr, const int length )
{

  if ( _broken ) 
    {
      //      cout << __LINE__ << "  " << __FILE__ << " broken ";
      //      identify();
      return 0; //  we had a catastrophic failure
    }

  if (etype != m_eventType )  // not our id
    {
      return 0;
    }

  if ( get_handle() ) return 0;

  const unsigned int reg    = 0x811C;
  const unsigned int pulseon  = 0xC000;
  const unsigned int pulseoff = 0x8000;

  _broken = CAEN_DGTZ_WriteRegister(handle, reg, pulseon);
  _broken = CAEN_DGTZ_WriteRegister(handle, reg, pulseoff);

  return 0;
}


void daq_device_CAENPulse::identify(std::ostream& os) const
{
  if ( _broken) 
    {
      os << "CAEN Pulse at Event Type: " << m_eventType 
	 << " link " << _linknumber << " ** not functional ** " << endl;
    }
  else
    {
      os << "CAEN Digitizer Pulse at Event Type: " << m_eventType 
	 << " link " << _linknumber << endl;
    }
}
