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
//	void Halt(); // останов
	void Reset(); // сброс

	unsigned int GetPortNumber(); // Считывание номера порта.
	void SetPortNumber(unsigned int); // Установить номер СОМ-порта.

	int CheckState(); // Считывание состояния счетчика, подключен (1)/отключен (0).
	int SetDiscLevel(signed int); // Установка уровня дискриминации.
	signed int GetDiscLevel(); // Считывание уровня дискриминации.
	void Close(); // Отключение счетчика.
	int Open(int); // Подключение счетчика.
	int SetSampleTime(unsigned int); // Установка времени выборки.
	unsigned int GetSampleTime(); // Считывание времени выборки.
	unsigned int GetDeadTime(); // Считывание времени "отдыха".
	unsigned int SetDeadTime(unsigned int); // Установка времени "отдыха".
	unsigned int GetCount(); // значение последнего отсчета с А
	void SetFrontSlope(Slope);
	Slope GetFrontSlope();


// delete ===========================
	void UpdateSetting(); // Запись в счетчик обновленных параметров.
	int GetCountPeriods(); // Считывание числа отсчетов.
	void SetCountPeriods(int); // Установка числа отсчетов.
	int NPcurrent(); // Считывание номера текущей позиции.
	int GetSameCount(int); // Считывание значения конкретного отсчета.
	int GetDWellTime(); // Считывание времени "отдыха".
	void SetDWellTime(int); // Установка времени "отдыха".



private:


	void Read(char *srData, char srDataSize); // Чтение массива данных из счетчика.
	void Write(void *srCommand, char srCommandSize); // Запись массива данных.
    void Write(std::string);
	template<std::size_t SIZE>
    void Read(std::array<char, SIZE>&); // Чтение из монохроматора блока данных.

	int comPort;// Номер подключаемого COM порта.
	HANDLE hCounter; // Дескриптор счетчика.
	signed int discLevel; // уровень дискриминации в милливольтах.
	unsigned int sampleTime;// время выборки, мс.
	Slope slopeMode;
	unsigned int deadTime;

// to delete: ===========================
/*
	int cInputBuff; // Переменная-счетчик для циклов работы с буфером.
	long dWellTime; // время отдыха между выборками, мс.
	int scanCycles; // от 1 до 2000. бывшая srPeriodDigit.
	char *inputBuff; // Указатель буфера ввода-вывода.
	void ReadComPort(void *comPort, char *inputBuff, char INPUT_BUFF_SIZE); // Побайтовое чтение буфера.
	char srCountString[10]; // строка для записи числа отсчетов.
*/

};
#endif
