libamqp-cpp - an Rabbitmq-compatible AMQP client library written in c++/boost-asio
=================================================================================

This library was written as the librabbitmq-c doesn't cover my needs.

Desired features above and beyond what librabbitmq-c provides
- A nice c++ interface
- Ability to cleanly have more than one channel on a connection
- Have more than one consumer.

Also gets me to learn a bit more about c++ and the boost family of libraries

Building
--------

### Prereqs:
- A modern c++ compiler (msvc10, g++4.2 tested)
- CMake v2.8 or better 
- Python 2.5 or better [Needed to generate the methods from the AMQP spec]
- Boost 1.46.1 or better, may work with older versions, YMMV

### Compiling
Works like most other cmake-based compiles, cmake, run the platform-specific build


