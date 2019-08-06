#include <ImCoreMain.h>

bool CProg::ScanThread(const char* pThreadName, CStr* pError)
{// ������������ ������ (�������� ���������)
	CStr	Error;
	char	szErrorText[256];	szErrorText[0] = 0;
	bool	bScanData			= true;
	bool	bFirstOpenChannel	= true;
	int		NeededAnswerSize	= 0;
	int		RealAnswerSize		= 0;

	double	PrevCurServerVerifyTime	= 0.0;
	double	PrevMainServerVerifyTime= 0.0;
	bool	bPutsMainAccessMessage	= true;
	bool	bPutsReservAccessMessage= true;
	double	TacktCounter			= 0;

	ErrorScanData.Empty();

	while(bScanData)
	{// �������� ���� ������
		if(GetFlag(FLG_STOP_SCAN_THREAD) != SV_NEEDED)
		{
			SetFlag(FLG_RUN_SCAN_THREAD, SV_PROCESS);

			if( !CProg::ScanRequestID )
			{// ���� ����� ��������� ������� ������ �� ������
				bool bMainServerAccess		= false;
				bool bReservServerAccess	= false;
				if((double)GetCurrentTime()-PrevCurServerVerifyTime > 1000)
				{
					int			SrvInfoCnt	= MAX_SERVER_INFO_CNT;
					CSrvInfo	SrvInfoArray[MAX_SERVER_INFO_CNT];
					if(SrvGetServersInfo( CStr("ImCore ") + pProg->pSettings->NodeName, SrvInfoArray, &SrvInfoCnt, NULL, szErrorText, sizeof(szErrorText)))
					{
						for(int SrvInfoPos=0 ; SrvInfoPos<SrvInfoCnt ; SrvInfoPos++)
						{// ���� ������� � ��������� ������� ����� ��������� �� SrvAgent
							if(	pSettings->MainServer.GetLength() &&
								pSettings->MainServer==SrvInfoArray[SrvInfoPos].ServerName &&
								SrvInfoArray[SrvInfoPos].ServerStatus&SRV_STATE_ENABLED &&
								SrvInfoArray[SrvInfoPos].ServerStatus&SRV_STATE_CURRENT_DATA &&
								!(SrvInfoArray[SrvInfoPos].ServerStatus&SRV_STATE_SUSPECT) )
							{
								bMainServerAccess		= true;
								bPutsMainAccessMessage	= true;
							}
							if(	pSettings->ReservServer.GetLength() &&
								pSettings->ReservServer==SrvInfoArray[SrvInfoPos].ServerName &&
								SrvInfoArray[SrvInfoPos].ServerStatus&SRV_STATE_ENABLED &&
								SrvInfoArray[SrvInfoPos].ServerStatus&SRV_STATE_CURRENT_DATA &&
								!(SrvInfoArray[SrvInfoPos].ServerStatus&SRV_STATE_SUSPECT) )
							{
								bReservServerAccess		= true;
								bPutsReservAccessMessage= true;
							}
						}
						if(pSettings->MainServer.GetLength() && bPutsMainAccessMessage && !bMainServerAccess)
						{// ���������� �������� ������ �����
							::ErrorScanData = CStr("������")+" "+pSettings->MainServer+" ����������";
							AddEvent(IDI_EVENT_INFO, MakeError(pThreadName, NULL, ::ErrorScanData));
							bPutsMainAccessMessage = false;
						}
						if(pSettings->ReservServer.GetLength() && bPutsReservAccessMessage && !bReservServerAccess)
						{// ���������� ��������� ������ �����
							::ErrorScanData = CStr("������")+" "+pSettings->ReservServer+" ����������";
							AddEvent(IDI_EVENT_INFO, MakeError(pThreadName, NULL, ::ErrorScanData));
							bPutsReservAccessMessage = false;
						}
					}
					PrevCurServerVerifyTime = (double)GetCurrentTime();
					PrevMainServerVerifyTime = (double)GetCurrentTime();
				}

				if(bMainServerAccess || bReservServerAccess)
				{// ���� ������ ������ � ������ �� ��������
					int		ParamCnt 	= pScanIdb->pParamLst->ItemCnt;
					int		RequestSize = sizeof(CSrvHeaderPackage) + sizeof(char) + sizeof(int) + ParamCnt*(sizeof(int)+sizeof(char));
					if( !CProg::pScanRequest || CProg::pScanRequest->Size < RequestSize )
					{// ���� ���� �������� ������ ��� ������
						if( CProg::pScanRequest ) ::SrvFreeMem(CProg::pScanRequest);
						CProg::pScanRequest 	  = ::SrvAllocMem(RequestSize, szErrorText, sizeof(szErrorText));
					}

					if( bScanData && CProg::pScanRequest )
					{// ��������� � ������ � ������� PT_GET_CURRENT_DATA
						int	 RequestOffset = 0;
						::SrvInitHeaderPackage((CSrvHeaderPackage*)pScanRequest->pMem, PT_GET_CURRENT_DATA, RequestSize, PSTYLE_VALUE_STATUS|PSTYLE_LAST_PACKAGE|PSTYLE_ALL_CURRENT_PACKAGE|PSTYLE_ENABLE_SRC_CONTROL);
						((CSrvHeaderPackage*)pScanRequest->pMem)->PackageNum = 1;
						RequestOffset += sizeof(CSrvHeaderPackage);
						*(char*)(pScanRequest->pMem + RequestOffset) 	= RHT_PARAM_ARRAY;
						RequestOffset += sizeof(char);
						*(int*)(pScanRequest->pMem + RequestOffset) 	= ParamCnt;
						RequestOffset += sizeof(int);

						for(int ParamPos=1 ; ParamPos<=ParamCnt ; ParamPos++)
						{
							*(int*)(pScanRequest->pMem + RequestOffset) = ((CScanParamInfo*)pScanIdb->pParamLst->Get(ParamPos))->OutsideID;
							RequestOffset += sizeof(int);
							*(char*)(pScanRequest->pMem + RequestOffset) = 1; // ������� - �������� ��������
							RequestOffset += sizeof(char);
						}
					}
					else{ bScanData = false; Error = szErrorText; }

					if( bScanData && pScanRequest )
					{// ���������� ������ � ������� PT_GET_CURRENT_DATA ���������� �������

						if(bMainServerAccess)
						{// ������� ����������� � ��������� �������
							if( ( CProg::ScanRequestID = SrvAllocRequest(CStr("ImCore ") + pProg->pSettings->NodeName, pSettings->MainServer, CProg::pScanRequest, 10, szErrorText, sizeof(szErrorText)))==0 )
							{
								CurScanServerName.Empty();
								bScanData = false; Error = szErrorText;
							}
							else
							{
								::ErrorScanData.Empty();
								CurScanServerName = CProg::pSettings->MainServer;
								::pProg->AddEvent(IDI_EVENT_INFO, MakeError(pThreadName, NULL, CStr((char*)(bFirstOpenChannel?"������":"������������")) + " ����� GET_CURRENT_DATA � �������� " + ::pProg->pSettings->MainServer));
							}
						}

						if(!CurScanServerName.GetLength() && bReservServerAccess)
						{// ������� ����������� � ���������� �������
							if( (CProg::ScanRequestID = ::SrvAllocRequest(CStr("ImCore ") + pProg->pSettings->NodeName, pSettings->ReservServer, CProg::pScanRequest, 10, szErrorText, sizeof(szErrorText)))==0 )
							{
								CurScanServerName.Empty();
								bScanData = false; Error = szErrorText;
							}
							else
							{
								::ErrorScanData.Empty();
								bScanData 				= true;
								CurScanServerName		= CProg::pSettings->ReservServer;
								::pProg->AddEvent(IDI_EVENT_INFO, MakeError(pThreadName, NULL, CStr((char*)(bFirstOpenChannel?"������":"������������")) + " ����� GET_CURRENT_DATA � �������� " + ::pProg->pSettings->ReservServer));
							}
						}
					}

					TacktCounter = 0; // ����� ����� ���������� ����������� ����� ���������� �����


				}
			}
			else
			{// ��������� ������ ��� ������� � �������� ����� �����

				if(bScanData)
				{
					int RequestSize = sizeof(CSrvHeaderPackage) + sizeof(char) + sizeof(int);
					if(pScanRequest->Size < RequestSize)
					{// ���� ���� �������� ������ ��� ������

						// �������� ���������
						CSrvHeaderPackage	Header = *(CSrvHeaderPackage*)pScanRequest->pMem;

						// ��������� ������ �������
						::SrvFreeMem(pScanRequest);

						// ����� ����� �������
						pScanRequest  = ::SrvAllocMem(RequestSize, szErrorText, sizeof(szErrorText));

						// ����������� ���������
						*(CSrvHeaderPackage*)pScanRequest->pMem	= Header;
					}
					((CSrvHeaderPackage*)pScanRequest->pMem)->PackageSize = RequestSize;

					int	 RequestOffset 	= sizeof(CSrvHeaderPackage);
					*(char*)(pScanRequest->pMem + RequestOffset) 	= RHT_PARAM_ARRAY;
					RequestOffset += sizeof(char);
					*(int*)(pScanRequest->pMem + RequestOffset) 	= 0;
					RequestOffset += sizeof(int);

					((CSrvHeaderPackage*)pScanRequest->pMem)->PackageNum++;

					if(!SrvSendRequestData(ScanRequestID, pScanRequest, szErrorText, sizeof(szErrorText)))
					{  bScanData = false; Error	= szErrorText; }
				}
			}

			if(bScanData && ScanRequestID)
			{// ���� ����� ����� ������, �� ���������� ������ � �������
				bFirstOpenChannel = false;

				if( (NeededAnswerSize = ::SrvGetAnswerState(ScanRequestID, true, szErrorText, sizeof(szErrorText))) > 0 )
				{
					if( pScanAnswer && pScanAnswer->Size<NeededAnswerSize )
					{// ���� ������ ����������� ������ ������ ����������, �� ����������� ���
						::SrvFreeMem(pScanAnswer);
						pScanAnswer = NULL;
					}

					// �������� ������ ��� �����
					if(!pScanAnswer) pScanAnswer = ::SrvAllocMem(NeededAnswerSize, szErrorText, sizeof(szErrorText));

					if(pScanAnswer)
					{
						if( (RealAnswerSize = ::SrvFetchAnswer(ScanRequestID, true, pScanAnswer, szErrorText, sizeof(szErrorText))) > 0 )
						{
							// ������ ���������� ����� ������
							ErrorScanData 	  = "";

							// ������ ����� ��������� � ����������
							for( int ParamPos=1; ParamPos<=pScanIdb->pParamLst->GetCnt(); ParamPos++ )
								((CScanParamInfo*)pScanIdb->pParamLst->Get(ParamPos))->ChangeFlag = false;

							//if(pSettings->bEnablePutsScanData) puts( (char*)MakeError(pThreadName, NULL, CStr(" ����� N ") + ((CSrvHeaderPackage*)pScanAnswer->pMem)->PackageNum + ", Time=" + TimeToStr(&LastScanTime) + ", �� " + pSettings->ServerName + ", Size=" + RealAnswerSize));

							// ���������� ��������� ������ (������ ����� ���� ���������)
							int					AnswerOffset 	= sizeof(CSrvHeaderPackage);
							int					PackageSize	 	= ((CSrvHeaderPackage*)pScanAnswer->pMem)->PackageSize;
							unsigned char*		pAnswerData	 	= (unsigned char*)pScanAnswer->pMem;
							int					ValuesCnt	 	= 0;
							unsigned int 		OutsideID	 	= 0;
							unsigned int 		InfoBasePos	 	= 0;

							while(bScanData && AnswerOffset<=(PackageSize - (int)(sizeof(char) + sizeof(CTime) + sizeof(char) + sizeof(int) )) )
							{
								unsigned char*	pParamInfoData	= pScanIdb->pParamLst->pData;
								CScanParamInfo*	pParamInfo;

									// ��������� ��������� ����
								if( bScanData && *((char*)(pAnswerData + AnswerOffset)) == RHT_TIME )
								{
									AnswerOffset  += sizeof(char);
									CurScanLayerTime   = *(CTime*)(pAnswerData + AnswerOffset);
									AnswerOffset  += sizeof(CTime);

//puts(CStr("CurScanLayerTime=") + &CurScanLayerTime);

								}
								else bScanData = false;

								if( bScanData && *(char*)(pAnswerData + AnswerOffset) == RHT_VALUES_ARRAY )
								{
									AnswerOffset 	 += sizeof(char);
									ValuesCnt  		  = *(int*)(pAnswerData + AnswerOffset);
									AnswerOffset 	 += sizeof(int);
								}
								else bScanData = false;

								if(pScanIdb->Lock())
								{// ��������� ���
									for( int ValuePos=0; ValuePos<ValuesCnt && AnswerOffset<PackageSize; ValuePos++ )
									{// �������� �������� (��� ��������)
										OutsideID 		  = *(int*)(pAnswerData + AnswerOffset);
										AnswerOffset 	 += sizeof(int);
										InfoBasePos		  = pScanIdb->OutsideIdToInfoBasePos(OutsideID);
										pParamInfo		  = InfoBasePos ? (CScanParamInfo*)(pParamInfoData + sizeof(CScanParamInfo)*InfoBasePos) : NULL;

										if( pParamInfo )
										{
											unsigned char	NewValue[5];
											if( ::IsAnalog(pParamInfo->OutsideType) )
											{
												*(float*)NewValue 	= ::GetOperationValue(*(float*)(pAnswerData+AnswerOffset),pParamInfo->CorrectiveType,pParamInfo->CorrectiveCoef);
												if(StrStr(pParamInfo->CalcShortName, "BP_SZ"))
												{// ��������� ��������� �� ��� �� ������
													*(float*)NewValue = Round(*(float*)NewValue, 0);
												}
											}
											else	*(char*)NewValue 	= *(char*)(pAnswerData+AnswerOffset);
											*(NewValue+pParamInfo->ValueByteSize) = *(pAnswerData+AnswerOffset+pParamInfo->ValueByteSize);

											if( !_CmpOutsideValue(pParamInfo->OutsideType, (unsigned char*)pParamInfo->Value, NewValue, true ))
											{// ���� ���� ����� ������ � ��  ���������
												_CopyOutsideValue(pParamInfo->OutsideType, (unsigned char*)pParamInfo->Value, NewValue, true);
												pParamInfo->ChangeFlag = true;
											}

											// ���������� � ���������� ��������
											AnswerOffset  += pParamInfo->ValueByteSize + sizeof(char);
										}
										else AnswerOffset += ::GetOutsideByteSize(GET_OUTSIDE_TYPE(OutsideID)) + sizeof(char);
									}
									pScanIdb->Unlock();
								}
							}

							CScanParamInfo* pTactParamInfo = (CScanParamInfo*)pScanIdb->GetParamInfoByName(::pProg->pSettings->TacktShortName);


							TacktCounter += 1;

//if( pTactParamInfo )
//	puts(CStr("SCAN_DATA TactParam=") + *(float*)pTactParamInfo->Value);

							if( bScanData && (!pTactParamInfo || pTactParamInfo->ChangeFlag) && (!pSettings->EmptyTacktCounter || TacktCounter>pSettings->EmptyTacktCounter) )
							{
								CTime LastChangeTime;
								if(pScanIdb->pLastChangeTeff) LastChangeTime.Sec = *(int*)&pScanIdb->pLastChangeTeff->Value;

								// ��������� ����������� �������� ������
								SetFlag(FLG_CHANGE_SCAN_DATA, SV_NEEDED);

/*
CTime CurTime = GetCurrentTime();
puts(CStr() + &CurTime + " CProg::ScanDataThread FLG_CHANGE_SCAN_DATA=NEEDED");
*/

								while(GetFlag(FLG_CHANGE_SCAN_DATA) == SV_NEEDED) ThreadYield();

//								puts(CStr("CProg::ScanThread ") + &CurScanLayerTime + " ValuesCnt=" + ValuesCnt);
								if(GetFlag(FLG_INPUT_TEFF) != SV_NEEDED)
								{// �������� ������������� ����� Teff

//CTime CurTime = GetCurrentTime();
//printf("scan %s ", (char*)TimeToStr(&CurTime));
//printf("%s ", (char*)TimeToStr(&LastChangeTime));
//printf("%g %d", (double)GetCurrentTime()-(double)LastChangeTime, pSettings->TeffCheckInterval*1000);
//printf("\n");

									if(	pSettings->TeffCheckInterval &&
										((double)GetCurrentTime()-(double)LastChangeTime)>(pSettings->TeffCheckInterval*1000))
									{// �������� ��������� ����� Teff
										TeffInputDisplay = pSettings->TeffInputDisplay;
										if(::pProgApp) StopProg(this, SS_CONSOLE);
										SetFlag(FLG_INPUT_TEFF_USER_CALL,	SV_READY);
										SetFlag(FLG_INPUT_TEFF,				SV_NEEDED);
										SetFlag(FLG_START_WND,				SV_NEEDED);
									}
								}
							}
							else Error = MakeError(pThreadName, "?????", "�������� ��������� ������");
						}
						else{ bScanData = false; Error	= szErrorText; }
					}else{ bScanData = false; Error	= szErrorText; }
				}else{ bScanData = false; Error	= szErrorText; }
			}

			if(bScanData && CurScanServerName.GetLength() && CurScanServerName!=CProg::pSettings->MainServer && ((double)GetCurrentTime()-PrevMainServerVerifyTime)>3000)
			{// ���� �� ���������� � ���������� �������, �� ������������ ����������� � �������� ��������� �������
				int			SrvInfoCnt = 1;
				CSrvInfo	SrvInfoArray[1];
				PrevMainServerVerifyTime = (double)GetCurrentTime();
				if(SrvGetServersInfo(CStr("ImCore ") + pProg->pSettings->NodeName, SrvInfoArray, &SrvInfoCnt, CProg::pSettings->MainServer, szErrorText, sizeof(szErrorText)) )
				{
					if(SrvInfoCnt)
					{// �������� ����������� �������� �������
						if( SrvInfoArray[0].ServerStatus&SRV_STATE_ENABLED && SrvInfoArray[0].ServerStatus&SRV_STATE_CURRENT_DATA && !(SrvInfoArray[0].ServerStatus&SRV_STATE_SUSPECT) )
						{// ���� ��������� ����� ���� ������� ������ �������������
							bScanData = false;
						}
					}
				}
			}

			if(!bScanData)
			{// ���� ���� ������, �� ������������ ��
				bScanData = true;

				if(ScanRequestID)
				{
					::ErrorScanData  = CStr("������ ����� GET_CURRENT_DATA � �������� ") + CurScanServerName;
					if( Error.GetLength() ) ::ErrorScanData += CStr("(") + Error + ")";
				}
				else ::ErrorScanData = Error;

				AddEvent(IDI_EVENT_ERROR, MakeError(pThreadName, NULL, ::ErrorScanData));

				if(ScanRequestID)
				{// ����������� ������� ������� ��������
					::SrvFreeRequest(ScanRequestID);
					ScanRequestID				= 0;
					PrevCurServerVerifyTime		= (double)GetCurrentTime()+2000;
					bPutsMainAccessMessage		= true;
					bPutsReservAccessMessage	= true;
				}
				CurScanServerName.Empty();
			}

			SetFlag(FLG_RUN_SCAN_THREAD, SV_READY);
		}
		else if(GetFlag(FLG_STOP_SCAN_THREAD) == SV_NEEDED)
		{// ���� �����������
			break;
		}
//		else
		{// ������ ������� ���� �� ���� �������������� ������
			ThreadYield();
		}
	}// ����� ��������� ����� ������

	if(ScanRequestID)
	{// ����������� ������� ������� ��������
		::SrvFreeRequest(ScanRequestID);
		ScanRequestID = 0;

		::ErrorScanData  = CStr("������ ����� GET_CURRENT_DATA � �������� ") + CurScanServerName;
		CurScanServerName.Empty();

		AddEvent(IDI_EVENT_INFO, MakeError(pThreadName, NULL, ::ErrorScanData));
	}

	if(pScanRequest)
	{
		::SrvFreeMem(pScanRequest);
		pScanRequest = NULL;
	}

	if(pScanAnswer)
	{
		::SrvFreeMem(pScanAnswer);
		pScanAnswer = NULL;
	}

	if(!bScanData && Error.GetLength())
	{
		if(pError) *pError = Error;
		else puts((char*)Error);
	}

	return bScanData;
}
