#ifndef _LIBAMQPP_SCOPED_BUFFER_H_INCLUDED_
#define _LIBAMQPP_SCOPED_BUFFER_H_INCLUDED_

#include <boost/asio/buffer.hpp>
#include <boost/cstdint.hpp>
#include <boost/noncopyable.hpp>
#include <boost/scoped_array.hpp>

namespace amqpp
{
namespace detail
{

/**
  * A runtime allocated buffer of a specified size
  */
template<class T>
class scoped_buffer : boost::noncopyable
{
public:
  explicit scoped_buffer(uint32_t size) :
    m_data(new T[size]), m_size(size)
  {
  }

  virtual ~scoped_buffer()
  {
  }

  uint32_t get_size() const { return m_size; }
  T* get_data() const { return m_data.get(); }

  boost::asio::mutable_buffer get_buffer() const { return boost::asio::buffer(m_data.get(), m_size); }

private:
  boost::scoped_array<T> m_data;
  const uint32_t m_size;
};

} // namespace detail
} // namespace amqpp
#endif // _LIBAMQPP_SCOPED_BUFFER_H_INCLUDED_