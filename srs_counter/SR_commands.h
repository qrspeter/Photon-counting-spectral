// SR_commands.h - строковые команды управления счетчиком

#ifndef __SR_COMMANDS_H
#define __SR_COMMANDS_H

#include "stdafx.h"


// Размер буфера.
const char INPUT_BUFF_SIZE=16;

//-----------байтовые КОМАНДЫ счетчика.-----------------
// очистка буфера перед каждой командой.
static	char SR_CLEAN[]={0x20,0x20,0x20,0x0D};  //;//[4];//  0x20 - пробел  [4];//srClean
	// старт/останов/сброс
static	char SR_START[]={'C','S',0x0D};  // "CS\r"//"\n" тоже работает //srStart
static	char SR_HALT[]={'C','H',0x0D}; // "CH\r"  //srHalt
static	char SR_RESET[]={'C','R',0x0D};//"CR\r"; 	//srReset
static	char SR_SW0[]={'S','W','0',0x0D};// сброс ожидания передачи символа. // srSW0
static	char SR_QA1[]={'Q','A','1',0x0D};//"QA1\r"; // считывание первого значения счетчика.	//srQA1
static	char SR_CR[]={0x0D}; // CR - возврат каретки. //CR

// Первоначальный режим работы счетчика.
static	char SR_AB_AT_T[]={'C','M','0',0x0D}; // "счет А, В по Т" "CM0\r"	//srABatT
static	char SR_T_AT_10M[]={'C','I','2',',','0',0x0D};	//	Т-вход10МГц: "CI2,0\r"	//srTat10M
static	char SR_1INPUT_A[]={'C','I','0',',','1',0x0D}; // A-вход1: "CI0,1\r"	//sr1InputA
static	char SR_2INPUT_B[]={'C','I','1',',','2',0x0D}; //	В-вход2: "CI1,2\r"	//sr2InputB
static	char SR_SE_MODE_STOP[]={'N','E','0',0x0D}; // Scan end mode STOP.
static	char SR_RISE_FRONT_A[]={'D','S','0',',','0',0x0D}; // работа по восходящему фронту

// 1. Количество циклов сканирования.
static	char SR_NP_SET[]={'N','P'}; // NP2000\r, значение по умолчанию. //,'2','0','0','0',0x0D //srNPset

// 2. Время выборки.
static	char SR_TIME_SET[]={'C','P','2',','}; // CP2,XX.XXX задание времени счета //srTset
//	static	char SR_TIME_GET[]={'C','P','2',0x0D}; // считывание времени счета.// srT

// 3. Время ожидания.
static	char SR_DTIME_SET[]={'D','T'}; // или DT.xxxx	//srDTset
//	static	char SR_DTIME_GET[]={'D','T',0x0D}; //srDTget

// 4. Команды для установки/считывания уровня дискриминации.
static	char SR_DL_A_GET[]={'D','L','0',0x0D}; // DL0. - вход А.	//srDL0
// надо подумать над этой командой...
static	char SR_DL_A_SET[]={'D','L','0',','}; // 'DL0,' //srDL0set
// или вставить пустые места, куда вносить значение, или выводить десятичным путем отдельно.
static char SR_DL_MINUS[]={'-'}; // знак минуса.
static char SR_DL_DOT[]={'.'}; // знак десятичной точки.
static char SR_DL_ZERO[]={'0'}; // знак нуля.


// Команды поллинга.
static char SR_SCAN_POSITION[]={'N','N',0x0D}; // количество завершенных сканов. // srScanPosition
static char SR_SAME_SCAN[]={'Q','A'}; // поллинг конкретного скана - "QA m".	//srSameCountDigit

// Массивы для вывода данных в виде строк (или их по функциям распихать???):
// 1. Количество циклов сканирования.
static	char _scanCyclesString[]={'0','0','0','0'}; // max 2000, инициализация - в конструкторе.
// 2. Время выборки, 3 байта.
static	char _sampleTimeString[]={'0','E','0'}; // {'1','E','6'};
// 3. Время ожидания, 4 байта (потому что м.б. знак).
static	char _dWellTimeString[]={'1','E','-','1'};//{'9','E','-','3',0x0D}; {'1','E','0','0'};
//4. Уровень дискриминатора, 3 байта (для милливольт - иначе расширить до 4).
static	char _discLevelString[3]={0,0,0}; // srDiscValue



#endif