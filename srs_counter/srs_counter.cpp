// SR_COUNTERclass.cpp - ������ ������ SR_COUNTERclass.

#include <windows.h>
#include <winuser.h> //CharToOem chartooem, oemtochar.
#include <iostream>
#include <array> //  for std::array<int, 3> inputBuff

//#include <math.h>
//#include <stdlib.h> // �������������� "������-�����" ��� �����-������ ������ ������������ � ������� �������.
//#include <string.h> // �����������/�������������� �����.

#include <unistd.h> // usleep(TIME_TO_SLEEP); ms

#include "srs_counter.h"






// ������ ������.
const char INPUT_BUFF_SIZE = 16;

// static	char SR_CR[] = {0x0D};  //;//[4];//  0x20 - ������  [4];//

//-----------�������� ������� ��������.-----------------
// ����� ����� ������ �������� ����� �������� ������, �.�. ��������� ��� ������ ������, �� ��� �� �� ���� ������� �������� ����
const std::string SR_CR = " \r";
const std::string SR_START = " CS\r";
// const std::string SR_HALT = " CH\r";
const std::string SR_RESET = " CR\r";
const std::string SR_QA = " QA\r"; //"QA\r"; // ���������� ���������� �������� ��������.
const std::string SR_QA1 = " QA1\r"; // ���������� ������� �������� ��������, ����� " -1".	//
const std::string SR_STATUS1 = " SS 1\r";
const std::string SR_AB_AT_T = " CM0\r";// "���� �, � �� �" "CM0\r"
const std::string SR_T_AT_10M = " CI2,0\r"; //	�-���� 10 ���: "CI2,0\r"
const std::string SR_1INPUT_A = " CI0,1\r"; // A-����1: "CI0,1\r"
const std::string SR_2INPUT_B = " CI1,2\r"; //	�-����2: "CI1,2\r"
const std::string SR_RISE_FRONT_A = " DS0,0\r";  // ������ �� ����������� ������
const std::string SR_FALL_FRONT_A = " DS0,1\r";  // ������ �� ���������� ������
const std::string SR_TIME_SET = " CP2,"; // /CP2,XX.XXX ������� ������� �����
const std::string SR_DL_A_SET = " DL0,"; // -0.300 <= V <= 0.300.




SR_COUNTER::SR_COUNTER()
{
//	inputBuff = new char[INPUT_BUFF_SIZE];


	// ������������ ���������� ��-���������.
//	hCounter = INVALID_HANDLE_VALUE;
//	comPort = 2; // ������� ��-��������� �� ���1.
////	1. ���������� �������� ��������. N=1 by default
////	scanCycles = 1800; // ����. 2000 ��������.
////	2. ����� �������, ��.
//	sampleTime = 200; //
////	3. ����� "������" ����� ���������.
////	dWellTime = 300; // �� 2E-3 �� 6E1, �.�. 2...6000��.  from 2 ms to 60 s
////	4. ������� �������������, ��.
//	discLevel = -75;
//	slopeMode = FALL;
//	deadTime = 0;


}

SR_COUNTER::~SR_COUNTER()
{
	if(hCounter != INVALID_HANDLE_VALUE)
	Close();
}

