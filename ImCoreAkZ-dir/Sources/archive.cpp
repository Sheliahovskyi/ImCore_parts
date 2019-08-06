#include "archive.h"


ARCHIVE::ARCHIVE( CORE * CorePointer ) {
    pCore = CorePointer;

    ArchiveListFilling();
}

ARCHIVE::~ARCHIVE() {
}


CoreState ARCHIVE::MakeCoreSlice() {
    CoreState core;
    ResizeCoreSliceFields( &core );

    ///< Local time calc
    time_t rawtime;
    struct tm * timeinfo;
    time (&rawtime);
    timeinfo = localtime (&rawtime);
    core.CurrentDate = TIME_IMCORE(*timeinfo);

    for ( int n = 0; n < pCore->NFA; n++ ) {
        for ( int h = 0; h < pCore->NL; h++ ) {
            core.CoreMap[n][h]    = pCore->FA[n].PRI[h].type;
            core.PrismHight[n][h] = pCore->FA[n].PRI[h].height;
            core.SL[n][h]         = pCore->FA[n].PRI[h].SL;
            core.SM[n][h]         = pCore->FA[n].PRI[h].SM;
            core.PM[n][h]         = pCore->FA[n].PRI[h].PM;
            core.XE[n][h]         = pCore->FA[n].PRI[h].XE;
            core.I[n][h]          = pCore->FA[n].PRI[h].I;
        }
    }

    for ( int n = 0 ; n < pCore->NKNI; n++ ) {
        for ( int h = 0; h < pCore->NLDPZ; h++) {
            core.ZE[n][h] = pCore->kni[n].ZE[h];
        }

    }

    for ( int n = 0; n < pCore->NSUZ; n++ ) {
        core.HSuz[n] = pCore->suz.RODS[n].position;
    }

    core.Teff   = pCore->TEFF;
    core.G      = pCore->G;
    core.CB     = pCore->CB;
    core.Pout   = pCore->P_out;
    core.Pdrop  = pCore->P_drop;
    core.Tin    = pCore->WT_in;

    return core;
}


