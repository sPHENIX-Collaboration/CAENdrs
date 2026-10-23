
#include <iostream>
#include <string.h>

#include <daq_device_CAENcalibration.h>

#include <CAENDigitizer.h>
#include <CAENDigitizerType.h>
#include <CAENdrs_handles.h>

using namespace std;

daq_device_CAENcalibration::daq_device_CAENcalibration(const int eventtype
						       , const int subeventid
						       , const int linknumber
						       , const int nodenumber
						       , const int connection
						       , const int readout_subid)
{
  m_eventType  = eventtype;
  m_subeventid = subeventid;

  _linknumber = linknumber;
  _nodenumber = nodenumber;
  _connection = connection;
  _readout_subid = readout_subid;

  _serial = 0;
  _last_status = 0;
}

// we open our own handle each time and close it again, so we always
// describe the unit that is connected right now

int daq_device_CAENcalibration::open_digitizer(int &handle) const
{
  CAEN_DGTZ_ConnectionType ct = CAEN_DGTZ_OpticalLink;
  if ( _connection == 1 ) ct = CAEN_DGTZ_USB;
  else if ( _connection == 2 ) ct = CAEN_DGTZ_USB_A4818;

  return CAEN_DGTZ_OpenDigitizer( ct, _linknumber , _nodenumber, 0 ,&handle);
}

int daq_device_CAENcalibration::max_length(const int etype) const
{
  if (etype != m_eventType) return 0;
  return SEVTHEADERLENGTH + CAENCALIB_HEADERLENGTH
    + CAENCALIB_NSPEEDS * ( 2 + CAENCALIB_NCHIPS * (CAENCALIB_NCHANNELS + 1) * CAENCALIB_NCELLS )
    + 2;  // padding
}


// the put_data function

int daq_device_CAENcalibration::put_data(const int etype, int * adr, const int length )
{

  if (etype != m_eventType )  // not our id
    {
      return 0;
    }

  if ( length < max_length(etype) )
    {
      cout << __FILE__ << " " << __LINE__ << " buffer too small for the calibration packet " << m_subeventid << endl;
      return 0;
    }

  // if a readout device has this digitizer open, we use its handle and
  // must not close it. Only when we are alone we open our own.
  int handle;
  int own_handle = 0;
  if ( ! caen_find_handle(_connection, _linknumber, _nodenumber, handle) )
    {
      _last_status = open_digitizer(handle);
      if ( _last_status )
	{
	  cout << __FILE__ << " " << __LINE__ << " Error " << _last_status
	       << " opening the digitizer at link " << _linknumber << " node " << _nodenumber << endl;
	  return 0;
	}
      own_handle = 1;
    }

  CAEN_DGTZ_BoardInfo_t BoardInfo;
  _last_status = CAEN_DGTZ_GetInfo(handle, &BoardInfo);
  if ( _last_status )
    {
      cout << __FILE__ << " " << __LINE__ << " Error " << _last_status << " in CAEN_DGTZ_GetInfo" << endl;
      if ( own_handle) CAEN_DGTZ_CloseDigitizer(handle);
      return 0;
    }
  _serial = BoardInfo.SerialNumber;

  subevtdata_ptr sevt =  (subevtdata_ptr) adr;
  sevt->sub_length =  SEVTHEADERLENGTH;
  sevt->sub_id =  m_subeventid;
  sevt->sub_type = 4;
  sevt->sub_decoding = 86;  // IDCAENV1742CALIB
  sevt->reserved[0] = 0;
  sevt->reserved[1] = 0;

  int *d = &sevt->data;

  memset (d, 0, CAENCALIB_HEADERLENGTH * 4);
  d[0] = 1;  // format version
  d[1] = BoardInfo.SerialNumber;
  d[2] = _readout_subid;
  d[3] = BoardInfo.Model;
  d[4] = CAENCALIB_NSPEEDS;
  d[5] = CAENCALIB_NCHIPS;
  d[6] = CAENCALIB_NCHANNELS;
  d[7] = CAENCALIB_NCELLS;
  strncpy ( (char *) &d[8],  BoardInfo.ModelName, 12);
  strncpy ( (char *) &d[11], BoardInfo.ROC_FirmwareRel, 20);
  strncpy ( (char *) &d[16], BoardInfo.AMC_FirmwareRel, 40);
  d += CAENCALIB_HEADERLENGTH;

  static CAEN_DGTZ_DRS4Correction_t CTable[CAENCALIB_NCHIPS];  // ~80KB, keep it off the stack

  const CAEN_DGTZ_DRS4Frequency_t speeds[CAENCALIB_NSPEEDS] = { CAEN_DGTZ_DRS4_5GHz, CAEN_DGTZ_DRS4_2_5GHz, CAEN_DGTZ_DRS4_1GHz };

  for ( int s = 0; s < CAENCALIB_NSPEEDS; s++)
    {
      memset ( CTable, 0, sizeof(CTable));
      int status = CAEN_DGTZ_GetCorrectionTables(handle, speeds[s], CTable);
      if ( status )
	{
	  cout << __FILE__ << " " << __LINE__ << " Error " << status
	       << " reading the correction tables for speed " << s << endl;
	  _last_status = status;
	}

      *d++ = s;
      *d++ = status;

      for ( int chip = 0; chip < CAENCALIB_NCHIPS; chip++)
	{
	  for ( int c = 0; c < CAENCALIB_NCHANNELS; c++)
	    {
	      for ( int t = 0; t < CAENCALIB_NCELLS; t++)
		{
		  uint32_t cell    = (uint16_t) CTable[chip].cell[c][t];
		  uint32_t nsample = (uint16_t) CTable[chip].nsample[c][t];  // sign-extended to 16 bits
		  *d++ = (nsample << 16) | cell;
		}
	    }
	  memcpy ( d, CTable[chip].time, CAENCALIB_NCELLS * sizeof(float));
	  d += CAENCALIB_NCELLS;
	}
    }

  if ( own_handle) CAEN_DGTZ_CloseDigitizer(handle);

  int len = d - &sevt->data;
  sevt->sub_padding = len%2;
  if ( sevt->sub_padding) *d = 0;
  len += sevt->sub_padding;
  sevt->sub_length += len;
  return sevt->sub_length;
}


void daq_device_CAENcalibration::identify(std::ostream& os) const
{
  os << "CAEN Calibration capture Event Type: " << m_eventType
     << " Subevent id: " << m_subeventid
     << " link " << _linknumber << " node " << _nodenumber;
  if ( _connection == 1 ) os << " (USB)";
  else if ( _connection == 2 ) os << " (A4818)";
  if ( _readout_subid ) os << " for packet " << _readout_subid;
  if ( _serial ) os << " Serial Number " << _serial;
  if ( _last_status ) os << " ** last capture failed, error " << _last_status << " **";
  os << endl;
}
