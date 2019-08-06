#include "prism.h"

PRISM::PRISM()
{

}



PRISM::~PRISM()
{

}

void PRISM::power()
{
    TP = 0;
    NP = 0;
    F1 = 0;
    F2 = 0;
    for (unsigned int i = 0; i < ND.size(); ++i)
    {
        ND[i].power();
        TP+=ND[i].TP;
        NP+=ND[i].NP;
        F1+= ND[i].F1 / ND.size();
        F2+= ND[i].F2 / ND.size();

    }

}

void PRISM::absorbers()
{
    double div_time = 0.042;
    int steps = int (myCore->dTIME/div_time);
    double less_part = myCore->dTIME - div_time * steps;
    double lxe = XE, lsm = SM, lpm = PM, li = I;
    double SfF =  (Sf1 * F1 + Sf2 * F2);

    I_eq  = gI * SfF / (myCore->LI / myCore->BARN);
    XE_eq = (gI + gXE) * SfF / (myCore->LXE / myCore->BARN  + saX1 * F1 + saX2 * F2);
    PM_eq = gPM * SfF / (myCore->LPM / myCore->BARN );
    SM_eq = gPM * SfF / (saS1 * F1 + saS2 * F2);

    for (unsigned int t = 0; t < steps; ++t)
    {

        li  += (gI *  SfF  - myCore->LI/myCore->BARN * li) * div_time * myCore->BARN * 24 * 3600.0;
        lxe += (myCore->LI/myCore->BARN * li + gXE * SfF  - myCore->LXE/myCore->BARN * lxe - lxe * (saX1 * F1 + saX2 * F2)) * div_time * myCore->BARN * 24 * 3600.0;
        lpm += (gPM * SfF - myCore->LPM / myCore->BARN * lpm) * div_time * myCore->BARN * 24 * 3600.0;
        lsm += (myCore->LPM / myCore->BARN * lpm - lsm * (saS1 * F1 + saS2 * F2))* div_time * myCore->BARN * 24 * 3600.0;
    }

    li  += (gI *  SfF  - myCore->LI/myCore->BARN * li) * less_part * myCore->BARN * 24 * 3600.0;
    lxe += (myCore->LI/myCore->BARN * li + gXE * SfF  - myCore->LXE/myCore->BARN * lxe - lxe * (saX1 * F1 + saX2 * F2)) * less_part * myCore->BARN * 24 * 3600.0;
    lpm += (gPM * SfF - myCore->LPM / myCore->BARN * lpm) * less_part * myCore->BARN * 24 * 3600.0;
    lsm += (myCore->LPM / myCore->BARN * lpm - lsm * (saS1 * F1 + saS2 * F2))* less_part * myCore->BARN * 24 * 3600.0;

    dXE = lxe - XE;
    dI  = li - I;
    dSM = lsm - SM;
    dPM = lpm - PM;



    if(myCore->fXe == CORE::eq)
        XE = XE_eq;
    else if(myCore->fXe == CORE::null)
        XE = 0;

    if(myCore->fSm == CORE::null)
        SM = 0;
    else if(myCore->fSm == CORE::eq)
        SM = SM_eq;
    else if(myCore->fSm == CORE::SMandPM)
        SM = SM+PM;
    else if(myCore->fSm == CORE::SMandPM_eq)
        SM = SM_eq + PM_eq;

}

void PRISM::burnout()
{
    SL += TP * myCore->dTIME / (myAssembly->MU * part);
    SL_f = SL;
    for (unsigned int pin = 0; pin < myCore->NPIN; pin++)
        PinSl[pin]+= pin_power[pin] * myCore->dTIME / (myAssembly->MU * part / myCore->NFPIN);

}

