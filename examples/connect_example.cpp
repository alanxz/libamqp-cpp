#include "connection.h"

int main()
{
  boost::shared_ptr<amqpp::connection> c = amqpp::connection::create_connection("localhost", "guest", "guest", "/");
}