void ARCHIVE::WriteCoreSlice() {
    std::chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now();

    CoreState core = MakeCoreSlice();
    char * sBufer = new char[100000000];
    int current_size = 0;

    current_size += sprintf(&sBufer[current_size], "%s\t\t\t|%20s\n", "Date", core.CurrentDate.givedate().c_str());
    current_size += sprintf(&sBufer[current_size], "%s\t\t\t|%20.5f\n", "Teff", core.Teff);
    current_size += sprintf(&sBufer[current_size], "%s\t\t\t|%20.5f\n", "G", core.G);
    current_size += sprintf(&sBufer[current_size], "%s\t\t\t|%20.5f\n", "CB", core.CB);
    current_size += sprintf(&sBufer[current_size], "%s\t\t\t|%20.5f\n", "Pout", core.Pout);
    current_size += sprintf(&sBufer[current_size], "%s\t\t\t|%20.5f\n", "Pdrop", core.Pdrop);
    current_size += sprintf(&sBufer[current_size], "%s\t\t\t|%20.5f\n", "Tin", core.Tin);

    for ( int n = 0; n < pCore->NSUZ; n++) {
        current_size += sprintf(&sBufer[current_size], "HSuz_%02d\t\t\t|%20.3f\n", n+1, core.HSuz[n]);
    }

    for ( int n = 0; n < pCore->NFA; n++ ) {
        for ( int h = 0; h < pCore->NL; h++ ) {
            current_size += sprintf(&sBufer[current_size], "map_%03d_%02d\t\t|%20d\n", n+1, h+1, core.CoreMap[n][h]);
        }
    }
    for ( int n = 0; n < pCore->NFA; n++ ) {
        for ( int h = 0; h < pCore->NL; h++ ) {
            current_size += sprintf(&sBufer[current_size], "PrismH_%03d_%02d\t\t|%20.5f\n", n+1, h+1, core.PrismHight[n][h]);
        }
    }
    for ( int n = 0; n < pCore->NFA; n++ ) {
        for ( int h = 0; h < pCore->NL; h++ ) {
            current_size += sprintf(&sBufer[current_size], "SL_%03d_%02d\t\t|%20.5f\n", n+1, h+1, core.SL[n][h]);
        }
    }
    for ( int n = 0; n < pCore->NFA; n++ ) {
        for ( int h = 0; h < pCore->NL; h++ ) {
            current_size += sprintf(&sBufer[current_size], "SM_%03d_%02d\t\t|%20.5e\n", n+1, h+1, core.SM[n][h]);
        }
    }
    for ( int n = 0; n < pCore->NFA; n++ ) {
        for ( int h = 0; h < pCore->NL; h++ ) {
            current_size += sprintf(&sBufer[current_size], "PM_%03d_%02d\t\t|%20.5e\n", n+1, h+1, core.PM[n][h]);
        }
    }
    for ( int n = 0; n < pCore->NFA; n++ ) {
        for ( int h = 0; h < pCore->NL; h++ ) {
            current_size += sprintf(&sBufer[current_size], "XE_%03d_%02d\t\t|%20.5e\n", n+1, h+1, core.XE[n][h]);
        }
    }
    for ( int n = 0; n < pCore->NFA; n++ ) {
        for ( int h = 0; h < pCore->NL; h++ ) {
            current_size += sprintf(&sBufer[current_size], "_I_%03d_%02d\t\t|%20.5e\n", n+1, h+1, core.I[n][h]);
        }
    }
    for ( int n = 0; n < pCore->NKNI; n++ ) {
        for ( int h = 0; h < pCore->NLDPZ; h++ ) {
            current_size += sprintf(&sBufer[current_size], "ZE_%02d_%02d\t\t|%20.5f\n", n+1, h+1, core.ZE[n][h]);
        }
    }
    for ( int n = 0; n < pCore->NFA; n++ ) {
        for ( int h = 0; h < pCore->NL; h++ ) {
            for ( int i = 0; i < pCore->NPIN; i++ ) {
                current_size += sprintf(&sBufer[current_size], "PinSL_%03d_%02d_%03d\t|%20.5f\n", n+1, h+1, i+1, core.PinSL[n][h][i]);
            }
        }
    }

    FILE * outFile = fopen((pCore->PATH[CORE::ArchiveDir] + "/IC_" + std::to_string(core.CurrentDate.give_sec_from_1970()) + "_" + core.CurrentDate.givedate() + ".arc").c_str(), "w");

    if(!outFile)
    {
        pCore->write_to_log("cannot find " + pCore->PATH[CORE::ArchiveDir]);
    } else {
        fprintf(outFile, "%s", sBufer);
        fclose(outFile);
    }

    std::chrono::high_resolution_clock::time_point t2 = std::chrono::high_resolution_clock::now();

    cout << "Archive has successful written in: " << std::chrono::duration_cast<std::chrono::microseconds>( t2 - t1 ).count()/1e6 << " sec." << endl;
}


void ARCHIVE::ResizeCoreSliceFields( CoreState* core ) {
    core->CoreMap.resize(pCore->NFA);
    core->PrismHight.resize(pCore->NFA);
    core->SL.resize(pCore->NFA);
    core->SM.resize(pCore->NFA);
    core->PM.resize(pCore->NFA);
    core->XE.resize(pCore->NFA);
    core->I.resize(pCore->NFA);
    core->PinSL.resize(pCore->NFA);
    core->ZE.resize(pCore->NKNI);
    core->HSuz.resize(pCore->NSUZ);

    for ( int n = 0; n < pCore->NFA; n++ ) {
        core->CoreMap[n].resize(pCore->NL);
        core->PrismHight[n].resize(pCore->NL);
        core->SL[n].resize(pCore->NL);
        core->SM[n].resize(pCore->NL);
        core->PM[n].resize(pCore->NL);
        core->XE[n].resize(pCore->NL);
        core->I[n].resize(pCore->NL);
        core->PinSL[n].resize(pCore->NL);
        for ( int h = 0; h < pCore->NL; h++ ) {
            core->PinSL[n][h].resize(pCore->NPIN);
        }
    }

    for ( int n = 0; n < pCore->NKNI; n++ ) {
        core->ZE[n].resize(pCore->NLDPZ);
    }

}


