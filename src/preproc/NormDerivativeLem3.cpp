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

#include "preproc/NormDerivativeLem3.h"
#include "preproc/RealPreProc.h"
#include "features/Features.h"
#include "features/RealFeatures.h"

CNormDerivativeLem3::CNormDerivativeLem3()
  : CRealPreProc("NormDerivativeLem3", "NDL3") 
{
}

CNormDerivativeLem3::~CNormDerivativeLem3()
{
}

/// initialize preprocessor from features
bool CNormDerivativeLem3::init(CFeatures* f)
{
	ASSERT(f->get_feature_class() == C_SIMPLE);
	ASSERT(f->get_feature_type() == F_DREAL);
	return true;
}

/// initialize preprocessor from features
void CNormDerivativeLem3::cleanup()
{
}

/// initialize preprocessor from file
bool CNormDerivativeLem3::load(FILE* f)
{
	return false;
}

/// save preprocessor init-data to file
bool CNormDerivativeLem3::save(FILE* f)
{
	return false;
}

/// apply preproc on feature matrix
/// result in feature matrix
/// return pointer to feature_matrix, i.e. f->get_feature_matrix();
DREAL* CNormDerivativeLem3::apply_to_feature_matrix(CFeatures* f)
{
	return NULL;
}

/// apply preproc on single feature vector
/// result in feature matrix
DREAL* CNormDerivativeLem3::apply_to_feature_vector(DREAL* f, INT len)
{
	return NULL;
}

//#warning TODO implement jahau 
//#ifdef JaaHau
// //this is the normalization used in jaahau
//    INT o_p=1;
//    double sum_p=0;
//    double sum_q=0;
//    //first do positive model
//    for (i=0; i<pos->get_N(); i++)
//    {
//	featurevector[p]=exp(pos->model_derivative_p(i, x)-posx);
//	sum_p=exp(pos->get_p(i))*featurevector[p++];
//	featurevector[p]=exp(pos->model_derivative_q(i, x)-posx);
//	sum_q=exp(pos->get_q(i))*featurevector[p++];
//
//	double sum_a=0;
//	for (j=0; j<pos->get_N(); j++)
//	{
//	    featurevector[p]=exp(pos->model_derivative_a(i, j, x)-posx);
//	    sum_a=exp(pos->get_a(i,j))*featurevector[p++];
//	}
//	p-=pos->get_N();
//	for (j=0; j<pos->get_N(); j++)
//	    featurevector[p++]-=sum_a;
//
//	double sum_b=0;
//	for (j=0; j<pos->get_M(); j++)
//	{
//	    featurevector[p]=exp(pos->model_derivative_b(i, j, x)-posx);
//	    sum_b=exp(pos->get_b(i,j))*featurevector[p++];
//	}
//	p-=pos->get_M();
//	for (j=0; j<pos->get_M(); j++)
//	    featurevector[p++]-=sum_b;
//    }
//
//    o_p=p;
//    p=1;
//    for (i=0; i<pos->get_N(); i++)
//    {
//	featurevector[p++]-=sum_p;
//	featurevector[p++]-=sum_q;
//    }
//    p=o_p;
//
//    for (i=0; i<neg->get_N(); i++)
//    {
//	featurevector[p]=-exp(neg->model_derivative_p(i, x)-negx);
//	sum_p=exp(neg->get_p(i))*featurevector[p++];
//	featurevector[p]=-exp(neg->model_derivative_q(i, x)-negx);
//	sum_q=exp(neg->get_q(i))*featurevector[p++];
//
//	double sum_a=0;
//	for (j=0; j<neg->get_N(); j++)
//	{
//	    featurevector[p]=-exp(neg->model_derivative_a(i, j, x)-negx);
//	    sum_a=exp(neg->get_a(i,j))*featurevector[p++];
//	}
//	p-=neg->get_N();
//	for (j=0; j<neg->get_N(); j++)
//	    featurevector[p++]-=sum_a;
//
//	double sum_b=0;
//	for (j=0; j<neg->get_M(); j++)
//	{
//	    featurevector[p]=-exp(neg->model_derivative_b(i, j, x)-negx);
//	    sum_b=exp(neg->get_b(i,j))*featurevector[p++];
//	}
//	p-=neg->get_M();
//	for (j=0; j<neg->get_M(); j++)
//	    featurevector[p++]-=sum_b;
//    }
//
//    p=o_p;
//    for (i=0; i<neg->get_N(); i++)
//    {
//	featurevector[p++]-=sum_p;
//	featurevector[p++]-=sum_q;
//    }
//#endif
