#include "xs.h"

XS::XS()
{}

XS::~XS()
{}

double myPow (double arg, int pow)
{
    double res = 1;
    for (unsigned int i = 0; i < pow; ++i)
        res *= arg;
    return res;
}

void XS::read_cs_data(std::string file_name)
{
    CS.clear();
    std::ifstream inFile;
    inFile.open(file_name);
    std::string stri;
    double num;
    vector<double> tmp;
    int cnt_line = 1;
    if(!inFile)
    {
        myCore->write_to_log( "cannot open " + file_name);
    }

    CS.push_back(tmp);
    while(std::getline(inFile, stri))
    {
        CS.push_back(tmp);
        istringstream iss(stri);
        while(iss >> num){
            CS[cnt_line].push_back(num);
        }
        cnt_line++;
    }
}

void XS::read_therm_data(std::string wd_file_name, std::string cp_file_name)
{
    CP_bank.clear();
    WD_bank.clear();
    temp_range.clear();
    press_range.clear();
    vector<double> a1, a2;
    string cpstr, wdstr;
    double cpnum, wdnum;

    ifstream cpin(cp_file_name); ifstream wdin(wd_file_name);
    if(!cpin)
    {
        myCore->write_to_log("cannot open " + cp_file_name);
    }
    if(!wdin)
    {
        myCore->write_to_log("cannot open " + wd_file_name);
    }

    getline(cpin, cpstr);
    getline(wdin, wdstr);
    istringstream cpiss(cpstr);
    istringstream wdiss(wdstr);
    while (cpiss >> cpnum && wdiss >> wdnum)
        temp_range.push_back(cpnum);
    while (!cpin.eof()) {
        getline(cpin, cpstr);
        getline(wdin, wdstr);
        istringstream cpiss(cpstr);
        istringstream wdiss(wdstr);
        cpiss >> cpnum;
        wdiss >> wdnum;
        press_range.push_back(cpnum);
        while (cpiss >> cpnum && wdiss >> wdnum) {
            a1.push_back(cpnum);
            a2.push_back(wdnum);
        }
        CP_bank.push_back(a1);
        WD_bank.push_back(a2);
        a1.clear();
        a2.clear();
    }
}


void XS::read_pin_data(std::string file_name)
{
    PF.clear();
    std::ifstream inFile;
    inFile.open(file_name);
    std::string stri;
    double num;
    vector<double> tmp;
    int cnt_line = 1;
    if(!inFile)
    {
        myCore->write_to_log( "cannot open " + file_name);
    }
    PF.push_back(tmp);
    while(std::getline(inFile, stri))
    {
        PF.push_back(tmp);
        istringstream iss(stri);
        while(iss >> num){
            PF[cnt_line].push_back(num);  }
        cnt_line++;
    }
}

void XS::read_cimb_data(std::string file_name)
{
    CIMB_bank.clear();
    std::ifstream inFile;
    inFile.open(file_name);
    std::string stri;
    double num;
    vector<double> tmp;
    int cnt_line = 1;
    CIMB_bank.push_back(tmp);

    if(!inFile)
    {
        myCore->write_to_log("cannot open " + file_name);
    }

    while(std::getline(inFile, stri))
    {
        CIMB_bank.push_back(tmp);
        istringstream iss(stri);
        while(iss >> num){
            CIMB_bank[cnt_line].push_back(num);
        }
        cnt_line++;
    }

}

