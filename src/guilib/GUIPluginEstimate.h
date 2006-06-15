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

#ifndef _GUIPLUGINESTIMATE_H__
#define _GUIPLUGINESTIMATE_H__ 

#include "classifier/PluginEstimate.h"
#include "features/Labels.h"

class CGUI;

class CGUIPluginEstimate
{

public:
	CGUIPluginEstimate(CGUI* g);
	~CGUIPluginEstimate();

	bool new_estimator(CHAR* param);
	bool train(CHAR* param);
	bool marginalized_train(CHAR* param);
	bool test(CHAR* param);
	bool load(CHAR* param);
	bool save(CHAR* param);

	inline CPluginEstimate* get_estimator() { return estimator; }

	CLabels* classify(CLabels* output=NULL);
	DREAL classify_example(INT idx);

 protected:
	CGUI* gui;

	CPluginEstimate* estimator;
	DREAL pos_pseudo;
	DREAL neg_pseudo;
};
#endif
