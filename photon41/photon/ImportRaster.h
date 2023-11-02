//---------------------------------------------------------------------------
#ifndef ImportRasterH
#define ImportRasterH
#include "DIBImage41.h"
//TImageDecoder class----------------------------
//абстракный базовый класс предназначенный для
//определения интерфейса объекта расшифровки растровых данных

class TImageDecoder
{
protected:
    unsigned char* FScanBuffer;     //Буфер, в котором содержится последняя прочитанная строка изображения
    int            FBufferSize;     //размер буфера строки
    int            FNumScanLines;   //число строк для чтения
    int            FCurrentScanLine;// номер строчки, которая будет читаться следующей.
    bool           FBof;            // флаг начала файла
    bool           FEof;            // флаг конца файла
    AnsiString     FFileName;       // имя файла для расшифровки
    int            FWidth;          // ширина изображения в пикселях
protected:
    virtual void __fastcall ReadScanLine(int ScanLine) = 0;
public:
    TImageDecoder(const AnsiString& FileName);
    ~TImageDecoder();

    virtual void __fastcall Init() =0;
    virtual void __fastcall Close()=0;

    void __fastcall NextScalLine();

    __property unsigned char* ScanLine = {read = FScanBuffer};
    __property int ScanLineSize = {read = FBufferSize};
    __property int NumScanLines = {read = FNumScanLines};
    __property int CurrentScanLine = {read = FCurrentScanLine};
    __property bool Bof = {read = FBof};
    __property bool Eof = {read = FEof};
    __property int  Width = {read = FWidth}; 
};


//TImportRaster class----------------------------
//класс расширяет TThred для чтения растровых данных
//для работы использует интерфейс TImageDecoder
//Правильный способ организации нитки - переопределение конструктора
//todo: Определить два конструктора для этого класса 1. с переменной TFImage 2. c переменной TBitmap
class TImportRaster /*: public TThread*/
{
friend class TFImage;
private:
    TDIBImage*       FBitmap;  //место, куда будут распаковываться растровые данные
    TImageDecoder*  FDecoder; //декодер растровых данных
    AnsiString      FFileName;//имя файла с растровыми данными
    void __fastcall SetFileName(AnsiString FileName);
    void __fastcall SetBitmap(TDIBImage* ABitmap);
public:
   __fastcall TImportRaster();
   __fastcall TImportRaster(const AnsiString& FileName);
   __fastcall ~TImportRaster();
   void __fastcall UpdateUI();
   virtual void __fastcall Execute();
   //properties
   __property TDIBImage*         Bitmap = {read = FBitmap, write = SetBitmap};
   __property AnsiString         FileName = {read = FFileName, write = SetFileName};
};
//---------------------------------------------------------------------------
#endif
