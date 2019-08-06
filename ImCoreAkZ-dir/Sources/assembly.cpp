#include "assembly.h"

ASSEMBLY::ASSEMBLY()
{
    KC_7.resize(7);
    SL_7.resize(7);
    WD_7.resize(7);
    TPl_7.resize(7);
    Z7.resize(7);
}

ASSEMBLY::~ASSEMBLY()
{

}

void ASSEMBLY::init(CORE * core)
{
    myCore = core;
    NL = myCore->NL;
    PRI.resize(NL);
    PRI_parts.resize(7);
    height = myCore->HCORE;
    for (unsigned int h =0; h < NL; h++)
    {
        PRI[h].myCore = myCore;
        PRI[h].myAssembly = this;
        PRI[h].pin_factors.resize(myCore->NPIN);
        PRI[h].pin_power.resize(myCore->NPIN);
        PRI[h].PinSl.resize(myCore->NPIN);
    }

}



void ASSEMBLY::power()
{
    TP = 0;
    NP = 0;
    for (unsigned int h = 0; h < NL; ++h)
    {
        PRI[h].power();
        TP+=PRI[h].TP;
        NP+=PRI[h].NP;
    }

}
void ASSEMBLY::burnout()
{
    SL = 0;
    for (unsigned int h = 0; h < NL; ++h)
    {
        PRI[h].burnout();
        SL+= PRI[h].SL * PRI[h].part;
    }

}

void ASSEMBLY::absorbers()
{
    XE = 0;
    SM = 0;
    PM = 0;
    I  = 0;
    XE_eq = 0;
    SM_eq = 0;
    PM_eq = 0;
    I_eq  = 0;
    for (unsigned int h = 0; h < NL; ++h)
    {
        PRI[h].absorbers();
        XE      += PRI[h].XE * PRI[h].part;
        SM      += PRI[h].SM * PRI[h].part;
        PM      += PRI[h].PM * PRI[h].part;
        I       += PRI[h].I  * PRI[h].part;
        XE_eq   += PRI[h].XE_eq * PRI[h].part;
        SM_eq   += PRI[h].SM_eq * PRI[h].part;
        PM_eq   += PRI[h].PM_eq * PRI[h].part;
        I_eq    += PRI[h].I_eq  * PRI[h].part;
    }

    XE_f = XE;
    SM_f = SM;
    PM_f = PM;
    I_f  = I;
}

void ASSEMBLY::set_begin_value()
{
    WT_in = myCore->WT_in;
    for (unsigned int h =0; h < NL; ++h)
    {
        PRI[h].WT = 575.0;
        PRI[h].WD = 0.7231;
        PRI[h].FT = 575.0;



        for (unsigned int d =0; d < myCore->DIV; ++ d)
        {
            PRI[h].ND[d].F1 = 1E14;
            PRI[h].ND[d].F2 = 1E13;
        }
    }
}


void ASSEMBLY::albedo_put()
{

    vector<vector<int>> position = {{1,   0}, {1,   1}, {1,   2}, {2,   1}, {2,   2}, {3,   1},
                                    {3,   2}, {4,   1}, {4,   2}, {5,   1}, {5,   2}, {6,   1},
                                    {6,   2}, {6,   3}, {14,  2}, {15,  1}, {15,  2}, {15,  3},
                                    {25,  2}, {25,  3}, {36,  2}, {36,  3}, {48,  2}, {48,  3},
                                    {61,  2}, {61,  3}, {75,  2}, {75,  3}, {75,  4}, {88,  3},
                                    {102, 2}, {102, 3}, {102, 4}, {115, 3}, {115, 4}, {127, 3},
                                    {127, 4}, {138, 3}, {138, 4}, {148, 3}, {148, 4}, {157, 3},
                                    {157, 4}, {157, 5}, {156, 4}, {163, 3}, {163, 4}, {163, 5},
                                    {162, 4}, {162, 5}, {161, 4}, {161, 5}, {160, 4}, {160, 5},
                                    {159, 4}, {159, 5}, {158, 4}, {158, 5}, {158, 0}, {150, 5},
                                    {149, 4}, {149, 5}, {149, 0}, {139, 5}, {139, 0}, {128, 5},
                                    {128, 0}, {116, 5}, {116, 0}, {103, 5}, {103, 0}, {89,  5},
                                    {89,  0}, {89,  1}, {76,  0}, {62,  5}, {62,  0}, {62,  1},
                                    {49,  0}, {49,  1}, {37,  0}, {37,  1}, {26,  0}, {26,  1},
                                    {16,  0}, {16,  1}, {7,   0}, {7,   1}, {7,   2}, {8,   1}};
    vector<vector<int>> node_on_border;

    switch(myCore->DIV)
    {
    case 54:
        node_on_border = {{1, 2, 3}, {7, 16, 27}, {33, 43, 51}, {52, 53, 54}, {28, 39, 48}, {4, 12, 22}};
        break;
    case 24:
        node_on_border = {{1, 2}, {5, 12}, {16, 22}, {23, 24}, {13, 20}, {3, 9}};
        break;
    case 6:
        node_on_border = {{1}, {3}, {5}, {6}, {4}, {2}};
        break;

    }


    for (unsigned int h = 0; h < NL; ++h)
    {
        for (unsigned int a = 0; a < position.size(); ++a )
            if (n == position[a][0])
                for(unsigned int k = 0; k < node_on_border[position[a][1]].size(); ++k)
                {
                    PRI[h].ND[node_on_border[position[a][1]][k]-1].alb_side_11 = myCore->alb_side_11[a];
                    PRI[h].ND[node_on_border[position[a][1]][k]-1].alb_side_12 = myCore->alb_side_12[a];
                    PRI[h].ND[node_on_border[position[a][1]][k]-1].alb_side_22 = myCore->alb_side_22[a];
                }

        for (unsigned int i = 0; i < myCore->DIV; ++i)
        {
            if (h==0)
            {
                PRI[h].ND[i].alb_bottom_11 = myCore->alb_bottom_11[n-1];
                PRI[h].ND[i].alb_bottom_12 = myCore->alb_bottom_12[n-1];
                PRI[h].ND[i].alb_bottom_22 = myCore->alb_bottom_22[n-1];
            }
            else if (h == NL - 1)
            {
                PRI[h].ND[i].alb_top_11 = myCore->alb_top_11[n-1];
                PRI[h].ND[i].alb_top_12 = myCore->alb_top_12[n-1];
                PRI[h].ND[i].alb_top_22 = myCore->alb_top_22[n-1];
            }
        }
    }
}

