#include <ImCorePMain.h>

//////////////////////////////////////////////////////////////////////
// class CPredictClient
CPredictClient::CPredictClient(CStr Host, int Port)
{
	CPredictClient::ClientHost	= Host;
	CPredictClient::ClientPort	= Port;
	CPredictClient::NetSocket	= -1;
	CPredictClient::MutexID		= ::MutexCreate();
	CPredictClient::ThreadPid	= 0;
	CPredictClient::ThreadName	= CStr("ClientThread(")+ClientHost+":"+ClientPort+")";

	int FreePos = ::pProg->pClientLst->GetCnt() + 1;
	ThreadFlgStart	= 1100 + 10*FreePos;
	ThreadFlgRun	= 1101 + 10*FreePos;
	ThreadFlgStop	= 1102 + 10*FreePos;
	::pProg->SetFlag(ThreadFlgStart,	SV_READY);
	::pProg->SetFlag(ThreadFlgRun,		SV_READY);
	::pProg->SetFlag(ThreadFlgStop,		SV_READY);

    //CPredictClient::pCORE			= new CORE;
    //CPredictClient::pXS				= new XS;
    //CPredictClient::pCORE->xs_data	= pXS;
    //CPredictClient::pForecast       = new FORECAST;
}
CPredictClient::~CPredictClient()
{
	if(MutexID) MutexDestroy(MutexID);
	Disconnect();

    delete pForecast;
}
void CPredictClient::Disconnect()
{
	if(CPredictClient::NetSocket != -1) NetCloseSocket(CPredictClient::NetSocket, NULL);
	CPredictClient::NetSocket = -1;
}
void CPredictClient::StartThread(){ ::pProg->SetFlag(ThreadFlgStart, SV_NEEDED); }

