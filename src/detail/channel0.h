#ifndef _LIBAMQPP_CHANNEL0_H_INCLUDED_
#define _LIBAMQPP_CHANNEL0_H_INCLUDED_

#include "detail/frame.h"
#include "detail/frame_handler.h"

#include <boost/noncopyable.hpp>

namespace amqpp
{
namespace detail
{

class channel0 : boost::noncopyable, public frame_handler
{
public:

  virtual ~channel0() {}

  virtual void process_frame(const frame::ptr_t& frame);
};

}
} // namespace amqpp
#endif // _LIBAMQPP_CHANNEL0_H_INCLUDED_