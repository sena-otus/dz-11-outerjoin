#include "netapp.h"
#include "simpledb.h"


#include <iostream>
#include <stdexcept>


const int generic_errorcode = 200;


int main(int argc, char* argv[])
{
  try
  {
    if (argc != 2)
    {
      std::cout << "Usage\n"
                <<"  join_server <tcp port>\n"
                << "\n"
                << "Where"
                << "  <tcp port>   is tcp port to listen for incoming connections\n"
                << "\n"
                << "Example"
                << "  join_server 9300\n";
      return 0;
    }

      //NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
    NetApp server(std::stoul(argv[1]));
    server.run();
  }
  catch (const std::exception& ex)
  {
    std::cerr << "Exception: " << ex.what() << "\n";
    return generic_errorcode;
  }

  return 0;
}
