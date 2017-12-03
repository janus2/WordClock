/*
	Licensed under the MIT license.
	Made for Haiku.
*/
#ifndef _LABELVIEW_H_
#define _LABELVIEW_H_


#include <StringView.h>
class _EXPORT LabelView;

class LabelView : public BStringView {
public:
			LabelView(const char* name, const char* text,
									uint32 flags = B_WILL_DRAW);
			LabelView(BMessage* archive);
			virtual ~LabelView() {};
			virtual	status_t Archive(BMessage* archive,
									bool deep = true) const;
			virtual void Draw (BRect updateRect);
			void SetLights(bool value) {fLights = value;};
			bool Lights() {return fLights;};
			
static		BArchivable* Instantiate(BMessage* archive);

private:	

			bool fLights;
};



#endif // _LABELVIEW_H_
