#ifndef _AMQPP_EXCEPTION_H_INCLUDED_
#define _AMQPP_EXCEPTION_H_INCLUDED_

#include <boost/cstdint.hpp>

#include <stdexcept>
#include <string>

namespace amqpp
{

class channel_exception : public std::runtime_error
{
public:
  explicit channel_exception(uint16_t reply_code, const std::string& reply_text, uint16_t class_id = 0, uint16_t method_id = 0) throw() : 
    std::runtime_error("A channel exception occurred"),
    m_reply_code(reply_code),
    m_reply_text(reply_text),
    m_class_id(class_id),
    m_method_id(method_id)
  {}
  virtual ~channel_exception() throw() {}

  uint16_t reply_code() const { return m_reply_code; }
  std::string reply_text() const { return m_reply_text; }
  uint16_t class_id() const { return m_class_id; }
  uint16_t method_id() const { return m_method_id; }

private:
  std::string m_reply_text;
  uint16_t m_reply_code;
  uint16_t m_class_id;
  uint16_t m_method_id;
};

class connection_exception : public std::runtime_error
{
public:
  explicit connection_exception() throw() : std::runtime_error("A connection exception occurred") {}
  virtual ~connection_exception() throw() {}
};

}
#endif // _AMQPP_EXCEPTION_H_INCLUDED_
