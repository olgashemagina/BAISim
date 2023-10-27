//---------------------------------------------------------------------------
#ifndef CommH
#define CommH
//---------------------------------------------------------------------------
#include <SysUtils.hpp>
#include <Controls.hpp>
#include <Classes.hpp>
#include <Forms.hpp>

#include <syncobjs.hpp>

#define sMsgExtention    "(Error: %d)"
#define sOpenError       "Error accessing specified device (Error: %d)"
#define sInvalidHandle   "Invalid device handle, access denied (Error: %d)"
#define sPortAlreadyOpen "Port already assigned (open) (Error: %d)"
#define sPortNotOpen     "Port not open (Error: %d)"
#define sSetupCommErr    "Error initializing Read/Write Buffers (Error: %d)"
#define sUpdateDCBErr    "Error updating DataControlBlock (Error: %d)"
#define sCommTimeoutsErr "Error updating CommTimeouts (Error: %d)"
#define sEscFuncError    "EscapeCommFunction failure (Error: %d)"
#define sReadError       "Read error (Error: %d)"
#define sWriteError      "Write error (Error: %d)"
#define PurgeRead        PURGE_RXABORT + PURGE_RXCLEAR
#define PurgeWrite       PURGE_TXABORT + PURGE_TXCLEAR
#define PurgeReadWrite   PurgeRead + PurgeWrite

typedef void __fastcall (__closure *TCommEvent)(TObject* Sender,
                                                   DWORD Status);
typedef void __fastcall (__closure *TCommErrorEvent)(TObject* Sender,
                                                   int Errors);
typedef void __fastcall (__closure *TCommRxCharEvent)(TObject* Sender,
                                                   DWORD Count);

typedef void __fastcall (__closure *TCommStatEvent)(TObject* Sender,
                                                    DWORD Status,
                                                    COMSTAT ComStat);

enum TCommEventType {evBreak, evCts, evDsr, evError, evRing,
                        evRlsd, evRxChar, evRxFlag, evTxEmpty };
typedef Set<TCommEventType,evBreak , evTxEmpty>  TCommEventTypes;

enum  TBaudRate {br110, br300, br600, br1200, br2400, br4800, br9600, br14400,
    br19200, br38400, br56000, br57600, br115200, br128000, br256000};
enum  TParity {paNone, paOdd, paEven, paMark, paSpace};
enum  TStopBits { sb10, sb15, sb20};
enum  TDataBits {da4, da5, da6, da7, da8};
enum  TFlowControl {fcNone, fcCTS, fcDTR, fcSoftware, fcDefault};

enum  TCommOption {coParityCheck, coDsrSensitivity, coIgnoreXOff,
    coErrorChar, coNullStrip};
typedef Set<TCommOption,coParityCheck , coNullStrip>  TCommOptions;

class TCommEventThread : public TThread
{
    private :
        HANDLE FCommHandle ;
        DWORD FEventMask ;
        DWORD FSuspendTime ;
        TCommEvent FOnSignal;
    protected :
        virtual void __fastcall Execute(void);
        void __fastcall DoOnSignal(void);

    public :
        __fastcall TCommEventThread::TCommEventThread(HANDLE Handle,
                                                    TCommEventTypes MonitorEvents,
                                                    DWORD SuspendTime);
        __property TCommEvent OnSignal= { read=FOnSignal, write=FOnSignal } ;
} ;

class TThreadComm;

class TCommEventChars : public TPersistent
{
  private :
    TThreadComm *FOwner ;
    char FXOnChar;
    char FXOffChar;
    char FErrorChar;
    char FEofChar;
    char FEvtChar;
    void __fastcall SetEventChar(int Index, char Value);
  public :
        __fastcall TCommEventChars::TCommEventChars(TThreadComm *Owner);
    virtual void __fastcall Assign(TPersistent *Source);
  __published :
    __property char XOnChar={index=1,read=FXOnChar,write=SetEventChar,default=17};
    __property char XOffChar={index=2,read=FXOffChar,write=SetEventChar,default=19};
    __property char ErrorChar={index=3,read=FErrorChar,write=SetEventChar,default=0};
    __property char EofChar={index=4,read=FEofChar,write=SetEventChar,default=0};
    __property char EvtChar={index=5,read=FEvtChar,write=SetEventChar,default=0};
} ;




