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
  std::optional<std::string> cmd = m_acculine.getNextCmd();
  if(cmd)
  {
    m_reply = m_simpledb.exec(*cmd);
    do_write();
    return;
  }
  auto self(shared_from_this());
  m_socket.async_read_some(
    boost::asio::buffer(m_data, max_length),
    [this, self](boost::system::error_code ec, std::size_t length)
      {
        if (!ec)
        {
          m_acculine.addNewInput(std::string_view(m_data.data(), length));
          do_read();
        }
      });
}

void session::do_write()
{
  auto self(shared_from_this());
  boost::asio::async_write(
    m_socket, boost::asio::buffer(m_reply),
    [this, self](boost::system::error_code ec, std::size_t /*length*/)
      {
        if(ec) {
          std::cerr << "got error while sending reply: " << ec.message() << std::endl;
        }
        do_read();
      });
}
