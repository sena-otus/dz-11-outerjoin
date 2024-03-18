#include "session.h"

#include <iostream>

using boost::asio::ip::tcp;

session::session(tcp::socket socket)
  : m_socket(std::move(socket)), m_data{0}
{
}


void session::start()
{
  do_read();
}

void session::do_read()
{
  auto self(shared_from_this());
  m_socket.async_read_some(
    boost::asio::buffer(m_data, max_length),
    [this, self](boost::system::error_code ec, std::size_t length)
      {
        if (!ec)
        {
          m_acculine.addNewInput(std::string_view(m_data.data(), length));
          std::optional<std::string> cmd;
          while((cmd = m_acculine.getNextCmd()))
          {
            auto reply = m_simpledb.exec(*cmd);
            std::cout << reply << std::endl;
          }
          do_read();
        }
      });
}
