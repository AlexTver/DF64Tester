// #include <unistd.h>
#include <errno.h>
// #include <fcntl.h>

// #include <algorithm>
// #include <iterator>

#include "df64iface.h"
#include "fn_log.h"
#include "Globals.h"

TDF64Iface::TDF64Iface() {
    isPresented = false;
    // portFD = -1;
    portFD = INVALID_HANDLE_VALUE;
    currDistance = -1;
    currTemp = -273;
    currAmp = -1;
    sLidarInfo = "";
    currState = -1; //not started
    bMustStop = true;
    memset(distance, 0x00, sizeof(distance));
}

TDF64Iface::~TDF64Iface()
{
    bMustStop = true;
    WaitReadThread();
    ClosePort();
}

void TDF64Iface::ClosePort()
{
    if (portFD != INVALID_HANDLE_VALUE) {
        LogDebug() << "Закрытие порта " << portFD;
    // if (portFD != -1) {
        CloseHandle(portFD);
        portFD = INVALID_HANDLE_VALUE;
        // close(portFD);
        // portFD = -1;
    }

}

void TDF64Iface::LidarDetect()
{
    if (currState == -1) {
        currState = 1; //TODO change to mutex lock
        isPresented = false;
        sLidarInfo = "No device detected";
        PurgeComm(portFD, PURGE_RXABORT | PURGE_RXCLEAR | PURGE_TXABORT | PURGE_TXCLEAR);
        uint8_t answ[255];
        size_t answLen;
        if (seekAnswer()) {
            //TODO заполнить информацией из COM info
            LogInfo() << "DF64 Lidar detected";
            sLidarInfo = "DF64";
            isPresented = true;
        }
        currState = -1;
    }
}