void PRISM::find_adj_node()
{
    double R = pow(S*4.0/3.0/myCore->SQRT3 / myCore->DIV,0.5);
    double t = pow(S*4.0/myCore->SQRT3/ myCore->DIV,0.5);
    double x, y, ran;

    for (unsigned int i = 0 ; i < ND.size(); i++)
    {
        for (unsigned int j = 0 ; j < ND.size(); j++)
        {
            x = abs(ND[i].X - ND[j].X);
            y = abs(ND[i].Y - ND[j].Y);
            ran =pow(x*x+y*y,0.5);
            if ( ran< 1.5 * R && i!=j)
            {
                ND[i].ADJ_R_NODE.push_back(&ND[j]);
                ND[i].contact_r.push_back(t * height);
                ND[i].range_r.push_back(ran / 2.0);
            }
        }

        for (unsigned int n =0; n < ADJ_PRISM.size(); n ++)
            for (unsigned int j = 0; j < ADJ_PRISM[n]->ND.size(); j++)
            {
                x = abs(ND[i].X - ADJ_PRISM[n]->ND[j].X);
                y = abs(ND[i].Y - ADJ_PRISM[n]->ND[j].Y);
                ran =pow(x*x+y*y,0.5);
                if ( ran< 1.5 * R)
                {
                    ND[i].ADJ_R_NODE.push_back(&(ADJ_PRISM[n]->ND[j]));
                    ND[i].contact_r.push_back(t * height);
                    ND[i].range_r.push_back(ran / 2.0);
                }
            }

        //axial neighbors
        if (h == 0)
        {
            ND[i].ADJ_A_NODE.push_back(&(myAssembly->PRI[h+1].ND[i]));
            ND[i].contact_a.push_back(S/myCore->DIV);
            ND[i].range_a.push_back(height/2.0);
        }
        else if (h == (myCore->NL-1))
        {
            ND[i].ADJ_A_NODE.push_back(&(myAssembly->PRI[h-1].ND[i]));
            ND[i].contact_a.push_back(S/myCore->DIV);
            ND[i].range_a.push_back(height/2.0);
        }
        else
        {
            ND[i].ADJ_A_NODE.push_back(&(myAssembly->PRI[h+1].ND[i]));
            ND[i].contact_a.push_back(S/myCore->DIV);
            ND[i].range_a.push_back(height/2.0);

            ND[i].ADJ_A_NODE.push_back(&(myAssembly->PRI[h-1].ND[i]));
            ND[i].contact_a.push_back(S/myCore->DIV);
            ND[i].range_a.push_back(height/2.0);
        }

        ND[i].preDif.hRad =  ND[i].contact_r[0] / ( ND[i].range_r[0] * 2.0 ) * 2.0;
        ND[i].preDif.hAxi =  ND[i].contact_a[0] /  ND[i].range_a[0];
        ND[i].numRadNei = ND[i].ADJ_R_NODE.size();
        ND[i].numAxiNei = ND[i].ADJ_A_NODE.size();

        ND[i].preDif.DDR1.resize(ND[i].numRadNei);
        ND[i].preDif.DDR2.resize(ND[i].numRadNei);

        ND[i].preDif.DpRNei1.resize(ND[i].numAxiNei);
        ND[i].preDif.DpRNei2.resize(ND[i].numAxiNei);

    }




}

