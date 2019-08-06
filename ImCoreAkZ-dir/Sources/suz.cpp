#include "suz.h"

SUZ::SUZ()
{}

void SUZ::read_suz_data(std::string file_name)
{
    vector<vector<double>> suz;
    vector<double> a1;
    string str;
    double num;
    std::ifstream inFile;
    inFile.open(file_name);
    if(!inFile)
    {

    }
    while(std::getline(inFile, str))
    {
        istringstream iss(str);
        while(iss >> num)
            a1.push_back(num);
        suz.push_back(a1);
        a1.clear();
    }
    RODS.resize(suz.size());

    int max_group = 0;
    for (unsigned int i = 0; i < RODS.size(); i++)
    {
        RODS[i].assembly = suz[i][0] - 1;
        RODS[i].group = suz[i][1];
        RODS[i].position = suz[i][2];
        if(RODS[i].group > max_group)
            max_group = RODS[i].group;
        for (unsigned int j = 0; j < suz[i].size()-3; j += 2)
        {
            RODS[i].type_length.push_back(suz[i][j+3]);
            RODS[i].type.push_back(suz[i][j+4]);
        }
    }

    GROUPS.resize(max_group);
}

void SUZ::move_group(int n, double position)
{
    for(int i = 0; i < RODS.size(); i++)
    {
        if(RODS[i].group == n)
            RODS[i].position = position;
    }
}

void SUZ::emergency_protection(double NJU)
{
    for(int i = 0; i < RODS.size(); i++)
        RODS[i].position = NJU;
}

double SUZ::give_group_position(int group)
{
    int cnt = 0;
    double sum = 0;
    for (unsigned int r = 0; r < RODS.size();r++)
        if (RODS[r].group == group)
        {
            cnt++;
            sum += RODS[r].position;
        }

    return sum / cnt;

}

vector<double> SUZ::suz_position(double z, double height, int n)
{
    int num_of_type = RODS[n].type_length.size();
    vector<double> parts(num_of_type+1), point;
    point.push_back(RODS[n].position);
    for (unsigned int i = 0; i < num_of_type; ++i)
        point.push_back(point[i] + RODS[n].type_length[i]);

    double bottom = z-height/2;
    double top = z+height/2;
    double up = 0;
    double dw = 0;
    double sum = 0;
    for (unsigned int i = 0; i < num_of_type; i++)
    {
        if(point[i]<bottom)
            dw = bottom;
        else
            dw = point[i];
        if (point[i+1] < bottom)
            up = bottom;

        if (point[i+1] > top)
            up =top;
        else
            up = point[i+1];

        if (point[i] > top)
            dw = top;

        if (up - dw <= 0)
            parts[RODS[n].type[i]] = 0;
        else
            parts[RODS[n].type[i]] = (up - dw) / height;
        sum +=  parts[RODS[n].type[i]];
    }
    parts[0] = 1 - sum;

    for (unsigned int i = 0; i< parts.size(); ++i)
        if (parts[i]< 1e-7)
            parts[i] = 0;


    return parts;
}



