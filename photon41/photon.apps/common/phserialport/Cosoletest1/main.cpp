#include <vcl.h>
#include <windows.h>

#pragma hdrstop
#pragma argsused

#include <tchar.h>
#include <stdio.h>
#include "../phserialport.h"

extern "C"
{
	#pragma link "../phserialport.lib"
}


int _tmain(int argc, _TCHAR* argv[])
{
	int c = 0;
	SetConsoleOutputCP(1251);

	phserialPortInit();
	phserialPortCount(c);
	printf("Количество коммуникационных портов: %i \n", c);
	for (int i = 0; i < c; i++)
	{
		char buf[PHSERIAL_BUF];
        printf("-------------------------------------------\n");
		printf("Имя порта: %s\n", phserialPortName(buf, i));
		printf("Системное расположение: %s\n", phserialPortLocation(buf, i));
		printf("Описание: %s\n", phserialPortDescription(buf, i));
		printf("Производитель: %s\n", phserialPortManufacturer(buf, i));
		printf("Серийный номер: %s\n", phserialPortSerialNumber(buf, i));
		bool value = false;
		phserialPortHasVendorIdentifier(value, i);
		printf("Имеется идентификатор поставщика: %s\n", value?"да":"нет");
		unsigned short value1;
		if (value) {

			phserialPortVendorIdentifier(value1, i);
			printf("Идентификатор поставщика: %hu \n", value1);
		}
		phserialPortHasProductIdentifier(value, i);
		printf("Имеется идентификатор продукта: %s\n", value?"да":"нет");
		if (value) {

			phserialPortProductIdentifier(value1, i);
			printf("Идентификатор продукта: %hu \n", value1);
		}

	}
	phserialPortFree();
	return 0;
}
