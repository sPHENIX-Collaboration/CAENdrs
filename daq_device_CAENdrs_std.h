#ifndef __DAQ_DEVICE_CAENDRS_STD__
#define __DAQ_DEVICE_CAENDRS_STD__


#include <daq_device_CAENDigitizer.h>
#include <stdio.h>
#include <CAENdrsTriggerHandler.h>
#include <CAENDigitizerType.h>


class daq_device_CAENdrs_std: public  daq_device_CAENDigitizer {


public:

  daq_device_CAENdrs_std(const int eventtype
			 , const int subeventid
			 , const int linknumber = 0
			 , const int nodenumber = 0
			 , const int trigger = 0
			 , const int speed = 0
			 , const int delay = 0
			 , const int endpulse = 0);
    
  ~daq_device_CAENdrs_std();


  void identify(std::ostream& os = std::cout) const;

  int max_length(const int etype) const;


  int init();
  int rearm( const int etype);
  int endrun();

 protected:


  
  CAEN_DGTZ_DRS4Frequency_t _speed;
  int _delay;


};


#endif
