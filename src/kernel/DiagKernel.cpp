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

#include "lib/config.h"

#ifdef HAVE_LAPACK
extern "C" {
#include <cblas.h>
}
#endif

#include "lib/common.h"
#include "kernel/DiagKernel.h"
#include "lib/io.h"

CDiagKernel::CDiagKernel(LONG size, DREAL d)
  : CKernel(size),diag(d)
{
}

CDiagKernel::~CDiagKernel() 
{
}
  
void CDiagKernel::cleanup()
{
}

bool CDiagKernel::load_init(FILE* src)
{
	return false;
}

bool CDiagKernel::save_init(FILE* dest)
{
	return false;
}

