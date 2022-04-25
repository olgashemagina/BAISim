#include "onsvm.h"


int main()
{
	using namespace OnSVM;
	
	CLinearKernel	kernel;
	CEngine engine( kernel, 0.01 );

	TVector		data[4];

	data[0].push_back( 1 );
	data[0].push_back( 1 );

	data[1].push_back( 0 );
	data[1].push_back( 1 );

	data[2].push_back( 1 );
	data[2].push_back( 0 );

	data[3].push_back( 0 );
	data[3].push_back( 0 );

	engine.update( data[0],1 );
	engine.update( data[1],-1 );
	engine.update( data[2],-1 );
	engine.update( data[3],-1 );

	engine.finish();

}