void ASSEMBLY::division()
{
    int parts = 0;
    int cnt = 1;
    double length = 0;
    int Fid = 0;

    parts_length.clear();
    parts_type.clear();

    for(int n = 0; n < myCore->xs_data->fuel_base.size(); n++)
    {
        if(type == myCore->xs_data->fuel_base[n][0])
        {
            parts = (myCore->xs_data->fuel_base[n].size() - 1)/2;

            for(int part = 0; part < parts; part++)
            {
                stringstream l(myCore->xs_data->fuel_base[n][cnt++]);
                l >> length;
                parts_length.push_back(length);

                stringstream t(myCore->xs_data->fuel_base[n][cnt++]);
                t >> Fid;
                parts_type.push_back(Fid);
            }
        }
    }

    cnt = 0;
    int layer = 0;
    double level = 0;
    bool flag = true;

    if(parts == 1)
        for(unsigned int h = 0; h < NL; h++)
            PRI[h].type = parts_type[0];

    else
    {
        level = parts_length[0];
        while(flag)
        {
            if(abs(PRI[layer].Z+PRI[layer].height/2 - level) < 1E-4)
            {
                PRI[layer++].type = parts_type[cnt++];
                level += parts_length[cnt];
            }

            else if(PRI[layer].Z+PRI[layer].height/2 < level)
                PRI[layer++].type = parts_type[cnt];

            else
            {
                myCore->write_to_log("Incorrect layers hight in map.imcore file");
            }

            if(layer == NL)
                flag = false;
        }
    }

    for (unsigned int h = 0; h < NL; h++)
    {
        PRI[h].S = S;
        PRI[h].X = X * myCore->FASTEP / 2.0;
        PRI[h].Y = Y * myCore->FASTEP * myCore->SQRT3 / 2.0;
        PRI[h].h = h;
        PRI[h].n = n;
        PRI[h].V = PRI[h].S * PRI[h].height;
        PRI[h].division();
        ZN.push_back(PRI[h].Z);
    }
    KDPZ7.resize(7,1);
    KDPZN.resize(NL,1);
    KTECON = 1;

}

double ASSEMBLY::internal_iteration()
{
    double delta_for_assembly = 0;

    for (unsigned int h = 0; h < NL; ++h)
        delta_for_assembly += PRI[h].internal_iteration();

    return delta_for_assembly / NL;
}


