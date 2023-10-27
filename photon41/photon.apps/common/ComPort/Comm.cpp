//---------------------------------------------------------------------------
//  Victor Chen
//  victorch@163.net

#include <vcl.h>
#pragma hdrstop

#include "Comm.h"
#pragma package(smart_init)
//---------------------------------------------------------------------------
// ValidCtrCheck is used to assure that the components created do not have
// any pure virtual functions.
//

//////////// begin internal objects define ///////////////////////////////
DWORD CommEventList[]={EV_BREAK,
      EV_CTS,
      EV_DSR,
      EV_ERR,
      EV_RING,
      EV_RLSD,
      EV_RXCHAR,
      EV_RXFLAG,
      EV_TXEMPTY};

DWORD    CommBaudRates[] =
         {
            CBR_110,CBR_300 ,  CBR_600  , CBR_1200 , CBR_2400 ,
            CBR_4800 , CBR_9600 , CBR_14400, CBR_19200,
            CBR_38400, CBR_56000, CBR_57600, CBR_115200,
            CBR_128000,CBR_256000
         } ;
BYTE    CommParity[]={ NOPARITY, EVENPARITY, ODDPARITY, MARKPARITY, SPACEPARITY } ;
BYTE    CommStopBits[]={ ONESTOPBIT, ONE5STOPBITS, TWOSTOPBITS } ;
BYTE    CommDataBits[]={  4, 5, 6, 7, 8 } ;

void RaiseCommError(String Msg, int ErrCode)
{
    throw Exception(Msg + Format(sMsgExtention, ARRAYOFCONST(((int)ErrCode))));
}

__fastcall TCommEventThread::TCommEventThread(HANDLE Handle,TCommEventTypes MonitorEvents,DWORD SuspendTime):TThread(true)
{
    int i ;
    DWORD AttrWord ;
    FreeOnTerminate = True;
    FCommHandle = Handle;
    FSuspendTime = SuspendTime;
    AttrWord=0;
    for (i=0;i<9;i++)
    {
        if (MonitorEvents.Contains(TCommEventType(i))) AttrWord=AttrWord+CommEventList[i];
    }
    SetCommMask(FCommHandle, AttrWord);
}

void __fastcall TCommEventThread::Execute(void)
{
  OVERLAPPED os ;
  bool WaitEventResult;

  memset( &os, 0, sizeof( OVERLAPPED ) ) ;

  os.hEvent = CreateEvent( NULL,TRUE,FALSE,NULL ) ;
  while (!Terminated)
  {
    WaitEventResult=WaitCommEvent( FCommHandle, &FEventMask, &os );
    if (GetLastError() == ERROR_IO_PENDING)
      WaitEventResult=(WaitForSingleObject(os.hEvent,INFINITE)==WAIT_OBJECT_0);
    if (WaitEventResult>0)
    {
      Synchronize(DoOnSignal);
      ResetEvent(os.hEvent) ;
    }
  }
  CloseHandle(os.hEvent) ;
  PurgeComm(FCommHandle, PurgeReadWrite);
}

void __fastcall TCommEventThread::DoOnSignal(void)
{
    if (FOnSignal) FOnSignal(this, FEventMask);
}

__fastcall TCommEventChars::TCommEventChars(TThreadComm *Owner): TPersistent()
{
  FOwner = Owner;
  FXOnChar = 17;
  FXOffChar = 19;
  FErrorChar = 0;
  FEofChar = 0;
  FEvtChar = 0;
}

void __fastcall TCommEventChars::SetEventChar(int Index, char Value)
{
//  if (!FOwner->Connected)
  switch (Index)
  {
    case 1: FXOnChar = Value;      break;
    case 2: FXOffChar = Value;     break;
    case 3: FErrorChar = Value;    break;
    case 4: FEofChar = Value;      break;
    case 5: FEvtChar = Value;      break;
  }
  if (FOwner!=NULL)
    FOwner->UpdateDataControlBlock();
}

void __fastcall TCommEventChars::Assign(TPersistent *Source)
{
    TCommEventChars *TempControl ;

    TempControl = dynamic_cast<TCommEventChars *>(Source) ;
    if (TempControl != NULL)
    {
        FXOnChar = TempControl->FXOnChar;
        FXOffChar = TempControl->FXOffChar;
        FErrorChar = TempControl->FErrorChar;
        FEofChar = TempControl->FEofChar;
        FEvtChar = TempControl->FEvtChar;
    }
    else TPersistent::Assign(Source);
}


