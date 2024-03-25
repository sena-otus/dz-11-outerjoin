#include "simpledb.h"
#include "pcommand.h"
#include <boost/algorithm/string.hpp>
#include <exception>
#include <mutex>
#include <sstream>
#include <string>
#include <unistd.h>

SimpleDB::SimpleDB()
{
  m_cmdmap["INSERT"              ] = [this](const PCommand &cmd){return insert      (cmd);};
  m_cmdmap["TRUNCATE"            ] = [this](const PCommand &cmd){return truncate    (cmd);};
  m_cmdmap["INTERSECTION"        ] = [this](const PCommand &cmd){return intersection(cmd);};
  m_cmdmap["SYMMETRIC_DIFFERENCE"] = [this](const PCommand &cmd){return sym_diff    (cmd);};
  m_table["A"];  // create table A
  m_table["B"];  // create table B
}

SimpleDB::Table::index_t& SimpleDB::Table::index()
{
  // index_t index;
  // for(const auto &rec : m_reclist) {
  //   index[rec.m_id] = &(rec);
  // }
  return m_index;
}


std::string SimpleDB::Table::insert(Record &&rec)
{
    // INTERSECTIOM и SYMMETRIC_DIFFERENCE могут исполнятся
    // TRUNCATE и INSERT заблокированы
  const std::lock_guard<std::mutex> guard(m_insMutex);
  auto &ref = m_reclist.emplace_front(rec);
  auto rez = m_index.insert(std::pair(ref.m_id, &ref));
  if(!rez.second) {
    m_reclist.erase_after(m_reclist.before_begin()); // удалить только что вставленный (первый)
    return "ERR дупликат "+std::to_string(rec.m_id)+"\n";
  }
  return "OK\n";
}

std::string SimpleDB::Table::truncate()
{
    // эксклюзивный доступ
  const std::scoped_lock lock(m_insMutex, m_selMutex);
  m_reclist.clear();
  m_index.clear();
  return "OK\n";
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
  auto tit = m_table.find(cmd.arg(1));
  if(tit == m_table.end()) {
    return "ERR таблица '"+cmd.arg(1)+"' не найдена\n";
  }
  auto &table = tit->second;
  return table.insert(Record{id, cmd.arg(3)});
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
  return tit->second.truncate();
}

std::string
SimpleDB::intersection(const PCommand & /*cmd*/)
{
  std::ostringstream result;
  auto& A = m_table["A"];
  auto& B = m_table["B"];
  Table::index_t Aindex;
  Table::index_t Bindex;

    // запретить только truncate, но разрешён insert и доступ для чтения
  std::shared_lock Alock(A.selMutex(), std::defer_lock);
  std::shared_lock Block(B.selMutex(), std::defer_lock);
  const std::scoped_lock lockall(Alock, Block);

  {
      // эксклюзивный доступ на короткий срок, чтобы скопировать индексы
    const std::scoped_lock lock(A.insMutex(), B.insMutex());
      // копируем индексы, чтобы не зависеть от insert
    Aindex = A.index();
    Bindex = B.index();
  }

  for(const auto & ait : Aindex) {
    auto bit = Bindex.find(ait.first);
    if(bit != Bindex.end()) {
      result << ait.first << "," << ait.second->m_str << "," << bit->second->m_str << "\n";
    }
  }
  result << "OK\n";
  return result.str();
}

std::string
SimpleDB::sym_diff(const PCommand & /*cmd*/)
{
  std::ostringstream result;
  auto& A = m_table["A"];
  auto& B = m_table["B"];

  Table::index_t Aindex;
  Table::index_t Bindex;

    // запретить только truncate, но разрешён insert и доступ для чтения
  std::shared_lock Alock(A.selMutex(), std::defer_lock);
  std::shared_lock Block(B.selMutex(), std::defer_lock);
  const std::scoped_lock insert_lock(Alock, Block);

  {
      // эксклюзивный доступ на короткий срок, чтобы скопировать индексы
    const std::scoped_lock exclusive_lock(A.insMutex(), B.insMutex());
      // копируем индексы, чтобы не зависеть от insert
    Aindex = A.index();
    Bindex = B.index();
  }

    // общий цикл (а не два) чтобы отсорировать по id
  auto ait = Aindex.cbegin();
  auto bit = Bindex.cbegin();
  auto diffA = [&Bindex, &ait, &result]() {
    auto curit = Bindex.find(ait->first);
    if(curit == Bindex.end()) {
      result << ait->first << "," << ait->second->m_str << ",\n";
    }
    ait++;
  };
  auto diffB = [&Aindex, &bit, &result]() {
    auto curit = Aindex.find(bit->first);
    if(curit == Aindex.end()) {
      result << bit->first << ",," << bit->second->m_str << "\n";
    }
    bit++;
  };


  while(true)
  {
    const bool Aend = ait == Aindex.cend();
    const bool Bend = bit == Bindex.cend();
    if(Aend && Bend) {
      break;
    }
    if(!Aend && !Bend) {
      if(ait->first < bit->first) diffA();
      else diffB();
    }
    else if(!Aend) diffA();
    else if(!Bend) diffB();
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