void ASSEMBLY::interpolation_7_lays()
{

    for (unsigned int k = 0 ; k < myCore->NLDPZ; ++k)
    {
        TPl_7[k] = 0;
        SL_7[k] = 0;
        WD_7[k] = 0;
        KC_7[k] = 0;


        if(kni)
        {
            kni->TPl_displ[k] = 0;
            kni->SL_displ[k] = 0;
            kni->WD_displ[k] = 0;
            kni->KC_displ[k] = 0;
            kni->F1_displ[k] = 0;
            kni->F2_displ[k] = 0;
            kni->WT_displ[k] = 0;
            kni->XE_displ[k] = 0;
            kni->SM_displ[k] = 0;
            kni->FT_displ[k] = 0;
        }


        for (unsigned int h = 0 ; h < NL; ++h)
        {
            if(kni)
            {
                kni->TPl_displ[k]+= PRI[h].TP / PRI[h].height * 100.0 * kni->PRI_parts[k][h];
                kni->SL_displ[k]+= PRI[h].SL * kni->PRI_parts[k][h];
                kni->WD_displ[k]+= PRI[h].WD * kni->PRI_parts[k][h];
                kni->KC_displ[k]+= PRI[h].KC * kni->PRI_parts[k][h];
                kni->F1_displ[k] += PRI[h].F1 * kni->PRI_parts[k][h];
                kni->F2_displ[k] += PRI[h].F2 * kni->PRI_parts[k][h];
                kni->WT_displ[k] += PRI[h].WT * kni->PRI_parts[k][h];
                kni->XE_displ[k] += PRI[h].XE * kni->PRI_parts[k][h];
                kni->SM_displ[k] += PRI[h].SM * kni->PRI_parts[k][h];
                kni->FT_displ[k] += PRI[h].FT * kni->PRI_parts[k][h];
            }

            TPl_7[k]+= PRI[h].TP / PRI[h].height * 100.0 * PRI_parts[k][h];
            SL_7[k]+= PRI[h].SL * PRI_parts[k][h];
            WD_7[k]+= PRI[h].WD * PRI_parts[k][h];
            KC_7[k]+= PRI[h].KC * PRI_parts[k][h];
        }

    }
}

void ASSEMBLY::normalization()
{
    TP = 0;
    NP = 0;
    for (unsigned int h = 0; h < NL; ++h )
    {
        PRI[h].normalization();
        TP += PRI[h].TP;
        NP += PRI[h].NP;
    }
}


void ASSEMBLY::poisoning_xe_sm()
{

    ro_XE = 0;
    ro_SM = 0;
    ro_XE_eq = 0;
    ro_SM_eq = 0;

    for(unsigned int h = 0; h < NL; h++)
    {
        PRI[h].poisoning_xe_sm();
        ro_XE += PRI[h].ro_XE * PRI[h].part;
        ro_SM += PRI[h].ro_SM * PRI[h].part;
        ro_XE_eq += PRI[h].ro_XE_eq * PRI[h].part;
        ro_SM_eq += PRI[h].ro_SM_eq * PRI[h].part;
    }
}

