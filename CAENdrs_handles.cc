
#include <CAENdrs_handles.h>

#include <map>
#include <mutex>
#include <tuple>

// devices are created from the command thread and read out from the readout thread
static std::mutex HandleMutex;
static std::map<std::tuple<int,int,int>, int> OpenHandles;

void caen_register_handle(const int connection, const int link, const int node, const int handle)
{
  std::lock_guard<std::mutex> lock(HandleMutex);
  OpenHandles[std::make_tuple(connection, link, node)] = handle;
}

void caen_unregister_handle(const int handle)
{
  std::lock_guard<std::mutex> lock(HandleMutex);
  for ( auto it = OpenHandles.begin(); it != OpenHandles.end(); ++it)
    {
      if ( it->second == handle)
	{
	  OpenHandles.erase(it);
	  return;
	}
    }
}

int caen_find_handle(const int connection, const int link, const int node, int &handle)
{
  std::lock_guard<std::mutex> lock(HandleMutex);
  auto it = OpenHandles.find(std::make_tuple(connection, link, node));
  if ( it == OpenHandles.end()) return 0;
  handle = it->second;
  return 1;
}
