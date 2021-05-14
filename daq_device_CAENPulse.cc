
#include <iostream>

#include <daq_device_CAENPulse.h>
#include <string.h>

#include <CAENDigitizer.h>
#include <CAENDigitizerType.h>

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

  _warning = 0;

  int node = 0;

  cout << __FILE__ << " " <<  __LINE__ << " Pulse on " << m_eventType << " " << _linknumber << endl;
  
  _broken = CAEN_DGTZ_OpenDigitizer( CAEN_DGTZ_PCI_OpticalLink, _linknumber , node, 0 ,&handle);


  if ( _broken )
    {
      cout << " Error in CAEN_DGTZ_OpenDigitizer " << _broken << endl;
      return;
    }

}

daq_device_CAENPulse::~daq_device_CAENPulse()
{


  CAEN_DGTZ_CloseDigitizer(handle);

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

  const unsigned int reg    = 0x811C;
  const unsigned int pulseon  = 0xC000;
  const unsigned int pulseoff = 0x8000;

  _broken = CAEN_DGTZ_WriteRegister(handle, reg, pulseon);
  _broken = CAEN_DGTZ_WriteRegister(handle, reg, pulseoff);

  return 0;
}


void daq_device_CAENPulse::identify(std::ostream& os) const
{

  CAEN_DGTZ_BoardInfo_t       BoardInfo;


  if ( _broken) 
    {
      os << "CAEN Pulse at Event Type: " << m_eventType 
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
      
      os << "CAEN Digitizer Pulse at Event Type: " << m_eventType << endl;

    }
}

