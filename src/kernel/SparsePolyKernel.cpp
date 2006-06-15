/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * Written (W) 1999-2006 Soeren Sonnenburg
 * Written (W) 1999-2006 Gunnar Raetsch
 * Written (W) 1999-2006 Fabio De Bona
 * Copyright (C) 1999-2006 Fraunhofer Institute FIRST and Max-Planck-Society
 */

#include "lib/common.h"
#include "lib/io.h"
#include "kernel/SparsePolyKernel.h"
#include "features/Features.h"
#include "features/SparseRealFeatures.h"
#include "features/SparseFeatures.h"
#include "kernel/SparseKernel.h"

CSparsePolyKernel::CSparsePolyKernel(LONG size, INT d, bool inhom, bool use_norm)
  : CSparseRealKernel(size), degree(d), inhomogene(inhom), 
	sqrtdiag_lhs(NULL), sqrtdiag_rhs(NULL), initialized(false), use_normalization(use_norm)
{
}

CSparsePolyKernel::~CSparsePolyKernel() 
{
	cleanup();
}

bool CSparsePolyKernel::init(CFeatures* l, CFeatures* r, bool do_init)
{
	bool result=CSparseRealKernel::init(l,r,do_init);

	initialized = false ;
	INT i;

	if (sqrtdiag_lhs != sqrtdiag_rhs)
	  delete[] sqrtdiag_rhs;
	sqrtdiag_rhs=NULL ;
	delete[] sqrtdiag_lhs;
	sqrtdiag_lhs=NULL ;

	if (use_normalization)
	{
		sqrtdiag_lhs= new DREAL[lhs->get_num_vectors()];

		for (i=0; i<lhs->get_num_vectors(); i++)
			sqrtdiag_lhs[i]=1;

		if (l==r)
			sqrtdiag_rhs=sqrtdiag_lhs;
		else
		{
			sqrtdiag_rhs= new DREAL[rhs->get_num_vectors()];
			for (i=0; i<rhs->get_num_vectors(); i++)
				sqrtdiag_rhs[i]=1;
		}

		ASSERT(sqrtdiag_lhs);
		ASSERT(sqrtdiag_rhs);

		this->lhs=(CSparseRealFeatures*) l;
		this->rhs=(CSparseRealFeatures*) l;

		//compute normalize to 1 values
		for (i=0; i<lhs->get_num_vectors(); i++)
		{
			sqrtdiag_lhs[i]=sqrt(compute(i,i));

			//trap divide by zero exception
			if (sqrtdiag_lhs[i]==0)
				sqrtdiag_lhs[i]=1-16;
		}

		// if lhs is different from rhs (train/test data)
		// compute also the normalization for rhs
		if (sqrtdiag_lhs!=sqrtdiag_rhs)
		{
			this->lhs=(CSparseRealFeatures*) r;
			this->rhs=(CSparseRealFeatures*) r;

			//compute normalize to 1 values
			for (i=0; i<rhs->get_num_vectors(); i++)
			{
				sqrtdiag_rhs[i]=sqrt(compute(i,i));

				//trap divide by zero exception
				if (sqrtdiag_rhs[i]==0)
					sqrtdiag_rhs[i]=1e-16;
			}
		}
	}

	this->lhs=(CSparseRealFeatures*) l;
	this->rhs=(CSparseRealFeatures*) r;

	initialized = true;
	CIO::message(M_INFO, "SparsePolyKernel initialized\n");
	return result;
}
  
void CSparsePolyKernel::cleanup()
{
	if (sqrtdiag_lhs != sqrtdiag_rhs)
		delete[] sqrtdiag_rhs;
	sqrtdiag_rhs=NULL;

	delete[] sqrtdiag_lhs;
	sqrtdiag_lhs=NULL;

	initialized=false;
}

bool CSparsePolyKernel::load_init(FILE* src)
{
	return false;
}

bool CSparsePolyKernel::save_init(FILE* dest)
{
	return false;
}
  
DREAL CSparsePolyKernel::compute(INT idx_a, INT idx_b)
{
  INT alen=0;
  INT blen=0;
  bool afree=false;
  bool bfree=false;

  //fprintf(stderr, "LinKernel.compute(%ld,%ld)\n", idx_a, idx_b) ;
  TSparseEntry<DREAL>* avec=((CSparseRealFeatures*) lhs)->get_sparse_feature_vector(idx_a, alen, afree);
  TSparseEntry<DREAL>* bvec=((CSparseRealFeatures*) rhs)->get_sparse_feature_vector(idx_b, blen, bfree);

  DREAL sqrt_a= 1 ;
  DREAL sqrt_b= 1 ;
  if (initialized && use_normalization)
    {
      sqrt_a=sqrtdiag_lhs[idx_a] ;
      sqrt_b=sqrtdiag_rhs[idx_b] ;
    } ;

  DREAL sqrt_both=sqrt_a*sqrt_b;
  
  DREAL result=0;

  //result remains zero when one of the vectors is non existent
  if (avec && bvec)
  {
	  if (alen<=blen)
	  {
	      INT j=0;
	      for (INT i=0; i<alen; i++)
	      {
	    	  INT a_feat_idx=avec[i].feat_index;

	    	  while ( (j<blen) && (bvec[j].feat_index < a_feat_idx) )
	    		  j++;

	    	  if ( (j<blen) && (bvec[j].feat_index == a_feat_idx) )
	    	  {
	    		  result+= avec[i].entry * bvec[j].entry;
	    		  j++;
	    	  }
	      }
	  }
	  else
	  {
	      INT j=0;
	      for (INT i=0; i<blen; i++)
	      {
	    	  INT b_feat_idx=bvec[i].feat_index;

	    	  while ( (j<alen) && (avec[j].feat_index < b_feat_idx) )
	    		  j++;

	    	  if ( (j<alen) && (avec[j].feat_index == b_feat_idx) )
	    	  {
	    		  result+= bvec[i].entry * avec[j].entry;
	    		  j++;
	    	  }
	      }
	  }

	  if (inhomogene)
		  result+=1;

	  DREAL re=result;

	  for (INT j=1; j<degree; j++)
		  result*=re;
  }
  else
  {
	  if (inhomogene)
		  result=1.0;
  }

  ((CSparseRealFeatures*) lhs)->free_feature_vector(avec, idx_a, afree);
  ((CSparseRealFeatures*) rhs)->free_feature_vector(bvec, idx_b, bfree);

  return result/sqrt_both;
}
