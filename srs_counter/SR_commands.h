// SR_commands.h - ��������� ������� ���������� ���������

#ifndef __SR_COMMANDS_H
#define __SR_COMMANDS_H

#include "stdafx.h"


// ������ ������.
const char INPUT_BUFF_SIZE=16;

//-----------�������� ������� ��������.-----------------
// ������� ������ ����� ������ ��������.
static	char SR_CLEAN[]={0x20,0x20,0x20,0x0D};  //;//[4];//  0x20 - ������  [4];//srClean
	// �����/�������/�����
static	char SR_START[]={'C','S',0x0D};  // "CS\r"//"\n" ���� �������� //srStart
static	char SR_HALT[]={'C','H',0x0D}; // "CH\r"  //srHalt
static	char SR_RESET[]={'C','R',0x0D};//"CR\r"; 	//srReset
static	char SR_SW0[]={'S','W','0',0x0D};// ����� �������� �������� �������. // srSW0
static	char SR_QA1[]={'Q','A','1',0x0D};//"QA1\r"; // ���������� ������� �������� ��������.	//srQA1
static	char SR_CR[]={0x0D}; // CR - ������� �������. //CR

// �������������� ����� ������ ��������.
static	char SR_AB_AT_T[]={'C','M','0',0x0D}; // "���� �, � �� �" "CM0\r"	//srABatT
static	char SR_T_AT_10M[]={'C','I','2',',','0',0x0D};	//	�-����10���: "CI2,0\r"	//srTat10M
static	char SR_1INPUT_A[]={'C','I','0',',','1',0x0D}; // A-����1: "CI0,1\r"	//sr1InputA
static	char SR_2INPUT_B[]={'C','I','1',',','2',0x0D}; //	�-����2: "CI1,2\r"	//sr2InputB
static	char SR_SE_MODE_STOP[]={'N','E','0',0x0D}; // Scan end mode STOP.
static	char SR_RISE_FRONT_A[]={'D','S','0',',','0',0x0D}; // ������ �� ����������� ������

// 1. ���������� ������ ������������.
static	char SR_NP_SET[]={'N','P'}; // NP2000\r, �������� �� ���������. //,'2','0','0','0',0x0D //srNPset

// 2. ����� �������.
static	char SR_TIME_SET[]={'C','P','2',','}; // CP2,XX.XXX ������� ������� ����� //srTset
//	static	char SR_TIME_GET[]={'C','P','2',0x0D}; // ���������� ������� �����.// srT

// 3. ����� ��������.
static	char SR_DTIME_SET[]={'D','T'}; // ��� DT.xxxx	//srDTset
//	static	char SR_DTIME_GET[]={'D','T',0x0D}; //srDTget

// 4. ������� ��� ���������/���������� ������ �������������.
static	char SR_DL_A_GET[]={'D','L','0',0x0D}; // DL0. - ���� �.	//srDL0
// ���� �������� ��� ���� ��������...
static	char SR_DL_A_SET[]={'D','L','0',','}; // 'DL0,' //srDL0set
// ��� �������� ������ �����, ���� ������� ��������, ��� �������� ���������� ����� ��������.
static char SR_DL_MINUS[]={'-'}; // ���� ������.
static char SR_DL_DOT[]={'.'}; // ���� ���������� �����.
static char SR_DL_ZERO[]={'0'}; // ���� ����.


// ������� ��������.
static char SR_SCAN_POSITION[]={'N','N',0x0D}; // ���������� ����������� ������. // srScanPosition
static char SR_SAME_SCAN[]={'Q','A'}; // ������� ����������� ����� - "QA m".	//srSameCountDigit

// ������� ��� ������ ������ � ���� ����� (��� �� �� �������� ���������???):
// 1. ���������� ������ ������������.
static	char _scanCyclesString[]={'0','0','0','0'}; // max 2000, ������������� - � ������������.
// 2. ����� �������, 3 �����.
static	char _sampleTimeString[]={'0','E','0'}; // {'1','E','6'};
// 3. ����� ��������, 4 ����� (������ ��� �.�. ����).
static	char _dWellTimeString[]={'1','E','-','1'};//{'9','E','-','3',0x0D}; {'1','E','0','0'};
//4. ������� ��������������, 3 ����� (��� ���������� - ����� ��������� �� 4).
static	char _discLevelString[3]={0,0,0}; // srDiscValue



#endif