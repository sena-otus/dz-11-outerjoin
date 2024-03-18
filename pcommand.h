#pragma once

#include <string_view>
#include <vector>
#include <string>

class PCommand
{
public:
  PCommand(std::string_view  cmdbuf);

  std::string arg(unsigned ii) const;
  unsigned size() const;
private:
  std::vector<std::string> m_args;
};
