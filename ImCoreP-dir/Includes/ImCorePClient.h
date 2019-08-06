#ifndef _IMCORE_P_CLIENT_H
#define _IMCORE_P_CLIENT_H
#include "ImCorePMain.h"


class FORECAST;

// типы запросов (RequestType)
#define	PT_CUR		0
#define	PT_REQ		1
#define	PT_GEN		2
#define	PT_MKU		3

// типы ответов (AnswerType)
#define	AT_POINT	0
#define	AT_MSG		1
#define	AT_STOP		2

// типы значений (ValueType)
#define	VT_FLOAT			0
#define	VT_STRING			1
#define	VT_FLOAT_ARRAY		2

#pragma pack(1)

struct	CPredictRequestHeader
{
	int		PackageSize;				// собственно размер пакета
	char	RequestName[32];			// наименование запроса
	int		RequestType;				// тип прогнозного запроса
	CTime	StartRequestTime;			// стартовое время запроса
	int		EtapCounter;				// количество этапов
};

struct	CPredictEtapHeader
{
	int		EtapSize;					// размер этапа вместе с данными
	CTime	Duration;					// длителбность этапа
	int		ValuesCounter;				// количество значений в етапе
};

struct	CPredictValueHeader
{
	int		ValueSize;					// размер значения вместе с данными
	int		ValueType;					// тип значения
	char	ValueName[20];				// наименование значения
	int		SizeX;						// размерность по Х
	int		SizeY;						// размерность по Y
};

struct CPredictAnswerHeader
{
	int		PackageSize;
	int		AnswerType;
	CTime	StartTime;
	CTime	PointTime;
	int		ValuesCounter;
};

#pragma pack()

//////////////////////////////////////////////////////////////////////
// class CPredictClient
class CPredictClient
{
public:
	CStr				ClientHost;
	int					ClientPort;
	int					NetSocket;

	int					MutexID;

	// атрибуты потока обработки
	CStr				ThreadName;
	int					ThreadPid;
	int					ThreadFlgStart;
	int					ThreadFlgRun;
	int					ThreadFlgStop;
	CStr				ThreadError;


    FORECAST*           pForecast;

public:
	CPredictClient(CStr Host, int Port);
	~CPredictClient();

	void	StartThread();
	bool	ClientThread();
	bool 	Lock(){	  return MutexID ? ::MutexLock(MutexID)	  : false; }
	bool	Unlock(){ return MutexID ? ::MutexUnlock(MutexID) : false; }
	bool	IsLock(){ return MutexID ? ::MutexIsLock(MutexID) : false; }
	void	Disconnect();
	void	PrepareClientRequest(unsigned char* pPackage, CPredictRequestHeader* pHeader, LST* pEtapLst);
	long	GetOffsetInStruct(const char* pValueName);
};



#endif // _IMCORE_P_CLIENT_H
