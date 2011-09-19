#ifndef _LIBAMQPCPP_CONNECTION_H_INCLUDED_
#define _LIBAMQPCPP_CONNECTION_H_INCLUDED_

#include "export.h"

#include <boost/shared_ptr.hpp>

#include <string>
#include <vector>

namespace amqpp 
{

class channel;

class AMQPP_EXPORT connection
{
	public:
        virtual boost::shared_ptr<channel> open_channel() = 0;
        virtual void close() = 0;
};
} // namespace amqpp

#endif // _LIBAMQPCPP_CONNECTION_H_INCLUDED_
