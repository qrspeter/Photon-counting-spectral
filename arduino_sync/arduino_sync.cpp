/**
   библиотека функций работы с Arduino, через который идет синхронизация измерения с лазером.


 */




//#include <iostream>
//#include <wx/filename.h> // для wxYield

#include <windows.h>
//#include <unistd.h> // usleep(TIME_TO_SLEEP); ms
//#include <string>
//#include <cmath> // pow(a, b) = ab

#include "arduino_sync.h"


ARDUINO_SYNC::ARDUINO_SYNC()
{

}

ARDUINO_SYNC::~ARDUINO_SYNC()
{
    if(hSerial != INVALID_HANDLE_VALUE)
	CloseHandle(hSerial);

}

int ARDUINO_SYNC::Open(int port)
{

    char com_name[] = "COM3";
    if(port > 9)
	{
		MessageBox(NULL, "Wrong number!", "Error", MB_OK);
		CloseHandle(hSerial);
		return(0);
	}

    com_port = port;

    com_name[3] = (char)(port + 0x30);

    hSerial = CreateFileA((LPCSTR)com_name, GENERIC_READ|GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, 0);

	if(hSerial == INVALID_HANDLE_VALUE) // проверка выделения порта. NULL for 32-bit Win and INVALID_HANDLE_VALUE for 64bit
	{
	    if(GetLastError() == ERROR_FILE_NOT_FOUND)
        MessageBox(NULL, "COM port is not found", "Error", MB_OK);
        else
		MessageBox(NULL, "Error Open COM", "Error", MB_OK);
		return(0);
	}

	SetupComm(hSerial, INPUT_BUFF_SIZE, INPUT_BUFF_SIZE); // задание размеров буфера порта.


	DCB dcb; //  = {0}
	if(!GetCommState(hSerial, &dcb))
	{
		MessageBox(NULL, "Port DCB error!","Error", MB_OK);
		CloseHandle(hSerial);
		return(0);
	}


	dcb.BaudRate    = CBR_9600;
	dcb.ByteSize    = 8;
	dcb.Parity      = NOPARITY;
	dcb.StopBits    = ONESTOPBIT;
	dcb.fDtrControl = DTR_CONTROL_ENABLE; // иначе работает только после включения терминала порта, идея отсюда https://github.com/dmicha16/simple_serial_port/blob/master/simple-serial-port/simple-serial-port/SimpleSerial.cpp
	// хотя странно, параметр отвечает за сброс при включении
	// DTR_CONTROL_DISABLE; // disable DTR to avoid reset SetCommState(m_hCom, &dcb);  https://forum.arduino.cc/t/disable-auto-reset-by-serial-connection/28248/12
	// хотя ... это "Data terminal ready" https://docs.microsoft.com/en-us/windows/win32/api/winbase/ns-winbase-dcb

    // Установка параметры порта.
	if(!SetCommState(hSerial, &dcb))
	{
		MessageBox(NULL,"Port parameter error!", "Error", MB_OK);
		CloseHandle(hSerial);
		return(0);
	}

	COMMTIMEOUTS  ct;

	ct.ReadIntervalTimeout = MAXDWORD ; // 3000;//MAXDWORD;
	ct.ReadTotalTimeoutMultiplier = MAXDWORD; // 3000;// MAXDWORD;
	ct.ReadTotalTimeoutConstant = 2000; //2000;//100;
	ct.WriteTotalTimeoutMultiplier = 0;
	ct.WriteTotalTimeoutConstant = 0;


	if(!SetCommTimeouts(hSerial, &ct))
	{
		MessageBox(NULL, "Timeouts parameter error!","Error", MB_OK);
		CloseHandle(hSerial);
		return(0);
	}

	if(!PurgeComm(hSerial, PURGE_TXABORT | PURGE_RXABORT | PURGE_TXCLEAR | PURGE_RXCLEAR))
	{
		MessageBox(NULL, "PurgeComm() failed!", "Error", MB_OK);
		CloseHandle(hSerial);
		return(0);
	}

	if(!ClearCommBreak(hSerial))
	{
		MessageBox(NULL, "ClearCommBreak() failed!", "Error", MB_OK);
		CloseHandle(hSerial);
		return(0);
	}

	for(int i = 0; i<3;++i)
    {
        WriteFile(hSerial, &ON, sizeof(ON), &bc, NULL);
        Sleep(300);
        WriteFile(hSerial, &OFF, sizeof(OFF), &bc, NULL);
        Sleep(300);

//        WriteFile(hSerial, "\x01", 1, &bc, NULL);
//        usleep(3*100000);
//        WriteFile(hSerial, "\x00", 1, &bc, NULL);
//        usleep(3*100000);
    }

	return 1;

}


//int SENSOR_FET::Reset()
//{
//   WriteFile(hSerial, &resetSensor, sizeof(resetSensor), &bc, NULL);
//
//   return 1;
//
//}

void ARDUINO_SYNC::Laser(laser las) // Возвращает да/нет, в зависимости от состояния подключения.
{
	if(las == LASER_ON)
        WriteFile(hSerial, &ON, sizeof(ON), &bc, NULL);
    else
        WriteFile(hSerial, &OFF, sizeof(OFF), &bc, NULL);

//    if(las == LASER_ON)
//        WriteFile(hSerial, "\x01", 1, &bc, NULL);
//    else
//        WriteFile(hSerial, "\x00", 1, &bc, NULL);
}


int ARDUINO_SYNC::CheckState() // Возвращает да/нет, в зависимости от состояния подключения.
{
	if(hSerial == INVALID_HANDLE_VALUE) // Т.е. счетчик не подключался.
	return 0;
	else
	return 1;
}

void ARDUINO_SYNC::Close()
{
	if(hSerial != INVALID_HANDLE_VALUE)
	{
		CloseHandle(hSerial); // похоже не срабатывает... - при отключении не сбрасывает.
		hSerial = INVALID_HANDLE_VALUE;
	}
}

int ARDUINO_SYNC::GetPortNumber()
{
    return com_port;
}

void ARDUINO_SYNC::SetSleeping(int sleeping)
{
    laser_sleeping = sleeping;
}

int ARDUINO_SYNC::GetSleeping()
{
    return laser_sleeping;
}