//////////// end internal objects define  ///////////////////////////////

static inline void ValidCtrCheck(TThreadComm *)
{
    new TThreadComm(NULL);
}
//---------------------------------------------------------------------------
__fastcall TThreadComm::TThreadComm(TComponent* Owner)
    : TComponent(Owner)
{
  FHandle = INVALID_HANDLE_VALUE;
  FDeviceName = "Com2";
  FReadTimeout = 1000;
  FWriteTimeout = 1000;
  FReadBufSize = 4096;
  FWriteBufSize = 2048;
  FSuspendTime = 5 ;
  FMonitorEvents <<evBreak<<evCts<<evDsr<<evError<<evRing ;
  FMonitorEvents <<evRlsd<<evRxChar<<evRxFlag<<evTxEmpty;
  FBaudRate = br9600;
  FParity = paNone;
  FStopBits = sb10;
  FDataBits = da8;
  FConnected = false ;
    WRITE_OS.Offset = 0 ;
    WRITE_OS.OffsetHigh = 0 ;
    WRITE_OS.hEvent = CreateEvent( NULL,TRUE,FALSE,NULL ) ;
    READ_OS.Offset = 0 ;
    READ_OS.OffsetHigh = 0 ;
    READ_OS.hEvent = CreateEvent( NULL,TRUE,FALSE,NULL ) ;
//  FOptions = [];
  FFlowControl = fcDefault;
  FEventChars = new TCommEventChars(this);
  FEvent = new TSimpleEvent(false);
  FCriticalSection = new TCriticalSection();
}

__fastcall TThreadComm::~TThreadComm(void)
{
  Close();
  FEventChars->Free();
  FEvent->Free();
  FCriticalSection->Free();
  CloseHandle(WRITE_OS.hEvent) ;
  CloseHandle(READ_OS.hEvent) ;
}

void __fastcall TThreadComm::Lock()
{
  FCriticalSection->Enter();
}

void __fastcall TThreadComm::Unlock()
{
  FCriticalSection->Leave();
}

bool __fastcall TThreadComm::Enabled()
{
  return  (FConnected);
}

void __fastcall TThreadComm::CheckOpen()
{
  if (Enabled()) RaiseCommError(sPortAlreadyOpen, -1);
}

void __fastcall TThreadComm::CreateHandle()
{
    if ((FHandle = CreateFile(FDeviceName.c_str(),
            GENERIC_READ | GENERIC_WRITE, 0, NULL,OPEN_EXISTING,
            FILE_ATTRIBUTE_NORMAL|FILE_FLAG_OVERLAPPED, NULL))==(HANDLE)-1)
        FConnected = false ;
    else
    {
        FConnected = true ;
        if (GetFileType(FHandle) != FILE_TYPE_CHAR)
        {
            DestroyHandle();
        } else SetCommMask(FHandle, 511);
    }
}

void __fastcall TThreadComm::DestroyHandle()
{
  CloseHandle(FHandle);
  FConnected = false ;
  FHandle = INVALID_HANDLE_VALUE;
}

void __fastcall TThreadComm::Open()
{
    int Error_Code =0 ;
    String ErrorString ;
    Lock();
    if (!Enabled())
    {
        CreateHandle();
        if (Enabled())
        {
                UpdateCommTimeouts();
                UpdateDataControlBlock();
                if (!SetupComm(FHandle, FReadBufSize, FWriteBufSize))
                {
                    Error_Code = GetLastError() ;
                    ErrorString = sSetupCommErr ;
                }
                FEventThread = new TCommEventThread(FHandle,FMonitorEvents,FSuspendTime) ;
                FEventThread->OnSignal = HandleCommEvent;
                FEventThread->Resume() ;
            if (FEventThread==NULL)
                DestroyHandle() ;
        }
    }
    else
    {
                Error_Code = -1 ;
                ErrorString = sPortAlreadyOpen ;
    }
    Unlock();
    if (Error_Code!=0) RaiseCommError(ErrorString, Error_Code);
}

