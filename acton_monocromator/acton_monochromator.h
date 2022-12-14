// acton_monochromator.h: interface for the ACTON_MONOCHROMATOR class.
//
//////////////////////////////////////////////////////////////////////
#ifndef ACTON_MONOCHROMATOR_H
#define ACTON_MONOCHROMATOR_H
/*
#if !defined(AFX_ARC_SPECTROMETER_H__E8A68800_3561_11DC_863E_0015E93D1D1D__INCLUDED_)
#define AFX_ARC_SPECTROMETER_H__E8A68800_3561_11DC_863E_0015E93D1D1D__INCLUDED_
*/

#include <windows.h>

#include <string>
#include <array> //  for std::array<int, 3> inputBuff



enum Mirror {FRONT, SIDE};


// ��� ���� ��������� �� ����� ����� �� �����, � ���? ��� ��������, �� ����� ����� �� ������������ �����������.
class ARC_MONOCHROMATOR
{
public:

	int     Open(int ); // ����������� �������������.
//	void    UpdateSetting(); // ������ ����������� �������� � ������������.
	void    Close(); // ���������� �������������.

	void    SetPortNumber(int); // ��������� ������ �����.
	int     GetPortNumber(); // ���������� ������ �����.
	void    SetWavelengthStart(double);
	void    SetWavelengthStop(double);
	void    SetWavelengthStep(double); // ��������� ����� ����� ������� �������.
	double  GetWavelengthStart(); // ���������� ������ ������� ���������.
	double  GetWavelengthStop(); // ���������� ������� ������� ���������.
	double  GetWavelengthStep(); // ���������� ����� ����� ������� �������.
	int     GoToWavelength(double); // ������� �� ��������� ����� �����.
	int     CheckState(); // ���������� ��������� �������� - ��������� (1) ��� ��� (0).
	int     GetGrate();
	void    SetGrate(int);
	int     getGratesNumber();
// ���� � ����������� ������� ������ �� ������
	Mirror  getMirrorPosition();
	void    setMirrorPosition(Mirror); // 1 - FRONT, 2 - SIDE

// ARC_MONOCHROMATOR(int _comPort, double _wavelengthStep, double _wavelengthStart, double _wavelengthStop, int _gratingNumber, Mirror _mirrorPosition);
	ARC_MONOCHROMATOR(); // �����������.
	virtual ~ARC_MONOCHROMATOR(); // ����������.

// 2022
//	int     GetTimeChangePos(); // ���������� ������� �������� ��� �������� �������.


/* // 2021
// void SetLaserWavelenght(double waveLenght); // ��������� ����� ����� ������.
// double GetLaserWavelenght(); // ���������� ����� ����� ������.
// void SetSlitWidthSide(int); // ��������� ������ ������� ����.
// int GetSlitWidthSide(); // ���������� ������ ������� ����.
// float Wavelenght2Shift(double); // ����������� ����� ����� � ����������� �����.
// double Shift2Wavelenght(float); // ����������� ������������ ������ � ����� �����.
// void SetShiftMinMax(float shMin,float shMax); // ��������� ��������� ������.
// GetShiftMax(); // ���������� ������� ������� ���������.
// float GetShiftMin(); // ���������� ������ ������� ���������

*/

private:

	template<std::size_t SIZE>
	int Read(std::array<char, SIZE>&); // ������ �� ������������� ����� ������.
    void Write(std::string);

// C++11: default member initialisers
// ����� ������, ������ ����������� DMI (direct member initializers).
	int comPort = 5;// ����� ������������� COM �����.
	double wavelengthStart = 900; // ������� ������� ��������� ���� ���� � ��.
	double wavelengthStop = 1700; // ������ ������� ��������� ���� ���� � ��.
	double wavelengthStep = 5; // ���������� ����� ������� �������.
	int gratingNumber = 1; // ����� �������,  �1 - 2400 ���/��,�2 - 1200 ���/��.
	Mirror mirrorPosition = FRONT; // �������� ��������� �������. 0 - front, 1 - side.
	HANDLE hMonochrom = INVALID_HANDLE_VALUE; // ���������� ��������������� ������������� ����� 0. ��� � �64 = -1;

/*
	int comPort;// ����� ������������� COM �����.
	double wavelengthStart; // ������� ������� ��������� ���� ���� � ��.
	double wavelengthStop; // ������ ������� ��������� ���� ���� � ��.
	double wavelengthStep; // ���������� ����� ������� �������.
	int gratingNumber; // ����� �������,  �1 - 2400 ���/��,�2 - 1200 ���/��.
	Mirror mirrorPosition; // �������� ��������� �������. 0 - front, 1 - side.
	HANDLE hMonochrom = INVALID_HANDLE_VALUE; // ���������� ��������������� ������������� ����� 0. ��� � �64 = -1;
	*/



// 2022
//	int cInputBuff; // ����������-������� ��� ������ ������ � �������.
	//int timeChangePos;	// ����� �������� ������� ����� ����������� � ��.
//    static std::array <char, ARC_INPUT_BUFF_SIZE> inputBuff;
//	char *inputBuff; // ��������� �� ����� �����/������.
//	DWORD bc; // ���������� ��� ����� ���������� �������� ���� ��� �����-������.
//	void ReadComPort(void *hCounter, char *inputBuff, char INPUT_BUFF_SIZE); // ������ �����.
//	void Read(char *srData, char srDataSize);
//	void Write(char *, int); // ������ � ������������ ����� ������.

//2021
/*
// int slitWidthSide; // ������ ������� �������� ���� � ���.
// int slitWidthFront; // ������ ����������� �������� ���� � ���.
// float shiftMin; // ������� ������ ������������ ������ � �������� �����������.
// float shiftMax; //
// double laserWavelenght; // ����� �����  � ����������


*/
};

#endif // ACTON_MONOCHROMATOR
// !defined(AFX_ARC_SPECTROMETER_H__E8A68800_3561_11DC_863E_0015E93D1D1D__INCLUDED_)
