#include <ImCoreMain.h>

//////////////////////////////////////////////////////////////////////
// ������ ��� ����������� ������
CRegisterServer::CRegisterServer(const char* pServerName)
{
	CRegisterServer::ServerName				= pServerName;
	CRegisterServer::pRegisterPackLst		= new LONG_LST(100);
	CRegisterServer::pRegRequest			= NULL;
	CRegisterServer::pRegAnswer				= NULL;
	CRegisterServer::RegRequestID			= 0;
	CRegisterServer::LastRegisterParamCnt	= 0;
	CRegisterServer::MutexID				= ::MutexCreate();
	CRegisterServer::RegThreadPid			= 0;
	CRegisterServer::RegThreadName			= CStr("RegisterData(") + CRegisterServer::ServerName + ")";
	CRegisterServer::RegChannelName			= ::pProg->GetTaskName();
	CRegisterServer::bFullRegData			= true;
	CRegisterServer::LastRegFullDataTime	= 0;
	CRegisterServer::bFirst					= true;
}
CRegisterServer::~CRegisterServer()
{
	if(Lock())
	{
		FreeRequest();
		ClearPacket();
		delete pRegisterPackLst;
		Unlock();
	}
	if(MutexID) MutexDestroy(MutexID);
}
void CRegisterServer::FreeRequest()
{
	if(RegRequestID)
	{// ����������� ������� ������� ��������
		::SrvFreeRequest(RegRequestID);
		RegRequestID = 0;
	}
	if(pRegRequest)
	{// ������� ����� ������ � ������� ��������� ������
		::SrvFreeMem(pRegRequest);
		pRegRequest = NULL;
	}
	if(pRegAnswer)
	{// ������� ����� ������ � ������� ��������� ������
		::SrvFreeMem(pRegAnswer);
		pRegAnswer = NULL;
	}
}
void CRegisterServer::AddPacket(unsigned char* pPacketData, int PacketSize, int MaxPackCnt)
{
    if(Lock())
    {
		if(MaxPackCnt)
		{// �������� ������������ �������
			if(pRegisterPackLst->ItemCnt > MaxPackCnt)
			{// ���� ������� �����������, ������� ��� ������ MaxPackCnt
	//puts("Clear Packet");
				for(int PackPos=1 ; PackPos<=pRegisterPackLst->ItemCnt-MaxPackCnt ; PackPos++)
				{
					unsigned char* pDelPacket = (unsigned char*)pRegisterPackLst->Get(1);
					::GlobalDelete(pDelPacket);
					pRegisterPackLst->Del(1);
				}
			}
		}

		if((RegRequestID||!pRegisterPackLst->ItemCnt) && pPacketData && PacketSize>0)
		{// ��������� ������ ���� ���� ��� ��������� � �������� ������

			unsigned char* pAddData = (unsigned char*)::GlobalNew(PacketSize);
			MemCpy(pAddData, pPacketData, PacketSize);
			pRegisterPackLst->Add((long)pAddData);
		}
		Unlock();
    }
}
void CRegisterServer::ClearPacket()
{
	for(int RegisterPackPos=pRegisterPackLst->GetCnt() ; RegisterPackPos>=1 ; RegisterPackPos--)
		::GlobalDelete((void*)pRegisterPackLst->Get(RegisterPackPos));
	pRegisterPackLst->SetCnt(0);
}

