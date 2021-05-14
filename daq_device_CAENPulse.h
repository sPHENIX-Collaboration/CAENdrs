#ifndef __DAQ_DEVICE_CAENPULSE__
#define __DAQ_DEVICE_CAENPULSE__


#include <daq_device.h>
#include <stdio.h>
#include <CAENdrsTriggerHandler.h>
#include <CAENDigitizerType.h>


class daq_device_CAENPulse: public  daq_device {


public:

  daq_device_CAENPulse(const int eventtype, const int subid, const int link =0);
    
  ~daq_device_CAENPulse();


  void identify(std::ostream& os = std::cout) const;

  int max_length(const int etype) const {return 0;};

  // functions to do the work

  int put_data(const int etype, int * adr, const int length);

  int init() { return 0;};
  //int rearm( const int etype);
  //int endrun();

 protected:

  int handle;

  int _linknumber;

  int _broken;
  int _warning;

};


#endif
