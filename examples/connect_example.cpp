#include "connection.h"
#include "channel.h"

int main()
{
  boost::shared_ptr<amqpp::connection> c = amqpp::connection::create_connection("localhost", "guest", "guest", "/");
  amqpp::channel::ptr_t chan = c->open_channel();
}
