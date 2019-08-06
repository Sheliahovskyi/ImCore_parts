#include "node.h"

NODE::NODE()
{

}

void NODE::power()
{
    NP = (myPrism->nSf1 * F1 + myPrism->nSf2 * F2) * V;
    TP = (myPrism->Sf1 * F1 + myPrism->Sf2 * F2) * (myPrism->E *0.95 )* myCore->EVTOJ * V ;

}

double NODE::internal_iteration() {

    double aD1F = 0;
    double aD2F = 0;

    double Frg1;
    double Frg2;
    double Fag1;
    double Fag2;

    for ( int r = 0; r < numRadNei; ++r)
    {
        aD1F += preDif.DDR1[r] * ADJ_R_NODE[r]->F1;
        aD2F += preDif.DDR2[r] * ADJ_R_NODE[r]->F2;
    }
    if(numRadNei < 3)
    {
        Frg1 = preDif.BR1 * F1;
        Frg2 = (preDif.BR2 * F2 + preDif.BR3 * Frg1 - preDif.BR4 * (Frg1 - F1)) / preDif.BR5;

        aD1F += preDif.hRad * myPrism->D1 * Frg1;
        aD2F += preDif.hRad * myPrism->D2 * Frg2;

    }

    for ( int a = 0; a < numAxiNei; ++a)
    {
        Fag1 = (preDif.DpR1 * F1 + ADJ_A_NODE[a]->preDif.DpR1 * ADJ_A_NODE[a]->F1) / preDif.DpRNei1[a];
        Fag2 = (preDif.DpR2 * F2 + ADJ_A_NODE[a]->preDif.DpR2 * ADJ_A_NODE[a]->F2) / preDif.DpRNei2[a];

        aD1F += preDif.DDA1 * Fag1;
        aD2F += preDif.DDA2 * Fag2;
    }

    if (numAxiNei < 2)
    {
        Fag1 = preDif.BA1 * F1;
        Fag2 = (preDif.BA2 * F2 + preDif.BA3 * Fag1 -preDif. BA4 * (Fag1 - F1)) / preDif.BA5;

        aD1F += preDif.hAxi * myPrism->D1 * Fag1;
        aD2F += preDif.hAxi * myPrism->D2 * Fag2;


    }

    F1p = F1;
    F2p = F2;

    F1 =  (NP + aD1F) / preDif.S_rem1;
    F2 =  (myPrism->S12 * F1 * V + aD2F) / preDif.S_rem2;

    return (abs(F1 - F1p) / F1 + abs(F2 - F2p) / F2) / 2.0;

}