void ARCHIVE::ReadCoreSlice( long long Time1970 ) {
    CoreState core;

    core = FindCoreArchive(Time1970);

    pCore->TEFF = core.Teff;
    pCore-> G = core.G;
    pCore->CB = core.CB;
    pCore->P_out = core.Pout;
    pCore->P_drop = core.Pdrop;
    pCore->set_pressure();
    pCore->WT_in = core.Tin;
    for ( int n = 0; n < pCore->NFA; n++ ) {
        pCore->FA[n].WT_in = core.Tin;
    }

    for ( int n = 0; n < pCore->NSUZ; n++ ) {
        pCore->suz.RODS[n].position = core.HSuz[n];
    }

    for ( int n = 0; n < pCore->NFA; n++ ) {
        for ( int h = 0; h < pCore->NL; h++ ) {
            pCore->FA[n].PRI[h].type = core.CoreMap[n][h];
            pCore->FA[n].PRI[h].height = core.PrismHight[n][h];
            pCore->FA[n].PRI[h].SL = core.SL[n][h];
            pCore->FA[n].PRI[h].SM = core.SM[n][h];
            pCore->FA[n].PRI[h].PM = core.PM[n][h];
            pCore->FA[n].PRI[h].XE = core.XE[n][h];
            pCore->FA[n].PRI[h].I = core.I[n][h];
            for( int i = 0; i < pCore->NPIN; i++ ) {
                pCore->FA[n].PRI[h].PinSl[i] = core.PinSL[n][h][i];
            }
        }
    }

    for (int n = 0; n < pCore->NKNI; n++ ) {
        for ( int h = 0; h < pCore->NLDPZ; h++ ) {
            pCore->kni[n].ZE[h] = core.ZE[n][h];
        }
    }

}


