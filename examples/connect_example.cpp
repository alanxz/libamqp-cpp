#include "connection.h"
#include "channel.h"

#include <iostream>

int main()
{
  boost::shared_ptr<amqpp::connection> c = amqpp::connection::create_connection("127.0.0.1", "guest", "guest", "/");
  try
  {
    amqpp::channel::ptr_t chan = c->open_channel();
  }
  catch (std::exception& e)
  {
    std::cout << "Err: " << e.what() << std::endl;
  }

  std::cout << "Press any key to continue . . .";
  std::cin.get();
  return 0;
}
