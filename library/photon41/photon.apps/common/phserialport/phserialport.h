/*
    c -api for qt serial port engine
*/
#ifndef PHSERIALPORT_H
#define PHSERIALPORT_H

#define PHSERIAL_BUF 2048
typedef void* HANDLE;

#if defined(PHSERIALPORT_LIBRARY)
#  define PHSERIALPORT_API  __declspec(dllexport)
#else
#  define PHSERIALPORT_API  __declspec(dllimport)
#endif

extern "C"
{
// serial port enumerator
void PHSERIALPORT_API phserialPortInit();
void PHSERIALPORT_API phserialPortFree();

void PHSERIALPORT_API phserialPortCount(int& count);
PHSERIALPORT_API const char* phserialPortName(char* buf, int index);
PHSERIALPORT_API const char* phserialPortLocation(char* buf, int index);
PHSERIALPORT_API const char* phserialPortDescription(char* buf, int index);
PHSERIALPORT_API const char* phserialPortManufacturer(char* buf, int index);
PHSERIALPORT_API const char* phserialPortSerialNumber(char* buf, int index);
PHSERIALPORT_API void phserialPortHasVendorIdentifier(bool& value, int index);
PHSERIALPORT_API void phserialPortHasProductIdentifier(bool& value, int index);
PHSERIALPORT_API void phserialPortVendorIdentifier(unsigned short& value, int index);
PHSERIALPORT_API void phserialPortProductIdentifier(unsigned short& value, int index);
}
#endif // PHSERIALPORT_H
