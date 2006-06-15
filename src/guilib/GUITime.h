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

#ifndef __GUITIME__H_
#define __GUITIME__H_

#include "lib/Time.h"

class CGUI;

class CGUITime
{
	public:
		CGUITime(CGUI *);
		~CGUITime();

		void start();
		void stop();

	protected:
		CGUI* gui;
		CTime* time;
};
#endif