void XS::cs_update(PRISM & prism)
{


    int t = prism.type;
    int num_burn_point = CS[t][4] / CS[t][5] +1;

    int LOR = num_burn_point * CS[t][6] + (CS[t][8] + CS[t][9] + CS[t][10] + CS[t][11]) * (CS[t][12]+1) * CS[t][7]; //len constants string of one rod

    int bof = 14; // begin offset point in line
    int rof;      // rod offset point


    double dWd;
    double dFt;
    double dCb;
    double dWt;
    double total;
    double ofBurn;
    double base;


    if (prism.SL*1e3 > CS[t][4])
    {
        prism.myCore->write_to_log( "Prism has biggest burnup then constant base");
    }
    int x1 = int(prism.SL * 1e3 / CS[t][5]);
    int x2 = x1 + 1;

    double pWT = prism.WT - 575.0;
    double pFT = prism.FT - prism.WT;
    double pCB = prism.myCore->CB * prism.WD;
    double pWD = prism.WD - 0.7241;

    int dof = num_burn_point * CS[t][6];     // base number offset point
    int start = 0;

    prism.myAssembly->MU = CS[t][0] * 0.8815;

    for (unsigned int c = 0; c < CS[t][6]; ++c)
    {
        total = 0;
        for (unsigned int r = 0; r < CS[t][13]; ++r)
            if (prism.Rod_part[r]>0)
            {
                rof = LOR * r;
                start = bof+rof+dof;
                base = CS[t][bof + rof + num_burn_point * c + x1] +
                        (CS[t][bof + rof + num_burn_point * c + x2] - CS[t][bof + rof + num_burn_point * c + x1]) /
                        CS[t][5] * (prism.SL * 1e3 - x1 * CS[t][5]);

                if (c < CS[t][7])
                {
                    dWd = 1;
                    dFt = 1;
                    dCb = 1;
                    dWt = 1;

                    for (unsigned int d = 0; d < CS[t][8]; ++d)
                    {
                        ofBurn = 0;
                        for(int b = 0; b < CS[t][12]+1; ++b)
                            ofBurn += CS[t][start + (d * CS[t][7]  + c) * (CS[t][12]+1) + b] * myPow(prism.SL, CS[t][12]-b);
                        dWd += ofBurn *  myPow(pWD, CS[t][8] - d);
                    }

                    for (unsigned int d = 0; d < CS[t][9]; ++d)
                    {
                        ofBurn = 0;
                        for(int b = 0; b < CS[t][12]+1; ++b)
                            ofBurn += CS[t][start + ((CS[t][8] + d) * CS[t][7] + c) * (CS[t][12]+1) + b] * myPow(prism.SL, CS[t][12]-b);
                        dFt += ofBurn *  myPow(pFT, CS[t][9] - d);
                    }

                    for (unsigned int d = 0; d < CS[t][10]; ++d)
                    {
                        ofBurn = 0;
                        for(int b = 0; b < CS[t][12]+1; ++b)
                            ofBurn += CS[t][start + ((CS[t][8] + CS[t][9] + d) * CS[t][7] + c) * (CS[t][12]+1) + b] * myPow(prism.SL, CS[t][12]-b);
                        dCb += ofBurn *  myPow(pCB, CS[t][10] - d);
                    }

                    for (unsigned int d = 0; d < CS[t][11]; ++d)
                    {
                        ofBurn = 0;
                        for(int b = 0; b < CS[t][12]+1; ++b)
                            ofBurn += CS[t][start + ((CS[t][8] + CS[t][9] + CS[t][10] + d) * CS[t][7] + c) * (CS[t][12]+1) + b] * myPow(prism.SL, CS[t][12]-b);
                        dWt += ofBurn *  myPow(pWT, CS[t][11] - d);
                    }
                        base *= (dWd * dFt * dCb * dWt);

                }
                total+=(base * prism.Rod_part[r]);

            }

        switch (c)
        {
        case 0:
            prism.D1 = total ;
            break;
        case 1:
            prism.Sa1 = total ;
            break;
        case 2:
            prism.nSf1 = total ;
            break;
        case 3:
            prism.Sf1 = total;
            break;
        case 4:
            prism.S12 = total ;
            break;
        case 5:
            prism.D2 = total ;
            break;
        case 6:
            prism.Sa2 = total ;
            break;
        case 7:
            prism.nSf2 = total;
            break;
        case 8:
            prism.Sf2 = total ;
            break;
        case 9:
            prism.E = total ;
            break;
        case 10:
            prism.saX1 = total;
            prism.Sa1 += total * (prism.XE + prism.dXE);
            break;
        case 11:
            prism.saS1 = total;
            prism.Sa1 += total * (prism.SM + prism.dSM);
            break;
        case 12:
            prism.saX2 = total;
            prism.Sa2 += total * (prism.XE + prism.dXE);
            break;
        case 13:
            prism.saS2 = total;
            prism.Sa2 += total * (prism.SM + prism.dSM);
            break;
        case 14:
            prism.gI = total;
            break;
        case 15:
            prism.gXE = total ;
            break;
        case 16:
            prism.gPM = total ;
            break;
        }
    }
}

void XS::pin_update(PRISM & prism)
{

    int t = prism.type;
    int num_burn_point = PF[t][0] / PF[t][1] + 1;

    int LOR = num_burn_point * prism.myCore->NPIN; //len constants string of one rod

    int bof = 3; // begin offset point in line

    double total;

    if (prism.SL*1e3 > PF[t][0])
    {
        prism.myCore->write_to_log("Prism has biggest burnup then constant base");
    }
    int sl1 = int(prism.SL * 1e3 / PF[t][1]) * PF[t][1];
    int x1 = int(prism.SL * 1e3 / PF[t][1]) * prism.myCore->NPIN;
    int x2 = x1 + prism.myCore->NPIN;


    for (unsigned int p = 0; p < prism.myCore->NPIN; ++p)
    {
        total = 0;
        for (unsigned int r = 0; r < prism.Rod_part.size(); ++r)
            if (prism.Rod_part[r]>1e-5)
                total+= (PF[t][bof + LOR * 0 + x1 + p] +
                        (PF[t][bof + LOR * 0 + x2 + p] - PF[t][bof + LOR * 0 + x1 + p]) /
                        PF[t][1] * (prism.SL * 1e3 - sl1)) * prism.Rod_part[r];


        prism.pin_factors[p] = total;

    }

}

