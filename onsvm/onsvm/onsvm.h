#pragma once

#include <list>
#include <vector>

namespace OnSVM
{
		
	typedef std::vector<double>		TVector;

	class IKernel
	{
	public:
		virtual double dot( TVector const& , TVector const& ) const = 0;
	};


	class CLinearKernel : public IKernel
	{
	public:
		virtual double dot( TVector const& a, TVector const& b ) const
		{
			if ( a.size() != b.size() ) return 0;

			double result = 0;
			for ( int i = 0; i < a.size(); ++i )
			{
				result += a[i]*b[i];
			}

			return result;
		}

	};

	class CEngine
	{
	public:
		enum { kHyperplanes = 2 };
		//typedef std::vector<double>		TVector;
		typedef int		TClass;

		//TODO: data sink;
		struct TSupVec
		{
			double		alpha;
			double		grad;
			double		degrad[kHyperplanes];
			bool		bBounded;
            bool        bDegrad;
			//TODO:
			TVector		data;

			TSupVec( TVector const& d ):alpha( 0 ), grad( 0 ), data(d), bBounded(false), bDegrad(false)
			{
				for ( int i = 0; i < kHyperplanes; ++i ) degrad[i] = 0;
			}
			
		};

        typedef std::list<TSupVec>		TSupVecs;
        typedef TSupVecs::iterator		TItSupVec;

        struct THyperplane
        {
            TSupVecs				sv;
            double					hiBound;
            size_t				    count;
            double					nu;
			double					b;

			THyperplane():hiBound(1.0), count(0), nu(0.5){}
        };

        struct TTauPair
        {
            TSupVec     *pPos;     
            TSupVec     *pNeg;

			TTauPair( TSupVec* pPositive = NULL, TSupVec* pNegative = NULL ):pPos(pPositive), pNeg(pNegative){} 
		};


	public:
		CEngine( IKernel& kernel, double tau ): m_pKernel(&kernel), m_tau(tau){}

		void	update( TVector const& data, TClass category );
		void	finish( void );

		void	setNU( double pnu, double mnu ) { m_hyps[0].nu = pnu; m_hyps[1].nu = mnu;}
	

	protected:

        bool		solve( TTauPair pairs[kHyperplanes] );

        TSupVec*	insert( TVector const& data, int hpid );

		bool		clean( double gradThreshold[kHyperplanes] );
		

	private:
		IKernel					*m_pKernel;
        THyperplane             m_hyps[kHyperplanes];
        		
		double					m_sigma;
		double					m_tau;
	};
	

}
