#ifndef __MPBCONTROLPANEL_H__
#define __MPBCONTROLPANEL_H__


#include "MPBLibrary.h"


class MPBControlPanel
{
	public:

		MPBControlPanel::MPBControlPanel(
			MPBFLOAT p_x,
			MPBFLOAT p_y,
			MPBFLOAT p_width,
			MPBFLOAT p_height,
			char* p_texture
		):
			x(p_x),
			y(p_y),
			width(p_width),
			height(p_height),
			tcx0(0),
			tcy0(0),
			tcx1(1),
			tcy1(1)
		{
			strcpy( texture, p_texture );
		}

		MPBFLOAT x;
		MPBFLOAT y;
		MPBFLOAT width;
		MPBFLOAT height;
		char texture[MAX_PATH];
		MPBFLOAT tcx0;
		MPBFLOAT tcy0;
		MPBFLOAT tcx1;
		MPBFLOAT tcy1;
};







#endif
