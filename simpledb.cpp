#include "simpledb.h"
#include "pcommand.h"
#include <boost/algorithm/string.hpp>
#include <exception>
#include <sstream>
#include <string>
#include <unistd.h>

SimpleDB::SimpleDB()
{
  m_cmdmap["INSERT"              ] = [this](const PCommand &cmd){return insert      (cmd);};
  m_cmdmap["TRUNCATE"            ] = [this](const PCommand &cmd){return truncate    (cmd);};
  m_cmdmap["INTERSECTION"        ] = [this](const PCommand &cmd){return intersection(cmd);};
  m_cmdmap["SYMMETRIC_DIFFERENCE"] = [this](const PCommand &cmd){return sym_diff    (cmd);};
}


std::string
SimpleDB::insert(const PCommand &cmd)
{
  if(cmd.size() < 3) {
    return "ERR недостаточно параметров\n";
  }
  int id = 0;
  try {
    id = std::stoi(cmd.arg(2));
  } catch (std::exception &ex) {
    return "ERR не могу конвертировать '"+cmd.arg(2)+"' в int\n";
  }
  auto &table = m_table[cmd.arg(1)];
  if(table.find(id) != table.end()){
    return "ERR дупликат "+cmd.arg(2)+"\n";
  }
  table[id] = cmd.arg(3);
  return "OK\n";
}

std::string
SimpleDB::truncate(const PCommand &cmd)
{
  if(cmd.size() < 2) {
    return "ERR недосаточно параметров\n";
  }
  auto tit = m_table.find(cmd.arg(1));
  if(tit == m_table.end()) {
    return "ERR таблица '"+cmd.arg(1)+"' не найдена\n";
  }
  tit->second.clear();
  return "OK\n";
}

std::string
SimpleDB::intersection(const PCommand & /*cmd*/)
{
  std::ostringstream result;
  const auto& A = m_table["A"];
  const auto& B = m_table["B"];
  for(auto && row : A)
  {
    auto bit = B.find(row.first);
    if(bit !=  B.end())
    {
      result << row.first << "," << row.second << "," << bit->second << "\n";
    }
  }
  result << "OK\n";
  return result.str();
}

std::string
SimpleDB::sym_diff(const PCommand & /*cmd*/)
{
  std::ostringstream result;
  const auto& A = m_table["A"];
  const auto& B = m_table["B"];
  for(auto && row : A)
  {
    auto bit = B.find(row.first);
    if(bit ==  B.end())
    {
      result << row.first << "," << row.second << ",\n";
    }
  }
  for(auto && row : B)
  {
    auto ait = A.find(row.first);
    if(ait ==  A.end())
    {
      result << std::to_string(row.first) << ",," << row.second << "\n";
    }
  }
  result << "OK\n";
  return result.str();
}

std::string
SimpleDB::exec(std::string_view cmdbuf)
{
  PCommand cmd(cmdbuf);
  if(cmd.size() == 0) {
    return "OK\n";
  }
  const auto cmdU = boost::to_upper_copy<std::string>(cmd.arg(0));
  auto cmdit = m_cmdmap.find(cmdU);
  if(cmdit == m_cmdmap.end()) {
    return "ERR неизвестная команда '" + cmd.arg(0) + "'\n";
  }
  return cmdit->second(cmd);
}