void PRISM::division()
{
    std::vector<int> numb;
    std::vector<double> y_start;
    std::vector<double> x_cur;
    double t, R;
    int cnt = 0;
    ND.resize(myCore->DIV);
    t = pow(4.0*S/myCore->DIV/myCore->SQRT3,0.5);
    R = pow(4.0*S/myCore->DIV/myCore->SQRT3/3.0,0.5);

    switch (myCore->DIV)
    {
    case 1:
        numb.insert(numb.end(),  {1});
        y_start.insert(y_start.end(),  {0});
        x_cur.insert(x_cur.end(),  {0});
    break;

    case 6:
        numb.insert(numb.end(),  {1, 2, 2, 1});
        y_start.insert(y_start.end(),  {0, 0.5*t, 0.5*t, 0});
        x_cur.insert(x_cur.end(),  {-R, -0.5*R, 0.5*R, R});
    break;

    case 24:
        numb.insert(numb.end(),  {2, 3, 3, 4, 4, 3, 3, 2});
        y_start.insert(y_start.end(),  {0.5*t, t, t, 1.5*t, 1.5*t, t, t, 0.5*t});
        x_cur.insert(x_cur.end(),  {-2.5*R, -2*R, -R, -0.5*R, 0.5*R, R, 2*R, 2.5*R});
    break;

    case 54:
        numb.insert(numb.end(),  {3, 4, 4, 5, 5, 6, 6, 5, 5, 4, 4, 3});
        y_start.insert(y_start.end(),  {t, 1.5*t, 1.5*t, 2*t, 2*t, 2.5*t, 2.5*t, 2*t, 2*t, 1.5*t, 1.5*t, t});
        x_cur.insert(x_cur.end(),  {-4*R, -3.5*R, -2.5*R, -2*R, -R, -0.5*R, 0.5*R, R, 2*R, 2.5*R, 3.5*R, 4*R});
    break;

    case 96:
        numb.insert(numb.end(),  {4, 5, 5, 6, 6, 7, 7, 8, 8, 7, 7, 6, 6, 5, 5, 4});
        y_start.insert(y_start.end(),  {1.5*t, 2*t, 2*t, 2.5*t, 2.5*t, 3*t, 3*t, 3.5*t, 3.5*t, 3*t, 3*t, 2.5*t, 2.5*t, 2*t, 2*t, 1.5*t});
        x_cur.insert(x_cur.end(),  {-5.5*R, -5*R, -4*R, -3.5*R, -2.5*R, -2*R, -R, -0.5*R, 0.5*R, R, 2*R, 2.5*R, 3.5*R, 4*R, 5*R, 5.5*R});
    break;

    default:
        myCore->write_to_log( "Wrong number of divide " + to_string(myCore->DIV) + ". Only in 1, 6, 24, 54, 96" );
    }

    for (unsigned int d =0; d < myCore->DIV; d++)
    {
        ND[d].height = height;
        ND[d].myCore = myCore;
        ND[d].myPrism = this;
        ND[d].Z = Z;
        ND[d].S = S / myCore->DIV;
        ND[d].d = d+1;
        ND[d].V = ND[d].S * ND[d].height;
        ND[d].my_number = d + (n-1) * myCore->DIV + h * myCore->DIV * myCore->NFA;
    }
    for (unsigned int col = 0; col < numb.size(); col++)
        for (unsigned int row = 0; row < numb[col]; row++)
        {
            ND[cnt].X = x_cur[col] + X;
            ND[cnt++].Y = -y_start[col] + t*row + Y;
        }

    //pin positions

    double x_step = pow(3.0,0.5)/2.0 * myCore->pin_step;
    double y_step = myCore->pin_step;


    vector<int> mas = {11,12,13,14,15,16,17,18,19,20,21,20,19,18,17,16,15,14,13,12,11};
    double left = -10.0*x_step;

    for (unsigned int i = 0; i < mas.size(); i++)
    {
        double x_start = left + i * x_step;
        double y_start = (mas[i]/2.0 - 0.5) * y_step;
        for (unsigned int j =0;  j < mas[i]; j++)
        {
            pin_x.push_back(x_start+X);
            pin_y.push_back(Y-(y_start-j*y_step));
        }
    }



}

double PRISM::internal_iteration()
{
    F1 = 0;
    F2 = 0;
    double delta_for_prism = 0;
    for(int d = 0; d < myCore->DIV; ++d) {
        delta_for_prism += ND[d].internal_iteration();
        F1 += ND[d].F1;
        F2 += ND[d].F2;
    }
    F1 /= myCore->DIV;
    F2 /= myCore->DIV;

    return delta_for_prism / myCore->DIV;
}

void PRISM::normalization()
{
    TP = 0;
    NP = 0;

    for (unsigned int d = 0; d < myCore->DIV; ++d)
    {
        ND[d].TP *= myCore->HEAT_POWER / myCore->TP;
        ND[d].NP *= myCore->HEAT_POWER / myCore->TP;

        TP += ND[d].TP;
        NP += ND[d].NP;
    }
}

void PRISM::poisoning_xe_sm()
{

    ro_XE = -(saX1 * F1 + saX2 * F2) * XE / (Sa1 * F1 + Sa2 * F2);
    ro_SM = -(saS1 * F1 + saS2 * F2) * SM / (Sa1 * F1 + Sa2 * F2);

    ro_XE_eq = -(saX1 * F1 + saX2 * F2) * XE_eq / (Sa1 * F1 + Sa2 * F2);
    ro_SM_eq = -(saS1 * F1 + saS2 * F2) * SM_eq / (Sa1 * F1 + Sa2 * F2);


}


