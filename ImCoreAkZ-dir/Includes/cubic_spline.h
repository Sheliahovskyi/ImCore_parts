#ifndef CUBIC_SPLINE_H
#define CUBIC_SPLINE_H

#include <cstdlib>
#include <cmath>
#include <limits>
#include <vector>
#include <iostream>

class cubic_spline
{
private:
    // ���������, ����������� ������ �� ������ �������� �����
    struct spline_tuple
    {
        double a, b, c, d, x;
    };

    spline_tuple *splines; // ������
    std::size_t n; // ���������� ����� �����

    void free_mem(); // ������������ ������

public:
    cubic_spline(); //�����������
    ~cubic_spline(); //����������

    // ���������� �������
    // x - ���� �����, ������ ���� ����������� �� �����������, ������� ���� ���������
    // y - �������� ������� � ����� �����
    // n - ���������� ����� �����

    void build_spline(const std::vector<double> x, const std::vector<double> y);

    // ���������� �������� ����������������� ������� � ������������ �����
    double f(double x) const;
};

#endif // CUBIC_SPLINE_H
