#pragma once

#include <string_view>
#include <vector>
#include <string>

class PCommand
{
public:
  explicit PCommand(std::string_view  cmdbuf);

  [[nodiscard]] std::string arg(unsigned ii) const;
  [[nodiscard]] unsigned size() const;
private:
  std::vector<std::string> m_args;
};