int SR_COUNTER::Open(int comPort)
{
    char Com_Name[] = "COM3";

    if(comPort > 9)
        return (0);

    Com_Name[3] = (char)(comPort + 0x30);



	hCounter = CreateFileA((LPCSTR)Com_Name,GENERIC_READ|GENERIC_WRITE, 0, NULL,OPEN_EXISTING,0, 0);

	if(hCounter == INVALID_HANDLE_VALUE) // �������� ��������� �����. NULL for 32-bit Win and INVALID_HANDLE_VALUE for 64bit
	{
	    if(GetLastError() == ERROR_FILE_NOT_FOUND)
        MessageBox(NULL, "COM port is not found", Com_Name, MB_OK);
        else
		MessageBox(NULL, "Error Open COM", "Error", MB_OK);
		return(0);
	}

	if(!PurgeComm(hCounter, PURGE_TXABORT | PURGE_RXABORT | PURGE_TXCLEAR | PURGE_RXCLEAR))
	{
		MessageBox(NULL, "PurgeComm() failed!", "Error", MB_OK);
		hCounter = NULL;
		return(0);
	}

	if(!ClearCommBreak(hCounter))
	{


		MessageBox(NULL, "ClearCommBreak() failed!", "Error", MB_OK);
		hCounter = NULL;
		return(0);
	}

	DCB dcb; //  = {0}
	if(!GetCommState(hCounter, &dcb))
	{
		MessageBox(NULL, "Port DCB error!","Error", MB_OK);
		CloseHandle(hCounter);
		return(0);
	}

	dcb.BaudRate = CBR_9600;
	dcb.ByteSize = 8;
	dcb.Parity = NOPARITY;
	dcb.StopBits = ONESTOPBIT; // TWOSTOPBITS

    // ��������� ��������� �����.
	if(!SetCommState(hCounter, &dcb))
	{
		MessageBox(NULL,"Port parameter error!","Error",MB_OK);
		CloseHandle(hCounter);
		return(0);
	}


	COMMTIMEOUTS  ct;
	ct.ReadIntervalTimeout = 30;//MAXDWORD;
	ct.ReadTotalTimeoutMultiplier = 100;// MAXDWORD;
	ct.ReadTotalTimeoutConstant = 100;
	ct.WriteTotalTimeoutMultiplier = 0;
	ct.WriteTotalTimeoutConstant = 0;

/*
	ct.ReadIntervalTimeout = MAXDWORD ; // 3000;//MAXDWORD;
	ct.ReadTotalTimeoutMultiplier = MAXDWORD; // 3000;// MAXDWORD;
	ct.ReadTotalTimeoutConstant = 2000; //2000;//100;
	ct.WriteTotalTimeoutMultiplier = 0;
	ct.WriteTotalTimeoutConstant = 0;


*/



	if(!SetCommTimeouts(hCounter,&ct))
	{
		MessageBox(NULL,"Timeouts parameter error!","Error",MB_OK);
		CloseHandle(hCounter);
		return(0);
	}

	SetupComm(hCounter, INPUT_BUFF_SIZE, INPUT_BUFF_SIZE); // ������� �������� ������ �����.

    std::array <char, INPUT_BUFF_SIZE> inputBuff;

    inputBuff.fill(0);

// https://www.thinksrs.com/downloads/pdfs/manuals/SR400m.pdf
// COMMON SOFTWARE PROBLEMS INCLUDE:
// 3) The initial command sent to the SR400 was invalid due to a garbage character received during power-up,
// or, the first character in your computer's UART is garbage, also due to power-up.
// It is good practice to send a few carriage returns to the SR400 when your program begins

	Write(SR_CR); //, sizeof(SR_CR));
	Write(SR_CR);
	Write(SR_RESET);

/*
	if(inputBuff[1] != 0x31) // ���� ����� �� ������ �� �������. (inputBuff[0] != 0x2D) && (inputBuff[1] != 0x31)
	{
		MessageBox(NULL,"No connection","Error",MB_OK);
		return(0);
	}
*/

// ������������ �������� �����������
// The default sequence for RS-232 is <cr> when the echo mode is off, and <cr><lf> when the echo mode is on.
    Write(SR_QA1); //, sizeof(SR_AB_AT_T));	//�-���� 10���: "CI2,0\r"
    Read(inputBuff); //  ��������� ����� - " -1 ". / or 2D 31

    int answer = std::strtol(inputBuff.data(), nullptr, 10);

	if(answer != -1) // if((inputBuff[1] != 0x31) && (inputBuff[2] != 0x31))
	{
		MessageBox(NULL,"No connection. Check if Echo=Off","Error",MB_OK);
		return(0);
	}


// ��������� ��������� ������: 10��� �� ���� T, 1 ���� �� �, 2� - �� � � �.�.
	Write(SR_1INPUT_A); //, sizeof(SR_1INPUT_A));	//A-����1: "CI0,1\r"
	Write(SR_2INPUT_B); //, sizeof(SR_2INPUT_B));	//�-����2: "CI1,2\r"
	Write(SR_T_AT_10M); //, sizeof(SR_T_AT_10M));	//�-���� 10���: "CI2,0\r"
	Write(SR_AB_AT_T);			// "���� �, � �� �" "CM0\r"
//	Write(SR_SE_MODE_STOP); // "��������� �� ��������� ����� ��� �����������".


// �� ������ ���� ��? ���� �����������
//	Write(SR_SW0); //����� �������� �������� �������.



	SetDiscLevel(discLevel);
	SetSampleTime(sampleTime);
	SetFrontSlope(slopeMode);



	return(1);
}


void SR_COUNTER::Close()
{
	if(hCounter != INVALID_HANDLE_VALUE)
	{
		CloseHandle(hCounter); // ������ �� �����������... - ��� ���������� �� ����������.
		hCounter = INVALID_HANDLE_VALUE;
	}
}