//---------------------------------------------------------------------------
class PACKAGE TThreadComm : public TComponent
{
  private:
    HANDLE FHandle;
    DCB    FDCB;
    String FDeviceName;
    TSimpleEvent *FEvent;
    TCriticalSection *FCriticalSection;
    int FReadTimeout;
    int FWriteTimeout;
    WORD FReadBufSize;
    WORD FWriteBufSize;
    DWORD FSuspendTime ;
    TCommEventTypes FMonitorEvents;
    TBaudRate FBaudRate;
    TParity FParity;
    TStopBits FStopBits;
    TDataBits FDataBits;
    bool        FConnected ;
    OVERLAPPED  WRITE_OS ;
    OVERLAPPED  READ_OS ;
    TCommEventThread *FEventThread;
    TCommEventChars *FEventChars;
    TCommOptions FOptions;
    TFlowControl FFlowControl;
    TNotifyEvent FOnBreak;
    TNotifyEvent FOnCts;
    TNotifyEvent FOnDsr;
    TCommErrorEvent FOnError;
    TNotifyEvent FOnRing;
    TNotifyEvent FOnRlsd;
    TCommRxCharEvent FOnRxChar;
    TNotifyEvent FOnRxFlag;
    TNotifyEvent FOnTxEmpty;
    TCommStatEvent FCommStatEvent ;
    void __fastcall SetDeviceName(String Value);
    void __fastcall SetMonitorEvents(TCommEventTypes Value);
    void __fastcall SetReadBufSize(WORD Value);
    void __fastcall SetWriteBufSize(WORD Value);
    void __fastcall SetSuspendTime(DWORD Value);
    void __fastcall SetBaudRate(TBaudRate Value);
    void __fastcall SetParity(TParity Value);
    void __fastcall SetStopBits(TStopBits Value);
    void __fastcall SetDataBits(TDataBits Value);
    void __fastcall SetOptions(TCommOptions Value);
    void __fastcall SetFlowControl(TFlowControl Value);
    bool __fastcall GetModemState(int Index);
    bool __fastcall GetComState(int Index);
    void __fastcall Lock();
    void __fastcall Unlock();
    void __fastcall CheckOpen();
    void __fastcall EscapeComm(int Flag);
    void __fastcall UpdateCommTimeouts();
protected:
    virtual void __fastcall CreateHandle();
    void __fastcall DestroyHandle();
    void __fastcall HandleCommEvent(TObject *Sender,DWORD Status);
    void __fastcall UpdateDataControlBlockLock();
    __property String DeviceName={ read=FDeviceName,write=SetDeviceName};
    __property int ReadTimeout  ={ read=FReadTimeout,write=FReadTimeout,default=1000};
    __property int WriteTimeout ={ read=FWriteTimeout,write=FWriteTimeout,default=1000};
    __property WORD ReadBufSize  ={ read=FReadBufSize,write=SetReadBufSize,default=4096};
    __property WORD WriteBufSize ={ read=FWriteBufSize,write=SetWriteBufSize,default=2048};
    __property DWORD SuspendTime ={ read=FSuspendTime,write=SetSuspendTime,default=5};
    __property TCommEventTypes MonitorEvents ={read=FMonitorEvents,write=SetMonitorEvents};
    __property TBaudRate BaudRate ={read=FBaudRate,write=SetBaudRate,default=br9600};
    __property TParity Parity ={ read=FParity,write=SetParity,default=paNone};
    __property TStopBits StopBits ={ read=FStopBits,write=SetStopBits,default=sb10};
    __property TDataBits DataBits ={ read=FDataBits,write=SetDataBits,default=da8};
    __property TCommEventChars* EventChars ={ read=FEventChars };
    __property TCommOptions Options ={ read=FOptions,write=SetOptions};
    __property TFlowControl FlowControl ={ read=FFlowControl,write=SetFlowControl,default=fcDefault};
    __property TNotifyEvent OnBreak ={ read=FOnBreak,write=FOnBreak};
    __property TNotifyEvent OnCts ={ read=FOnCts, write=FOnCts} ;
    __property TNotifyEvent OnDsr ={ read=FOnDsr, write=FOnDsr};
    __property TNotifyEvent OnRing ={ read=FOnRing,write=FOnRing};
    __property TNotifyEvent OnRlsd ={ read=FOnRlsd,write=FOnRlsd};
    __property TCommErrorEvent OnError={ read=FOnError, write=FOnError};
    __property TCommRxCharEvent OnRxChar={ read=FOnRxChar, write=FOnRxChar};
    __property TNotifyEvent OnRxFlag={ read=FOnRxFlag, write=FOnRxFlag};
    __property TNotifyEvent OnTxEmpty={read=FOnTxEmpty, write=FOnTxEmpty};
    __property TCommStatEvent OnComStatEvent={read=FCommStatEvent,write=FCommStatEvent};
public:
    __fastcall TThreadComm(TComponent* Owner);
    virtual __fastcall ~TThreadComm();
    void __fastcall Open();
    void __fastcall Close();
    bool __fastcall Enabled();
    int __fastcall Write(char Buf[], DWORD Count) ;
    int __fastcall Read(char Buf[] , DWORD Count);
    int __fastcall InQueCount();
    int __fastcall OutQueCount();
    void __fastcall PurgeIn();
    void __fastcall PurgeOut();
//    {Comm escape functions}
    void __fastcall SetDTRState(bool State);
    void __fastcall SetRTSState(bool State);
    void __fastcall SetBREAKState(bool State);
    void __fastcall SetXONState(bool State);
    void __fastcall UpdateDataControlBlock();
    __property bool Connected={ read=FConnected} ;
//    {Comm status flags}
    __property bool CTS={ index=1, read=GetModemState } ;
    __property bool DSR ={ index=2, read=GetModemState} ;
    __property bool RING={ index=3, read=GetModemState } ;
    __property bool RLSD ={ index=4, read=GetModemState} ;

    __property bool CtsHold ={ index=1, read=GetComState };
    __property bool DsrHold ={ index=2, read=GetComState };
    __property bool RlsdHold ={ index=3, read=GetComState };
    __property bool XOffHold ={ index=4, read=GetComState };
    __property bool XOffSent ={ index=5, read=GetComState };

    __property HANDLE Handle ={ read=FHandle };
__published:
};

class PACKAGE TComm : public TThreadComm
{
  public:
    __fastcall virtual TComm(TComponent* Owner);
    virtual __fastcall ~TComm();
  __published:
    __property DeviceName;
    __property ReadTimeout;
    __property WriteTimeout;
    __property ReadBufSize;
    __property WriteBufSize;
    __property SuspendTime ;
    __property MonitorEvents;
    __property BaudRate;
    __property Parity;
    __property StopBits;
    __property DataBits;
    __property EventChars;
    __property Options;
    __property FlowControl;
    __property OnBreak;
    __property OnCts;
    __property OnDsr;
    __property OnRing;
    __property OnRlsd;
    __property OnError;
    __property OnRxChar;
    __property OnRxFlag;
    __property OnTxEmpty;
    __property OnComStatEvent;
};

//---------------------------------------------------------------------------
#endif
