// acton_monochromator.cpp: implementation of the ACTON_MONOCHROMATOR class.
//
//////////////////////////////////////////////////////////////////////
#include "acton_monochromator.h"

#include <windows.h>
#include <unistd.h> // usleep(TIME_TO_SLEEP); ms

#include <string> //
#include <array> //  for std::array<int, 3> inputBuff
//#include <thread> // try to wait monochromator

#include <wx/app.h>


/*  // �� ����� ���
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
*/



const int ARC_INPUT_BUFF_SIZE = 16;

const double ARC_WAVELENGTH_MIN = 0;
const double ARC_WAVELENGTH_MAX = 5000; // change to a function SetMonochromatorMax() later that set the value for different grates...

// ��������� ������� ���������� ��������������.
const std::string ARC_SET_WAVELENGTH = " GOTO\r";
const std::string ARC_SET_GRATING = " GRATING\r";
const std::string ACR_GET_GRATING = " ?GRATING\r";

const std::string ARC_CR = " \r"; //  {0x0D};  //  - ������� �������.
// static	char ARC_ANSWER[] =             {0x20,0x20,0x6F,0x6B,0x0D,0x0A}; // ����������� ����� �������� OK  � ����� �������� �������, �� ����� ������������ � ������.
// const std::string ARC_ANSWER = "\x20\x20\x6F\x6B\x0D\x0A";

// ���� � ����������� ������� ������ �� ������, ��������� �� ������������
const std::string ARC_SET_MIRROR_FRONT = " FRONT\r";
const std::string ARC_SET_MIRROR_SIDE = " SIDE\r";
const std::string ACR_GET_MIRROR = " ?MIRROR\r";
// This command is for SpectraPro monochromators which can accept two diverter mirrors
const std::string ARC_EXIT_MIRROR = " EXIT-MIRROR\r";







//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

// ARC_MONOCHROMATOR::ARC_MONOCHROMATOR() : comPort{5}, wavelengthStep{5}, gratingNumber{1}, mirrorPosition{FRONT}, wavelengthStart{900}, wavelengthStop{1700}
// ARC_MONOCHROMATOR::ARC_MONOCHROMATOR(int _comPort = 5, double _wavelengthStart = 900, double _wavelengthStop = 1700, double _wavelengthStep = 5.0, int _gratingNumber = 1, Mirror _mirrorPosition = FRONT)
// : comPort{_comPort}, wavelengthStart{_wavelengthStart}, wavelengthStop{_wavelengthStop}, wavelengthStep{_wavelengthStep}, gratingNumber{_gratingNumber}, mirrorPosition{_mirrorPosition}
ARC_MONOCHROMATOR::ARC_MONOCHROMATOR()
{

// ������������ �������������, ���� ��������, ��� ����, ��������� ��� ��������� ��-���������, � ��������� �������� ���������� ��� ������ ������������.
// ����� ��� ��-��������� ����� ���� ��������, � � ���������� - ������. �� ���� ������ �������� ��������� ��������,  ���������� ������� ������ ����� �������� ����.
// �� �� ����� � ��� ������������.
// ��� ������ ���� ����������� ��� ���������� � ��������� ������� ��-���������, � ��� ����� �����.
// � ��� ��� ����� ���� ������� ��������� � �����.. ����� ����� ��� �������� ��������� ������ ����, � ���� ��� ��� ��� �������� ����� - �� �������� ����������� ��-���������.
// ����������� ��������� ��� ���
// https://www.geeksforgeeks.org/when-do-we-use-initializer-list-in-c/
// https://en.cppreference.com/w/cpp/language/constructor
// https://stackoverflow.com/questions/9903248/initializing-fields-in-constructor-initializer-list-vs-constructor-body
// https://stackoverflow.com/questions/187640/default-parameters-with-c-constructors

// ��� �� �������� � ���������� �������� ��-������, ���� ���� �����.

/*

https://codewiki.imagetube.xyz/code/C++/%D0%A1%D0%BF%D0%B8%D1%81%D0%BE%D0%BA_%D0%B8%D0%BD%D0%B8%D1%86%D0%B8%D0%B0%D0%BB%D0%B8%D0%B7%D0%B0%D1%86%D0%B8%D0%B8

class SequenceClass {
public:
  SequenceClass(std::initializer_list<int> list);
};



struct AltStruct {
    AltStruct(int x, double y) : x_(x), y_(y) {} //� x_ ����������� x, � y_ ����������� y
private:
    int x_;
    double y_;
};
AltStruct var2{2, 4.3};
AltStruct var3 = {2, 4.3};

����� ��������� �: x_(x), y_(y)� ��� ������������� ������������� ��� ���������� ���������� ������� �������������.
����� ������ �������� �����������, ������������� (�������� � �������� �����),
���������� (�������� ������ ��� ������������� ������� �����) � �������������.


// With Initializer List
class MyClass {
	Type variable;
public:
	MyClass(Type a):variable(a) { // Assume that Type is an already
					// declared class and it has appropriate
					// constructors and operators
	}
};
*/


/*
	// ������������� ���������� "�� ���������".
	comPort = 5;//

// 3. ��� � ��... ��� ��� ����� ���� ������� ������. � ��� ����
	wavelengthStep = 5; //
// 4. ����� ������������ �������, ������� ���� ������ ����, � ���� ����� ������ ����� - ����� ��������.
	gratingNumber = 1; // �2 - 1200 ���/��, �1 - 2400 ���/��. � 300/150
// 5. ��������� ��������� �������. (� ��� ����� ����� ���� ������� enum )
	mirrorPosition = FRONT; // 0 - front, 1 - side (������� ?MIR).

//	7. �������� ���� ���� �� ���������. .
	wavelengthStart = 900; //
	wavelengthStop = 1700;
// 8. ����� �� ������� ������� ����� ����� ��������� �����������/
	//timeChangePos = 300; //1000;// � �������������.

 //   hMonochrom = INVALID_HANDLE_VALUE; //NULL; // ���������� ��������������� ������������� ����� 0. ��� � �64 = -1
    // � ��� ������ ���� ����� ����������������, ��� ��������..
*/

}

