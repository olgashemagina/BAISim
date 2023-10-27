//---------------------------------------------------------------------------
#pragma hdrstop
#include "ImportRaster.h"
#include "tifflibio.h"
#define Z_PREFIX
#include "zlib.h"
#include "png.h"
#include "tiff.h"
#include "pnglibio.h"

/* TImageDecoder-------------------------------------------------------------
    Function:  TImageDecoder::TImageDecoder(const AnsiString& FileName)
    Purpose:   Базовый конструктор для всех объектов расшифровки растровых
               изображений
    Comments:  Вызыввает виртуальный метод Init(), который может вызвать
               исключтельную ситуацию
---------------------------------------------------------------------------*/
TImageDecoder::TImageDecoder(const AnsiString& FileName)
{
    FFileName = FileName;
    FBof = false;
    FEof = false;
    FCurrentScanLine = 0;
    FNumScanLines    = 0;
    FBufferSize      = 0;
    FScanBuffer      = NULL;
    FWidth           = 0;
}
/* TImageDecoder--------------------------------------------------------------
    Function:  TImageDecoder::~TImageDecoder()
    Purpose:    Базовый деструктор для всех объектов дешифровки растровых
                данных
    Comments:   Вызывает виртуальный  метод Close()
---------------------------------------------------------------------------*/
TImageDecoder::~TImageDecoder()
{
    if (FScanBuffer != NULL)
        delete[] FScanBuffer;
}
/*TImageDecoder--------------------------------------------------------------
    Function:  TImageDecoder::NextScalLine()
    Purpose:   Заполняет буфер строки данными из текущей строки и увеличивает
               счетчик прочитанных строк на единицу. Если счечтик достиг
               значения равного чмслу строк, он перестает увеличиваться и
               выставляется флаг Eof
    Comments:  Вызывает виртуальный метод ReadScanLine
---------------------------------------------------------------------------*/
void __fastcall TImageDecoder::NextScalLine()
{
   ReadScanLine(FCurrentScanLine);
   FCurrentScanLine++;
   if (FCurrentScanLine >= FNumScanLines)
   {
       FCurrentScanLine = FNumScanLines;
       FEof = true;
   }
}
/*TImportRaster-------------------------------------------------------------
    Function: TImportRaster::TImportRaster(bool CreateSuspended)
    Purpose:  Конструктор класса TImportRaster
    Comments: Инициализирует внутренние переменные
---------------------------------------------------------------------------*/
__fastcall TImportRaster::TImportRaster()
{
     FBitmap  = NULL;
     FDecoder = NULL;
     FFileName = "";
}
/*TImportRaster-------------------------------------------------------------
    Function: TImportRaster::TImportRaster(bool CreateSuspended)
    Purpose:  Конструктор класса TImportRaster
    Comments: Инициализирует внутренние переменные
---------------------------------------------------------------------------*/
__fastcall TImportRaster::TImportRaster(const AnsiString& FileName)
{
    SetFileName(FileName);
}


