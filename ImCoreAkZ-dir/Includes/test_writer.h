#pragma once

#include <fstream>
#include <string>
#include <iostream>
#include <unistd.h>

/*!
 * \brief The test_writer class Необходим для корректной записи информации при работе нескольких потоков
 * \author Шлапак Игорь
 * \date 30.10.2018
 */

class test_writer
{
    std::string file_name;          ///< Имя пути, гдн будет вестись запись
    unsigned int flag;              ///< Флаг доступа к потоку вывода

public:
    test_writer(std::string path_name);
    void write(std::string information);
};