ARC_MONOCHROMATOR::~ARC_MONOCHROMATOR()
{

}

int ARC_MONOCHROMATOR::Open(int port) // ����������� ������������,
{
    if((port > 9) || (port < 1))
        return (0);

    comPort = port;
	// ��� �++ ����� �������� ����� https://blablacode.ru/programmirovanie/392

	char comName[] = "COM3"; //
    comName[3] = (char)(comPort + 0x30);

// or
/*
	switch (comPort)	// ����� ���������� �����.
	{					//���������� �����, ��� ��������� �������.
		case 1:
		strcpy(comName,"COM1"); break;
		case 2:
		strcpy(comName,"COM2"); break;
//  ...
		default:
		strcpy(comName,"COM3"); break;
	}
*/

// For COM10 and higher - another rules -     LPCSTR sPortName = "\\\\.\\COM11"; / LPCTSTR sPortName = L"\\\\.\\COM21";
// traditional way for legacy up to 9
// http://msdn2.microsoft.com/en-us/library/aa363858.aspx

	hMonochrom = CreateFileA((LPCSTR)comName, GENERIC_READ|GENERIC_WRITE, 0, NULL,OPEN_EXISTING, 0, 0);

	if(hMonochrom == INVALID_HANDLE_VALUE) // �������� ��������� �����. NULL for 32-bit Win and INVALID_HANDLE_VALUE for 64bit
	{
	    if(GetLastError() == ERROR_FILE_NOT_FOUND)
            MessageBox(NULL, "COM port is not found", comName, MB_OK);
        else
            MessageBox(NULL, "Error Open COM", "Error", MB_OK);
		return(0);
	}

//		GetLastError MessageBox(NULL,"OK Open COM","Error",MB_OK);


	//	������� ������.
//	int retcode = PurgeComm(hMonochrom, PURGE_TXABORT | PURGE_RXABORT | PURGE_TXCLEAR | PURGE_RXCLEAR);
	if(!PurgeComm(hMonochrom, PURGE_TXABORT | PURGE_RXABORT | PURGE_TXCLEAR | PURGE_RXCLEAR))
	{
		MessageBox(NULL,"PurgeComm() failed!","Error",MB_OK);
		Close();
		return(0);
	}

//	MessageBox(NULL,"PurgeComm() OK!","Error",MB_OK);
//		retcode = ClearCommBreak(hMonochrom);
	if(!ClearCommBreak(hMonochrom))
	{
		MessageBox(NULL,"ClearCommBreak() failed!","Error",MB_OK);
		Close();
		return(0);
	}

//	��������� DCB ��� �������� ��������� ��������� � ��������� �����.
	DCB dcb; //  = {0}
//	retcode = GetCommState(hMonochrom,&dcb); // ��������� ���������� �����.
	if(!GetCommState(hMonochrom, &dcb))
	{
		MessageBox(NULL,"Port DCB error!","Error",MB_OK);
		Close();
		return(0);
	}

//	MessageBox(NULL,"Port DCB OK!","Error",MB_OK);

// ��������� ������ ������� ����� ���-����
	dcb.BaudRate = CBR_9600;
	dcb.ByteSize = 8;
	dcb.Parity = NOPARITY;
	dcb.StopBits = ONESTOPBIT; // TWOSTOPBITS

    // ��������� ��������� �����.
	if(!SetCommState(hMonochrom, &dcb))
	{
		MessageBox(NULL,"Port parameter error!","Error",MB_OK);
		Close();
		return(0);
	}

//	������� ��������� ������/������.
	COMMTIMEOUTS  ct;
	ct.ReadIntervalTimeout = MAXDWORD; // 8000;// ����� ��������� ��� ������������ �.�. �����...  �� ������-�� ��� ����� �� ������.
	ct.ReadTotalTimeoutMultiplier = MAXDWORD; // 3000;//
	ct.ReadTotalTimeoutConstant = 5000;
	ct.WriteTotalTimeoutMultiplier = 10;
	ct.WriteTotalTimeoutConstant = 10;

/* // counter:
	ct.ReadIntervalTimeout = MAXDWORD ; // 3000;//MAXDWORD;
	ct.ReadTotalTimeoutMultiplier = MAXDWORD; // 3000;// MAXDWORD;
	ct.ReadTotalTimeoutConstant = 2000; //2000;//100;
	ct.WriteTotalTimeoutMultiplier = 0;
	ct.WriteTotalTimeoutConstant = 0;


// mono  �� ������� ����� �� ���������� ������������

	ct.ReadIntervalTimeout = 3000;//MAXDWORD;
	ct.ReadTotalTimeoutMultiplier = 3000;// MAXDWORD;
	ct.ReadTotalTimeoutConstant = 2000;
	ct.WriteTotalTimeoutMultiplier = 0;
	ct.WriteTotalTimeoutConstant = 0;
*/


// 	retcode = SetCommTimeouts(hMonochrom,&ct);
	if(!SetCommTimeouts(hMonochrom,&ct))
	{
		MessageBox(NULL,"Timeouts parameter error!","Error",MB_OK);
		Close();
		return(0);
	}

//MessageBox(NULL,"Timeouts parameter OK!","Error",MB_OK);

	SetupComm(hMonochrom,ARC_INPUT_BUFF_SIZE, ARC_INPUT_BUFF_SIZE); // ������� �������� ������ �����.

//MessageBox(NULL,"SetupComm OK!","Error",MB_OK);

    std::array <char, ARC_INPUT_BUFF_SIZE> inputBuff;

    inputBuff.fill(0);

//	�������� ����������� - ���� ��� ���.
//	Write(ARC_CR); // CR - "������� �������", 0x0D.
// https://www.thinksrs.com/downloads/pdfs/manuals/SR400m.pdf
// COMMON SOFTWARE PROBLEMS INCLUDE:
// 3) The initial command sent to the SR400 was invalid due to a garbage character received during power-up,
// or, the first character in your computer's UART is garbage, also due to power-up.
// It is good practice to send a few carriage returns to the SR400 when your program begins

    Write(ARC_CR);
    Write(ARC_CR);
    Read(inputBuff);

//MessageBox(NULL,"WriteRead OK!","Error",MB_OK);


// sometimes the first answer is {0x20,0x6F,0x6B,0x0D,0x0A} instead of {0x20,0x20,0x6F,0x6B,0x0D,0x0A}
// ������������ ������ ����� �������� � ����� ������, � ����� ���������� ����� ������, ��� ����� ����������. ������� ������ ������ ������ ������ ���-�� ���������.

    int x = 0;
    while(inputBuff[x] == 0x20)
        x++;

    if((inputBuff[x] != 0x6F) && (inputBuff[x + 1] != 0x6B))
//    if( !((inputBuff[2] == ARC_ANSWER[2]) && (inputBuff[3] == ARC_ANSWER[3])) ) // if answer is {0x20,0x20,0x6F,0x6B,0x0D,0x0A} only
	{
/*		// ���������������� ������, ����� � ���� ����������� ������.
		char name[]="input_buff.dat";
		ofstream ofs(name, ios::out | ios::binary);
		if (ofs)
		ofs.write(inputBuff, ARC_INPUT_BUFF_SIZE);
		ofs.close ();
*/
		MessageBox(NULL,"No connection","Error",MB_OK);
		Close();
		return(0);
		// � ������� ����?
	}

// ������� ������� �� �������� ����. �������� ����� ������ � ������� �� ������, ����� ��
// -------------------
//	Write(ARC_EXIT_MIRROR); // ������� ������� � ��� ���
//	setMirrorPosition(mirrorPosition);
// -------------------

// ��� ���� ��������� ������?

// ���� - ���� ��������� ������ �����??
	SetGrate(gratingNumber);
	usleep(50000);  // ������ ���� ��������� � ������������, � �� ����� ���


//MessageBox(NULL,"inputBuff OK!","Error",MB_OK);

	GoToWavelength(wavelengthStart);

	// ��������� ������������ �������. ���� �����?
	// ����� ��������� ������� �������� ������ "��"?.


//MessageBox(NULL,"UpdateSetting OK!","Error",MB_OK);


	return(TRUE);
}