bool TDF64Iface::InitDF64(const char *dev)
{
    LogDebug() << "Open dev '" << dev << "'";
    ClosePort();
    portFD = CreateFileA(dev, GENERIC_READ | GENERIC_WRITE,
                       0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    // portFD = open(dev, O_RDWR | O_NONBLOCK);
    // if (portFD != -1) {
    if (portFD != INVALID_HANDLE_VALUE) {
        LogDebug() << "Device '" << dev << "' opened, fd ID " << portFD;
        PurgeComm(portFD, PURGE_RXABORT | PURGE_RXCLEAR | PURGE_TXABORT | PURGE_TXCLEAR);

        SetupComm(portFD,300,300);
        DCB dcb;
        GetCommState(portFD, &dcb);
        // dcb.BaudRate = CBR_115200;
        dcb.BaudRate = CBR_9600;
        dcb.ByteSize = 8;
        dcb.Parity = NOPARITY;
        dcb.StopBits = ONESTOPBIT;
        dcb.fDtrControl = DTR_CONTROL_ENABLE;
        if(SetCommState(portFD, &dcb)) {
            COMMTIMEOUTS tms;
            tms.ReadIntervalTimeout = 100;
            tms.ReadTotalTimeoutMultiplier = 0;
            tms.ReadTotalTimeoutConstant =100;
            tms.WriteTotalTimeoutMultiplier = 0;
            tms.WriteTotalTimeoutConstant = 0;
            if(SetCommTimeouts(portFD,&tms)) {
                LogInfo() << "Device '" << dev << "' opened. Detect connected lidar...";
                LidarDetect();
            } else {
                // dwError=GetLastError();
                int locErr = errno;
                LogError() << "Error setting timeouts '" << locErr << "' " << strerror(locErr);
            }
        } else {
            // dwError=GetLastError();
            int locErr = errno;
            LogError() << "Error setting speed '" << locErr << "' " << strerror(locErr);
        }
    } else {
        int locErr = errno;
        LogError() << "Error open device '" << dev << "' code :'" << locErr << "' " << strerror(locErr);
    }

    return isPresented;
}

bool TDF64Iface::IsPresented()
{
    return isPresented;
}
// ---------------------------------------------------------------------------
int TDF64Iface::GetCurrDistance()
{
    return currDistance;
}
// ---------------------------------------------------------------------------
int TDF64Iface::GetCurrAmp()
{
    return currAmp;
}
// ---------------------------------------------------------------------------
double TDF64Iface::GetCurrTemp()
{
    return currTemp;
}
// ---------------------------------------------------------------------------
DWORD WINAPI ReadCardThreadFunc() {
    DWORD dwRetCode = 0x100;

    df64.ProcessLongRead();

    ExitThread(dwRetCode);
    return 0;
}

// ---------------------------------------------------------------------------
void TDF64Iface::StartReadThread()
{
    hReadThread = CreateThread(NULL, // default security attributes
                               0, // use default stack size
                               (LPTHREAD_START_ROUTINE) ReadCardThreadFunc, // thread function
                               NULL, // no thread function argument
                               0, // use default creation flags
                               &readThreadID); // returns thread identifier
}

// ---------------------------------------------------------------------------
bool TDF64Iface::WaitReadThread() {
    bool bRet = true;
    DWORD dwRetResult;
    dwRetResult = WaitForSingleObject(hReadThread, INFINITE);
    if (dwRetResult == WAIT_OBJECT_0) {
        if (GetExitCodeThread(hReadThread, &dwRetResult)) {
            // LogString("Поток чтения закончен с результатом " + IntToHex((uint64_t)dwRetResult, 8));
            ;
        }
        else
            // LogString("GetExitCodeThread fail");
            ;
    }
    else {
        uint64_t lastErr = GetLastError();
        LogError() << "WaitForSingleObject '" << dwRetResult << "'";
        LogError() << "GetLastError '" << lastErr << "'";
        bRet = false;
    }
    return bRet;
}

// ---------------------------------------------------------------------------
void TDF64Iface::ProcessLongRead() {
    size_t i = 0; //текущая позиция в возвратном буфере
    bool prefixFind = false;
    bool fullDataFind = false;
    unsigned long r; // фактически прочитано
    size_t iToReadLen = 1; //начинаем по 1 байту
    uint8_t answ[255];
    answ[254] = 0x00;
    size_t answLen;
    uint8_t checksum = 0;
    int locDistance;
    int locAmp;
    double locTemp;
    while (!bMustStop) {
        // LogTrace() << "Step";
        if (!prefixFind && !fullDataFind)
            PurgeComm(portFD, PURGE_RXABORT | PURGE_RXCLEAR | PURGE_TXABORT | PURGE_TXCLEAR);
        ReadFile(portFD, &answ[i], iToReadLen, &r, NULL);
        answ[254] = 0x00;
        LogTrace() << (char*)(&answ[i]);
        if (r) {
            if ((!prefixFind) && (answ[i] == 0x59)) {
                prefixFind = true;
                iToReadLen = 1;
                i += r;
            } else if (prefixFind && !fullDataFind) {
                if (answ[i] == 0x59) {
                    fullDataFind = true;
                    i += r;
                    iToReadLen = 7;
                } else {
                    iToReadLen = 1;
                    i = 0;
                    prefixFind = false;
                    fullDataFind = false;
                }
            } else if (prefixFind && fullDataFind &&(r>=7)) {
                //парсим данные
                checksum = 0;
                for (size_t j = 0; j < 8; j++)
                {
                    checksum += answ[j];
                }
                if (answ[8] == (checksum % 256))
                {
                    locDistance = answ[2] + answ[3] * 256;
                    locAmp = answ[4] + answ[5] * 256;
                    locTemp = (answ[6] + answ[7] * 256) / 100.;
                    if ((locDistance != currDistance) ||
                        (abs(locAmp - currAmp) >= 100) ||
                        (abs(locTemp - currTemp) >= 0.1))
                    {
                        currDistance = locDistance;
                        currAmp = locAmp;
                        currTemp = locTemp;
                        LogDebug() << "Data changed: currDistance " << currDistance << " currAmp " << currAmp << " currTemp " << currTemp;
                        qmlLayer.dataChanged();
                    }
                    iToReadLen = 1;
                    i = 0;
                    prefixFind = false;
                    fullDataFind = false;
                    Sleep(100);
                }
            } else {
                iToReadLen = 1;
                i = 0;
                prefixFind = false;
                fullDataFind = false;
            }
        }
    }
    LogDebug() << "Poll stopped";
}

std::string TDF64Iface::GetDeviceInfo()
{
    return sLidarInfo;
}

bool TDF64Iface::ChangeState()
{
    bool bRet = false;
    if (currState == -1) {
        if (isPresented) {
            currState = 2; //start poll state
            bMustStop = false;
            StartReadThread();
            currState = 3; //poll started
        }
    } else if (currState == 3) {
        //stop poll
        bMustStop = true;
        WaitReadThread();
        currState = -1;
    }
    return bRet;
}

int TDF64Iface::GetCurrState()
{
    return currState;
}

//---------------------------------------------------------------------------
void TDF64Iface::sendtoport(const uint8_t *buf, uint16_t MesLen)
{
    bool fSuccess;
    unsigned long out;
    WriteFile(portFD, buf, MesLen, &out, NULL);
    LogTrace() << "Sent '" << out << "' bytes";
}
//---------------------------------------------------------------------------
bool TDF64Iface::seekAnswer()
{
    bool bRet;
    time_t start = time(NULL);
    size_t i = 0; //текущая позиция в возвратном буфере
    bool prefixFind = false;
    bool commandFind = false;
    unsigned long r; // фактически прочитано
    std::array<uint8_t, 3> toFind = {'y','0',':'};
    std::array<uint8_t, 2> toFindEnd = {'\r','\n'};
    uint8_t findIdx = 0;
    size_t iToReadLen = ((readBuff.size() > 64)?64:readBuff.size()); //читаем по строке
    auto itBegin = &readBuff[i];
    bool bNextRead = false;
    do
    {
        ReadFile(portFD, &readBuff[i], iToReadLen, &r, NULL);
        LogTrace() << "Read " << r << " bytes";
        //для отладки
        {
            // char MesRet[64];
            // memcpy(MesRet, &readBuff[i], 64);
            // MesRet[63] = 0x00;
            LogTrace() << &readBuff[i];
        }
        if (r >= iToReadLen) {
            do {
                //ищем сигнатуру yN:
                toFind[1] = 0x30+findIdx;
                itBegin = std::search(itBegin, std::end(readBuff), std::begin(toFind), std::end(toFind));
                if (itBegin != end(readBuff)) {
                    //найдено начало посылки.
                    auto itEnd = std::search(itBegin, std::end(readBuff), std::begin(toFindEnd), std::end(toFindEnd));
                    if (itEnd != end(readBuff)) {
                        itBegin+=3;
                        std::sscanf((char*)itBegin, "%d,%d,%d,%d,%d,%d,%d,%d",
                                    &distance[findIdx][0],
                                    &distance[findIdx][1],
                                    &distance[findIdx][2],
                                    &distance[findIdx][3],
                                    &distance[findIdx][4],
                                    &distance[findIdx][5],
                                    &distance[findIdx][6],
                                    &distance[findIdx][7]);
                        bRet = true;
                        ++findIdx;
                    } else {
                        bNextRead = true;
                    }
                } else {
                    bNextRead = true;
                }
            } while((bNextRead != true) && (findIdx < 8));
            i+=r;
            bNextRead = false;
        } else {
            Sleep(100);
        }
    // } while ((!bRet));
    } while (((time(NULL) - start) <= 5) && (findIdx < 8)); //ждем 5 сек
    // } while (findIdx < 8); //ждем 5 сек

    // *MesRetLen=i;
    return bRet;
}