bool CPredictClient::ClientThread()
{// поток работы с клиентом
#ifdef __sparc
	SetAbortProc(AbortProg);
#endif

	bool bProcess = true;
	ThreadError.Empty();

	while(bProcess)
	{// основной цикл потока
		if(::pProg->GetFlag(ThreadFlgStop) != SV_NEEDED)
		{//
			::pProg->SetFlag(ThreadFlgRun, SV_PROCESS);
			ThreadYield();

			if(NetSocket != -1)
			{
				CPredictRequestHeader RequestHeader;
				int PeekSize = ::NetReadData(NetSocket, &RequestHeader, sizeof(RequestHeader), 1000, &ThreadError, true);
				if(PeekSize == sizeof(RequestHeader))
				{
					// вычитываем данные из канала
					unsigned char* pPackage = (unsigned char*)::GlobalNew(RequestHeader.PackageSize);
					// вычитываем данные из канала
					if(::NetReadData(NetSocket, pPackage, RequestHeader.PackageSize, 0, &ThreadError) == RequestHeader.PackageSize)
					{
                        //CPredictRequestHeader	Header;
                        //LST*					pEtapLst = new LST(20, sizeof(Etap_data));
                        //PrepareClientRequest(pPackage, &Header, pEtapLst);
						//pCORE->start();
						//pCORE->read_forecast_info(&Header, pEtapLst);
						//delete pEtapLst;
						//pCORE->forecast();
					}
					::GlobalDelete(pPackage);
				}
				else if(PeekSize == -1) Disconnect();

				if(NetSocket != -1)
				{
                    //int ResultCnt = pCORE->pForecastResultLst->GetCnt();
                    //if(ResultCnt)
                    //{
                    //	printf("ResultCnt=%d\n", ResultCnt);
                    //
                    //	// формируем ответ
                    //	int				MaxValuesCnt	= (sizeof(ForecastResultData) - 2*sizeof(int) - 2*sizeof(CTime)) / sizeof(float);
                    //	unsigned char*	pAnswer			= (unsigned char*)::GlobalNew(sizeof(CPredictAnswerHeader) + (sizeof(CPredictValueHeader)+sizeof(float))*MaxValuesCnt);
                    //
                    //	for(int ResultPos=1 ; ResultPos<=ResultCnt ; ResultPos++)
                    //	{
                    //		ForecastResultData* pResultData = (ForecastResultData*)pCORE->pForecastResultLst->Get(ResultPos);
                    //
                    //		int AnswerOffset = 0;
                    //
                    //		CPredictAnswerHeader* pAnswerHeader = (CPredictAnswerHeader*)pAnswer;
                    //		pAnswerHeader->AnswerType		= pResultData->AnswerType;
                    //		pAnswerHeader->PackageSize		= 0;
                    //		//pAnswerHeader->StartTime		= ;
                    //		//pAnswerHeader->PointTime.Sec	= 0;
                    //		pAnswerHeader->ValuesCounter	= 0;
                    //		AnswerOffset += sizeof(CPredictAnswerHeader);
                    //
                    //		CPredictValueHeader* pValueHeader = (CPredictValueHeader*)(pAnswer+AnswerOffset);
                    //		pValueHeader->ValueSize	= sizeof(CPredictValueHeader) + sizeof(float);
                    //		pValueHeader->ValueType	= VT_FLOAT;
                    //		StrCpy(pValueHeader->ValueName, "POWER");
                    //		pValueHeader->SizeX		= 1;
                    //		pValueHeader->SizeY		= 1;
                    //		AnswerOffset += sizeof(CPredictValueHeader);
                    //		*(float*)(pAnswer+AnswerOffset) = pResultData->POWER;
                    //		AnswerOffset += sizeof(float);
                    //		pAnswerHeader->ValuesCounter++;
                    //
                    //		pValueHeader = (CPredictValueHeader*)(pAnswer+AnswerOffset);
                    //		pValueHeader->ValueSize	= sizeof(CPredictValueHeader) + sizeof(float);
                    //		pValueHeader->ValueType	= VT_FLOAT;
                    //		StrCpy(pValueHeader->ValueName, "AO");
                    //		pValueHeader->SizeX		= 1;
                    //		pValueHeader->SizeY		= 1;
                    //		AnswerOffset += sizeof(CPredictValueHeader);
                    //		*(float*)(pAnswer+AnswerOffset) = pResultData->AO;
                    //		AnswerOffset += sizeof(float);
                    //		pAnswerHeader->ValuesCounter++;
                    //
                    //		pValueHeader = (CPredictValueHeader*)(pAnswer+AnswerOffset);
                    //		pValueHeader->ValueSize	= sizeof(CPredictValueHeader) + sizeof(float);
                    //		pValueHeader->ValueType	= VT_FLOAT;
                    //		StrCpy(pValueHeader->ValueName, "CB");
                    //		pValueHeader->SizeX		= 1;
                    //		pValueHeader->SizeY		= 1;
                    //		AnswerOffset += sizeof(CPredictValueHeader);
                    //		*(float*)(pAnswer+AnswerOffset) = pResultData->CB;
                    //		AnswerOffset += sizeof(float);
                    //		pAnswerHeader->ValuesCounter++;
                    //
                    //		pValueHeader = (CPredictValueHeader*)(pAnswer+AnswerOffset);
                    //		pValueHeader->ValueSize	= sizeof(CPredictValueHeader) + sizeof(float);
                    //		pValueHeader->ValueType	= VT_FLOAT;
                    //		StrCpy(pValueHeader->ValueName, "H1");
                    //		pValueHeader->SizeX		= 1;
                    //		pValueHeader->SizeY		= 1;
                    //		AnswerOffset += sizeof(CPredictValueHeader);
                    //		*(float*)(pAnswer+AnswerOffset) = pResultData->H1;
                    //		AnswerOffset += sizeof(float);
                    //		pAnswerHeader->ValuesCounter++;
                    //
                    //		pValueHeader = (CPredictValueHeader*)(pAnswer+AnswerOffset);
                    //		pValueHeader->ValueSize	= sizeof(CPredictValueHeader) + sizeof(float);
                    //		pValueHeader->ValueType	= VT_FLOAT;
                    //		StrCpy(pValueHeader->ValueName, "H2");
                    //		pValueHeader->SizeX		= 1;
                    //		pValueHeader->SizeY		= 1;
                    //		AnswerOffset += sizeof(CPredictValueHeader);
                    //		*(float*)(pAnswer+AnswerOffset) = pResultData->H2;
                    //		AnswerOffset += sizeof(float);
                    //		pAnswerHeader->ValuesCounter++;
                    //
                    //		pValueHeader = (CPredictValueHeader*)(pAnswer+AnswerOffset);
                    //		pValueHeader->ValueSize	= sizeof(CPredictValueHeader) + sizeof(float);
                    //		pValueHeader->ValueType	= VT_FLOAT;
                    //		StrCpy(pValueHeader->ValueName, "H3");
                    //		pValueHeader->SizeX		= 1;
                    //		pValueHeader->SizeY		= 1;
                    //		AnswerOffset += sizeof(CPredictValueHeader);
                    //		*(float*)(pAnswer+AnswerOffset) = pResultData->H3;
                    //		AnswerOffset += sizeof(float);
                    //		pAnswerHeader->ValuesCounter++;
                    //
                    //		pValueHeader = (CPredictValueHeader*)(pAnswer+AnswerOffset);
                    //		pValueHeader->ValueSize	= sizeof(CPredictValueHeader) + sizeof(float);
                    //		pValueHeader->ValueType	= VT_FLOAT;
                    //		StrCpy(pValueHeader->ValueName, "H4");
                    //		pValueHeader->SizeX		= 1;
                    //		pValueHeader->SizeY		= 1;
                    //		AnswerOffset += sizeof(CPredictValueHeader);
                    //		*(float*)(pAnswer+AnswerOffset) = pResultData->H4;
                    //		AnswerOffset += sizeof(float);
                    //		pAnswerHeader->ValuesCounter++;
                    //
                    //		pValueHeader = (CPredictValueHeader*)(pAnswer+AnswerOffset);
                    //		pValueHeader->ValueSize	= sizeof(CPredictValueHeader) + sizeof(float);
                    //		pValueHeader->ValueType	= VT_FLOAT;
                    //		StrCpy(pValueHeader->ValueName, "H5");
                    //		pValueHeader->SizeX		= 1;
                    //		pValueHeader->SizeY		= 1;
                    //		AnswerOffset += sizeof(CPredictValueHeader);
                    //		*(float*)(pAnswer+AnswerOffset) = pResultData->H5;
                    //		AnswerOffset += sizeof(float);
                    //		pAnswerHeader->ValuesCounter++;
                    //
                    //		pValueHeader = (CPredictValueHeader*)(pAnswer+AnswerOffset);
                    //		pValueHeader->ValueSize	= sizeof(CPredictValueHeader) + sizeof(float);
                    //		pValueHeader->ValueType	= VT_FLOAT;
                    //		StrCpy(pValueHeader->ValueName, "H6");
                    //		pValueHeader->SizeX		= 1;
                    //		pValueHeader->SizeY		= 1;
                    //		AnswerOffset += sizeof(CPredictValueHeader);
                    //		*(float*)(pAnswer+AnswerOffset) = pResultData->H6;
                    //		AnswerOffset += sizeof(float);
                    //		pAnswerHeader->ValuesCounter++;
                    //
                    //		pValueHeader = (CPredictValueHeader*)(pAnswer+AnswerOffset);
                    //		pValueHeader->ValueSize	= sizeof(CPredictValueHeader) + sizeof(float);
                    //		pValueHeader->ValueType	= VT_FLOAT;
                    //		StrCpy(pValueHeader->ValueName, "H7");
                    //		pValueHeader->SizeX		= 1;
                    //		pValueHeader->SizeY		= 1;
                    //		AnswerOffset += sizeof(CPredictValueHeader);
                    //		*(float*)(pAnswer+AnswerOffset) = pResultData->H7;
                    //		AnswerOffset += sizeof(float);
                    //		pAnswerHeader->ValuesCounter++;
                    //
                    //		pValueHeader = (CPredictValueHeader*)(pAnswer+AnswerOffset);
                    //		pValueHeader->ValueSize	= sizeof(CPredictValueHeader) + sizeof(float);
                    //		pValueHeader->ValueType	= VT_FLOAT;
                    //		StrCpy(pValueHeader->ValueName, "H8");
                    //		pValueHeader->SizeX		= 1;
                    //		pValueHeader->SizeY		= 1;
                    //		AnswerOffset += sizeof(CPredictValueHeader);
                    //		*(float*)(pAnswer+AnswerOffset) = pResultData->H8;
                    //		AnswerOffset += sizeof(float);
                    //		pAnswerHeader->ValuesCounter++;
                    //
                    //		pValueHeader = (CPredictValueHeader*)(pAnswer+AnswerOffset);
                    //		pValueHeader->ValueSize	= sizeof(CPredictValueHeader) + sizeof(float);
                    //		pValueHeader->ValueType	= VT_FLOAT;
                    //		StrCpy(pValueHeader->ValueName, "H8");
                    //		pValueHeader->SizeX		= 1;
                    //		pValueHeader->SizeY		= 1;
                    //		AnswerOffset += sizeof(CPredictValueHeader);
                    //		*(float*)(pAnswer+AnswerOffset) = pResultData->H8;
                    //		AnswerOffset += sizeof(float);
                    //		pAnswerHeader->ValuesCounter++;
                    //
                    //		pValueHeader = (CPredictValueHeader*)(pAnswer+AnswerOffset);
                    //		pValueHeader->ValueSize	= sizeof(CPredictValueHeader) + sizeof(float);
                    //		pValueHeader->ValueType	= VT_FLOAT;
                    //		StrCpy(pValueHeader->ValueName, "H9");
                    //		pValueHeader->SizeX		= 1;
                    //		pValueHeader->SizeY		= 1;
                    //		AnswerOffset += sizeof(CPredictValueHeader);
                    //		*(float*)(pAnswer+AnswerOffset) = pResultData->H9;
                    //		AnswerOffset += sizeof(float);
                    //		pAnswerHeader->ValuesCounter++;
                    //
                    //		pValueHeader = (CPredictValueHeader*)(pAnswer+AnswerOffset);
                    //		pValueHeader->ValueSize	= sizeof(CPredictValueHeader) + sizeof(float);
                    //		pValueHeader->ValueType	= VT_FLOAT;
                    //		StrCpy(pValueHeader->ValueName, "H10");
                    //		pValueHeader->SizeX		= 1;
                    //		pValueHeader->SizeY		= 1;
                    //		AnswerOffset += sizeof(CPredictValueHeader);
                    //		*(float*)(pAnswer+AnswerOffset) = pResultData->H10;
                    //		AnswerOffset += sizeof(float);
                    //		pAnswerHeader->ValuesCounter++;
                    //
                    //		pValueHeader = (CPredictValueHeader*)(pAnswer+AnswerOffset);
                    //		pValueHeader->ValueSize	= sizeof(CPredictValueHeader) + sizeof(float);
                    //		pValueHeader->ValueType	= VT_FLOAT_ARRAY;
                    //		StrCpy(pValueHeader->ValueName, "KQ");
                    //		pValueHeader->SizeX		= 163;
                    //		pValueHeader->SizeY		= 1;
                    //		AnswerOffset += sizeof(CPredictValueHeader);
                    //		MemCpy(pAnswer+AnswerOffset, &pResultData->KQ, sizeof(float)*pValueHeader->SizeX*pValueHeader->SizeY);
                    //		AnswerOffset += sizeof(float) * pValueHeader->SizeX * pValueHeader->SizeY;
                    //		pAnswerHeader->ValuesCounter++;
                    //
                    //		pValueHeader = (CPredictValueHeader*)(pAnswer+AnswerOffset);
                    //		pValueHeader->ValueSize	= sizeof(CPredictValueHeader) + sizeof(float);
                    //		pValueHeader->ValueType	= VT_FLOAT_ARRAY;
                    //		StrCpy(pValueHeader->ValueName, "KV");
                    //		pValueHeader->SizeX		= 163;
                    //		pValueHeader->SizeY		= 7;
                    //		AnswerOffset += sizeof(CPredictValueHeader);
                    //		MemCpy(pAnswer+AnswerOffset, &pResultData->KV, sizeof(float)*pValueHeader->SizeX*pValueHeader->SizeY);
                    //		AnswerOffset += sizeof(float) * pValueHeader->SizeX * pValueHeader->SizeY;
                    //		pAnswerHeader->ValuesCounter++;
                    //
                    //		pAnswerHeader->PackageSize = AnswerOffset;
                    //		if(::NetWriteData(NetSocket, pAnswer, pAnswerHeader->PackageSize, 1000, &ThreadError) != pAnswerHeader->PackageSize)
                    //		{
                    //			CStr Error(CStr("Ошибка при передачи ответа клиенту : ") + ThreadError);
                    //			::pProg->AddEvent(IDI_EVENT_INFO, ::MakeError(ThreadName, NULL, Error));
                    //		}
                    //	}
                    //	::GlobalDelete(pAnswer);
                    //}
                    //pCORE->pForecastResultLst->SetCnt(0);


/*



					// формируем ответ
					unsigned char*	pAnswer			= (unsigned char*)::GlobalNew(sizeof(CPredictAnswerHeader) + sizeof(CPredictValueHeader)*2 + sizeof(float) + 50);
					int				AnswerOffset	= 0;

					CPredictAnswerHeader* pAnswerHeader = (CPredictAnswerHeader*)pAnswer;
					pAnswerHeader->PackageSize		= sizeof(CPredictAnswerHeader);
					pAnswerHeader->AnswerType		= AT_STOP;
					pAnswerHeader->StartTime		= pHeader->StartRequestTime;
					pAnswerHeader->PointTime.Sec	= 0;
					pAnswerHeader->ValuesCounter	= 0;

					AnswerOffset += sizeof(CPredictAnswerHeader);
					CPredictValueHeader* pValueHeader = (CPredictValueHeader*)(pAnswer+AnswerOffset);
					pValueHeader->ValueSize	= sizeof(CPredictValueHeader) + sizeof(float);
					pValueHeader->ValueType	= VT_FLOAT;
					StrCpy(pValueHeader->ValueName, "FloatValue");
					pValueHeader->SizeX		= 1;
					pValueHeader->SizeY		= 1;
					AnswerOffset += sizeof(CPredictValueHeader);
					*(float*)(pAnswer+AnswerOffset) = 1234;
					AnswerOffset += sizeof(float);
					pAnswerHeader->ValuesCounter++;

					CStr String("Строковое знвчение");	int StringLength = String.GetLength();
					pValueHeader = (CPredictValueHeader*)(pAnswer+AnswerOffset);
					pValueHeader->ValueSize	= sizeof(CPredictValueHeader) + StringLength;
					pValueHeader->ValueType	= VT_STRING;
					StrCpy(pValueHeader->ValueName, "StringValue");
					pValueHeader->SizeX		= 0;
					pValueHeader->SizeY		= 0;
					AnswerOffset += sizeof(CPredictValueHeader);
					MemCpy(pAnswer+AnswerOffset, (char*)String, StringLength);
					AnswerOffset += StringLength;
					pAnswerHeader->ValuesCounter++;

					pAnswerHeader->PackageSize = AnswerOffset;
					if(::NetWriteData(NetSocket, pAnswer, pAnswerHeader->PackageSize, 1000, &ThreadError) != pAnswerHeader->PackageSize)
					{
						CStr Error(CStr("Ошибка при передачи ответа клиенту : ") + ThreadError);
						::pProg->AddEvent(IDI_EVENT_INFO, ::MakeError(ThreadName, NULL, Error));
					}
					::GlobalDelete(pAnswer);*/
				}
			}

			if(NetSocket == -1)
			{// нет сокета - нет потока
				::pProg->SetFlag(ThreadFlgStop, SV_NEEDED);
			}

			::pProg->SetFlag(ThreadFlgRun, SV_READY);
		}
		else if(::pProg->GetFlag(ThreadFlgStop) == SV_NEEDED)
		{// надо выгружаться
			break;
		}
		else
		{// заснем немного если не надо регистрировать данные
			ThreadYield();
		}
	}// конец основного цикла потока

	return bProcess;
}