void ARC_MONOCHROMATOR::Close() // ���������� ��������.
{
	if(hMonochrom != INVALID_HANDLE_VALUE) // INVALID_HANDLE_VALUE
	{
		CloseHandle(hMonochrom);
		hMonochrom = INVALID_HANDLE_VALUE;
	}
}

// Writes command and CR
// template<std::size_t SIZE>
void ARC_MONOCHROMATOR::Write(std::string  arcCommand) // const string ...
{
	if(hMonochrom == INVALID_HANDLE_VALUE)
	return;

    DWORD bc;
    const char *cstr = arcCommand.data();
    WriteFile(hMonochrom, cstr, arcCommand.size(), &bc, NULL);


 /*
 // convert string into char*
 //   string str = "some string" ;
   // char *cstr = str.data();
//
//    std::string str = "string";
//    const char *cstr = str.c_str();


  std::array <char, ARC_INPUT_BUFF_SIZE> outputBuff;
    outputBuff.fill(0);
    using std::begin; using std::end;
    std::copy(begin(arcCommand), end(arcCommand), begin(outputBuff));
//    std::copy(arcCommand.begin(), arcCommand.end(), outputBuff.data());
    outputBuff[arcCommand.size()] = 0x0D;
*/
}


// void ARC_MONOCHROMATOR::Read(char *arcData, char arcDataSize)
template<std::size_t SIZE>
int ARC_MONOCHROMATOR::Read(std::array<char, SIZE>& arcData)
{
	if(hMonochrom == INVALID_HANDLE_VALUE)
	return 0;

// ����� ��� � �������� ������� ������� � ������ ������? ����� � ������ ���, �� ���� ����� ���������� ��� ������� - �� ������� �����������.
	char tmpBuff; // = 0x00;


	for(size_t cInputBuff = 0; cInputBuff < arcData.size(); cInputBuff++)
	{
		DWORD bc;
		tmpBuff = 0x00;
		do
		{
			ReadFile(hMonochrom, &tmpBuff, 1, &bc, NULL);
		}
		while(tmpBuff == 0x00);  // ������� ������ ������.
		if(tmpBuff == 0x0A)// \r -> 0
		{
			arcData[cInputBuff] = 0x00;
			break;
		}
		arcData[cInputBuff] = tmpBuff;
	}
	return 1;


}

