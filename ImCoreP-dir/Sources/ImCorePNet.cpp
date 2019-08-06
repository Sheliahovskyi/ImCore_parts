#include <ImCorePMain.h>

bool CProg::NetThread(const char* pThreadName, CStr* pError)
{// ��ɣ� ������� ���������� (�������� ���������)
	bool	bNetwork = true;
	CStr	Error;

	ErrorNetwork.Empty();

	if(SrvSocket<0 && pSettings->SrvHost.GetLength() && pSettings->SrvPort>0)
	{
		SrvSocket = ::NetOpenTCPServerSocket(pSettings->SrvHost, pSettings->SrvPort, &Error);
		if(SrvSocket < 0)
		{
			AddEvent(IDI_EVENT_ERROR, MakeError(pThreadName, NULL, CStr("������ �������� ���������� ������ ��� ��ɣ�� ����������")+" : "+Error));
			bNetwork = false;
		}
	}

	while(bNetwork)
	{
		if(GetFlag(FLG_STOP_NET_THREAD) != SV_NEEDED)
		{// ���� ��� ��������
			SetFlag(FLG_RUN_NET_THREAD, SV_PROCESS);
			if(SrvSocket != -1)
			{// ��������� ������� ����� ��������
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
						// ��������� ������� � ���������
						pClientLst->Add((long)pClient);
						pClient->StartThread();

						pClientLst->Unlock();
					}
				}
			}

			SetFlag(FLG_RUN_NET_THREAD, SV_READY);
		}
		else if(GetFlag(FLG_STOP_NET_THREAD) == SV_NEEDED)
		{// ���� �����������
			break;
		}
		// ������ �������
		ThreadYield();
	}// ����� ��������� ����� ������

	if(SrvSocket >= 0)
	{// ������� ��������� �����
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

