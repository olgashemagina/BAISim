//---------------------------------------------------------------------------

#ifndef PhReadImagesThreadH
#define PhReadImagesThreadH
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include "awpipl.h"
#include "FImage41.h"
//---------------------------------------------------------------------------
// this is abstract PhThread to deal with items list.
class IPhThread : public TThread
{
protected:
    TList*    			m_items;
	UnicodeString 		m_msg;
	int       			m_progress;
	bool      			m_cancel;
	TPhProgressEvent    m_OnProgress;
	void __fastcall ProgressHelper();

public:
	__fastcall IPhThread(TList* items);
	void __fastcall Cancel();

	__property bool Canceled = {read = m_cancel};
	__property TPhProgressEvent OnProgress = {read = m_OnProgress, write = m_OnProgress};
};

class TPhJobThread : public IPhThread
{
private:
	EPhJobReason 	m_reason;
	TPhJobEvent  	m_FihishEvent;
	UnicodeString 	m_FolderName;
	awpImage* 		m_mosaic;
	int       		m_tmbWidth;
	int       		m_tmbHeight;
	EPhImageFormats m_targetFormat;
	bool            m_keepSource;
	EPhCopyActioin  m_action;
protected:
	virtual void __fastcall BeforeDestruction(void);
	int __fastcall  GetNumSelectedItems();
	void __fastcall DoMosaic();

	void __fastcall DoReadJob();
	void __fastcall DoCopyJob();
	void __fastcall DoMoveJob();
	void __fastcall DoDeleteJob();
	void __fastcall DoConvertJob();
	void __fastcall DoProcessJob();

	void __fastcall Execute();
public:
	__fastcall  TPhJobThread(TList* names, const LPWSTR lpwFolderName, EPhJobReason reason = copyJob);
	__property  EPhJobReason reason = {read = m_reason};
	__property  EPhCopyActioin action = {read = m_action, write = m_action};
	__property awpImage* Mosaic = {read = m_mosaic};
	__property int tmbWidth 	= {read = m_tmbWidth, write = m_tmbWidth};
	__property int tmbHeight    = {read = m_tmbHeight, write = m_tmbHeight};
	__property EPhImageFormats TargetFormat = {read = m_targetFormat, write = m_targetFormat};
	__property bool KeepSource = {read = m_keepSource, write = m_keepSource};
	__property  TPhJobEvent  OnFinish = {read = m_FihishEvent, write =m_FihishEvent};
};

#endif