int ARC_MONOCHROMATOR::GetPortNumber()
{
	return(comPort);
}

void ARC_MONOCHROMATOR::SetPortNumber(int numberPort)
{
	if((numberPort > 0) && (numberPort < 10))
    if(hMonochrom == INVALID_HANDLE_VALUE) //  ������ ���� ������������ ��������
	comPort = numberPort;
}

int ARC_MONOCHROMATOR::CheckState()
{
	if(hMonochrom == INVALID_HANDLE_VALUE) //||(hMonochrom == NULL))
	return 0;
	else
	return 1;
}


double ARC_MONOCHROMATOR::GetWavelengthStep()
{
	return wavelengthStep;
}


int ARC_MONOCHROMATOR::GoToWavelength(double targetWavelength)
{

    PurgeComm(hMonochrom, PURGE_TXCLEAR | PURGE_RXCLEAR);


    std::string buffer = std::to_string(targetWavelength);
    buffer.resize(7);
    Write(buffer); // 6 digits and "."

//	Write(tempWaveString, sizeof(tempWaveString));
	Write(ARC_SET_WAVELENGTH);


    std::array <char, ARC_INPUT_BUFF_SIZE> inputBuff;
    inputBuff.fill(0);

    Read(inputBuff);

//    std::thread t1(ARC_MONOCHROMATOR::Read, this, std::ref(inputBuff));
//    t1.join();


    int x = 0;
    while(inputBuff[x] == 0x20)
        x++;

    if((inputBuff[x] != 0x6F) && (inputBuff[x + 1] != 0x6B))
	{
		return(0);
	}

	return 1;

// ������ ����� ��������������� � ������ � ����� �������� �������� 7 ��������, ������� �����. ��� 7 ���� ����� ���� 1000.
// https://www.tutorialspoint.com/how-do-i-convert-a-double-into-a-string-in-cplusplus
// https://en.cppreference.com/w/cpp/string/basic_string/to_string

// ������ http://www.codenet.ru/progr/cpp/spru/gcvt.php
// https://stackoverflow.com/questions/50710587/convert-double-to-char-array-c

// ������������� std::stringstream � setprecision ���� �� �������� ������ / ����������� �������
/*
#include <sstream>

// ...

float number = 30.0f;

std::ostringstream oss;
oss << std::setprecision(1) << number;
std::string result = oss.str();


or
#include <iostream>     // std::cout, std::fixed
#include <iomanip>      // std::setprecision
  double f =3.14159;
 std::cout << std::fixed;
  std::cout << std::setprecision(3) << f << '\n';

//
// Converting double to string with custom precision 2


*/


}

