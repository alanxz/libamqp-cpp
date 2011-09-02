#ifndef _AMQPP_EXPORT_H_INCLUDED_
#define _AMQPP_EXPORT_H_INCLUDED_

#if defined(WIN32) && !defined(AMQPP_STATIC_LIB)
# ifdef amqp_cpp_EXPORTS
#  define AMQPP_EXPORT __declspec(dllexport)
# else
#  define AMQPP_EXPORT __declspec(dllimport)
# endif // defined(amqp_cpp_EXPORTS)
#else
# define AMQPP_EXPORT 
#endif

#endif // _AMQPP_EXPORT_H_INCLUDED_