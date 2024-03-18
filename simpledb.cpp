#include "simpledb.h"
#include "pcommand.h"
#include <boost/algorithm/string.hpp>
#include <exception>
#include <string>
#include <unistd.h>

SimpleDB::SimpleDB()
{
  m_cmdmap["INSERT"              ] = [this](const PCommand &cmd){return insert      (cmd);};
  m_cmdmap["TRUNCATE"            ] = [this](const PCommand &cmd){return truncate    (cmd);};
  m_cmdmap["INTERSECTION"        ] = [this](const PCommand &cmd){return intersection(cmd);};
  m_cmdmap["SYMMETRIC_DIFFERENCE"] = [this](const PCommand &cmd){return sym_diff    (cmd);};
  m_table["A"];
  m_table["B"];
}


std::string
SimpleDB::insert(const PCommand &cmd)
{
  if(cmd.size() < 3) {
    return "ERR недосаточно параметров";
  }
  int id = 0;
  try {
    id = std::stoi(cmd.arg(2));
  } catch (std::exception &ex) {
    return "ERR не могу конвертировать '"+cmd.arg(2)+"' в int";
  }
  auto tit = m_table.find(cmd.arg(1));
  if(tit == m_table.end()) {
    return "ERR таблица '"+cmd.arg(1)+"' не найдена";
  }
  if(tit->second.find(id) != tit->second.end()){
    return "ERR дупликат "+cmd.arg(2);
  }
  tit->second[id] = cmd.arg(3);
  return "OK";
}

std::string
SimpleDB::truncate(const PCommand &cmd)
{
  if(cmd.size() < 2) {
    return "ERR недосаточно параметров";
  }
  auto tit = m_table.find(cmd.arg(1));
  if(tit == m_table.end()) {
    return "ERR таблица '"+cmd.arg(1)+"' не найдена";
  }
  tit->second.clear();
  return "OK";
}

std::string
SimpleDB::intersection(const PCommand & /*cmd*/)
{
  std::string result;
  const auto& A = m_table["A"];
  const auto& B = m_table["B"];
  for(auto && row : A)
  {
    auto bit = B.find(row.first);
    if(bit !=  B.end())
    {
      result += std::to_string(row.first)+","+row.second+","+bit->second+"\n";
    }
  }
  result += "OK";
  return result;
}

std::string
SimpleDB::sym_diff(const PCommand & /*cmd*/)
{
  std::string result;
  const auto& A = m_table["A"];
  const auto& B = m_table["B"];
  for(auto && row : A)
  {
    auto bit = B.find(row.first);
    if(bit ==  B.end())
    {
      result += std::to_string(row.first)+","+row.second+",\n";
    }
  }
  for(auto && row : B)
  {
    auto ait = A.find(row.first);
    if(ait ==  A.end())
    {
      result += std::to_string(row.first)+",,"+row.second+"\n";
    }
  }
  result += "OK";
  return result;
}

std::string
SimpleDB::exec(std::string_view cmdbuf)
{
  PCommand cmd(cmdbuf);
  if(cmd.size() == 0) {
    return "OK";
  }
  const auto cmdU = boost::to_upper_copy<std::string>(cmd.arg(0));
  auto cmdit = m_cmdmap.find(cmdU);
  if(cmdit == m_cmdmap.end()) {
    return "ERR неизвестная команда '" + cmd.arg(0) + "'";
  }
  return cmdit->second(cmd);
}