CoreState ARCHIVE::FindCoreArchive( long long Time1970 ) {
    CoreState core;
    ResizeCoreSliceFields( &core );
    vector<vector<vector<double>>> PinSL;

    std::map<string, double> intData;
    std::map<string, double> doubleData;
    std::string FileGlobalPath, stri, id, trash;
    int intValue;
    int N = 0, H = 0, I = 0;
    double doubleValue;
    std::chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now();

    for ( int n = 0; n < ArchiveList.size(); n++ ) {
        if ( Time1970 < ArchiveList[n].TimeFrom1970 ) {
            FileGlobalPath = ArchiveList[n-1].ArchiveFileGlobalPath;
            break;
        } else if ( Time1970 == ArchiveList[n].TimeFrom1970 ) {
            FileGlobalPath = ArchiveList[n].ArchiveFileGlobalPath;
            break;
        }
    }
    if ( Time1970 - ArchiveList[ArchiveList.size()-1].TimeFrom1970 > 3600 * 24 ) {
        pCore->write_to_log( "Picked date is too far from last ImCore archive state, last date is: " + ArchiveList[ArchiveList.size()-1].date );
    } else if ( ArchiveList[0].TimeFrom1970 - Time1970 > 3600 * 24 ) {
        pCore->write_to_log( "Picked date is too far from first ImCore archive state, first date is: " + ArchiveList[0].date );
    }

    cout << "Picked archive file is: " << FileGlobalPath << endl;

    ifstream inFile;
    inFile.open(FileGlobalPath);
    if(!inFile) {
        pCore->write_to_log("cannot open " + FileGlobalPath);
    }
    while (!inFile.eof()) {
        getline(inFile, stri);
        if ( stri.empty() ) {
            continue;
        }
        istringstream iss(stri);
        iss >> id >> trash;


        if ( id.find( "map" ) != std::string::npos ) {
            iss >> intValue;
            intData.insert( std::pair<std::string, double>( id, intValue ) );
        } else if ( id.find( "PinSL" ) != std::string::npos ) {
            iss >> doubleValue;

            core.PinSL[std::stoi( id.substr( 6, 3 ) ) - 1][std::stoi( id.substr( 10, 2 ) ) - 1][std::stoi( id.substr( 13, 3 ) ) - 1] = doubleValue;
        } else {
            iss >> doubleValue;
            doubleData.insert( std::pair<std::string, double>( id, doubleValue ) );
        }
    }

    char name [1000];

    cout << "+++++++++++++++++++++++++++++" << endl;

    sprintf(name, "CB");
    core.CB    = doubleData[ std::string( name ) ];

    sprintf(name, "G");
    core.G     = doubleData[ std::string( name ) ];

    sprintf(name, "Pout");
    core.Pout  = doubleData[ std::string( name ) ];

    sprintf(name, "Pdrop");
    core.Pdrop = doubleData[ std::string( name ) ];

    sprintf(name, "Teff");
    core.Teff  = doubleData[ std::string( name ) ];

    sprintf(name, "Tin");
    core.Tin   = doubleData[ std::string( name ) ];


    for ( int n = 0; n < pCore->NSUZ; n++) {
        sprintf(name, "HSuz_%02d", n+1);
        core.HSuz[n] = doubleData[ std::string( name ) ];
    }

    for ( int n = 0; n < pCore->NFA; n++ ) {
        for ( int h = 0; h < pCore->NL; h++ ) {
            sprintf(name, "map_%03d_%02d", n+1, h+1);
            core.CoreMap[n][h] = intData[ std::string( name ) ];
        }
    }
    for ( int n = 0; n < pCore->NFA; n++ ) {
        for ( int h = 0; h < pCore->NL; h++ ) {
            sprintf(name, "PrismH_%03d_%02d", n+1, h+1);
            core.PrismHight[n][h] = doubleData[ std::string ( name )];
        }
    }
    for ( int n = 0; n < pCore->NFA; n++ ) {
        for ( int h = 0; h < pCore->NL; h++ ) {
            sprintf(name, "SL_%03d_%02d", n+1, h+1);
            core.SL[n][h] = doubleData[ std::string ( name )];
        }
    }
    for ( int n = 0; n < pCore->NFA; n++ ) {
        for ( int h = 0; h < pCore->NL; h++ ) {
            sprintf(name, "SM_%03d_%02d", n+1, h+1);
            core.SM[n][h] = doubleData[ std::string ( name )];
        }
    }
    for ( int n = 0; n < pCore->NFA; n++ ) {
        for ( int h = 0; h < pCore->NL; h++ ) {
            sprintf(name, "PM_%03d_%02d", n+1, h+1);
            core.PM[n][h] = doubleData[ std::string ( name )];
        }
    }
    for ( int n = 0; n < pCore->NFA; n++ ) {
        for ( int h = 0; h < pCore->NL; h++ ) {
            sprintf(name, "XE_%03d_%02d", n+1, h+1);
            core.XE[n][h] = doubleData[ std::string ( name )];
        }
    }
    for ( int n = 0; n < pCore->NFA; n++ ) {
        for ( int h = 0; h < pCore->NL; h++ ) {
            sprintf(name, "_I_%03d_%02d", n+1, h+1);
            core.I[n][h] = doubleData[ std::string ( name )];
        }
    }
    for ( int n = 0; n < pCore->NKNI; n++ ) {
        for ( int h = 0; h < pCore->NLDPZ; h++ ) {
            sprintf(name, "ZE_%02d_%02d", n+1, h+1);
            core.ZE[n][h] = doubleData[ std::string ( name )];
        }
    }

    std::chrono::high_resolution_clock::time_point t2 = std::chrono::high_resolution_clock::now();

    cout << "Archive has successful written in: " << std::chrono::duration_cast<std::chrono::microseconds>( t2 - t1 ).count()/1e6 << " sec." << endl;

    return core;
}


void ARCHIVE::ArchiveListFilling() {
    glob_t glob_result;

    glob( (pCore->PATH[CORE::ArchiveDir] + "/*").c_str(), GLOB_TILDE, NULL, &glob_result );  // Automatically sorted list

    ArchiveList.resize( glob_result.gl_pathc );

    for ( unsigned int i = 0; i < glob_result.gl_pathc; ++i ) {
        ArchiveList[i].ArchiveFileGlobalPath = glob_result.gl_pathv[i];
        char * pch = strtok ( glob_result.gl_pathv[i],"/._" );

        while ( true ) {
            pch = strtok ( NULL, "/_" );
            if ( std::string(pch).find( "IC" ) != std::string::npos ) {
                pch = strtok ( NULL, "_" );
                ArchiveList[i].TimeFrom1970 = std::stoi( pch );           // sring DATE DD/MM/YY...
                pch = strtok ( NULL, "_." );
                ArchiveList[i].date = pch;                                // int   TIME from 1970 in sek
                break;
            }
        }
    }
}

