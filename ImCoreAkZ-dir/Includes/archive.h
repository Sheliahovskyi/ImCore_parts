#pragma once

#include "core.h"
#include <glob.h>
#include <map>
#include <cstring>


/*!
 * \date 16.07.2019
 * \brief Класс для работы с архивами
 *
 * Записывает архивы
 * Считывает состояние зоны из архивов
 * Передает текущее состояние зоны
 * Выполняет поиск нужного архива по дате
 *
 */

struct CoreState {                          ///< Структура, содержащая необходимую информацию о состоянии АкЗ в некий момент времени
    ///< Date
    TIME_IMCORE CurrentDate;
    double Teff;
    ///< Geometry
    vector<vector<int>> CoreMap;
    vector<vector<double>> PrismHight;
    ///< Fields
    vector<vector<double>> SL;
    vector<vector<double>> SM;
    vector<vector<double>> PM;
    vector<vector<double>> XE;
    vector<vector<double>> I;
    vector<vector<vector<double>>> PinSL;
    vector<vector<double>> ZE;

    ///< Parameters
    vector<double> HSuz;
    double G;
    double CB;
    double Pout;
    double Pdrop;
    double Tin;
};

struct MemoryPoint {
    string date;
    long long TimeFrom1970;
    string ArchiveFileGlobalPath;
};

class ARCHIVE {

public:
    CORE * pCore;

    ARCHIVE( CORE* CorePointer );
    ~ARCHIVE();

    vector<MemoryPoint> ArchiveList;

    CoreState MakeCoreSlice();                      ///< Формирование структуры CoreState которая описывает состояние АкЗ
    CoreState FindCoreArchive( long long TimeFrom1970 );               ///< Поиск архивного файла по необходимой дате
    void ArchiveListFilling();                      ///< Заполнение списка имеющихся архивных точек при старте
    void ResizeCoreSliceFields( CoreState * CoreState );      ///< Resize() полей объека CoreState для их подалшего заполнения

    void WriteCoreSlice();                          ///< Запись состояния АкЗ в архивный файл
    void ReadCoreSlice( long long TimeFrom1970 );                      ///< Чтение состояния АкЗ из архивного файла

};
