/*
	Licensed under the MIT license.
	Made for Haiku.
*/

#include <Screen.h>

#include "LabelView.h"


LabelView::LabelView(const char* name, const char* text, uint32 flags)
	:
	BStringView(name, text, flags)
{

}


LabelView::LabelView(BMessage* archive)
	: 
	BStringView(archive) 
{ 

}


status_t 
LabelView::Archive(BMessage* archive, bool deep) const 
{					
	status_t status = BStringView::Archive(archive, deep);
	return status;		
}


void
LabelView::Draw(BRect updateRect)
{	
	rgb_color	color = HighColor();
	if (ViewColor() == B_TRANSPARENT_COLOR) {
		rgb_color low =  BScreen(Window()).DesktopColor();
	
		if (low.red + low.green + low.blue > 128 * 3) {
			color = tint_color(low, B_DARKEN_MAX_TINT);
		} else {
			color = tint_color(low, B_LIGHTEN_MAX_TINT);
		}
		color.alpha = fLights ? 255 : 50;
	} else
	{
		if (fLights) {
			color = tint_color(make_color(128,128,128), B_DARKEN_MAX_TINT);
		} else {
			color = tint_color(make_color(128,128,128), B_LIGHTEN_MAX_TINT);
		}
	} 
	
	
	SetHighColor(color);	
	drawing_mode oldMode = DrawingMode();
	SetDrawingMode(B_OP_ALPHA);
	BStringView::Draw(updateRect);
	SetDrawingMode(oldMode);
}


BArchivable*
LabelView::Instantiate(BMessage* archive)
{
	if (!validate_instantiation(archive, "LabelView"))
		return NULL;

	return new LabelView(archive);
}