double ARC_MONOCHROMATOR::GetWavelengthStop()
{
	return wavelengthStop;
}

double ARC_MONOCHROMATOR::GetWavelengthStart()
{
	return wavelengthStart;
}


void ARC_MONOCHROMATOR::SetWavelengthStart(double waveStart)
{
    if((waveStart <= ARC_WAVELENGTH_MAX)&&(waveStart >= ARC_WAVELENGTH_MIN))
    {
        wavelengthStart = waveStart;
    }
    else
    {
        if(waveStart > ARC_WAVELENGTH_MAX)
            wavelengthStart = ARC_WAVELENGTH_MAX;
        if(waveStart < ARC_WAVELENGTH_MIN)
            wavelengthStart = ARC_WAVELENGTH_MIN;
    }
}


void ARC_MONOCHROMATOR::SetWavelengthStop(double waveStop)
{
    if((waveStop <= ARC_WAVELENGTH_MAX)&&(waveStop >= ARC_WAVELENGTH_MIN))
    {
        wavelengthStop = waveStop;
    }
    else
    {
        if(waveStop > ARC_WAVELENGTH_MAX)
            wavelengthStop = ARC_WAVELENGTH_MAX;
        if(waveStop < ARC_WAVELENGTH_MIN)
            wavelengthStop = ARC_WAVELENGTH_MIN;
    }
}

