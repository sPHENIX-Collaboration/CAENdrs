
#include <iostream>

#include <sstream>
#include <parseargument.h>

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "getopt.h"


//#include <CAENDigitizer.h>
//#include <CAENDigitizerType.h>

#include "caen_manager.h"

using namespace std;

void exithelp( const int ret =1)
{

  cout  << std::endl;
  cout << " caen_pulse usage " << std::endl;
  cout << "    -d link (optical fiber) number,  default 0 " << std::endl;
  cout <<  std::endl;
  cout << "   caen_pulse  pulse" << std::endl;
  cout << "   caen_pulse  on/off" << std::endl;
  cout << "    " << std::endl;
  exit(ret);
}

int main(int argc, char *argv[])
{

  if ( argc==1) 
    {
      exithelp(1);
    }

  int verbosity = 0;
  int linknum = 0;

  int opt;
  while ((opt = getopt(argc, argv, "hvd:")) != EOF)
    {
      switch (opt) 
	{
	  // the -d  (which linknum) switch 
	case 'd':
	  if ( !sscanf(optarg, "%d",  &linknum) ) exithelp();
	  if ( linknum < 0 || linknum > 15) exithelp();
	  break;

	case 'v':
	  verbosity++;
	  break;

	case 'h':
	  exithelp(0);
	  break;

	default:
	  break;
						
	}
    }


  caen_manager * cm = new caen_manager(linknum);
  if (cm->GetStatus())
    {
      cout << "Failure, Status = "  << cm->GetStatus() << endl;
      delete cm;
      return 1;
    }

  
  const unsigned int index    = 0x811C;
  const unsigned int pulseon  = 0xC000;
  const unsigned int pulseoff = 0x8000;
  unsigned int val;
  int status=-99;

  if ( argc > optind  )  //we have 2+ parameters, say "level 16000" 
    {

      if ( strcmp( argv[optind], "pulse") == 0)  
	{
	  //	  val = get_uvalue( argv[optind+1]);
	  status = cm->WriteRegister(index, pulseon);
	  //usleep(100);
	  status = cm->WriteRegister(index, pulseoff);
	}

      else if ( strcmp( argv[optind], "on") == 0)  
	{
	  status = cm->WriteRegister(index, pulseon);
	}

      else if ( strcmp( argv[optind], "off") == 0)  
	{
	  status = cm->WriteRegister(index, pulseoff);
	}


    }

  else if (status)
    {
      cout << "Failure, Status = " << status << " cm status = " << cm->GetStatus() << endl;
      return 1;
    }
  return 0;
}