void ASSEMBLY::recovery()
{

//    int ns = 0;
//    int nd = kni ? myCore->nd : 0;
//    int nc = myCore->nc;
//    int ntc = myCore->ntc;


//    double Q = 0;
//    if(ntc && NTC)
//    {
//        Q = (myCore->xs_data->cp_calc(thermocouple,myCore->P_out) - myCore->xs_data->cp_calc(WT_in,myCore->P_out + myCore->P_drop)) * myCore->G * flow;
//        KTECON = (nc + ntc * (Q - TP) / TP) / (nc + ntc);
//    }
//    else
//        KTECON = 1;


//    //Расчет поправок по ДПЗ
//    for (unsigned int h = 0; h < myCore->NLDPZ ; ++h)
//    {
//        ns = myCore->aver_of_orb[orb][h] ? myCore->ns : 0;
//        //Для достоверных ДПЗ
//        if (kni && kni->reliability[h])
//        {
//            //Проверяем признак hard/soft симметрии,
//            //Если условие выполняется то soft симметрия
//            if (abs(myCore->aver_of_orb[orb][h] - kni->KV_dpz[h]) / kni->KV_dpz[h] * 100.0 > 2.5)
//            {
//                double sumNei = kni->KV_dpz[h] / myCore->aver_of_orb[orb][h];
//                int cnt = 1;
//                for (unsigned int nn = 0; nn < ADJ.size(); ++nn)
//                    if (ADJ[nn]->kni && ADJ[nn]->kni->reliability[h])
//                    {
//                        sumNei += ADJ[nn]->kni->KV_dpz[h] / myCore->aver_of_orb[ADJ[nn]->orb][h];
//                        cnt++;
//                    }
//                //если есть соседи с ДПЗ то расчитываются значения KDPZ7
//                if (cnt > 1)
//                    KDPZ7[h] = (nd * kni->KV_dpz[h] + sumNei / cnt * ns * myCore->aver_of_orb[orb][h] + nc * myCore->KV_7[n-1][h])
//                               / (nd + ns + nc) / myCore->KV_7[n-1][h];
//                //если нет соседей с ДПЗ то расчитываются значения KDPZ7
//                else
//                    KDPZ7[h] = (nd * kni->KV_dpz[h] + ns * myCore->aver_of_orb[orb][h] + nc * myCore->KV_7[n-1][h])
//                               / (nd + ns + nc) / myCore->KV_7[n-1][h];

//            }
//            // hard симметрия
//            else
//                KDPZ7[h] = (nd * kni->KV_dpz[h] + ns * myCore->aver_of_orb[orb][h] + nc * myCore->KV_7[n-1][h])
//                           / (nd + ns + nc) / myCore->KV_7[n-1][h];
//        }
//        //Если нет достоверного ДПЗ, но есть значения на орбите симметрии то рассчитывается поправка
//        else if (myCore->aver_of_orb[orb][h] > 0)
//            KDPZ7[h] = ( ns * myCore->aver_of_orb[orb][h] + nc * myCore->KV_7[n-1][h])
//                     / (ns + nc) / myCore->KV_7[n-1][h];

//    }

    int ns = 0;
    int nd = kni ? myCore->nd : 0;
    int nc = myCore->nc;
    int ntc = myCore->ntc;




    double Q = 0;
    if(ntc && NTC)
    {
        Q = (myCore->xs_data->cp_calc(thermocouple,myCore->P_out) - myCore->xs_data->cp_calc(WT_in,myCore->P_out)) * myCore->G * flow;
        KTECON = (nc + ntc * (Q - TP) / TP) / (nc + ntc);
    }
    else
        KTECON = 1;


    //Расчет поправок по ДПЗ
    for (unsigned int h = 0; h < myCore->NLDPZ ; ++h)
    {
        int cnt = 1;
        ns = myCore->aver_of_orb[orb][h] ? myCore->ns : 0;
        double sumNei = 1;
        //Для достоверных ДПЗ
        if (kni && kni->reliability[h] && (orb == 1 || orb == 3 || orb == 7 || orb == 11 || orb == 13 || orb == 15))
        {
            sumNei = kni->KV_dpz[h] / myCore->aver_of_orb[orb][h];
            cnt = 1;
        } else {
            sumNei = 1;
            cnt = 1;
        }


        for (unsigned int nn = 0; nn < ADJ.size(); ++nn)
            if (ADJ[nn]->kni && ADJ[nn]->kni->reliability[h] > 0 && (ADJ[nn]->orb == 1 || ADJ[nn]->orb == 3 || ADJ[nn]->orb == 7 || ADJ[nn]->orb == 11 || ADJ[nn]->orb == 13 || ADJ[nn]->orb == 15))
            {
                sumNei += ADJ[nn]->kni->KV_dpz[h] / myCore->aver_of_orb[ADJ[nn]->orb][h];
                cnt++;
            }

//        if ( cnt == 0) {
//            cnt = 1;
//            sumNei = 1;
//        }

        if (kni && kni->reliability[h]) {
            KDPZ7[h] = (nd * kni->KV_dpz[h] + sumNei / cnt * ns * myCore->aver_of_orb[orb][h] + nc * myCore->KV_7[n-1][h])
                       / (nd + ns + nc) / myCore->KV_7[n-1][h];
            //cout << "with " << n << " kni->reliability[h] " <<  kni->reliability[h] << endl;
        } else {
            KDPZ7[h] = (sumNei / cnt * ns * myCore->aver_of_orb[orb][h] + nc * myCore->KV_7[n-1][h])
                       / (ns + nc) / myCore->KV_7[n-1][h];
            //cout << "with " << n << " sumNei " <<  sumNei << " cnt " << cnt << " aver " << myCore->aver_of_orb[orb][h] << endl;
        }
    }

}

void ASSEMBLY::wt_calc()
{
    // *** Water temperature calculating ***
    double begin_temp_to_next = WT_in;
    double heat_on_prism;
    for (unsigned int h = 0; h < PRI.size(); h++)
    {
        heat_on_prism = PRI[h].TP*1e6 / ((myCore->G * flow) * PRI[h].CP);
        PRI[h].WT = begin_temp_to_next + heat_on_prism / 2.0;
        begin_temp_to_next = begin_temp_to_next + heat_on_prism;
    }
    WT_out = begin_temp_to_next;
    dT = WT_out - WT_in;
}

void ASSEMBLY::ft_calc()
{
    // *** Fuel temperature calculating ***
    double tp;
    for (unsigned int h = 0; h < PRI.size(); h++)
    {
        tp = PRI[h].TP*(1e3/PRI[h].height);
        // local fuel temperamure value:
        PRI[h].FT = PRI[h].WT + myCore->AFT[0] * tp + myCore->AFT[1] * tp * tp + myCore->AFT[2] * tp * tp * tp +
                     myCore->AFT[3] * tp * tp * tp * tp + myCore->AFT[4] * tp * tp * tp * tp * tp;
    }

}


