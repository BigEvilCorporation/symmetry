#pragma once

#include "wx/wxprec.h"
#include <wx/setup.h>
#include <wx/wx.h>

#include <wx/statusbr.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/string.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/menu.h>
#include <wx/stattext.h>
#include <wx/choice.h>
#include <wx/spinctrl.h>
#include <wx/toolbar.h>
#include <wx/panel.h>
#include <wx/sizer.h>

#include <wx/propgrid/propgrid.h>
#include <wx/propgrid/manager.h>
#include <wx/propgrid/propdev.h>

class wxVector3f
{
public:
    wxVector3f()
    {
        x = y = z = 0.0;
    }

    wxVector3f( double x, double y, double z )
    {
        x = x; y = y; z = z;
    }
	
	void operator=(const wxVector3f &v)
	{
		x = v.x; y = v.y; z = v.z;
	}

	bool operator==(const wxVector3f &v)
	{
		return x == v.x && y == v.y && z == v.z;
	}

    double x, y, z;
};

/*
bool operator == (const wxVector3f& a, const wxVector3f& b)
{
    return (a.x == b.x && a.y == b.y && a.z == b.z);
}
*/

WX_PG_DECLARE_VALUE_TYPE_VOIDP(wxVector3f)

WX_PG_DECLARE_PROPERTY(wxVectorProperty,const wxVector3f&,wxVector3f())