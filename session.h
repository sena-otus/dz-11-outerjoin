#pragma once

#include "simpledb.h"
#include "acculine.h"

#include <utility>
#include <boost/asio.hpp>
#include <memory>
#include <array>

/** @brief TCP/IP сессия */
class session
  : public std::enable_shared_from_this<session>
{
public:
  session(boost::asio::ip::tcp::socket socket, SimpleDB &simpledb);

  void start();
private:
    /** прочитать и обработать запросы */
  void do_read();
    /** послать ответы */
  void do_write();

  boost::asio::ip::tcp::socket m_socket;
  enum { max_length = 1024 };
  std::array<char, max_length> m_data; ///<! входящий буфер с запросами
  SimpleDB &m_simpledb; ///<! экземпляр базы данных
  AccuLine m_acculine; ///<! аккумулятор команд (запросов)
  std::string m_reply; ///<! ответ сервера
};
