#pragma once

#include "pcommand.h"
#include <map>
#include <string>
#include <string_view>
#include <functional>


class SimpleDB
{
public:

  SimpleDB();

  std::string exec(std::string_view cmdbuf);

  std::string insert(const PCommand &cmd);
  std::string truncate(const PCommand &cmd);
  std::string intersection(const PCommand &cmd);
  std::string sym_diff(const PCommand &cmd);

private:
  std::map<std::string, std::map<int, std::string>> m_table;
  std::map<std::string, std::function<std::string(PCommand&)>> m_cmdmap;
};
