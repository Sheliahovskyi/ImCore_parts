#include <ImCorePMain.h>

bool CProg::NetThread(const char* pThreadName, CStr* pError)
{// приём сетевых соединений (работает постоянно)
	bool	bNetwork = true;
	CStr	Error;

	ErrorNetwork.Empty();

	if(SrvSocket<0 && pSettings->SrvHost.GetLength() && pSettings->SrvPort>0)
	{
		SrvSocket = ::NetOpenTCPServerSocket(pSettings->SrvHost, pSettings->SrvPort, &Error);
		if(SrvSocket < 0)
		{
			AddEvent(IDI_EVENT_ERROR, MakeError(pThreadName, NULL, CStr("Ошибка создания серверного сокета для приёма соединений")+" : "+Error));
			bNetwork = false;
		}
	}

	while(bNetwork)
	{
		if(GetFlag(FLG_STOP_NET_THREAD) != SV_NEEDED)
		{// если нет запретов
			SetFlag(FLG_RUN_NET_THREAD, SV_PROCESS);
			if(SrvSocket != -1)
			{// проверяем наличие новых клиентов
				struct sockaddr_in	ClientAddr;
				socklen_t			ClientAddrLen = sizeof(ClientAddr);
				int NewClientSocket = ::NetAccept(SrvSocket, 3000, &Error, (struct sockaddr*)&ClientAddr, &ClientAddrLen);
				if(NewClientSocket != -1)
				{
					char	ClientHost[INET_ADDRSTRLEN];	inet_ntop(AF_INET, &ClientAddr.sin_addr, ClientHost, sizeof(ClientHost));
					int		ClientPort = ntohs(ClientAddr.sin_port);
					if(pClientLst->Lock())
					{
						CPredictClient* pClient = new CPredictClient(CStr(ClientHost), ClientPort);
						pClient->NetSocket = NewClientSocket;
						// добавляем клиента в обработку
						pClientLst->Add((long)pClient);
						pClient->StartThread();

						pClientLst->Unlock();
					}
				}
			}

			SetFlag(FLG_RUN_NET_THREAD, SV_READY);
		}
		else if(GetFlag(FLG_STOP_NET_THREAD) == SV_NEEDED)
		{// надо выгружаться
			break;
		}
		// заснем немного
		ThreadYield();
	}// конец основного цикла потока

	if(SrvSocket >= 0)
	{// закроем серверный сокет
		::NetCloseSocket(SrvSocket);
		SrvSocket = -1;
	}

	if(!bNetwork && Error.GetLength())
	{
		if(pError) *pError = Error;
		else puts((char*)Error);
	}

	return bNetwork;
}

