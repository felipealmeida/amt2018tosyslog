// Copyright Felipe Magno de Almeida 2018.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <amt2018/communication.hpp>

#if __has_include(<variant>)
#include <variant>
#else
#include <experimental/variant>
#endif

#include <iostream>
#include <iomanip>
#include <fstream>

#include <boost/asio.hpp>

#include <syslog.h>

int main()
{
  boost::asio::io_service io_service;

  boost::asio::deadline_timer timer(io_service);

  amt2018::connection connection
    (io_service, boost::asio::ip::tcp::endpoint{boost::asio::ip::tcp::v4(), 9009});

  connection.listen();
  amt2018::communication communication(connection);

  // communication.zone_state.connect
  //   ([&communication] (auto open, auto bypass)
  //    {
  //      std::cout << "zone state has changed" << std::endl;
  //      std::cout << "porta 18 esta " << (open[18-1] ? " aberta " : " fechada ") << std::endl;

  //      // if(!open[17])
  //      // {
  //      //   std::cout << "activating partition 1" << std::endl;
  //      //   communication.activate_partition(1, "909000");
  //      // }
  //    });

  communication.event.connect
    ([&] (auto event, auto id, auto part, auto zone)
     {
       std::cout << "Should log this event? " << message_event(event) << " id " << id << " partition " << part <<  " zone " << zone << std::endl;
       std::stringstream stream;
       stream << "id=" << id << " partition=" << part << " zone=" << zone << " event-id=" << static_cast<int>(event) << ": " << message_event(event) << "\n";
       ::syslog(LOG_NOTICE, stream.str().c_str());
     });

  connection.data.connect
    ([&] (auto const& ec, auto buffer)
     {
       // static bool never = true;
       if(ec)
       {
         connection.listen();
       }
       // else if(never/* && buffer.size() == 7 && (unsigned char)buffer[0] == 0xc4*/)
       // {
       //   never = false;
       //   std::cout << "request zone" << std::endl;
       //   communication.request_zones("909000");
       // }
     });

  io_service.run();

  ::closelog();
}