/*TImportRaster-------------------------------------------------------------
    Function: TImportRaster::~TImportRaster()
    Purpose:  Деструктор класса TImportRaster
    Comments: Предназначен для удаления объекта Decoder если тот был создан
---------------------------------------------------------------------------*/
__fastcall TImportRaster::~TImportRaster()
{
   if (FDecoder != NULL)
   {
        delete FDecoder;
   }
}
/*TImportRaster-------------------------------------------------------------
    Function: TImportRaster::Execute()
    Purpose:  Главная функция нитки TImportRaster выполняет декодирование
              растрового файла и преобразование его в Bitmap
    Comments:
---------------------------------------------------------------------------*/
void __fastcall TImportRaster::Execute()
{
  // инциализируем объект FBitmap
  __try
  {
    try
    {
      FDecoder->Init();
      FBitmap->Clear();
      FBitmap->Width  = FDecoder->Width;
      FBitmap->Height = FDecoder->NumScanLines;
      FBitmap->DIBInfo.bmiHeader.biBitCount = 8*FDecoder->ScanLineSize/FDecoder->Width;
      FBitmap->OpenPixels();
      while(!FDecoder->Eof)
      {
           FDecoder->NextScalLine();
           // DONE: copy pixels from decoder to bitmap
           // как нам известно декодер всегда поставлет данные в формате RGB
           // скопируем их в строчку FBitmap
           unsigned char* dst = (unsigned char*)FBitmap->ScanLine[FDecoder->CurrentScanLine-1];
           unsigned char* src = FDecoder->ScanLine;
           memcpy(dst, src, FDecoder->ScanLineSize);
      }
      FBitmap->ClosePixels();
      if (FDecoder != NULL)
      {
          try
          {
            FDecoder->Close();
          }
          catch(...)
          {
                //
                FBitmap->Clear();
          }
          delete FDecoder;
          FDecoder = NULL;
      }
    }
    catch(...)
    {
        if (FDecoder != NULL)
        {
            try
            {
                FDecoder->Close();
            }
            catch(...)
            {
                FBitmap->Clear();
            }
            delete FDecoder;
            FDecoder = NULL;
        }
        throw;
    }
  }
  __finally
  {
    if (FDecoder != NULL)
    {
      delete FDecoder;
      FDecoder = NULL;
    }
  }
}
/*TImportRaster--------------------------------------------------------------
    Function: TImportRaster::UpdateUI()
    Purpose:  Предназначен для вызова функции обновления пользовательского
              интерфейса во время загрузки растровых данных
    Comments:
---------------------------------------------------------------------------*/
void __fastcall TImportRaster::UpdateUI()
{
  // DONE: implement TImportRaster::UpdateUI()
  if (FBitmap != NULL && FBitmap->OnProgress)
  {
    Byte PercentDone = 100*FDecoder->CurrentScanLine/FDecoder->NumScanLines;
    bool Redraw = PercentDone %10 == 0?true:false; // call redraw each 10 %
    TRect R; R.left = 0; R.top = 0;
    R.Right = FDecoder->Width; R.Bottom = FDecoder->CurrentScanLine;
    FBitmap->OnProgress(FBitmap, psRunning, PercentDone, Redraw, R, "Loading");
  }
}
/*TImportRaster-------------------------------------------------------------
    Function: TImportRaster::SetFileName(AnsiString& FileName)
    Purpose:  Устанавливает имя файла для импорта Создает и инициализирует
              декодер растровых данных, основываея на расширении файла
    Comments: Установка этой переменной недопустима во время выполнения
              нитки
---------------------------------------------------------------------------*/
void __fastcall TImportRaster::SetFileName(AnsiString FileName)
{
  try
  {
        AnsiString strExt = ExtractFileExt(FileName);
        strExt = strExt.UpperCase();
       if (strExt == ".TIFF" || strExt == ".TIF")
            FDecoder = new TTiffDecoder(FileName);
        else if (strExt == ".PNG")
            FDecoder = new TPngDecoder(FileName);
        else  //todo: add another file formats
            throw   Exception("Invalid file format");
  }
  catch(const Exception& e)// <-- catch exeptions
  {
     // освободим декодер если необходимо
     if (FDecoder != NULL)
        delete FDecoder;
     FFileName = "";
     throw;
  }
  catch(...)
  {
     // освободим декодер если необходимо
     if (FDecoder != NULL)
        delete FDecoder;
     FFileName = "";
     throw;
  }
}
/*TImportRaster--------------------------------------------------------------
    Function: TImportRaster::SetBitmap(Graphics::TBitmap* ABitmap)
    Purpose:  Устанавливает указатель на Bitmap в который будет импортироваться
              изображение
    Comments: Установка этой переменной недопустима во время выполнения
              нитки
---------------------------------------------------------------------------*/
void __fastcall TImportRaster::SetBitmap(TDIBImage* ABitmap)
{
    if (FBitmap != ABitmap)
        FBitmap = ABitmap;
}
#pragma package(smart_init)
