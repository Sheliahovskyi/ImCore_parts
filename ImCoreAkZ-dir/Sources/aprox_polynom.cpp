#include "aprox_polynom.h"
 

double APROX_POLYNOM::Power( double v,int p)
{
    double res = 1;
    for (int i = 0; i < p; i++)
        res*=v;
    return res;
}


APROX_POLYNOM::APROX_POLYNOM()
{

}

APROX_POLYNOM::APROX_POLYNOM(vector<double> x, vector<double> y, int pow)
{
    make_coef(x,y,pow);
}

void APROX_POLYNOM::make_coef(vector<double> x, vector<double> y, int pow)
{
    if(x.size() != y.size())
    {
        cout << "X and Y has different length" << endl;
    }
    int i,k,  j, pts = y.size();
    double s,M;
    double tx, ty;
    vector<vector<double>> sums(pow+1, vector<double>(pow+1, 0));
    vector<double> b(pow+1, 0);
    coef_of_poly.resize(pow+1);

    //упорядочиваем узловые точки по возрастанию абсцисс
    for(unsigned int i=1; i< x.size(); i++)
    {

        tx = x[i];
        ty = y[i];
        j = i -1;
        while(j >= 0 && x[j] > tx)
        {
            x[j+1] = x[j];
            y[j+1] = y[j];
            j--;
        }
        x[j+1] = tx;
        y[j+1] = ty;

    }
    //заполняем коэффициенты системы уравнений
    for(i=0; i<pow+1; i++)
    {
        for(j=0; j<pow+1; j++)
        {
            sums[i][j] = 0;
            for(k=0; k<pts; k++)
                sums[i][j]+=Power(x[k],i+j);
        }
    }
    //заполняем столбец свободных членов
    for(i=0; i<pow+1; i++)
    {
        b[i]=0;
        for(k=0; k<pts; k++)
            b[i] +=Power(x[k],i)*y[k];
    }
    //применяем метод Гаусса для приведения матрицы системы к треугольному виду
    for(k=0; k<pow+1; k++)
    {
        for(i=k+1; i<pow+1; i++)
        {
            M=sums[i][k]/sums[k][k];
            for(j=k; j<pow+1; j++)
                sums[i][j] -= M * sums[k][j];
            b[i] -= M*b[k];
        }
    }
    //вычисляем коэффициенты аппроксимирующего полинома
    for(i=pow;i>=0;i--)
    {
        s=0;
        for(j = i; j<pow+1; j++)
          s+=sums[i][j]*coef_of_poly[j];
        coef_of_poly[i] = (b[i]-s)/sums[i][i];
    }

}

vector<double> APROX_POLYNOM::give_coef()
{
    return coef_of_poly;
}

vector<double> APROX_POLYNOM::calc_value(vector<double> x)
{
    vector<double> res(x.size(), 0);

    for (unsigned int i=0; i<x.size(); i++)
    {
        for (unsigned int j = 0; j < coef_of_poly.size(); j++)
            res[i] +=  coef_of_poly[j] * Power(x[i], j);
    }
    return res;
}
