#ifndef _LIBAMQPP_STATEMENT_H_INCLUDED_
#define _LIBAMQPP_STATEMENT_H_INCLUDED_

#include <boost/noncopyable.hpp>

namespace amqpp
{
namespace detail
{

class statement : boost::noncopyable
{
public:


private:
  enum statement_build_state
  {
    EXPECT_METHOD,
    EXPECT_HEADER,
    EXPECT_BODY,
    STATEMENT_COMPLETE
  };

};

} // namespace detail
} // namespace amqpp
#endif // _LIBAMQPP_STATEMENT_H_INCLUDED_