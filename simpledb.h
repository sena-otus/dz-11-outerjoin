#pragma once

#include "pcommand.h"
#include <map>
#include <string>
#include <string_view>
#include <functional>
#include <forward_list>
#include <set>
#include <mutex>
#include <shared_mutex>

/** @brief простейшая СУБД */
class SimpleDB
{
  struct Record
  {
    int m_id;
    std::string m_str;
  };

  class Table
  {
    using container_t = std::forward_list<Record>;
    using iterator = container_t::iterator;
  public:
    using const_iterator = container_t::const_iterator;
    using index_t = std::unordered_map<int, const Record*>;

    std::string insert(Record &&rec);
    std::string truncate();
    // [[nodiscard]] iterator begin() {return m_reclist.begin();}
    // [[nodiscard]] iterator end() {return m_reclist.end();}
    [[nodiscard]] const_iterator cbegin() const {return m_reclist.cbegin();}
    [[nodiscard]] const_iterator cend() const {return m_reclist.cend();}
    std::shared_mutex& selMutex() { return m_selMutex;}
    std::mutex& insMutex() { return m_insMutex;}
    index_t index();
  private:
    std::mutex m_insMutex;
    std::shared_mutex m_selMutex;
    container_t m_reclist; ///!< список позвляет добавлять без инвалидации итераторов
  };


public:

    /** регистрирует доступные команды */
  SimpleDB();

    /** исполнить запрос и вернуть ответ, поддерживаемые команды
     * - INSERT <table name> <id> <string>
     * - TRUNCATE <table name>
     * - INTERSECTION
     * - SYMMETRIC_DIFFERENCE
     * @return строка начинается с ERR при ошибке, заканчивается на OK
     *         при успехе, в конце всегда '\n'.
     **/
  std::string exec(std::string_view cmdbuf);

    /** вставка строки в таблицу, создаёт таблицу если она отсутствует */
  std::string insert(const PCommand &cmd);
    /** очистка таблицы */
  std::string truncate(const PCommand &cmd);
    /** пересечение таблиц A, B по полю id */
  std::string intersection(const PCommand &cmd);
    /** симметрическая разность таблиц A, B по полю id  */
  std::string sym_diff(const PCommand &cmd);

private:
  std::map<std::string, Table> m_table;
  std::map<std::string, std::function<std::string(PCommand&)>> m_cmdmap;
};