void __fastcall TThreadComm::Close()
{
    if (Enabled())
    {
        Lock();
        try
        {
            FEventThread->OnSignal = NULL ;
            TerminateThread((void*)FEventThread->Handle,0);
            DestroyHandle();
        } catch (...) { Unlock(); }
        Unlock();
    }
}

int __fastcall TThreadComm::Write(char Buf[], DWORD Count)
{
   BOOL        fWriteStat ;
   DWORD       dwBytesWritten ;
   DWORD       dwErrorFlags;
   DWORD   	   dwError;
   DWORD       dwBytesSent=0;
   COMSTAT     ComStat;

  if (!Enabled()) return -1;
  Lock();
  try
  {
   fWriteStat = WriteFile(FHandle, Buf, Count,
                           &dwBytesWritten, &WRITE_OS ) ;
   if (!fWriteStat)
   {
      if(GetLastError() == ERROR_IO_PENDING)
      {
         while(!GetOverlappedResult(FHandle,&WRITE_OS, &dwBytesWritten, FALSE ))
         {
            dwError = GetLastError();
            if(dwError == ERROR_IO_INCOMPLETE)
            {
               dwBytesSent += dwBytesWritten;
               continue;
            }
            else
            {
               ClearCommError(FHandle, &dwErrorFlags, &ComStat ) ;
               break;
            }
         }
         dwBytesSent += dwBytesWritten;
      }
      else
      {
         ClearCommError(FHandle, &dwErrorFlags, &ComStat ) ;
      }
   }
   else dwBytesSent += dwBytesWritten;
  }
  catch(...) { Unlock(); }
  Unlock();
  return dwBytesSent ;
}

int __fastcall TThreadComm::Read(char Buf[], DWORD Count)
{
   BOOL       fReadStat ;
   COMSTAT    ComStat ;
   DWORD      dwErrorFlags;
   DWORD      dwLength ;
   DWORD      dwBytesReceive=0;
   DWORD      dwError;

    if (!Enabled()) return -1;
    Lock();
    try
    {
        ClearCommError( FHandle, &dwErrorFlags, &ComStat ) ;
        dwLength =  Count ;

        if (dwLength > 0)
        {
            fReadStat = ReadFile( FHandle, Buf,
		                    dwLength, &dwLength, &READ_OS ) ;
            if (!fReadStat)
            {
                if (GetLastError() == ERROR_IO_PENDING)
                {
                    while(!GetOverlappedResult(FHandle,
                        &READ_OS , &dwLength, TRUE ))
                    {
                        dwError = GetLastError();
                        if(dwError == ERROR_IO_INCOMPLETE)
                        {
                            dwBytesReceive += dwLength;
                            continue;
                        }
                        else
                        {
                            ClearCommError(FHandle, &dwErrorFlags, &ComStat ) ;
                        }
                        break;
                    }
                    dwBytesReceive += dwLength;
               }
            }
            else dwBytesReceive += dwLength;
        }
        else
        {
            ClearCommError(FHandle, &dwErrorFlags, &ComStat ) ;
        }
    }
    catch(...) { Unlock(); }
    Unlock();
    return dwBytesReceive ;
}

int __fastcall TThreadComm::InQueCount()
{
  COMSTAT ComStat;
  DWORD   Errors;
  int Result ;

  if (Enabled())
  {
    ClearCommError(FHandle, &Errors, &ComStat);
    Result = ComStat.cbInQue;
  } else Result = -1;
  return Result ;
}

int __fastcall TThreadComm::OutQueCount()
{
  COMSTAT ComStat;
  DWORD   Errors;
  int Result ;

  if (Enabled())
  {
    ClearCommError(FHandle, &Errors, &ComStat);
    Result = ComStat.cbOutQue;
  } else Result = -1;
  return Result ;
}

void __fastcall TThreadComm::PurgeIn()
{
  if (Enabled())
    PurgeComm(FHandle, PurgeRead);
}

void __fastcall TThreadComm::PurgeOut()
{
  if (Enabled())
    PurgeComm(FHandle, PurgeWrite);
}

void __fastcall TThreadComm::SetDeviceName(String Value)
{
  if ((!Enabled())&&(FDeviceName!=Value))
  {
//    CheckOpen();
    FDeviceName = Value;
  }
}

void __fastcall TThreadComm::SetMonitorEvents(TCommEventTypes Value)
{
    if ((!Enabled())&&(FMonitorEvents != Value))
    {
//    CheckOpen();
        FMonitorEvents = Value;
    }
}