void SR_COUNTER::Start() // �����.
{
	Write(SR_START); //, sizeof(SR_START)); // ������ ��������.

}

void SR_COUNTER::Reset() // �����.
{
	Write(SR_RESET); // ����� ��������.

}


void SR_COUNTER::Write(void *srCommand, char srCommandSize)
{
//	if(hMonochrom==NULL)
//	return;
    DWORD bc;
	WriteFile(hCounter, srCommand, srCommandSize, &bc, NULL);
}

void SR_COUNTER::Write(std::string  counterCommand) // const string ...
{
	if(hCounter == INVALID_HANDLE_VALUE)
	return;

    DWORD bc;
    const char *cstr = counterCommand.data();
    WriteFile(hCounter, cstr, counterCommand.size(), &bc, NULL);

 //   WriteFile(hCounter, SR_CR, sizeof(SR_CR), &bc, NULL);
}



template<std::size_t SIZE>
void SR_COUNTER::Read(std::array<char, SIZE>& counterData)
{
	if(hCounter == INVALID_HANDLE_VALUE)
	return;

	char tmpBuff; // = 0x00;
	for(size_t cInputBuff = 0; cInputBuff < counterData.size(); cInputBuff++)
	{
		DWORD bc;
		tmpBuff = 0x00;
		do
		{
			ReadFile(hCounter, &tmpBuff, 1, &bc, NULL);
		}
		while(tmpBuff == 0x00);  // ������� ������ ������.
		if(tmpBuff == 0x0D)// ����� ��� ��������� SR_CR, �������� ���� �������� �� ����. // � �����, ���� ��� ���������� �� ��������?
		{
			counterData[cInputBuff] = 0x00;
			break;
		}
		counterData[cInputBuff] = tmpBuff;
	}


}


int SR_COUNTER::GetCount() // �������� ���������� ������� � �
{

	// !!!!! QA and QB commands should only be sent after checking the Data Ready status bit.
	// This bit is set at the end of each complete count period and signals the availability of valid data.
	// The Data Ready status bit is reset after it is read. Sending QA or QB commands without polling
	// the Data Ready status can cause data points to be read multiple times.
	// Note that QA and QB do not reset the Data Ready status.

	// Bit 1. Data Ready. This bit is set at the end of each count  period to indicate that counter data is available.
	//  The status byte is read using the SS command which returns the value of the byte in ASCII coded decimal.

	// SS { j } The SS command reads the status byte. (See the following section for a definition of the status byte).
	// j designates one bit, 0-7, of the status byte. If j is included, the designated bit of the status byte is returned
	//("0" or "1"). The bit which is read is then reset.
	// If j is absent, the value of the entire byte is returned (0-255) and all status bits are then reset.

    std::array <char, INPUT_BUFF_SIZE> inputBuff;

    inputBuff.fill(0);


	do
    {
        if(inputBuff[0] != 0) // first time 0, then '0', so delay isnot need first time
        usleep(5000);

        Write(SR_STATUS1);
        // �������� ��������, ����� �� ��������� ����������?
        // �� � ������ ����� �������� ����� ��������, ������ ������� �������?
        Read(inputBuff);
    }
	while(inputBuff[0] != 0x31); //  std::strtol(inputBuff.data(), nullptr, 10) != 1


	Write(SR_QA);

    inputBuff.fill(0);

	Read(inputBuff); //, INPUT_BUFF_SIZE);
//	long sampleValue = std::atol(inputBuff);

    long sampleValue = std::strtol(inputBuff.data(), nullptr, 10); // https://www.delftstack.com/howto/cpp/how-to-convert-char-array-to-int-in-cpp/


	return(sampleValue);
}


int SR_COUNTER::GetSampleTime()
{
	return	sampleTime;
}