/*
void CPredictClient::PrepareClientRequest(unsigned char* pPackage, CPredictRequestHeader* pHeader, LST* pEtapLst)
{
	if(pPackage)
	{
		MemCpy(pHeader, (CPredictRequestHeader*)pPackage, sizeof(CPredictRequestHeader));
		printf("Recv request from %s RequestType=%d EtapCnt=%d StartTime=%s\n", pHeader->RequestName, pHeader->RequestType, pHeader->EtapCounter, (char*)TimeToStr(&pHeader->StartRequestTime));

		pEtapLst->SetSize(pHeader->EtapCounter);
		pEtapLst->SetCnt(pHeader->EtapCounter);

		Etap_data	EtapData;

		int EtapOffset = sizeof(CPredictRequestHeader);
		for(int EtapPos=0 ; EtapPos<pHeader->EtapCounter ; EtapPos++)
		{
			CPredictEtapHeader* pEtapHeader = (CPredictEtapHeader*)(pPackage + EtapOffset);
			printf("\tEtap %d duration=%s, valuecnt=%d\n", EtapPos, (char*)TimeToStr(&pEtapHeader->Duration), pEtapHeader->ValuesCounter);

			EtapData.Etap_number	= EtapPos+1;
			EtapData.Etap_Duration	= pEtapHeader->Duration;

			int ValueOffset = EtapOffset + sizeof(CPredictEtapHeader);
			for(int ValuePos=0 ; ValuePos<pEtapHeader->ValuesCounter ; ValuePos++)
			{
				CPredictValueHeader* pValueHeader = (CPredictValueHeader*)(pPackage + ValueOffset);
				printf("\t\tValue %d name=%s, type=%d Size(%d, %d)\n", ValuePos, pValueHeader->ValueName, pValueHeader->ValueType, pValueHeader->SizeX, pValueHeader->SizeY);

				long OffsetInStruct = GetOffsetInStruct(pValueHeader->ValueName);
				if(OffsetInStruct >= 0)
				{
					//printf("\tValue %s\n", ((char*)(pPackage + ValueOffset)));
					if(pValueHeader->ValueType == VT_FLOAT)
					{
						MemCpy((unsigned char*)&EtapData+OffsetInStruct, pPackage+sizeof(CPredictValueHeader)+ValueOffset, sizeof(float));
					}
					else if(pValueHeader->ValueType == VT_STRING)
					{
						MemCpy(&EtapData+OffsetInStruct, pPackage+sizeof(CPredictValueHeader)+ValueOffset, pValueHeader->ValueSize-sizeof(CPredictValueHeader));
						printf("\tValue %s\n", ((char*)(pPackage + ValueOffset)));
					}
					else if(pValueHeader->ValueType == VT_FLOAT_ARRAY)
					{
//						for(int PosX=0 ; PosX<pValueHeader->SizeX ; PosX++)
//						{
//							for(int PosY=0 ; PosY<pValueHeader->SizeY ; PosY++)
//							{
//								printf("\t\t\t(%d, %d) %g\n", PosX, PosY, *( (float*)(pPackage + sizeof(CPredictValueHeader) + ValueOffset) + PosX*pValueHeader->SizeY+PosY));
//							}
//						}
//
//						//printf("\tValue %s\n", ((char*)(pPackage + ValueOffset)));
					}
				}

				ValueOffset += pValueHeader->ValueSize;
			}

			if(0)
			{// отладка
				printf("Etap_number=%d\n", EtapData.Etap_number);
				printf("Etap_Duration=%s\n", (char*)TimeToStr(&EtapData.Etap_Duration, MTD_DURATIONTIME));
				printf("iPOWER=%g\n", EtapData.iPOWER);
				printf("iH10=%g\n", EtapData.iH10);
				printf("iH9=%g\n", EtapData.iH9);
				printf("iH8=%g\n", EtapData.iH8);
				printf("iH7=%g\n", EtapData.iH7);
				printf("iH6=%g\n", EtapData.iH6);
				printf("iH5=%g\n", EtapData.iH5);
				printf("iH4=%g\n", EtapData.iH4);
				printf("iH3=%g\n", EtapData.iH3);
				printf("iH2=%g\n", EtapData.iH2);
				printf("iH1=%g\n", EtapData.iH1);
				printf("iCB=%g\n", EtapData.iCB);
				printf("iTIN=%g\n", EtapData.iTIN);
				printf("iGV1K=%g\n", EtapData.iGV1K);
				printf("iP1K=%g\n", EtapData.iP1K);
				printf("iOFSTS=%g\n", EtapData.iOFSTS);
				printf("iSGCN1=%g\n", EtapData.iSGCN1);
				printf("iSGCN2=%g\n", EtapData.iSGCN2);
				printf("iSGCN3=%g\n", EtapData.iSGCN3);
				printf("iSGCN4=%g\n", EtapData.iSGCN4);
				printf("iGpodp=%g\n", EtapData.iGpodp);
				printf("iCBpodp=%g\n", EtapData.iCBpodp);
				for(int Sz=0 ; Sz<=60 ; Sz++)
					printf("iSZ[%d]=%g\n", Sz+1, EtapData.iSZ[Sz]);
				printf("iCritParam=%g\n", EtapData.iCritParam);
				printf("iAOStrategy=%g\n", EtapData.iAOStrategy);
				printf("iSZStrategy=%g\n", EtapData.iSZStrategy);
				printf("iSZInterception=%g\n", EtapData.iSZInterception);
				printf("iKV_SIZE=%g\n", EtapData.iKV_SIZE);
				printf("iDNBR_SIZE=%g\n", EtapData.iDNBR_SIZE);
				printf("iQVMF_SIZE=%g\n", EtapData.iQVMF_SIZE);
			}


			EtapOffset += pEtapHeader->EtapSize;
			pEtapLst->Set(EtapData.Etap_number, (int)&EtapData);
		}
	}
}
*/

