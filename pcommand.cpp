#include "pcommand.h"

#include <boost/iostreams/stream.hpp>
#include <sstream>


PCommand::PCommand(std::string_view cmdbuf)
{
  boost::iostreams::stream<boost::iostreams::array_source> iss(cmdbuf.data(), cmdbuf.size());
  auto str = std::string{};
  while (iss >> str) {
    m_args.emplace_back(str);
  }
}

std::string
PCommand::arg(unsigned ii) const
{
  if(ii < m_args.size()) return m_args[ii];
  return "";
}

unsigned PCommand::size() const
{
  return m_args.size();
}
