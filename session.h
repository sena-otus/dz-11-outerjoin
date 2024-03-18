#pragma once

#include "simpledb.h"
#include "acculine.h"

#include <utility>
#include <boost/asio.hpp>
#include <memory>
#include <array>

class session
  : public std::enable_shared_from_this<session>
{
public:
  session(boost::asio::ip::tcp::socket socket);

  void start();
private:
  void do_read();

  boost::asio::ip::tcp::socket m_socket;
  enum { max_length = 1024 };
  std::array<char, max_length> m_data;
  SimpleDB m_simpledb;
  AccuLine m_acculine;
};
