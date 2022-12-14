//SR400class.h Класс работы со счетчиком RS400 через последовательный порт.

#ifndef __SRS_COUNTER_H
#define __SRS_COUNTER_H

#include <array> //  for std::array<int, 3> inputBuff

enum Slope {RISE = 0, FALL = 1};

class SR_COUNTER
{
public:
	SR_COUNTER(); // конструктор.
	~SR_COUNTER(); // деструктор.
	void Start(); // старт
	void Reset(); // сброс

	int GetPortNumber(); // Считывание номера порта.
	void SetPortNumber(int); // Установить номер СОМ-порта.

	int CheckState(); // Считывание состояния счетчика, подключен (1)/отключен (0).
	int SetDiscLevel(int); // Установка уровня дискриминации.
	int GetDiscLevel(); // Считывание уровня дискриминации.
	void Close(); // Отключение счетчика.
	int Open(int); // Подключение счетчика.
	int SetSampleTime(int); // Установка времени выборки.
	int GetSampleTime(); // Считывание времени выборки.
	int GetDeadTime(); // Считывание времени "отдыха".
	int SetDeadTime(int); // Установка времени "отдыха".
	int GetCount(); // значение последнего отсчета с А
	void SetFrontSlope(Slope);
	Slope GetFrontSlope();




private:


	void Read(char *srData, char srDataSize); // Чтение массива данных из счетчика.
	void Write(void *srCommand, char srCommandSize); // Запись массива данных.
    void Write(std::string);
	template<std::size_t SIZE>
    void Read(std::array<char, SIZE>&); // Чтение из монохроматора блока данных.

	// Инициализаия параметров по-умолчанию.
    int comPort = 2;// Номер подключаемого COM порта.
	int discLevel = -75; // уровень дискриминации в милливольтах.
	int sampleTime = 500;// время выборки, мс.
	Slope slopeMode = FALL;
	int deadTime = 0;
	HANDLE hCounter = INVALID_HANDLE_VALUE; // Дескриптор счетчика.

//	int comPort;// Номер подключаемого COM порта.
//	HANDLE hCounter; // Дескриптор счетчика.
//	signed int discLevel; // уровень дискриминации в милливольтах.
//	unsigned int sampleTime;// время выборки, мс.
//	Slope slopeMode;
//	unsigned int deadTime;

// to delete: ===========================
/*

// 2022
// delete ===========================
//	void Halt(); // останов
	void UpdateSetting(); // Запись в счетчик обновленных параметров.
	int GetCountPeriods(); // Считывание числа отсчетов.
	void SetCountPeriods(int); // Установка числа отсчетов.
	int NPcurrent(); // Считывание номера текущей позиции.
	int GetSameCount(int); // Считывание значения конкретного отсчета.
	int GetDWellTime(); // Считывание времени "отдыха".
	void SetDWellTime(int); // Установка времени "отдыха".



	int cInputBuff; // Переменная-счетчик для циклов работы с буфером.
	long dWellTime; // время отдыха между выборками, мс.
	int scanCycles; // от 1 до 2000. бывшая srPeriodDigit.
	char *inputBuff; // Указатель буфера ввода-вывода.
	void ReadComPort(void *comPort, char *inputBuff, char INPUT_BUFF_SIZE); // Побайтовое чтение буфера.
	char srCountString[10]; // строка для записи числа отсчетов.
*/

};
#endif