int SR_COUNTER::SetSampleTime(int setSampleTime) // ms
{
	if((sampleTime == 0) || (sampleTime >= 90000)) // �������� ������� ����������.
	return 0;

    sampleTime = setSampleTime;

	if(hCounter != INVALID_HANDLE_VALUE) // �.�. ������� ���������.
    {
        Write(SR_TIME_SET); //, sizeof(SR_TIME_SET));	// ����� �������, ��������� �������� �.

        // CP2,n - ������� ������� ����� (�������� �� 10��)? ������ ������� -
        // n may be expressed in any format but must be greater than or equal to 1 and only the most significant digit is used.
        // For example, "CP2,10" , "CP2,1E1" , "CP2,0.1E2" , "CP2,12" all set T SET to 1E1.
        // n is the number of cycles of the 10 MHz clock

        // �� ���� ���� ������ ��������� �� � 10���, ������� �� 10000, � ������������� � ������?
        // � 1 �� � ��� 10000 ��������, � � ���� ���������� 20000 ��, ��� 20 ���. ����� ��� ����?
        // signed long ��� 2 ���, �� ���� 200 ��?
        // ��� ���������� � ����� ������ E4, �� �������� �� � ������ �������? ����� ����� 2000 ���.

        std::string buffer = std::to_string(setSampleTime);
        buffer.resize(6);

// �������� ������ ��� ����� 200 ������������� ��������� ��� \0 (� ����� 6 ������, ��, �� �������� ����� ��� ���������� ���� � ����, ���-��...)
// ������������ - ��. �� https://coderoad.ru/13686482/, ���� str.erase ��� �������� ����� ������������� �����, ���������� ����� str.find_last_not_of
// ��� std::stringstream ���� sprintf(buf, "%f", value)
// https://coderoad.ru/29797515/  std::to_string & sprintf �� ���� ��� ������� ����������� �������������� ���������� ����� trailing,
// ������� �� ��������� ��� �������������� �������� � ������. ���������� ������ ����� ������������ std::stringstream
// ���  �� ������ �������� ������������� ������� https://coderoad.ru/35724411 ��� �� ������ ������������ string stream (sstring) � �������������� ������� ��� ������������ ������ boost
//  https://habr.com/ru/post/471652/
        for(int i = 0; i < 6; i++)
        {
          if(buffer[i] == 0)
            buffer[i] = ' ';

        }

        Write(buffer); // 5 digits and ".", ���� 90000 ��

        const std::string ms_to_10MHz = "E4";
        Write(ms_to_10MHz);
        Write(SR_CR);


        /*

        char _sampleTimeString[]={'0','E','0'}; // {'1','E','6'};
        char tempSampleTime[10]; // ����� ��� ����� �������� �������.
        ltoa(sampleTime,tempSampleTime,10); // ������� � ������, ����� ������ ������ ������.
        _sampleTimeString[0]=tempSampleTime[0]; // ������ �������� ������.
        _sampleTimeString[1]='E';
        _sampleTimeString[2]=(char)log10l(sampleTime)+(char)0x30+(char)4; // ��������� �� ASCII � �� 4 ������� (10��� vs. 1 ��).
        Write(_sampleTimeString, sizeof(_sampleTimeString));// ���������� ����� �������.
        Write(SR_CR);
        */

    }

    return 1;
}


void SR_COUNTER::SetFrontSlope(Slope newSlope) // ������-�� �� ��������
{
    slopeMode = newSlope;

	if(hCounter != INVALID_HANDLE_VALUE) // �.�. ������� �� �����������.
    {
        if(newSlope == FALL)
    	Write(SR_FALL_FRONT_A); //, sizeof(SR_FALL_FRONT_A)); // ������ �� ���������� ������ (�������� ����� �������������� ��������).
    	else
        if(newSlope == RISE)
        Write(SR_RISE_FRONT_A); //, sizeof(SR_RISE_FRONT_A));

 //       Write(SR_CR,sizeof(SR_CR));


    }


}

Slope SR_COUNTER::GetFrontSlope()
{

    return slopeMode;
}



int SR_COUNTER::GetDiscLevel()
{
	return(discLevel);
}