void PRISM::prepareForDifCal() {

    for ( int d = 0; d < myCore->DIV; d++ ) {
        ND[d].preDif.aD1  = 0;
        ND[d].preDif.aD2  = 0;

        for ( int r = 0; r < ND[d].numRadNei; r++ ) {
            ND[d].preDif.DDR1[r] = ND[d].preDif.hRad * D1 * ND[d].ADJ_R_NODE[r]->myPrism->D1
                    / (D1 + ND[d].ADJ_R_NODE[r]->myPrism->D1);
            ND[d].preDif.DDR2[r] = ND[d].preDif.hRad * D2 * ND[d].ADJ_R_NODE[r]->myPrism->D2
                    / (D2 + ND[d].ADJ_R_NODE[r]->myPrism->D2);

            ND[d].preDif.aD1 += ND[d].preDif.DDR1[r];
            ND[d].preDif.aD2 += ND[d].preDif.DDR2[r];

        }

        if (ND[d].numRadNei < 3) {

            ND[d].preDif.BR1 = D1 / (ND[d].range_r[0] * 2.0) * (1 + ND[d].alb_side_11)
                    / (D1 / (ND[d].range_r[0] * 2.0) *(1 + ND[d].alb_side_11) + (1 - ND[d].alb_side_11) / 4.0 );
            ND[d].preDif.BR2 = D2 / (ND[d].range_r[0] * 2.0) * (1 + ND[d].alb_side_22);
            ND[d].preDif.BR3 = ND[d].alb_side_12 / 4.0;
            ND[d].preDif.BR4 = D1 / (ND[d].range_r[0] * 2.0)* ND[d].alb_side_12;
            ND[d].preDif.BR5 = (D2 / (ND[d].range_r[0] * 2.0) *(1 + ND[d].alb_side_22) + (1 - ND[d].alb_side_22) / 4.0);

            ND[d].preDif.aD1 += ND[d].preDif.hRad * D1;
            ND[d].preDif.aD2 += ND[d].preDif.hRad * D2;
        }

        ND[d].preDif.DpR1 = D1 / ND[d].range_a[0];
        ND[d].preDif.DpR2 = D2 / ND[d].range_a[0];
        ND[d].preDif.DDA1 = ND[d].preDif.hAxi * D1;
        ND[d].preDif.DDA2 = ND[d].preDif.hAxi * D2;

        ND[d].preDif.aD1 += ND[d].preDif.DDA1 * 2.0;
        ND[d].preDif.aD2 += ND[d].preDif.DDA2 * 2.0;

        for ( int a = 0; a < ND[d].numAxiNei; a++ ) {
            ND[d].preDif.DpRNei1[a] = ND[d].preDif.DpR1 + ND[d].ADJ_A_NODE[a]->myPrism->D1 /  ND[d].ADJ_A_NODE[a]->range_a[0];
            ND[d].preDif.DpRNei2[a] = ND[d].preDif.DpR2 + ND[d].ADJ_A_NODE[a]->myPrism->D2 /  ND[d].ADJ_A_NODE[a]->range_a[0];
        }

        if (ND[d].numAxiNei < 2) {
            double A11 = 0;
            double A12 = 0;
            double A22 = 0;

            if ( h == myCore->NL-1 ) {
                A11 = ND[d].alb_top_11;
                A12 = ND[d].alb_top_12;
                A22 = ND[d].alb_top_22;
            }
            else if ( h == 0 ) {
                A11 = ND[d].alb_bottom_11;
                A12 = ND[d].alb_bottom_12;
                A22 = ND[d].alb_bottom_22;
            }

            ND[d].preDif.BA1 = D1 / (ND[d].range_a[0] * 2.0) * (1.0 + A11) /
                       (D1 / (ND[d].range_a[0] * 2.0) * (1.0 + A11) + (1.0 - A11) / 4.0 );

            ND[d].preDif.BA2 = D2 / (ND[d].range_a[0] * 2.0) * (1 + A22);
            ND[d].preDif.BA3 = A12 / 4.0;
            ND[d].preDif.BA4 = D1 / (ND[d].range_a[0] * 2.0) * A12;
            ND[d].preDif.BA5 = D2 / (ND[d].range_a[0] * 2.0) * (1 + A22) + (1 - A22) / 4.0;


        }
        ND[d].preDif.S_rem1 = ND[d].preDif.aD1 + ND[d].V * (Sa1 + S12);
        ND[d].preDif.S_rem2 = ND[d].preDif.aD2 + ND[d].V * Sa2;
    }
}