void ARC_MONOCHROMATOR::SetWavelengthStep(double step)
{
	wavelengthStep = step;
}

int ARC_MONOCHROMATOR::GetGrate()
{


/*
//    PurgeComm(hMonochrom, PURGE_TXABORT | PURGE_RXABORT | PURGE_TXCLEAR | PURGE_RXCLEAR))l
    Write(ACR_GET_GRATING);
    std::array <char, ARC_INPUT_BUFF_SIZE> inputBuff;
    inputBuff.fill(0);
    Read(inputBuff);  // \CR ��� �������� �� 0 � Read

    // stdlib.h
 //   int number = atoi(inputBuff); // but it needs \0 in the end

    int number = std::strtol(inputBuff.data(), nullptr, 10); // https://www.delftstack.com/howto/cpp/how-to-convert-char-array-to-int-in-cpp/

    return  number;
*/
    return gratingNumber;
}

void ARC_MONOCHROMATOR::SetGrate(int grating)
{
    if((grating <= 3) && (grating > 0))
    {
        gratingNumber = grating;

        if(hMonochrom != INVALID_HANDLE_VALUE)
        {
            DWORD bc;
            char gratingNumberChar = char(grating + 0x30);
            WriteFile(hMonochrom, &gratingNumberChar, sizeof(gratingNumberChar), &bc, NULL);
            //Write(&gratingNumberChar, sizeof(gratingNumberChar));
            Write(ARC_SET_GRATING);
        }
    }
}

int ARC_MONOCHROMATOR::getGratesNumber()
{
    // calculate strings in answer?
    // ������� "?GRATINS" ������ ������ �����, � ���� ������������ ������� �����, �� ������ � ��� ���� " 1  150 g/mm"
    // �� ���� ���� �������� �� 7-8 ��������, ��� ������ ����� 4-7, �� ������ ��� ������� ��� "Not" ��� �������������.
    // ����� ����� ��� ������ ��������� � ������ ����������� ������, ������ ������ ������.
    // ��� �������� ������ �� ������ �����, ������� ��� ������������ � ������� �������?
    // ��� ����� ������ ��� ������ � ���� � ��������?
    return 0;
}

Mirror ARC_MONOCHROMATOR::getMirrorPosition()
{
 //    PurgeComm(hMonochrom, PURGE_TXABORT | PURGE_RXABORT | PURGE_TXCLEAR | PURGE_RXCLEAR))l

 //   int position = 0;
    Write(ACR_GET_MIRROR);
    std::array <char, ARC_INPUT_BUFF_SIZE> inputBuff;
    inputBuff.fill(0);
    Read(inputBuff);  // \CR ��� �������� �� 0 � Read

        int x = 0;
        while(x == 0x20)
            x++;

        if((inputBuff[x] == 'F') && (inputBuff[x + 1] == 'R') && (inputBuff[x + 2] == 'O'))
        {
            return FRONT;
        }
        else //if((inputBuff[x] == 'S') && (inputBuff[x + 1] == 'I') && (inputBuff[x + 2] == 'D'))
        {
            return SIDE;
        }
}

void ARC_MONOCHROMATOR::setMirrorPosition(Mirror position)
{

    if(position == FRONT)
    Write(ARC_SET_MIRROR_FRONT);
    else
    Write(ARC_SET_MIRROR_SIDE);

}


// deleted ====================
/*

*/

// 2022
// ������� �� ������������ ������
/*
int ARC_MONOCHROMATOR::GetTimeChangePos()
{
	return timeChangePos;
}
*/

