#pragma once

#include <string_view>
#include <vector>
#include <string>

/** @brief  разобранная команда */
class PCommand
{
public:
    /** разобранная команда
     * @param cmdbuf буфер с командой для разбора
     * */
  explicit PCommand(std::string_view  cmdbuf);

    /** вернуть аргумент по индексу, пустая строка если аргумента нет */
  [[nodiscard]] std::string arg(unsigned ii) const;
    /** количество аргументов */
  [[nodiscard]] unsigned size() const;
private:
  std::vector<std::string> m_args;
};
