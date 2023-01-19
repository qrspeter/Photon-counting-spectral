#ifndef ARDUINO_SYNC_H_INCLUDED
#define ARDUINO_SYNC_H_INCLUDED



enum laser{
    LASER_OFF, LASER_ON};


class ARDUINO_SYNC
{
public:
    ARDUINO_SYNC();
    ~ARDUINO_SYNC();

    int  Open(int);
    void Close();

    void Laser(laser); // Включает или выключает лазер/светодиод
    int GetSleeping();
    void SetSleeping(int);

	int GetPortNumber(); // Считывание номера порта.
//	void SetPortNumber(int); // Установить номер СОМ-порта.
    int CheckState(); // Считывание состояния счетчика, подключен (1)/отключен (0).
//    int Reset();

private:
    int laser_sleeping = 100; // msec
    int ttl_output = 13;
	int com_port = 6;// Номер подключаемого COM порта.

	HANDLE hSerial = INVALID_HANDLE_VALUE;
    const int INPUT_BUFF_SIZE = 16;

    DWORD bc;

    const char OFF = '\x00';
    const char ON = '\x01';




};


#endif // ARDUINO_SYNC_H_INCLUDED
