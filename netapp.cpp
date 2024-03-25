#include "netapp.h"
#include "session.h"

#include <iostream>

using boost::asio::ip::tcp;

NetApp::NetApp(unsigned short port)
  : m_acceptor(m_io_context, tcp::endpoint(tcp::v4(), port))
{
  do_accept();
}


void NetApp::do_accept()
{
  m_acceptor.async_accept(boost::asio::make_strand(m_io_context),
    [this](boost::system::error_code ec, tcp::socket socket)
      {
          // Check whether the server was stopped by a signal before this
          // completion handler had a chance to run.
        if (!m_acceptor.is_open()) {
          return;
        }

        if (!ec) {
          std::make_shared<session>(std::move(socket), m_simpledb)->start();
        }

        do_accept();
      });
}

void NetApp::run()
{
  std::cout << "Create a pool of " << std::thread::hardware_concurrency() << " threads to run the io_context." << std::endl;
  std::vector<std::thread> threads;
  for (std::size_t i = 0; i < std::thread::hardware_concurrency(); ++i)
  {
    threads.emplace_back([this]{ m_io_context.run(); });
  }

    // Wait for all threads in the pool to exit.
  for (auto & thread : threads)
  {
    thread.join();
  }
}
