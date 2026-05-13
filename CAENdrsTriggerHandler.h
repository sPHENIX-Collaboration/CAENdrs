#ifndef __CAENDRSTRIGGERHANDLER_H__
#define __CAENDRSTRIGGERHANDLER_H__


#include "TriggerHandler.h"

class DRSBoard;

class CAENdrsTriggerHandler : public TriggerHandler {

public:

  CAENdrsTriggerHandler(int handle, const int etype , const int max_size, const int is_USB=0)
    {
      _handle = handle;
      _etype = etype;
      _is_USB = is_USB;
      _max_size = max_size;
      _buffer = new int[_max_size];
      _current_size = 0;
    }

  ~CAENdrsTriggerHandler()
  {
    if (_buffer) delete [] _buffer;
  };

  //  virtual void identify(std::ostream& os = std::cout) const = 0;

  // this is the virtual worker routine
  int wait_for_trigger( const int moreinfo=0);

  int fill_data(int * adr, const int size);

 protected:
  
  int _handle;
  int _etype ;
  int _is_USB;

  int _current_size;
  int _max_size;
  int *_buffer;
  
};

#endif
