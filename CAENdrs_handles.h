#ifndef __CAENDRS_HANDLES_H__
#define __CAENDRS_HANDLES_H__

// Within one process, a second CAEN_DGTZ_OpenDigitizer on the same link
// shares the connection with the first, and closing it pulls the link out
// from under the other device. So the readout devices register their open
// handle here, and helper devices (such as the calibration capture) use it
// instead of opening their own.
//
// connection: 0 = optical link, 1 = USB, 2 = USB via A4818 (CONET)

void caen_register_handle(const int connection, const int link, const int node, const int handle);
void caen_unregister_handle(const int handle);

// returns 1 and sets handle if a device has this digitizer open, 0 otherwise
int caen_find_handle(const int connection, const int link, const int node, int &handle);

#endif
