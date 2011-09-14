#ifndef _LIBAMQPP_AMQP_SASL_H_INCLUDED_
#define _LIBAMQPP_AMQP_SASL_H_INCLUDED_

#include <string>

namespace amqpp
{

class sasl
{
public:
  static std::string select_sasl_mechanism(const std::string& mechanism_list);
  static std::string get_sasl_response(const std::string& mechanism, const std::string& username, const std::string& password);

private:
  static const std::string PLAIN_MECHANISM;

  static std::string get_plain_response(const std::string& username, const std::string& password);
};

} // namespace amqpp
#endif // _LIBAMQPP_AMQP_SASL_H_INCLUDED_