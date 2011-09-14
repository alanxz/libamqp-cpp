#include "amqp_sasl.h"

#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>

#include <algorithm>
#include <stdexcept>
#include <vector>

namespace amqpp
{

const std::string sasl::PLAIN_MECHANISM("PLAIN");

std::string sasl::select_sasl_mechanism(const std::string& mechanism_list)
{

  typedef std::vector<std::string> mechanism_list_t;

  mechanism_list_t mechanisms;
  boost::algorithm::split(mechanisms, mechanism_list, boost::is_space());

  mechanism_list_t::const_iterator mechanism_it = std::find(mechanisms.begin(), mechanisms.end(), PLAIN_MECHANISM);
  if (mechanisms.end() == mechanism_it)
  {
    throw std::runtime_error("Broker does not support PLAIN SASL mechanism");
  }
  
  return *mechanism_it;
}

std::string sasl::get_sasl_response(const std::string& mechanism, const std::string& username, const std::string& password)
{
  if (PLAIN_MECHANISM == mechanism)
  {
    return get_plain_response(username, password);
  }
  else
  {
    throw std::runtime_error("Unknown SASL mechanism");
  }
}

std::string sasl::get_plain_response(const std::string& username, const std::string& password)
{
  if (std::string::npos != username.find('\0') ||
      std::string::npos != password.find('\0'))
  {
    throw std::runtime_error("Username or password contains a NULL character");
  }

  std::string res;
  res.append('\0');
  res.append(username);
  res.append('\0');
  res.append(password);

  return res;
}

} // namespace amqpp