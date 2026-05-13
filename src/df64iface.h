#ifndef TDF64IFACE_H
#define TDF64IFACE_H

#include <winsock2.h>
#include <Windows.h>
#include <stdint.h>
#include <string>
#include <array>

class TDF64Iface
{
private:
    bool isPresented;
    // int portFD;
    HANDLE portFD;
    volatile int currDistance;
    volatile int currAmp;
    volatile double currTemp;
    void ClosePort();
    void LidarDetect();
    void StartReadThread();
    bool WaitReadThread();
    HANDLE hReadThread;
    DWORD readThreadID;
    void sendtoport(const uint8_t *buf, uint16_t MesLen);
    bool seekAnswer(); //заполнение массива измерений
    std::string sLidarInfo;
    volatile int currState;
    volatile bool bMustStop;
    uint16_t distance[8][8];
    std::array<uint8_t, 1024> readBuff;
public:
    TDF64Iface();
    ~TDF64Iface();
    bool InitDF64(const char *dev);
    bool IsPresented();
    int GetCurrDistance();
    int GetCurrAmp();
    double GetCurrTemp();
    void ProcessLongRead();
    std::string GetDeviceInfo();
    bool ChangeState();
    int GetCurrState();
};

DWORD WINAPI ReadCardThreadFunc();

#endif // TDF64IFACE_H
