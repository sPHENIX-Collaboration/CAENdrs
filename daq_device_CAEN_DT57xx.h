#ifndef __DAQ_DEVICE_CAEN_DT57xx__
#define __DAQ_DEVICE_CAEN_DT57xx__


#include <daq_device_CAENDigitizer.h>
#include <stdio.h>
#include <CAENdrsTriggerHandler.h>
#include <CAENDigitizerType.h>


class daq_device_CAEN_DT57xx: public  daq_device_CAENDigitizer {


public:

  daq_device_CAEN_DT57xx(const int eventtype
		     , const int subeventid
		     , const int CONET_Flag = 0
		     , const int linknumber = 0
		     , const int nodenumber = 0
		     , const int trigger = 0
                     , const int endpulse = 0);
    
  ~daq_device_CAEN_DT57xx();


  void identify(std::ostream& os = std::cout) const;

  int max_length(const int etype) const;

  int init();
  int rearm( const int etype);
  int endrun();

 protected:

  int _is_conet;
};


#endif
