#pragma once

#include "pcommand.h"
#include <map>
#include <string>
#include <string_view>
#include <functional>

/** @brief простейшая СУБД */
class SimpleDB
{
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
  std::map<std::string, std::map<int, std::string>> m_table;
  std::map<std::string, std::function<std::string(PCommand&)>> m_cmdmap;
};