////////////////////////////////////////////////////////////////////////////////////////
// ����������� ������ (�������� ���������)
bool CProg::RegThread(CRegisterServer* pServer)
{
	bool			bRegisterData 			= true;
	CStr			Error;
	char			szErrorText[256];		szErrorText[0] = 0;
	int				NeededAnswerSize		= 0;
	int				RealAnswerSize			= 0;
	double			PrevServerVerifyTime	= 0;
	bool			bPutsAccessMessage		= true;
	int				MaxPackageSize  		= sizeof(CSrvHeaderPackage) + sizeof(char) + sizeof(CTime) + sizeof(char) + sizeof(int);

	int				RegisterPackCnt			= 0;
	int				SendedPackageNum		= 0;	// ���������� ������ ������������ �������
	LONG_LST		TmpSourceDataLst;
	LONG_LST		TmpPackageLst;
	int				MultyPackageSize		= 0;

	//puts(CStr("RegisterThread ParamCnt=") + pRegIdb->pParamLst->ItemCnt + " MaxRegisterRequestSize=" + MaxRegisterRequestSize);

	pServer->Error.Empty();

	for( int ParamPos=1; ParamPos<=pRegIdb->pParamLst->GetCnt(); ParamPos++ )
	{
		CRegParamInfo* pParamInfo = (CRegParamInfo*)pRegIdb->pParamLst->Get(ParamPos);
		if(pParamInfo && pParamInfo->OutsideID) MaxPackageSize += sizeof(int) + pParamInfo->ValueByteSize + sizeof(char);
	}

	while( bRegisterData )
	{// �������� ���� ������

		if(pServer->pRegisterPackLst->Lock())
		{// ������� ���������� ������ � ������� �� �����������
			RegisterPackCnt = pServer->pRegisterPackLst->GetCnt();
			pServer->pRegisterPackLst->Unlock();
		}

		if( !pServer->RegRequestID &&  RegisterPackCnt )
		{// ���� ����� �� ������, �� ������� ��� ������ �� �������
			if(pServer->pRegisterPackLst->Lock())
			{
				for( int Pos=1; Pos<=pServer->pRegisterPackLst->ItemCnt; Pos++ )
					FreeRegisterPackage((unsigned char*)pServer->pRegisterPackLst->Get(Pos));
				pServer->pRegisterPackLst->SetCnt(0);
				pServer->pRegisterPackLst->Unlock();
			}
			RegisterPackCnt 	= 0;
			SendedPackageNum 	= 0;
		}

		if((GetFlag(pServer->RegThreadFlgStop)!=SV_NEEDED && RegisterPackCnt) || (!pServer->RegRequestID && ((double)GetCurrentTime()-PrevServerVerifyTime > 1000)) )
		{// ���� ��������� ������� �� ����������� � ��� ��������

			if( RegisterPackCnt && pServer->pRegisterPackLst->Lock() )
			{// ��������� �� ������� ��� ������ ����������� ��� �����������
				for(int PackPos=1; PackPos<=RegisterPackCnt; PackPos++ )
				{
					TmpSourceDataLst.Add(pServer->pRegisterPackLst->Get(1));
					pServer->pRegisterPackLst->Del(1);
				}
				pServer->pRegisterPackLst->Unlock();
			}

			for( int SourceDataPos=1; SourceDataPos<=TmpSourceDataLst.GetCnt(); SourceDataPos++ )
			{// ��������������� ������ � ���� �������� ��� �����������
				unsigned char* 	pSourceData 	= (unsigned char*)TmpSourceDataLst.Get(SourceDataPos);

				int				DataSize 		= *(int*)(pSourceData + sizeof(CTime) + sizeof(int)*0);
				int				ParamCnt 		= *(int*)(pSourceData + sizeof(CTime) + sizeof(int)*1);
				unsigned char*	pNextParam 		= pSourceData + sizeof(CTime) + sizeof(int)*3;
				int				PackageSize  	= sizeof(CSrvHeaderPackage) + sizeof(char) + sizeof(CTime) + sizeof(char) + sizeof(int) + DataSize;
				unsigned char* 	pPackage 		= (unsigned char*)GlobalNew(PackageSize);
				unsigned char* 	pPackOffset 	= pPackage + sizeof(CSrvHeaderPackage);

				::SrvInitHeaderPackage((CSrvHeaderPackage*)pPackage, PT_SEND_CURRENT_DATA,  PackageSize, PSTYLE_VALUE_STATUS);

				((CSrvHeaderPackage*)pPackage)->PackageNum 		= ++SendedPackageNum; // 23.06.2016

				*(char*)pPackOffset		= RHT_TIME;					pPackOffset += sizeof(char);
				*(CTime*)pPackOffset	= *(CTime*)pSourceData;		pPackOffset += sizeof(CTime);
				*(char*)pPackOffset		= RHT_VALUES_ARRAY;			pPackOffset += sizeof(char);
				*(int*)pPackOffset		= ParamCnt;					pPackOffset += sizeof(int);

//puts(CStr("RegisterData  Time=") + (CTime*)pSourceData + " ParamCnt=" + ParamCnt + " PackageNum=" + PackageSize + " SendedPackageNum=" + SendedPackageNum);
				for( int ParamPos=1; ParamPos<=ParamCnt; ParamPos++ )
				{
					CRegParamInfo* pParamInfo = (CRegParamInfo*)*(int*)pNextParam;
					// �������� ��� �������
					*(int*)pPackOffset = pParamInfo->OutsideID;

					// ����� ��������
					_CopyOutsideValue(pParamInfo->OutsideType, (unsigned char*)(pPackOffset + sizeof(int)), (unsigned char*)(pNextParam+sizeof(int)), true );

					// ��������� � ���������� ������ ��������
					pPackOffset += sizeof(int) + pParamInfo->ValueByteSize + sizeof(char);

//if(!StrCmp(pParamInfo->ShortName, "YAF_S"))
//{
//	puts(CStr("RegisterThread ShortName=") + pParamInfo->ShortName + " OutsideID=" + pParamInfo->OutsideID +" ParamPos=" + ParamPos+" Value="+*(float*)(pNextParam+sizeof(int)));
//	printf("pParamInfo=%p\n", pParamInfo);
//}
					// ��������� � ���������� ���������
					pNextParam += sizeof(int)+pParamInfo->ValueByteSize + sizeof(char);
				}

				// ���������� ���� ���������� ������ �� �����������
				pServer->SystemRegTime = *(CTime*)pSourceData;

				// ��������� �������� ������ ������
				((CSrvHeaderPackage*)pPackage)->PackageSize = pPackOffset - pPackage;

//::pProgRepl->AddEvent(IDI_EVENT_INFO, MakeProgReplError(pThreadName, NULL, CStr("PackTime=") + (CTime*)(pPackage+sizeof(CSrvHeaderPackage) + sizeof(char)) + " ParamCnt=" + ParamCnt + " PackSize=" + ((CSrvHeaderPackage*)pPackage)->PackageSize));

				TmpPackageLst.Add((int)pPackage);


				FreeRegisterPackage(pSourceData);
			}
			TmpSourceDataLst.SetCnt(0);

			if( TmpPackageLst.GetCnt() )
			{
				LONG_LST	OffsetLst(TmpPackageLst.GetCnt());	OffsetLst.SetCnt(TmpPackageLst.GetCnt());
				MultyPackageSize = sizeof(CSrvHeaderPackage) + sizeof(int) + sizeof(int)*(TmpPackageLst.GetCnt());

				for(int PackagePos=1 ; PackagePos<=TmpPackageLst.GetCnt() ; PackagePos++)
				{// ���������� ������ ������ � �������� ������ ��������
					CSrvHeaderPackage* pPackHeader	= (CSrvHeaderPackage*)TmpPackageLst.Get(PackagePos);
					OffsetLst.Set(PackagePos, MultyPackageSize);
					MultyPackageSize += pPackHeader->PackageSize;
				}

				if(!pServer->pRegRequest || pServer->pRegRequest->Size<MultyPackageSize)
				{
					if(pServer->pRegRequest) ::SrvFreeMem(pServer->pRegRequest);
					pServer->pRegRequest = ::SrvAllocMem(MultyPackageSize, szErrorText, sizeof(szErrorText));
				}

				if(pServer->pRegRequest)
				{
					// ���������
					::SrvInitHeaderPackage((CSrvHeaderPackage*)pServer->pRegRequest->pMem, PT_SEND_CURRENT_DATA, MultyPackageSize, PSTYLE_VALUE_STATUS|PSTYLE_MULTY_PACK);
					int Offset = sizeof(CSrvHeaderPackage);

					((CSrvHeaderPackage*)pServer->pRegRequest->pMem)->PackageNum = 0;//++SendedPackageNum;

					// ���-�� �������
					*((int *)(pServer->pRegRequest->pMem + Offset))	= RegisterPackCnt;	Offset += sizeof(int);

					// ��������
					int OffsetCnt = OffsetLst.GetCnt();
					for(int OffsetPos=1 ; OffsetPos<=OffsetCnt ; OffsetPos++)
					{
						*((int *)(pServer->pRegRequest->pMem + Offset)) = OffsetLst.Get(OffsetPos);
						Offset += sizeof(int);
					}

					// ������
					for(int PackagePos=1 ; PackagePos<=TmpPackageLst.GetCnt() ; PackagePos++)
					{
						CSrvHeaderPackage* pPackHeader	= (CSrvHeaderPackage*)TmpPackageLst.Get(PackagePos);
						MemCpy(pServer->pRegRequest->pMem+Offset, pPackHeader, pPackHeader->PackageSize);
						Offset += pPackHeader->PackageSize;
					}


//puts(CStr("MultyPackageSize ") + MultyPackageSize + " PackCnt=" + TmpPackageLst.GetCnt() + " Offset=" + Offset);
//::pProgRepl->AddEvent(IDI_EVENT_ERROR, MakeProgReplError(pThreadName, NULL, CStr("MultyPackageSize ") + MultyPackageSize + " PackCnt=" + TmpPackageLst.GetCnt() + "Offset=" + Offset));

//					CSrvHeaderPackage* pPackHeader	= (CSrvHeaderPackage*)TmpPackageLst.Get(1);
//					MemCpy(pRegRequest->pMem, pPackHeader, pPackHeader->PackageSize);


					// ������ �� �����
					for(int PackagePos=TmpPackageLst.GetCnt(); PackagePos>=1 ; PackagePos--)
					{
						CSrvHeaderPackage* pPackHeader	= (CSrvHeaderPackage*)TmpPackageLst.Get(PackagePos);
						::GlobalDelete(pPackHeader);
						TmpPackageLst.Del(PackagePos);
					}
				}
			}

			if( bRegisterData && !pServer->RegRequestID )
			{// ������� ����� ����� � �������� ������
				bool bServerAccess = false;
				if((double)GetCurrentTime()-PrevServerVerifyTime > 1000)
				{
					int			SrvInfoCnt	= 1;
					CSrvInfo	SrvInfoArray[1]; MemSet(SrvInfoArray, 0, sizeof(CSrvInfo));
					if(SrvGetServersInfo(CStr(pServer->RegChannelName)+" "+pSettings->NodeName, SrvInfoArray, &SrvInfoCnt, pServer->ServerName, szErrorText, sizeof(szErrorText),(char*)pSettings->SrvAgentTaskName))
					{
						if(SrvInfoCnt && SrvInfoArray[0].ServerStatus&SRV_STATE_ENABLED )
						{// ������ ������ � �������
							bServerAccess		= true;
							bPutsAccessMessage	= true;
						}
						else if(bPutsAccessMessage)
						{
							pServer->Error = CStr("������")+" "+pServer->ServerName+" ����������";
							AddEvent(IDI_EVENT_INFO, MakeError(pServer->RegThreadName, NULL, pServer->Error));
							bPutsAccessMessage = false;
						}
					}
					PrevServerVerifyTime = (double)GetCurrentTime();
				}

				if(bServerAccess)
				{// ���� ������ ������ � �������

					if(!pServer->pRegRequest || pServer->pRegRequest->Size<MaxPackageSize)
					{
						if(pServer->pRegRequest) ::SrvFreeMem(pServer->pRegRequest);
						pServer->pRegRequest = ::SrvAllocMem(MaxPackageSize, szErrorText, sizeof(szErrorText));
					}

					if(pServer->pRegRequest)
					{
						unsigned char* pRequestOffset = (unsigned char*)pServer->pRegRequest->pMem + sizeof(CSrvHeaderPackage);
						::SrvInitHeaderPackage((CSrvHeaderPackage*)pServer->pRegRequest->pMem, PT_SEND_CURRENT_DATA, sizeof(CSrvHeaderPackage) + sizeof(int), PSTYLE_VALUE_STATUS);

						*(char*)pRequestOffset	= RHT_TIME;					pRequestOffset += sizeof(char);
						*(CTime*)pRequestOffset	= GetCurrentTime();			pRequestOffset += sizeof(CTime);
						*(char*)pRequestOffset	= RHT_VALUES_ARRAY;			pRequestOffset += sizeof(char);
						*(int*)pRequestOffset	= 0;						pRequestOffset += sizeof(int);

						SendedPackageNum = 1;
						((CSrvHeaderPackage*)pServer->pRegRequest->pMem)->PackageNum  = SendedPackageNum;
						((CSrvHeaderPackage*)pServer->pRegRequest->pMem)->PackageSize = pRequestOffset-(unsigned char*)pServer->pRegRequest->pMem;


//::pProgRepl->AddEvent(IDI_EVENT_ERROR, MakeProgReplError(pThreadName, NULL, "AllocRequest"));

						if( (pServer->RegRequestID = SrvAllocRequest(CStr(pServer->RegChannelName)+" "+pSettings->NodeName, pServer->ServerName, pServer->pRegRequest, 3, szErrorText, sizeof(szErrorText),(char*)pSettings->SrvAgentTaskName))==0 )
						{  bRegisterData = false; Error	= szErrorText; }
						else
						{
							pServer->Error.Empty();
							AddEvent(IDI_EVENT_INFO, MakeError(pServer->RegThreadName, NULL, CStr((char*)(pServer->bFirst?"������":"������������")) + " ����� SEND_CURRENT_DATA � " + pServer->ServerName));
						}
					}
				}
				else ThreadYield();
			}
			else if(bRegisterData && pServer->RegRequestID)
			{// ���� ����� ��� ������

//				puts(CStr("RegThread Time=") + (CTime*)(pServer->pRegRequest->pMem + sizeof(CSrvHeaderPackage) + sizeof(char)) + " PackageNum=" + ((CSrvHeaderPackage*)pServer->pRegRequest->pMem)->PackageNum + " Owner=" + ((CSrvHeaderPackage*)pServer->pRegRequest->pMem)->szOwner);

				if( SrvSendRequestData( pServer->RegRequestID, pServer->pRegRequest, szErrorText, sizeof(szErrorText))==0 )
					{  bRegisterData = false; Error	= szErrorText; }
			}

			if( bRegisterData && pServer->RegRequestID )
			{// ���� ����� ����� ������, �� ���������� ������������� ������
				pServer->bFirst = false;

				if( (NeededAnswerSize = ::SrvGetAnswerState(pServer->RegRequestID, true, szErrorText, sizeof(szErrorText))) > 0 )
				{
					if( pServer->pRegAnswer &&  pServer->pRegAnswer->Size < NeededAnswerSize )
					{// ���� ������ ����������� ������ ������ ����������, �� ����������� ���
						::SrvFreeMem(pServer->pRegAnswer);
						pServer->pRegAnswer = NULL;
					}

					// �������� ������ ��� �����
					if( !pServer->pRegAnswer )
						pServer->pRegAnswer = ::SrvAllocMem(NeededAnswerSize, szErrorText, sizeof(szErrorText) );

					if( pServer->pRegAnswer )
					{
						if( (RealAnswerSize = ::SrvFetchAnswer(pServer->RegRequestID, true, pServer->pRegAnswer, szErrorText, sizeof(szErrorText))) > 0 )
						{// ������ ���������� � �������� �������������

							// ������ ���������� ����� ������
							pServer->Error.Empty();

							pServer->LastRegTime = GetCurrentTime();
//								if( pSettings->bEnablePutsRegisterData )
//									puts( MakeProgReplError( pThreadName, NULL, CStr("����� N ") + ((CSrvHeaderPackage*)pRegRequest->pMem)->PackageNum + ", Time=" + TimeToStr(pPackageTime)));

							// ����� ����� ���������� � �������� �����������, ��������� � �������
							// ...
						}
						else{ bRegisterData = false; Error	= szErrorText; }
					}else{ bRegisterData = false; Error	= szErrorText; }
				}else{ bRegisterData = false; Error	= szErrorText; }
			}

			if(!bRegisterData)
			{// ���� ���� ������, �� ������������ ��
				bRegisterData = true;

				if(pServer->RegRequestID) pServer->Error = CStr("������ ����� SEND_CURRENT_DATA � ") + pServer->ServerName;
				else pServer->Error = Error;
				AddEvent(IDI_EVENT_ERROR, MakeError(pServer->RegThreadName, NULL, pServer->Error));

				if(pServer->RegRequestID)
				{// ����������� ������� ������� ��������
					::SrvFreeRequest(pServer->RegRequestID);	pServer->RegRequestID = 0;
					PrevServerVerifyTime = (double)GetCurrentTime()+1000;
					bPutsAccessMessage	= true;
				}
			}
//
//
//			}
//			else{ bRegisterData = false; Error = szErrorText; } // ������ ��� ��������� ������
		}
		else if(GetFlag(pServer->RegThreadFlgStop) == SV_NEEDED)
		{// ���� �����������
			break;
		}
		else
		{// ������ ������� ���� �� ���� �������������� ������
			ThreadYield();
		}
	}// ����� ��������� ����� ������


	if( pServer->RegRequestID )
	{// ����������� ������� ������� ��������		
		::SrvFreeRequest(pServer->RegRequestID);
		pServer->RegRequestID = 0;
	}	

	if( pServer->pRegRequest)
	{// ������� ����� ������ � ������� ��������� ������	
		::SrvFreeMem(pServer->pRegRequest);
		pServer->pRegRequest = NULL;
	}	

	if( pServer->pRegAnswer)
	{// ������� ����� ������ � ������� ��������� ������	
		::SrvFreeMem(pServer->pRegAnswer);
		pServer->pRegAnswer = NULL;
	}

//	if(!bRegisterData && pServer->Error.GetLength())
//	{
//		if(pError) *pError = pServer->Error;
//		else puts((char*)pServer->Error);
//	}

	return bRegisterData;
}
unsigned char*	CProg::MakeRegisterPackage(int DataSize, int ValuesCnt)
{// �������� ����� ������ ��� ������ ��� �����������
	int				PackageSize = sizeof(CTime) + sizeof(int)*3 + DataSize;
	unsigned char*	pPackage	= (unsigned char*)GlobalNew(PackageSize);
	
	*(int*)(pPackage + sizeof(CTime) + sizeof(int)*0) = DataSize;
	*(int*)(pPackage + sizeof(CTime) + sizeof(int)*1) = ValuesCnt;
	*(int*)(pPackage + sizeof(CTime) + sizeof(int)*2) = sizeof(CTime) + sizeof(int)*3;

//puts(CStr("MakeRegisterPackage DataSize=") + DataSize + " ValuesCnt=" + ValuesCnt);
//printf("MakeRegisterPackage DataSize=%d,  ValuesCnt=%d %p PackageSize=%d\n", DataSize, ValuesCnt, pPackage, PackageSize);


	return pPackage;
}
void CProg::FreeRegisterPackage(unsigned char* pPackage)
{
//printf("CProg::FreeRegisterPackage Time=%s Size=%d %p\n", (char*)TimeToStr((CTime*)pPackage), *(int*)(pPackage+sizeof(CTime)), pPackage);
//printf("CProg::FreeRegisterPackage Size=%d %p\n", *(int*)(pPackage+sizeof(CTime)), pPackage);
    if(pPackage)  ::GlobalDelete(pPackage);
}
void CProg::AppendRegisterPackage(unsigned char*& pPackage, CRegParamInfo* pParamInfo, unsigned char* pValue)
{// ��������� � ����� �������� 
//	int  			DataSize 	= *(int*)(pPackage+sizeof(CTime)+sizeof(int)*0);
//	int  			ValuesCnt 	= *(int*)(pPackage+sizeof(CTime)+sizeof(int)*1)+1;
	unsigned int 	NextPos 	= *(int*)(pPackage+sizeof(CTime)+sizeof(int)*2);
	unsigned char*	pNextParam 	= pPackage + NextPos;

//	if( NextPos < (sizeof(CTime) + sizeof(int)*3 + DataSize) )
	{
		*(int*)pNextParam = (int)pParamInfo;

		if( ::IsArray(pParamInfo->OutsideType) )
				_CopyOutsideValue(pParamInfo->OutsideType, pNextParam+sizeof(int), (unsigned char*)*(unsigned int*)pValue, true );
		else	_CopyOutsideValue(pParamInfo->OutsideType, pNextParam+sizeof(int), pValue, true );

		// �������� ��������� �� ��������� ��������
		*(int*)(pPackage + sizeof(CTime) + sizeof(int)*1) += 1;
		*(int*)(pPackage + sizeof(CTime) + sizeof(int)*2) += sizeof(int) + pParamInfo->ValueByteSize + sizeof(char);
//puts(CStr("      AppendRegisterPackage PackageSize=") + (sizeof(CTime) + sizeof(int)*3 + DataSize) + " ValuesCnt=" + ValuesCnt + " NextPos=" + NextPos + " "  + pParamInfo->ShortName + " ChangeFlag=" + (int)pParamInfo->ChangeFlag);
	}
}
void CProg::SendRegisterPackage(CTime PacketTime, unsigned char* pPackage, int PackageSize)
{// ������ ����� � ������� �� ��������
	*(CTime*)pPackage	= PacketTime;
	int RegSrvCnt = pRegisterServersLst->GetCnt();
	for(int RegSrvPos=1 ; RegSrvPos<=RegSrvCnt ; RegSrvPos++)
	{
		CRegisterServer* pServer = (CRegisterServer*)pRegisterServersLst->Get(RegSrvPos);
		pServer->AddPacket(pPackage, PackageSize, 0);
	}
	FreeRegisterPackage(pPackage);
}

