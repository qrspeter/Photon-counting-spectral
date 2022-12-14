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


// или надо указывать не число точек на спетр, а шаг? Это логичнее, тк видно связь со спектральным разрешением.
class ARC_MONOCHROMATOR
{
public:

	int     Open(int ); // Подключение монохроматора.
//	void    UpdateSetting(); // Запись обновленных настроек в монохроматор.
	void    Close(); // Отключение монохроматора.

	void    SetPortNumber(int); // Установка номера порта.
	int     GetPortNumber(); // Считывание номера порта.
	void    SetWavelengthStart(double);
	void    SetWavelengthStop(double);
	void    SetWavelengthStep(double); // Установка числа точек отсчета спектра.
	double  GetWavelengthStart(); // Считывание нижней границы диапазона.
	double  GetWavelengthStop(); // Считывание верхней границы диапазона.
	double  GetWavelengthStep(); // Считывание числа точек отсчета спектра.
	int     GoToWavelength(double); // Переход на указанную длину волны.
	int     CheckState(); // Считывание состояния счетчика - подключен (1) или нет (0).
	int     GetGrate();
	void    SetGrate(int);
	int     getGratesNumber();
// пока с управлением зеркала ничего не делаем
	Mirror  getMirrorPosition();
	void    setMirrorPosition(Mirror); // 1 - FRONT, 2 - SIDE

// ARC_MONOCHROMATOR(int _comPort, double _wavelengthStep, double _wavelengthStart, double _wavelengthStop, int _gratingNumber, Mirror _mirrorPosition);
	ARC_MONOCHROMATOR(); // конструктор.
	virtual ~ARC_MONOCHROMATOR(); // деструктор.

// 2022
//	int     GetTimeChangePos(); // Считывание времени перехода при повороте зеркала.


/* // 2021
// void SetLaserWavelenght(double waveLenght); // Установка длины волны лазера.
// double GetLaserWavelenght(); // Считывание длины волны лазера.
// void SetSlitWidthSide(int); // Установка ширины боковой щели.
// int GetSlitWidthSide(); // Считывание ширины боковой щели.
// float Wavelenght2Shift(double); // Конвертация длины волны в рамановский сдвиг.
// double Shift2Wavelenght(float); // Конвертация рамановского сдвига в длину волны.
// void SetShiftMinMax(float shMin,float shMax); // Установка диапазона сдвига.
// GetShiftMax(); // Считывание верхней границы диапазона.
// float GetShiftMin(); // Считывание нижней границы диапазона

*/

private:

	template<std::size_t SIZE>
	int Read(std::array<char, SIZE>&); // Чтение из монохроматора блока данных.
    void Write(std::string);

// C++11: default member initialisers
// когда можете, всегда используйте DMI (direct member initializers).
	int comPort = 5;// Номер подключаемого COM порта.
	double wavelengthStart = 900; // Верхняя граница диапазона длин волн в нм.
	double wavelengthStop = 1700; // Нижняя граница диапазона длин волн в нм.
	double wavelengthStep = 5; // количество точек отсчета спектра.
	int gratingNumber = 1; // номер решетки,  №1 - 2400 штр/мм,№2 - 1200 штр/мм.
	Mirror mirrorPosition = FRONT; // Положние выходного зеркала. 0 - front, 1 - side.
	HANDLE hMonochrom = INVALID_HANDLE_VALUE; // Дескриптор неподключенного монохроматора равен 0. или в х64 = -1;

/*
	int comPort;// Номер подключаемого COM порта.
	double wavelengthStart; // Верхняя граница диапазона длин волн в нм.
	double wavelengthStop; // Нижняя граница диапазона длин волн в нм.
	double wavelengthStep; // количество точек отсчета спектра.
	int gratingNumber; // номер решетки,  №1 - 2400 штр/мм,№2 - 1200 штр/мм.
	Mirror mirrorPosition; // Положние выходного зеркала. 0 - front, 1 - side.
	HANDLE hMonochrom = INVALID_HANDLE_VALUE; // Дескриптор неподключенного монохроматора равен 0. или в х64 = -1;
	*/



// 2022
//	int cInputBuff; // переменная-счетчик для циклов работы с буфером.
	//int timeChangePos;	// время поворота решетки между положениями в мс.
//    static std::array <char, ARC_INPUT_BUFF_SIZE> inputBuff;
//	char *inputBuff; // указатель на буфер ввода/вывода.
//	DWORD bc; // переменная для числа фактически принятых байт при вводе-выводе.
//	void ReadComPort(void *hCounter, char *inputBuff, char INPUT_BUFF_SIZE); // чтение порта.
//	void Read(char *srData, char srDataSize);
//	void Write(char *, int); // Запись в монохроматор блока данных.

//2021
/*
// int slitWidthSide; // Ширина боковой выходной щели в мкм.
// int slitWidthFront; // Ширина фронтальной выходной щели в мкм.
// float shiftMin; // Пределы границ рамановского сдвига в обратных сантиметрах.
// float shiftMax; //
// double laserWavelenght; // длина волны  в нанометрах


*/
};

#endif // ACTON_MONOCHROMATOR
// !defined(AFX_ARC_SPECTROMETER_H__E8A68800_3561_11DC_863E_0015E93D1D1D__INCLUDED_)
