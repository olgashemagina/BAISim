#include "onsvm.h"

namespace OnSVM
{


	void CEngine::update( TVector const& data, TClass category )
	{
		int hpid = 0;
		if ( category < 0 ) hpid = 1;

		TTauPair	pairs[kHyperplanes];
		TSupVec* pNewVec = insert( data, hpid );

		if ( m_hyps[0].sv.size() < 2 || m_hyps[1].sv.size() < 2 ) return;

		for ( int i = 0; i < kHyperplanes; ++i )
		{
			pairs[i].pPos = &m_hyps[i].sv.front(); pairs[i].pNeg = pairs[i].pPos;
			for ( TItSupVec it = m_hyps[i].sv.begin(); it != m_hyps[i].sv.end(); ++it )
			{
				if ( pairs[i].pPos->grad <= it->grad && it->alpha < m_hyps[i].hiBound )	pairs[i].pPos = &*it;
				else if ( pairs[i].pNeg->grad >= it->grad && it->alpha > 0 ) pairs[i].pNeg = &*it;
			}
		}

		if ( (pairs[hpid].pPos->grad - pNewVec->grad) > (pNewVec->grad - pairs[hpid].pNeg->grad) )
			pairs[hpid].pNeg = pNewVec;
		else pairs[hpid].pPos = pNewVec;

		double eff = 0;
		for ( int i = 0; i < kHyperplanes; ++i )
			eff += pairs[i].pPos->grad - pairs[i].pNeg->grad;

		if ( eff >= m_tau ) solve( pairs );

		eff = 0;
		for ( int i = 0; i < kHyperplanes; ++i )
		{			
			for ( TItSupVec it = m_hyps[i].sv.begin(); it != m_hyps[i].sv.end(); ++it )
			{
				if ( pairs[i].pPos->grad <= it->grad && it->alpha < m_hyps[i].hiBound )	pairs[i].pPos = &*it;
				else if ( pairs[i].pNeg->grad >= it->grad && it->alpha > 0 ) pairs[i].pNeg = &*it;
			}
			eff += pairs[i].pPos->grad - pairs[i].pNeg->grad;
		}

		if ( eff >= m_tau ) solve( pairs );
		double gradThreshold[kHyperplanes];

		for ( int i = 0; i < kHyperplanes; ++i )
		{
			for ( TItSupVec it = m_hyps[i].sv.begin(); it != m_hyps[i].sv.end(); ++it )
			{
				if ( pairs[i].pPos->grad <= it->grad && it->alpha < m_hyps[i].hiBound )	pairs[i].pPos = &*it;
				else if ( pairs[i].pNeg->grad >= it->grad && it->alpha > 0 ) pairs[i].pNeg = &*it;
			}
			gradThreshold[i] = pairs[i].pNeg->grad;
			m_hyps[i].b = 0.5*(pairs[i].pPos->grad + pairs[i].pNeg->grad);
		}

		clean( gradThreshold );
	}

	void CEngine::finish( void )
	{
		TTauPair	pairs[kHyperplanes];
		while (true)
		{
			double eff = 0;
			for ( int i = 0; i < kHyperplanes; ++i )
			{
				pairs[i].pPos = &m_hyps[i].sv.front(); pairs[i].pNeg = pairs[i].pPos;
				for ( TItSupVec it = m_hyps[i].sv.begin(); it != m_hyps[i].sv.end(); ++it )
				{
					if ( pairs[i].pPos->grad <= it->grad && it->alpha < m_hyps[i].hiBound )	pairs[i].pPos = &*it;
					else if ( pairs[i].pNeg->grad >= it->grad && it->alpha > 0 ) pairs[i].pNeg = &*it;
				}
				eff += pairs[i].pPos->grad - pairs[i].pNeg->grad;
				m_hyps[i].b = 0.5*(pairs[i].pPos->grad + pairs[i].pNeg->grad);
			}
			if ( eff < m_tau || !solve( pairs ) ) break;
		}
	}

