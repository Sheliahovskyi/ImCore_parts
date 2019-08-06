#pragma once

#include <fstream>
#include <string>
#include <iostream>
#include <unistd.h>

/*!
 * \brief The test_writer class ��������� ��� ���������� ������ ���������� ��� ������ ���������� �������
 * \author ������ �����
 * \date 30.10.2018
 */

class test_writer
{
    std::string file_name;          ///< ��� ����, ��� ����� ������� ������
    unsigned int flag;              ///< ���� ������� � ������ ������

public:
    test_writer(std::string path_name);
    void write(std::string information);
};