int SR_COUNTER::SetDiscLevel(int dLevel)
{
    // Set DISC i LVL to -0.3000 <= v <= 0.3000 V
    if((dLevel < -3000) || (dLevel > 3000)) // �������� ������� ����������.
	return 0;

    discLevel = dLevel;

	if(hCounter != INVALID_HANDLE_VALUE) // �.�. �������  ���������.
    {
        // ������ ������ ����� ������� "DL0," ����� - "-0.3000", ��������.

        Write(SR_DL_A_SET); //, sizeof(SR_DL_A_SET));

        double buffVoltage = (double)dLevel/(double)1000;
        std::string buffer = std::to_string(buffVoltage);
        buffer.resize(6);
        Write(buffer); // 6 digits and "."
        Write(SR_CR);

/*
        char _discLevelString[3] =  { 0,0,0};
        static char SR_DL_MINUS[] = {'-'}; // ���� ������.
        static char SR_DL_DOT[] = {'.'}; // ���� ���������� �����.
        static char SR_DL_ZERO[] = {'0'}; // ���� ����.

        // �������� ���� ���� ��� ���
        if(discLevel < 0)
        Write(SR_DL_MINUS,sizeof(SR_DL_MINUS)); // ���� ����� ������������� - ���������� "-".
        Write(SR_DL_DOT,sizeof(SR_DL_DOT)); // ����� ����������.
        itoa(abs(discLevel),_discLevelString,10);

        int sizeArray = sizeof(_discLevelString); // ������ 3 �����.
        int lenghtString = strlen(_discLevelString); // ����� �� 0 �� 3 ����.

        for(int cDL_ZERO=sizeArray-lenghtString; cDL_ZERO<lenghtString; cDL_ZERO++)
        Write(SR_DL_ZERO, sizeof(SR_DL_ZERO)); // ������ ����� �� ����� ��������� ������ - �� 0 �� 3.

        Write(_discLevelString, strlen(_discLevelString));

        Write(SR_CR); //,sizeof(SR_CR));
*/
    }

    return 1;

}

int SR_COUNTER::CheckState() // ���������� ��/���, � ����������� �� ��������� �����������.
{
	if(hCounter == INVALID_HANDLE_VALUE) // �.�. ������� �� �����������.
	return 0;
	else
	return 1;
}


void SR_COUNTER::SetPortNumber(int numberPort) // ���������� ����� ���-�����.
{
	if((numberPort != 0) && (numberPort < 10))
    if(hCounter == INVALID_HANDLE_VALUE) //  ������ ���� ������� ��������
	comPort = numberPort;
}

int SR_COUNTER::GetPortNumber()
{
	return(comPort);
}

int SR_COUNTER::SetDeadTime(int dTime)
{
	if(dTime >= 90000) // �������� ������� ����������.
	return 0;


	deadTime = dTime;

	return 1;
}

int SR_COUNTER::GetDeadTime()
{
	return deadTime;
}


