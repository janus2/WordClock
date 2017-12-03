/*
	Licensed under the MIT license.
	Made for Haiku.
*/
#ifndef _CLOCKVIEW_H_
#define _CLOCKVIEW_H_


#include <Bitmap.h>
#include <Button.h>
#include <DateFormat.h>
#include <Dragger.h>
#include <FormattingConventions.h>
#include <GridLayout.h>
#include <GridView.h>
#include <GroupView.h>
#include <MenuBar.h>
#include <Resources.h>
#include <String.h>
#include <StringView.h>
#include <View.h>
#include <Window.h>

#include "LabelView.h"

const uint32 kAutoUpdateMessage = 'Au.U';
const uint32 kUpdateMessage = 'Upd.';
const uint32 kSettingsMessage = 'Pref';

extern const char* kSettingsFileName;

class _EXPORT ClockView;


class ClockView : public BView {
public:
					ClockView(BRect frame, BMessage* settings);
					ClockView(BMessage* archive);
	virtual			~ClockView();
	virtual void	MessageReceived(BMessage* msg);
	virtual void	AttachedToWindow();
	virtual void	AllAttached();
			void    Update();
virtual status_t	Archive(BMessage* into, bool deep = true) const;
static	BArchivable* Instantiate(BMessage* archive);
status_t			SaveState(BMessage* into, bool deep = true) const;



	void			SetUpdateDelay(int32 delay);
	int32			UpdateDelay();

	void			SetTextColor(rgb_color color);
	void			SetBackgroundColor(rgb_color color);
	bool			IsDefaultColor() const;
	bool			IsConnected() const;
	
	void 			SetFontSize(float fontSize) {
						BFont font;
						font.SetSize(fontSize);
						for (int i = 0; i < 22; i++)  {
							fWords[i]->SetFont(&font, B_FONT_SIZE);
						}
						SetFont(&font, B_FONT_SIZE);
					}
	void 			SetFontStyle(const BFont* font, uint16 fontFace) {
					SetFont(font, fontFace);
					for (int i = 0; i < 22; i++)  {
							fWords[i]->SetFont(font, fontFace);
					}
					}								
					
private:
	void			_Init();
	status_t		_ApplyState(BMessage *settings);
	
	bool			_SupportTransparent();

	bool			fReplicated;

	BMessageRunner*	fAutoUpdate;
	BDragger* 		fDragger;
	rgb_color		fBackgroundColor;
	rgb_color		fTextColor;

	LabelView*		fWords[23];
};


#endif // _CLOCKVIEW_H_