/*
long CPredictClient::GetOffsetInStruct(const char* pValueName)
{
	int RetValue = -1;
	Etap_data* pEtapStruct = NULL;
	if(!StrCmp(pValueName, "iPOWER"))				RetValue = (long)&pEtapStruct->iPOWER;
	else if(!StrCmp(pValueName, "iH10"))			RetValue = (long)&pEtapStruct->iH10;
	else if(!StrCmp(pValueName, "iH9"))				RetValue = (long)&pEtapStruct->iH9;
	else if(!StrCmp(pValueName, "iH8"))				RetValue = (long)&pEtapStruct->iH8;
	else if(!StrCmp(pValueName, "iH7"))				RetValue = (long)&pEtapStruct->iH7;
	else if(!StrCmp(pValueName, "iH6"))				RetValue = (long)&pEtapStruct->iH6;
	else if(!StrCmp(pValueName, "iH5"))				RetValue = (long)&pEtapStruct->iH5;
	else if(!StrCmp(pValueName, "iH4"))				RetValue = (long)&pEtapStruct->iH4;
	else if(!StrCmp(pValueName, "iH3"))				RetValue = (long)&pEtapStruct->iH3;
	else if(!StrCmp(pValueName, "iH2"))				RetValue = (long)&pEtapStruct->iH2;
	else if(!StrCmp(pValueName, "iH1"))				RetValue = (long)&pEtapStruct->iH1;
	else if(!StrCmp(pValueName, "iCB"))				RetValue = (long)&pEtapStruct->iCB;
	else if(!StrCmp(pValueName, "iTIN"))			RetValue = (long)&pEtapStruct->iTIN;
	else if(!StrCmp(pValueName, "iGV1K"))			RetValue = (long)&pEtapStruct->iGV1K;
	else if(!StrCmp(pValueName, "iP1K"))			RetValue = (long)&pEtapStruct->iP1K;
	else if(!StrCmp(pValueName, "iOFSTS"))			RetValue = (long)&pEtapStruct->iOFSTS;
	else if(!StrCmp(pValueName, "iSGCN1"))			RetValue = (long)&pEtapStruct->iSGCN1;
	else if(!StrCmp(pValueName, "iSGCN2"))			RetValue = (long)&pEtapStruct->iSGCN2;
	else if(!StrCmp(pValueName, "iSGCN3"))			RetValue = (long)&pEtapStruct->iSGCN3;
	else if(!StrCmp(pValueName, "iSGCN4"))			RetValue = (long)&pEtapStruct->iSGCN4;
	else if(!StrCmp(pValueName, "iGpodp"))			RetValue = (long)&pEtapStruct->iGpodp;
	else if(!StrCmp(pValueName, "iCBpodp"))			RetValue = (long)&pEtapStruct->iCBpodp;
	else if(!StrCmp(pValueName, "iCBpodp"))			RetValue = (long)&pEtapStruct->iCBpodp;
	else if(!StrCmp(pValueName, "iCritParam"))		RetValue = (long)&pEtapStruct->iCritParam;
	else if(!StrCmp(pValueName, "iAOStrategy"))		RetValue = (long)&pEtapStruct->iAOStrategy;
	else if(!StrCmp(pValueName, "iSZStrategy"))		RetValue = (long)&pEtapStruct->iSZStrategy;
	else if(!StrCmp(pValueName, "iSZInterception"))	RetValue = (long)&pEtapStruct->iSZInterception;
	else if(!StrCmp(pValueName, "iKV_SIZE"))		RetValue = (long)&pEtapStruct->iKV_SIZE;
	else if(!StrCmp(pValueName, "iDNBR_SIZE"))		RetValue = (long)&pEtapStruct->iDNBR_SIZE;
	else if(!StrCmp(pValueName, "iQVMF_SIZE"))		RetValue = (long)&pEtapStruct->iQVMF_SIZE;
	else if(StrStr(pValueName, "iSZ") == pValueName)
	{
		int Index = StrToLong(pValueName+3);
		RetValue = (long)&pEtapStruct->iSZ[Index-1];
	}

	return RetValue;
}
*/
