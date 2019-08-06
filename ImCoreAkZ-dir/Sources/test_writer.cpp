#include "test_writer.h"


/*!
 * \brief test_writer::test_writer �����������
 * \param path  ��������� ��� ����� � ��������� �� ����������� ��������
 */
test_writer::test_writer(std::string path)
{
    std::ofstream file(path, std::ios_base::out);
    flag = 1;

    if (file)
        file_name = path;
    else
        std::cout << "�� �������� ������� " << path << std::endl;
}


/*!
 * \brief test_writer::write ����� ������
 * \param information   �������� ������, ������� ����� �������� � ����
 */
void test_writer::write(std::string information)
{

    while(!flag)
    {
        usleep(10000);
    }

    flag = 0;

    std::ofstream file(file_name, std::ios_base::app);
    file << information << std::endl;

    file.close();

    flag = 1;


}
