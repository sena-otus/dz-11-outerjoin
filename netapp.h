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
     * @param port номер TCP порта на котором сервер будет принимать соединения
     */
  explicit NetApp(unsigned short port);

    /** запуск рабочих тредов */
  void run();

private:
    /** принять соединение и создать tcp сессию @see class session */
  void do_accept();

  boost::asio::io_context m_io_context;
  boost::asio::ip::tcp::acceptor m_acceptor;
  SimpleDB m_simpledb; //<! база данных
};
