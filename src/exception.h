#ifndef _AMQPP_EXCEPTION_H_INCLUDED_
#define _AMQPP_EXCEPTION_H_INCLUDED_

#include <stdexcept>

namespace amqpp
{

class channel_exception : public std::runtime_error
{
public:
  explicit channel_exception() throw() : std::runtime_error("A channel exception occurred") {}
  virtual ~channel_exception() throw() {}

};

class connection_exception : public std::runtime_error
{
public:
  explicit connection_exception() throw() : std::runtime_error("A connection exception occurred") {}
  virtual ~connection_exception() throw() {}
};

}
#endif // _AMQPP_EXCEPTION_H_INCLUDED_
