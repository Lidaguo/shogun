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

#include "preproc/LogPlusOne.h"
#include "preproc/RealPreProc.h"
#include "features/Features.h"
#include "features/RealFeatures.h"
#include "lib/Mathmatics.h"

CLogPlusOne::CLogPlusOne() : CRealPreProc("LogPlusOne", "LG+1") 
{
}

CLogPlusOne::~CLogPlusOne()
{
}

/// initialize preprocessor from features
bool CLogPlusOne::init(CFeatures* f)
{
	ASSERT(f->get_feature_class() == C_SIMPLE);
	ASSERT(f->get_feature_type() == F_DREAL);
	return true;
}

/// clean up allocated memory
void CLogPlusOne::cleanup()
{
}

/// initialize preprocessor from file
bool CLogPlusOne::load(FILE* f)
{
	return false;
}

/// save preprocessor init-data to file
bool CLogPlusOne::save(FILE* f)
{
	return false;
}

/// apply preproc on feature matrix
/// result in feature matrix
/// return pointer to feature_matrix, i.e. f->get_feature_matrix();
DREAL* CLogPlusOne::apply_to_feature_matrix(CFeatures* f)
{
	INT i,j;
	INT num_vec;
	INT num_feat;
	DREAL* matrix=((CRealFeatures*) f)->get_feature_matrix(num_feat, num_vec);

	for (i=0; i<num_vec; i++)
	{
		DREAL* vec=&matrix[i*num_feat];

		for (j=0; j<num_feat; j++)
			vec[j]=log(vec[j]+1);
	}
	return matrix;
}

/// apply preproc on single feature vector
/// result in feature matrix
DREAL* CLogPlusOne::apply_to_feature_vector(DREAL* f, INT& len)
{
	DREAL* vec=new DREAL[len];
	INT i=0;

	for (i=0; i<len; i++)
		vec[i]=log(f[i]+1);

	return vec;
}

/// initialize preprocessor from file
bool CLogPlusOne::load_init_data(FILE* src)
{
	return true;
}

/// save init-data (like transforamtion matrices etc) to file
bool CLogPlusOne::save_init_data(FILE* dst)
{
	return true;
}