void __fastcall TThreadComm::SetReadBufSize(WORD Value)
{
  if ((!Enabled())&&(FReadBufSize != Value))
  {
//    CheckOpen();
    FReadBufSize = Value;
  }
}

void __fastcall TThreadComm::SetWriteBufSize(WORD Value)
{
  if ((!Enabled())&&(FWriteBufSize != Value))
  {
//    CheckOpen();
    FWriteBufSize = Value;
  }
}

void __fastcall TThreadComm::SetSuspendTime(DWORD Value)
{
  if ((!Enabled())&&(FSuspendTime != Value))
  {
    FSuspendTime = Value;
  }
}

void __fastcall TThreadComm::SetBaudRate(TBaudRate Value)
{
  if (FBaudRate != Value)
  {
    FBaudRate = Value;
    UpdateDataControlBlockLock();
  }
}

void __fastcall TThreadComm::SetParity(TParity Value)
{
  if (FParity != Value)
  {
    FParity = Value;
    UpdateDataControlBlockLock();
  }
}

void __fastcall TThreadComm::SetStopBits(TStopBits Value)
{
  if (FStopBits != Value)
  {
    FStopBits = Value;
    UpdateDataControlBlockLock();
  }
}

void __fastcall TThreadComm::SetDataBits(TDataBits Value)
{
  if (FDataBits != Value)
  {
    FDataBits=Value;
    UpdateDataControlBlockLock();
  }
}

void __fastcall TThreadComm::SetOptions(TCommOptions Value)
{
  if (FOptions != Value)
  {
    FOptions = Value;
    UpdateDataControlBlockLock();
  }
}

void __fastcall TThreadComm::SetFlowControl(TFlowControl Value)
{
  if (FFlowControl != Value)
  {
    FFlowControl = Value;
    UpdateDataControlBlockLock();
  }
}

void __fastcall TThreadComm::HandleCommEvent(TObject *Sender, DWORD Status)
{
  COMSTAT ComStat;
  DWORD   Errors;

  ClearCommError(FHandle, &Errors, &ComStat);
  if ((Status & EV_BREAK) > 0)
    if (FOnBreak)  FOnBreak(this);

  if ((Status & EV_CTS) > 0)
    if (FOnCts)  FOnCts(this);

  if ((Status & EV_DSR) > 0)
    if (FOnDsr)  FOnDsr(this);

  if ((Status & EV_ERR) > 0)
    if (FOnError)  FOnError(this, Errors);

  if ((Status & EV_RING) > 0)
    if (FOnRing)  FOnRing(this);

  if ((Status & EV_RLSD) > 0)
    if (FOnRlsd)  FOnRlsd(this);

  if ((Status & EV_RXCHAR) > 0)
    if (ComStat.cbInQue > 0 )
      if (FOnRxChar) FOnRxChar(this, ComStat.cbInQue);

  if ((Status & EV_RXFLAG) > 0)
    if (FOnRxFlag)  FOnRxFlag(this);

  if ((Status & EV_TXEMPTY) > 0)
    if (FOnTxEmpty)  FOnTxEmpty(this);
  if (FCommStatEvent)  FCommStatEvent(this,Status , ComStat);
}

bool __fastcall TThreadComm::GetModemState(int Index)
{
  DWORD Flag, State;
  bool Result ;

  switch  (Index)
  {
    case 1:  State = MS_CTS_ON;   break;
    case 2:  State = MS_DSR_ON;   break;
    case 3:  State = MS_RING_ON;  break;
    case 4:  State = MS_RLSD_ON;  break;
    default: State = 0;           break;
  }
  Result = false;
  if (Enabled())
    if (GetCommModemStatus(FHandle, &Flag))
      Result = ((Flag & State) > 0);
  return Result ;
}

bool __fastcall TThreadComm::GetComState(int Index)
{
  COMSTAT       ComStat;
  DWORD         Errors;
  bool          Result;

  if (Enabled())
  {
    ClearCommError(FHandle, &Errors, &ComStat);
  }
  switch (Index)
  {
    case 1:  Result = ComStat.fCtsHold;  break;
    case 2:  Result = ComStat.fDsrHold;  break;
    case 3:  Result = ComStat.fRlsdHold;  break;
    case 4:  Result = ComStat.fXoffHold; break;
    case 5:  Result = ComStat.fXoffSent; break;
    default: Result = ComStat.fCtsHold;  break;
  }
  return Result ;
}

