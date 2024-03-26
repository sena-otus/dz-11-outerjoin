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
    /** @brief запись (структура) таблиц */
  struct Record
  {
    int m_id;
    std::string m_str;
  };

    /** @brief таблица БД */
  class Table
  {
      /** записи таблицы хранятся в однонаправленном списке,
       * он экономит потребление памяти и позволяет
       * делать вставки без инвалидации итераторов */
    using container_t = std::forward_list<Record>;

      // using iterator = container_t::iterator;
  public:
    using const_iterator = container_t::const_iterator; //<! итератор по таблице
    using index_t = std::map<int, const Record*>; //<! индекс хранит указатель на запись

      /** @brief вставка новой записи.
       * может исполнятся параллельно с запросами INTERSECTION,
       * SYMMETRIC_DIFFERENCE
       * @param rec запись для вставки
       * @return сообщение об ошибке или успех
       * */
    std::string insert(Record &&rec);

      /** @brief очистка таблицы
       * @return сообщение об ошибке или OK при успехе
       * */
    std::string truncate();
    [[nodiscard]] const_iterator cbegin() const {return m_reclist.cbegin();}
    [[nodiscard]] const_iterator cend() const {return m_reclist.cend();}
    std::shared_mutex& selMutex() { return m_selMutex;}
    std::mutex& insMutex() { return m_insMutex;}
    index_t& index();
  private:
    std::mutex m_insMutex; //!< мьютекс для эксклюзивного доступа (INSERT, TRUNCATE)
    std::shared_mutex m_selMutex; //!< мьютекс доступ для чтения (INTERSECTION, SYMMETRIC_DIFFERENCE)
    container_t m_reclist; //!< список позвляет добавлять без инвалидации итераторов
    index_t m_index;  //!< индекс для ускорения поиска
  };


public:

    /** конструктор регистрирует доступные команды */
  SimpleDB();

    /** @brief исполнить запрос и вернуть ответ.
     * Поддерживаемые команды
     * - INSERT <table name> <id> <string>
     * - TRUNCATE <table name>
     * - INTERSECTION
     * - SYMMETRIC_DIFFERENCE
     * @param cmdbuf полная команда в виде строки
     * @return строка начинается с ERR при ошибке, заканчивается на OK
     *         при успехе, в конце всегда '\n'.
     **/
  std::string exec(std::string_view cmdbuf);

    /** @brief вставка строки в таблицу
     * может исполнятся параллельно с запросами INTERSECTION,
     * SYMMETRIC_DIFFERENCE
     * @param cmd аргументы <имя_таблицы> <целое> <строка>
     * @return сообщение об ошибке или OK при успехе
     * */
  std::string insert(const PCommand &cmd);
    /** @brief очистка таблицы
     * Требует эксклюзивный доступ к таблице, поэтому будет ждать
     * окончания команд INTERSECTION, SYMMETRIC_DIFFERENCE, INSERT
     * @param cmd аргументы <имя_таблицы>
     * @return сообщение об ошибке или OK при успехе
     * */
  std::string truncate(const PCommand &cmd);

    /** @brief пересечение таблиц A, B по полю id.
     * Может исполнятся параллельно с INSERT, SYMMETRIC_DIFFERENCE
     * @param cmd пустой список
     * @return сообщение об ошибке или OK при успехе
     * */
  std::string intersection(const PCommand &cmd);

    /** @brief симметрическая разность таблиц A, B по полю id.
     * Может исполнятся параллельно с INSERT, INTERSECTION
     * @param cmd пустой список
     * @return сообщение об ошибке или OK при успехе
     * */
  std::string sym_diff(const PCommand &cmd);

private:
  std::map<std::string, Table> m_table; //!< мапа таблиц
  std::map<std::string, std::function<std::string(PCommand&)>> m_cmdmap; //!< мапа команд
};