void XS::therm_update(PRISM & prism)
{
    prism.WD = wd_calc(prism.WT, prism.P);
    prism.CP = cp_calc(prism.WT, prism.P);
}

double XS::cp_calc(double T, double P)
{
    // *** Thermohydraulic parameters calculating ***
    int t_low_index  = 0;
    int t_high_index = 0;
    int p_low_index  = 0;
    int p_high_index = 0;

    double cp_1 = 0;
    double cp_2 = 0;


    double WT = T - myCore->CtoK;

    // Neighbors value for cp and wd matrix finding
    // TEMPERATURE:
    for (unsigned int i = 0; i < temp_range.size(); i++) {
        if ((WT - temp_range[i]) > 0 && (WT - temp_range[i]) < (temp_range[temp_range.size() - 1] - temp_range[0]) / (temp_range.size()-1))
        {
            t_low_index = i;
            t_high_index = i+1;
        }
        else if (WT - temp_range[i] == 0)
        {
            t_low_index = i;
            t_high_index = i+1;
        }
    }
    // PRESSURE:
    if (P > press_range[0])
    {
        for (unsigned int j = 0; j < press_range.size(); j++)
        {
            if(P < press_range[j])
            {
                p_low_index = j-1;
                p_high_index = j;
                break;
            }

        }
        if (p_low_index == p_high_index)
        {
            myCore->write_to_log("Pressure exit to max value");
        }
    }

    if(t_low_index == t_high_index || p_low_index == p_high_index)
    {
        myCore->write_to_log("Invalid thermohydraulic parameters updating!");
    }

    // Boiling check
    if(WD_bank[p_high_index][t_high_index] == 0)
    {myCore->write_to_log("Coolant is boiling!");}


    // *** local Cp value calculating: ***
    cp_1 = CP_bank[p_low_index][t_low_index] + (CP_bank[p_high_index][t_low_index] - CP_bank[p_low_index][t_low_index]) /
           (press_range[p_high_index] - press_range[p_low_index]) * (P - press_range[p_low_index]);
    cp_2 = CP_bank[p_low_index][t_high_index] + (CP_bank[p_high_index][t_high_index] - CP_bank[p_low_index][t_high_index]) /
           (press_range[p_high_index] - press_range[p_low_index]) * (P - press_range[p_low_index]);

    return cp_1 + (cp_2 - cp_1) / (temp_range[t_high_index] - temp_range[t_low_index]) * (WT - temp_range[t_low_index]);
}

double XS::wd_calc(double T, double P)
{
    // *** Thermohydraulic parameters calculating ***
    int t_low_index  = 0;
    int t_high_index = 0;
    int p_low_index  = 0;
    int p_high_index = 0;

    double wd_1 = 0;
    double wd_2 = 0;

    double WT = T - myCore->CtoK;

    // Neighbors value for cp and wd matrix finding
    // TEMPERATURE:
    for (unsigned int i = 0; i < temp_range.size(); i++) {
        if ((WT - temp_range[i]) > 0 && (WT - temp_range[i]) < (temp_range[temp_range.size() - 1] - temp_range[0]) / (temp_range.size()-1))
        {
            t_low_index = i;
            t_high_index = i+1;
        }
        else if (WT - temp_range[i] == 0)
        {
            t_low_index = i;
            t_high_index = i+1;
        }
    }
    // PRESSURE:
    if (P > press_range[0])
    {
        for (unsigned int j = 0; j < press_range.size(); j++)
        {
            if(P < press_range[j])
            {
                p_low_index = j-1;
                p_high_index = j;
                break;
            }

        }
        if (p_low_index == p_high_index)
        {
            myCore->write_to_log("Pressure exit to max value");
        }
    }

    if(t_low_index == t_high_index || p_low_index == p_high_index)
    {
        myCore->write_to_log("Invalid thermohydraulic parameters updating!");
    }

    // Boiling check
    if(WD_bank[p_high_index][t_high_index] == 0)
    {myCore->write_to_log("Coolant is boiling!");}


    // *** local WD value calculating: ***
    wd_1 = WD_bank[p_low_index][t_low_index] + (WD_bank[p_high_index][t_low_index] - WD_bank[p_low_index][t_low_index]) /
           (press_range[p_high_index] - press_range[p_low_index]) * (P - press_range[p_low_index]);
    wd_2 = WD_bank[p_low_index][t_high_index] + (WD_bank[p_high_index][t_high_index] - WD_bank[p_low_index][t_high_index]) /
           (press_range[p_high_index] - press_range[p_low_index]) * (P - press_range[p_low_index]);

    return wd_1 + (wd_2 - wd_1) / (temp_range[t_high_index] - temp_range[t_low_index]) * (WT - temp_range[t_low_index]);
}