void __fastcall TThreadComm::UpdateDataControlBlock()
{
bool Result ;
  if (Enabled())
  {
    SetupComm( FHandle, FReadBufSize, FWriteBufSize ) ;
    GetCommState(FHandle, &FDCB);

    FDCB.BaudRate = CommBaudRates[FBaudRate];
    FDCB.Parity = CommParity[FParity];
    FDCB.StopBits = CommStopBits[FStopBits];
    FDCB.ByteSize = CommDataBits[FDataBits];
    FDCB.XonChar = FEventChars->XOnChar;
    FDCB.XoffChar = FEventChars->XOffChar;
    FDCB.ErrorChar = FEventChars->ErrorChar;
    FDCB.EofChar = FEventChars->EofChar;
    FDCB.EvtChar = FEventChars->EvtChar;
    FDCB.XonLim = 1024;//WORD(FReadBufSize / 4);
    FDCB.XoffLim = 1024; //WORD(FReadBufSize / 4);

//    InitHandshaking(FDCB);

    FDCB.fParity = FOptions.Contains(coParityCheck);
    FDCB.fDsrSensitivity = FOptions.Contains(coDsrSensitivity) ;
    FDCB.fTXContinueOnXoff = FOptions.Contains(coIgnoreXOff) ;
    FDCB.fErrorChar = FOptions.Contains(coErrorChar);
    FDCB.fNull = FOptions.Contains(coNullStrip) ;

    Result = SetCommState(FHandle, &FDCB) ;
        if (!Result)
          RaiseCommError(sUpdateDCBErr, GetLastError());
  }
}

void __fastcall TThreadComm::UpdateDataControlBlockLock()
{
    Lock();
    UpdateDataControlBlock();
    Unlock();
}

void __fastcall TThreadComm::EscapeComm(int Flag)
{
  bool Escaped;

  if (Enabled())
  {
    Lock();
    Escaped = EscapeCommFunction(FHandle, Flag);
    Unlock();
    if (!Escaped)
      RaiseCommError(sEscFuncError, GetLastError());
  } else RaiseCommError(sPortNotOpen, -1);
}

void __fastcall TThreadComm::SetDTRState(bool State)
{
  int DTR[] = {CLRDTR, SETDTR};
  EscapeComm(DTR[State]);
}

void __fastcall TThreadComm::SetRTSState(bool State)
{
  int RTS[]= { CLRRTS, SETRTS};
  EscapeComm(RTS[State]);
}

void __fastcall TThreadComm::SetBREAKState(bool State)
{
  int BREAK[] = { CLRBREAK, SETBREAK };

  EscapeComm(BREAK[State]);
  if (Enabled())
  {
    Lock();
    PurgeComm(FHandle, PurgeReadWrite);
    Unlock();
  }
}

void __fastcall TThreadComm::SetXONState(bool State)
{
  int XON[] = { SETXOFF, SETXON };
  EscapeComm(XON[State]);
}

void __fastcall TThreadComm::UpdateCommTimeouts()
{
  COMMTIMEOUTS CommTimeOuts;
  bool Result ;

	      CommTimeOuts.ReadIntervalTimeout = 0xFFFFFFFF;
    	  CommTimeOuts.ReadTotalTimeoutMultiplier = 0 ;
	      CommTimeOuts.ReadTotalTimeoutConstant = 1000 ;
	      CommTimeOuts.WriteTotalTimeoutMultiplier = 2*CBR_9600/FBaudRate ;
    	  CommTimeOuts.WriteTotalTimeoutConstant = 0 ;
  Result = SetCommTimeouts(FHandle, &CommTimeOuts) ;
  if (!Result)
    RaiseCommError(sCommTimeoutsErr, GetLastError());
}

__fastcall TComm::TComm(TComponent* Owner):TThreadComm(Owner)
{
}

__fastcall TComm::~TComm()
{
}
//---------------------------------------------------------------------------
namespace Comm
{
    void __fastcall PACKAGE Register()
    {
        TComponentClass classes[1] = {__classid(TComm)};
        RegisterComponents("Samples", classes, 0);
    }
}
//---------------------------------------------------------------------------