	bool CEngine::solve( TTauPair pairs[kHyperplanes] )
	{
		double A =  m_pKernel->dot( pairs[0].pPos->data,  pairs[0].pPos->data) + 
			m_pKernel->dot( pairs[0].pNeg->data, pairs[0].pNeg->data ) - 
			2.0*m_pKernel->dot( pairs[0].pPos->data, pairs[0].pNeg->data );

		double B =  m_pKernel->dot( pairs[1].pPos->data,  pairs[1].pPos->data) + 
			m_pKernel->dot( pairs[1].pNeg->data, pairs[1].pNeg->data ) - 
			2.0*m_pKernel->dot( pairs[1].pPos->data, pairs[1].pNeg->data );


		double C = -m_pKernel->dot(pairs[0].pPos->data, pairs[1].pPos->data) + 
			m_pKernel->dot( pairs[0].pPos->data, pairs[1].pNeg->data ) + 
			m_pKernel->dot( pairs[0].pNeg->data, pairs[1].pPos->data ) - 
			m_pKernel->dot( pairs[0].pNeg->data, pairs[1].pNeg->data );

		double mult = 1.0/(A*B-C*C);

		double delta[kHyperplanes] = { pairs[0].pPos->grad - pairs[0].pNeg->grad, 
			pairs[1].pPos->grad - pairs[1].pNeg->grad };

		double lambda[kHyperplanes] = {	mult*(B*delta[0] - C*delta[1]),
			mult*(A*delta[1] - C*delta[0])};

		//clamp to constraint;
		for ( int i = 0; i < kHyperplanes; i++ )
		{
			TSupVec	*pPos = pairs[i].pPos, *pNeg = pairs[i].pNeg;
			lambda[i] = std::min( std::min( m_hyps[i].hiBound - pPos->alpha, pNeg->alpha ), lambda[i] );
			pPos->alpha += lambda[i];
			pNeg->alpha -= lambda[i];
			if ( pPos->alpha >= m_hyps[i].hiBound ) pPos->bBounded = true;
			if ( pNeg->alpha < m_hyps[i].hiBound ) pNeg->bBounded = false;

		}

		if ( lambda[0] == 0 && lambda[1] == 0 ) return false;

		for ( int i = 0; i < kHyperplanes; i++ )
		{
			THyperplane* pPlane = &m_hyps[i];

			for ( TItSupVec it = pPlane->sv.begin(); it != pPlane->sv.end(); ++it )
			{
				double kernPos = m_pKernel->dot(it->data, pairs[i].pPos->data);
				double kernOppPos = m_pKernel->dot(it->data, pairs[1-i].pPos->data);
				double kernNeg = m_pKernel->dot(it->data, pairs[i].pNeg->data);
				double kernOppNeg = m_pKernel->dot(it->data, pairs[1-i].pNeg->data);

				it->grad -= lambda[i] * ( kernPos - kernNeg ) -
					lambda[1-i] * ( kernOppPos - kernOppNeg );

				if ( !pairs[i].pPos->bDegrad && pairs[i].pPos->bBounded ) it->degrad[i] -= kernPos;
				if ( pairs[i].pNeg->bDegrad && !pairs[i].pNeg->bBounded ) it->degrad[i] += kernNeg;
				if ( !pairs[1-i].pPos->bDegrad && pairs[1-i].pPos->bBounded ) it->degrad[1-i] += kernOppPos;
				if ( pairs[1-i].pNeg->bDegrad && !pairs[1-i].pNeg->bBounded ) it->degrad[1-i] -= kernOppNeg;
			}
		}

		for ( int i = 0; i < kHyperplanes; i++ )
		{
			if ( !pairs[i].pPos->bDegrad && pairs[i].pPos->bBounded ) pairs[i].pPos->bDegrad = true;
			if ( pairs[i].pNeg->bDegrad && !pairs[i].pNeg->bBounded ) pairs[i].pNeg->bDegrad = false;
		}
		return true;
	}

	CEngine::TSupVec* CEngine::insert( TVector const& data, int hpid )
	{
		THyperplane *pPlane = &m_hyps[hpid];
		THyperplane *pOppPlane = &m_hyps[1-hpid];

		double oldHiBound = pPlane->hiBound;
		pPlane->count++;
		pPlane->hiBound = 1.0/( pPlane->nu * pPlane->count );

		double newalpha = 0;

		//Update the bound;
		for ( TItSupVec it = pPlane->sv.begin(); it != pPlane->sv.end(); ++it )
		{
			it->grad -= (oldHiBound - pPlane->hiBound)*it->degrad[hpid];
			if ( it->bBounded ) it->alpha = pPlane->hiBound;
		}

		//Update the bound;
		for ( TItSupVec it = pOppPlane->sv.begin(); it != pOppPlane->sv.end(); ++it )
		{
			it->grad -= (oldHiBound - pPlane->hiBound)*it->degrad[hpid];
		}


		//Bound other sv;
		for ( TItSupVec it = pPlane->sv.begin(); it != pPlane->sv.end(); ++it )
		{
			//TODO: unbounded list
			if (it->alpha >= pPlane->hiBound && !it->bBounded)
			{
				double dealpha = it->alpha - pPlane->hiBound;

				for ( TItSupVec jt = pPlane->sv.begin(); jt != pPlane->sv.end(); ++jt )
				{
					double kernel = m_pKernel->dot(it->data, jt->data);
					jt->degrad[hpid] -= kernel;
					jt->grad += dealpha*kernel;
				}

				for ( TItSupVec jt = pOppPlane->sv.begin(); jt != pOppPlane->sv.end(); ++jt )
				{
					double kernel = m_pKernel->dot(it->data, jt->data);
					jt->degrad[hpid] += kernel;
					jt->grad -= dealpha*kernel;
				}

				//newalpha += dealpha;
				it->alpha = pPlane->hiBound;
				it->bBounded = true;
				it->bDegrad = true;

			}

			newalpha += it->alpha;
		}

		TSupVec	newSupVec( data );
		
		
		newSupVec.alpha = 1.0 - newalpha;
		newSupVec.grad = -newSupVec.alpha * m_pKernel->dot(newSupVec.data, newSupVec.data);

		//The new vector cant be a bounded;
		for ( TItSupVec it = pPlane->sv.begin(); it != pPlane->sv.end(); ++it )
		{
			double kernel = m_pKernel->dot(it->data, newSupVec.data);

			it->grad -= newSupVec.alpha * kernel;
			newSupVec.grad -= it->alpha * kernel;
		}

		for ( TItSupVec it = pOppPlane->sv.begin(); it != pOppPlane->sv.end(); ++it )
		{
			double kernel = m_pKernel->dot(it->data, newSupVec.data);

			it->grad += newSupVec.alpha * kernel;
			newSupVec.grad += it->alpha * kernel;
		}

		pPlane->sv.push_front( newSupVec );

		return &pPlane->sv.front();
	}

	bool CEngine::clean( double gradThreshold[kHyperplanes] )
	{
		bool bRemoved = false;
		for ( int i = 0; i < kHyperplanes; i++ )
		{
			TItSupVec it = m_hyps[i].sv.begin();
			while ( it != m_hyps[i].sv.end() )
			{
				TItSupVec itErase = it++;

				if ( itErase->alpha <= 0 && itErase->grad <= gradThreshold[i] )
				{
					m_hyps[i].sv.erase( itErase );
					bRemoved = true;
				}

			}
		}

		return bRemoved;
	}
};