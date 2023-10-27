//---------------------------------------------------------------------------

#ifndef PhBatchProcessorH
#define PhBatchProcessorH
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
//---------------------------------------------------------------------------
class TBatchProcessor : public TThread
{
private:
protected:
	void __fastcall Execute();
public:
	__fastcall TBatchProcessor(bool CreateSuspended);
};
//---------------------------------------------------------------------------
#endif