// delete: =================================================================================
/*

void	SR_COUNTER::ReadComPort(void *hCounter, char *inputBuff, char INPUT_BUFF_SIZE)
{
	char tmpBuff = 0x00;

	for(char cInputBuff = 0; cInputBuff<INPUT_BUFF_SIZE; cInputBuff++)
	{
		DWORD bc;
		tmpBuff = 0x00;
		do
		{
			ReadFile(hCounter, &tmpBuff, 1, &bc, NULL);
		}
		while(tmpBuff == 0x00);  // ������� ������ ������.
		if(tmpBuff == 0x0D)// ����� ��� ��������� SR_CR, �������� ���� �������� �� ����.
		{
			inputBuff[cInputBuff] = 0x00;
			break;
		}
		inputBuff[cInputBuff] = tmpBuff;
	}
}

void SR_COUNTER::Read(char *srData, char srDataSize) // srDataSize==INPUT_BUFF_SIZE
{
//	if(hMonochrom==NULL)
//	return;
	ReadComPort(hCounter, srData, srDataSize);
}



int SR_COUNTER::GetCountPeriods()
{
	return scanCycles;
}

void SR_COUNTER::SetCountPeriods(int countPeriods)
{
	scanCycles = countPeriods;
}

void SR_COUNTER::SetDWellTime(long dTime)
{
	dWellTime = dTime;
}

long SR_COUNTER::GetDWellTime()
{
	return dWellTime;
}



int SR_COUNTER::NPcurrent()
{
	Write(SR_SCAN_POSITION, sizeof(SR_SCAN_POSITION));
	Read(inputBuff, sizeof(inputBuff));
	int currentScanPosition=atoi(inputBuff);
	return currentScanPosition;
}

long SR_COUNTER::GetSameCount(int sameCount) // �������� � ����������� �������.
{
//	if((sameCount<1)||(sameCount>2000))
//	�����-�� �������� �������� ����������... ��� ��������� � � ������� ����������.
	strset(_scanCyclesString, 0x00); // ���������� ������ �����.
	itoa(sameCount,_scanCyclesString,10); // ����� ����������� � ������.
	Write(SR_SAME_SCAN, sizeof(SR_SAME_SCAN)); 	//������� ����������� �����.
	Write(_scanCyclesString, strlen(_scanCyclesString)); // ������ ������.
	Write(SR_CR); //,sizeof(SR_CR));
	Read(inputBuff,INPUT_BUFF_SIZE);
	long sampleValue=atol(inputBuff);
	return(sampleValue);
}



// ��������� ����� 3-4 ��������� ��������� �������, ������ � �������.
void SR_COUNTER::UpdateSetting()
{
	if(hCounter == NULL) // ���� ������� �� ���������
	return;

	//	������� �� ������ ������ ������� - �� ������������� ��� ����������? � ��� - ���������
// �.�. ���� �������� ��������.


//	1. ��������� ���������� ������ �������.
	itoa(scanCycles,_scanCyclesString,10); // ������� ����� ������ � ��������� ������.
	Write(SR_NP_SET, sizeof(SR_NP_SET));	// ����� �������� NP
	Write(_scanCyclesString,strlen(_scanCyclesString));
	Write(SR_CR, sizeof(SR_CR));

//	2. ��������� ������� �������.
    char _sampleTimeString[]={'0','E','0'}; // {'1','E','6'};
	Write(SR_TIME_SET, sizeof(SR_TIME_SET));	// ����� �������, ��������� �������� �.
	char tempSampleTime[10]; // ����� ��� ����� �������� �������.
	ltoa(sampleTime,tempSampleTime,10); // ������� � ������, ����� ������ ������ ������.
	_sampleTimeString[0]=tempSampleTime[0]; // ������ �������� ������.
	_sampleTimeString[1]='E';
	_sampleTimeString[2]=(char)log10l(sampleTime)+(char)0x30+(char)4; // ��������� �� ASCII � �� 4 ������� (10��� vs. 1 ��).
	Write(_sampleTimeString, sizeof(_sampleTimeString));// ���������� ����� �������.
	Write(SR_CR, sizeof(SR_CR));

//	3. ��������� ������� ��������.
	Write(SR_DTIME_SET, sizeof(SR_DTIME_SET));//������� - ����� ��������.
	char tempDWellString[10];
	ltoa(dWellTime,tempDWellString,10); // ������������� ������������ � ������.
	float fDWellTime=((float)dWellTime)/1000;	// ������� �� ����������� � �������.
	_dWellTimeString[0]=tempDWellString[0];// ������ ����.
	_dWellTimeString[1]='E';
	if(fDWellTime<1)// ����� ����� �������.
	{
		_dWellTimeString[2]='-'; // ������ 1 ���., ����. ������������� �������.
		_dWellTimeString[3]=-(char)log10l(fDWellTime)+0x30+1; // ��������� �� ASCII � �� �������.
		Write(_dWellTimeString, sizeof(_dWellTimeString)); // ���� ����� ��������.
		Write(SR_CR, sizeof(SR_CR));
	}
	else // ������� � ������.
	{
		_dWellTimeString[2]=(char)log10l(fDWellTime)+0x30; // ��������� �� ASCII.
		_dWellTimeString[3]=0x0D; // ����� ���� - ������ ���� ��������.
		Write(_dWellTimeString, sizeof(_dWellTimeString)); // ���� ����� ��������.
	}

//	4. ��������� ������ �������������. � ������������
	char _discLevelString[3]={0,0,0};
	static char SR_DL_MINUS[]={'-'}; // ���� ������.
    static char SR_DL_DOT[]={'.'}; // ���� ���������� �����.
    static char SR_DL_ZERO[]={'0'}; // ���� ����.

	Write(SR_DL_A_SET, sizeof(SR_DL_A_SET));
	// �������� ���� ���� ��� ���
	if(discLevel<0)
	Write(SR_DL_MINUS,sizeof(SR_DL_MINUS)); // ���� ����� ������������� - ���������� "-".
	Write(SR_DL_DOT,sizeof(SR_DL_DOT)); // ����� ����������.
	itoa(abs(discLevel),_discLevelString,10);

	int sizeArray=sizeof(_discLevelString); // ������ 3 �����.
	int lenghtString=strlen(_discLevelString); // ����� �� 0 �� 3 ����.

	for(int cDL_ZERO=sizeArray-lenghtString; cDL_ZERO<lenghtString; cDL_ZERO++)
	Write(SR_DL_ZERO, sizeof(SR_DL_ZERO)); // ������ ����� �� ����� ��������� ������ - �� 0 �� 3.

	Write(_discLevelString, strlen(_discLevelString));
	Write(SR_CR,sizeof(SR_CR));

	// ��������

}


void SR_COUNTER::Halt() // �������.
{
	Write(SR_HALT); //, sizeof(SR_HALT)); // ������� ��������.

}


*/
