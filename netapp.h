#pragma once

#include "simpledb.h"
#include <utility>
#include <boost/asio.hpp>


/** @brief приложение TCP сервер */
class NetApp
{
public:
    /**
     * @brief конструктор
     * @param io_context контекст ввода-вывода бустасио
     * @param port номер TCP порта на котором сервер будет принимать соединения
     */
  NetApp(boost::asio::io_context& io_context, unsigned short port);

private:
    /** принять соединение и создать tcp сессию @see class session */
  void do_accept();

  boost::asio::ip::tcp::acceptor m_acceptor;
  SimpleDB m_simpledb;
};
