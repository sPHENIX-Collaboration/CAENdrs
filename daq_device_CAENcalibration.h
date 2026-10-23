#ifndef __DAQ_DEVICE_CAENCALIBRATION__
#define __DAQ_DEVICE_CAENCALIBRATION__

// This device captures the factory calibration (the DRS4 correction tables)
// of a V1742/DT5742 for all three sampling speeds into one packet, so the
// calibration travels with the data. Meant for the begin-run event (type 9).
//
// Packet layout (hit format 86, IDCAENV1742CALIB, 32-bit words):
//
//  header (CAENCALIB_HEADERLENGTH words)
//    0  format version
//    1  serial number
//    2  subevent id of the readout packet this belongs to (0 if not given)
//    3  model number
//    4  number of speeds (3)
//    5  number of chips  (4)
//    6  number of channels per chip (9, 8 + the trigger channel)
//    7  number of cells  (1024)
//    8 -10  model name, 12 chars
//   11 -15  ROC firmware, 20 chars
//   16 -25  AMC firmware, 40 chars
//
//  then for each speed (0 = 5GS, 1 = 2.5GS, 2 = 1GS)
//    speed code
//    status of the CAEN_DGTZ_GetCorrectionTables call (0 = ok; the tables are zero otherwise)
//    for each chip
//      9 x 1024 words  (channel-major) low 16 bits: cell offset,  high 16 bits: nsample correction
//      1024 words      the cell time (float)

#include <daq_device.h>
#include <stdio.h>
#include <CAENDigitizerType.h>

#define CAENCALIB_HEADERLENGTH 26
#define CAENCALIB_NSPEEDS 3
#define CAENCALIB_NCHIPS 4
#define CAENCALIB_NCHANNELS 9
#define CAENCALIB_NCELLS 1024

class daq_device_CAENcalibration: public  daq_device {


public:

  daq_device_CAENcalibration(const int eventtype
			     , const int subeventid
			     , const int linknumber = 0
			     , const int nodenumber = 0
			     , const int connection = 0
			     , const int readout_subid = 0);

  ~daq_device_CAENcalibration() {};


  void identify(std::ostream& os = std::cout) const;

  int max_length(const int etype) const;

  // functions to do the work

  int put_data(const int etype, int * adr, const int length);

  int init() { return 0;};

 protected:

  int open_digitizer(int &handle) const;

  int _linknumber;
  int _nodenumber;
  int _connection;     // 0 = optical link, 1 = USB, 2 = USB via A4818 (CONET), as in caen_client
  int _readout_subid;

  uint32_t _serial;    // from the last capture, for identify
  int _last_status;

};


#endif
