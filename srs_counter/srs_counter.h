//SR400class.h ����� ������ �� ��������� RS400 ����� ���������������� ����.

#ifndef __SRS_COUNTER_H
#define __SRS_COUNTER_H

#include <array> //  for std::array<int, 3> inputBuff

enum Slope {RISE = 0, FALL = 1};

class SR_COUNTER
{
public:
	SR_COUNTER(); // �����������.
	~SR_COUNTER(); // ����������.
	void Start(); // �����
	void Reset(); // �����

	int GetPortNumber(); // ���������� ������ �����.
	void SetPortNumber(int); // ���������� ����� ���-�����.

	int CheckState(); // ���������� ��������� ��������, ��������� (1)/�������� (0).
	int SetDiscLevel(int); // ��������� ������ �������������.
	int GetDiscLevel(); // ���������� ������ �������������.
	void Close(); // ���������� ��������.
	int Open(int); // ����������� ��������.
	int SetSampleTime(int); // ��������� ������� �������.
	int GetSampleTime(); // ���������� ������� �������.
	int GetDeadTime(); // ���������� ������� "������".
	int SetDeadTime(int); // ��������� ������� "������".
	int GetCount(); // �������� ���������� ������� � �
	void SetFrontSlope(Slope);
	Slope GetFrontSlope();




private:


	void Read(char *srData, char srDataSize); // ������ ������� ������ �� ��������.
	void Write(void *srCommand, char srCommandSize); // ������ ������� ������.
    void Write(std::string);
	template<std::size_t SIZE>
    void Read(std::array<char, SIZE>&); // ������ �� ������������� ����� ������.

	// ������������ ���������� ��-���������.
    int comPort = 2;// ����� ������������� COM �����.
	int discLevel = -75; // ������� ������������� � ������������.
	int sampleTime = 500;// ����� �������, ��.
	Slope slopeMode = FALL;
	int deadTime = 0;
	HANDLE hCounter = INVALID_HANDLE_VALUE; // ���������� ��������.

//	int comPort;// ����� ������������� COM �����.
//	HANDLE hCounter; // ���������� ��������.
//	signed int discLevel; // ������� ������������� � ������������.
//	unsigned int sampleTime;// ����� �������, ��.
//	Slope slopeMode;
//	unsigned int deadTime;

// to delete: ===========================
/*

// 2022
// delete ===========================
//	void Halt(); // �������
	void UpdateSetting(); // ������ � ������� ����������� ����������.
	int GetCountPeriods(); // ���������� ����� ��������.
	void SetCountPeriods(int); // ��������� ����� ��������.
	int NPcurrent(); // ���������� ������ ������� �������.
	int GetSameCount(int); // ���������� �������� ����������� �������.
	int GetDWellTime(); // ���������� ������� "������".
	void SetDWellTime(int); // ��������� ������� "������".



	int cInputBuff; // ����������-������� ��� ������ ������ � �������.
	long dWellTime; // ����� ������ ����� ���������, ��.
	int scanCycles; // �� 1 �� 2000. ������ srPeriodDigit.
	char *inputBuff; // ��������� ������ �����-������.
	void ReadComPort(void *comPort, char *inputBuff, char INPUT_BUFF_SIZE); // ���������� ������ ������.
	char srCountString[10]; // ������ ��� ������ ����� ��������.
*/

};
#endif