// 2021
/*
double ARC_MONOCHROMATOR::GetLaserWavelenght()
{
	return laserWavelenght;
}


void ARC_MONOCHROMATOR::SetLaserWavelenght(double waveLenght)
{
	laserWavelenght=waveLenght;
}

void ARC_MONOCHROMATOR::SetShiftMinMax(float shMin, float shMax)
{
	shiftMin=shMin;
	shiftMax=shMax;
}

double ARC_MONOCHROMATOR::Shift2Wavelenght(float newShift)
{
	double newWavelenght;
	newWavelenght=laserWavelenght/((double)1.0-(double)newShift*laserWavelenght*(double)0.0000001);
	return newWavelenght;
}

float ARC_MONOCHROMATOR::Wavelenght2Shift(double newWavelenght)
{
	float newShift;
	newShift=(float)(10000000*(newWavelenght-laserWavelenght)/(laserWavelenght*newWavelenght));
	return newShift;
}

int ARC_MONOCHROMATOR::GetSlitWidthSide()
{
	return slitWidthSide;
}

void ARC_MONOCHROMATOR::SetSlitWidthSide(int slitWidth)
{
	slitWidthSide=slitWidth;
}

*/

/*
void ARC_MONOCHROMATOR::ReadComPort(void *hMonochrom, char *inputBuff, char ARC_INPUT_BUFF_SIZE)
{
	char tmpBuff; // = 0x00;
	for(char cInputBuff = 0; cInputBuff < ARC_INPUT_BUFF_SIZE; cInputBuff++)
	{
		DWORD bc;
		tmpBuff = 0x00;
		do
		{
			ReadFile(hMonochrom, &tmpBuff, 1, &bc, NULL);
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
*/

/*
// writes just string, without CR
void ARC_MONOCHROMATOR::Write(char *arcCommand, int arcCommandSize)
{
	if(hMonochrom == INVALID_HANDLE_VALUE)
	return;

	WriteFile(hMonochrom, arcCommand, arcCommandSize, &bc, NULL);
	// ��� �������� bc �� �� ��� ���� - LPDWORD, ��� �������� ������� �� �������, �.�. ���� �� API ������� ������ read(fd, buf1, length);
	// �������, � ��� � ReadFile  ���� �����������, ������ ���. ��� ������ ��� ������ ������ ��� ����
	// � ����� ���� ������ �� WinAPI �������. ��� �� �������������������� :)
	// �������� � ������� ��� �++ https://www.udacity.com/blog/2021/05/how-to-read-from-a-file-in-cpp.html
	// ��� ��� ������ � WibAPI https://blablacode.ru/programmirovanie/392:
	//  WriteFile (hSerial,data,dwSize,&dwBytesWritten,NULL);

}
*/

/*
// �������� �������� �������, �� ������ �� ���������� ������
void ARC_MONOCHROMATOR::UpdateSetting() // ���������� ���������� �������������.
{
	if(hMonochrom == INVALID_HANDLE_VALUE)
	return;

	// ������� ������� �� ��������� ������� ����� �����.
	GoToWavelength(wavelengthStart);

	// ��������� ������������ �������.
	SetGrate(gratingNumber);

//	char gratingNumberCorr = gratingNumber + 0x30;	// ������������� �� ASCII.
//	Write(&gratingNumberCorr, sizeof(gratingNumberCorr));				// ������ ������.
//	Write(ARC_SET_GRATING); // GRATING + 0x0D.

	// ������� ������� �� �������� ����.
	setMirrorPosition(mirrorPosition);
	// ���� ���� ������������ ������ ���� �������� �� ���������, � ����� ������� ����� �������.
//	char tempSlitWidthString[4];
//	if(mirrorPosition == 0) // ��������� �� ���� front.
//	{
//		Write(ARC_SET_MIRROR_FRONT);
//		Write(ARC_SELECT_FRONT_SLIT, sizeof(ARC_SELECT_FRONT_SLIT));
//		itoa(slitWidthFront,tempSlitWidthString,10);
//	}
//	else				// ��������� �� ���� side.
//	{
//		Write(ARC_SET_MIRROR_SIDE);
//		Write(ARC_SELECT_SIDE_SLIT, sizeof(ARC_SELECT_SIDE_SLIT));
//		itoa(slitWidthSide,tempSlitWidthString,10);
//	}

 // 2021
	// ��������� ������ ����.
//	Write(tempSlitWidthString, strlen(tempSlitWidthString));
//	Write(ARC_SET_SLIT_WIDTH, sizeof(ARC_SET_SLIT_WIDTH));

}
*/
