/*
	Licensed under the MIT license.
	Made for Haiku.
*/
#include <Alert.h>
#include <AppKit.h>
#include <Catalog.h>
#include <ControlLook.h>
#include <DateTime.h>
#include <FindDirectory.h>
#include <Font.h>
#include <GroupLayout.h>
#include <LayoutBuilder.h>
#include <Locale.h>
#include <Menu.h>
#include <MenuBar.h>
#include <MenuItem.h>
#include <PopUpMenu.h>
#include <TranslationUtils.h>

#include "ClockView.h"


const float kDraggerSize = 7;
const char* kSettingsFileName = "WordClock settings";

extern const char* kSignature;

#undef B_TRANSLATION_CONTEXT
#define B_TRANSLATION_CONTEXT "ClockView"



enum clockwords {
	words_ITIS,
	words_HALF,
	words_TEN2,
	words_QUARTER,
	words_TWENTY,
	words_FIVE2,
	words_MINUTES,
	words_TO,
	words_PAST,
	words_ONE,
	words_THREE,
	words_TWO,
	words_FOUR,
	words_FIVE,
	words_SIX,
	words_SEVEN,
	words_EIGHT,
	words_NINE,
	words_TEN,
	words_ELEVEN,
	words_TWELVE,
	words_OCLOCK
};


ClockView::ClockView(BRect frame, BMessage* settings)
	:
	BView(frame, B_TRANSLATE_SYSTEM_NAME("WordClock"), B_FOLLOW_NONE,
		B_WILL_DRAW | B_FRAME_EVENTS | B_DRAW_ON_CHILDREN),
	fReplicated(false),
	fAutoUpdate(NULL)
{
	_ApplyState(settings);
	_Init();
}


ClockView::~ClockView()
{
	delete fAutoUpdate;
}


void
ClockView::_Init()
{

	fWords[0] = new LabelView("w1", B_TRANSLATE("IT IS"));
	fWords[1] = new LabelView("w2", B_TRANSLATE("HALF"));
	fWords[2] = new LabelView("w3", B_TRANSLATE("TEN"));
	fWords[3] = new LabelView("w4", B_TRANSLATE("QUARTER"));
	fWords[4] = new LabelView("w5", B_TRANSLATE("TWENTY"));
	fWords[5] = new LabelView("w6", B_TRANSLATE("FIVE"));
	fWords[6] = new LabelView("w7", B_TRANSLATE("MINUTES"));
	fWords[7] = new LabelView("w8", B_TRANSLATE("TO"));
	fWords[8] = new LabelView("w9", B_TRANSLATE("PAST"));
	fWords[9] = new LabelView("w10", B_TRANSLATE("ONE"));
	fWords[10] = new LabelView("w11", B_TRANSLATE("THREE"));
	fWords[11] = new LabelView("w12", B_TRANSLATE("TWO"));
	fWords[12] = new LabelView("w13", B_TRANSLATE("FOUR"));
	fWords[13] = new LabelView("w14", B_TRANSLATE("FIVE"));
	fWords[14] = new LabelView("w15", B_TRANSLATE("SIX"));
	fWords[15] = new LabelView("w16", B_TRANSLATE("SEVEN"));
	fWords[16] = new LabelView("w17", B_TRANSLATE("EIGHT"));
	fWords[17] = new LabelView("w18", B_TRANSLATE("NINE"));
	fWords[18] = new LabelView("w19", B_TRANSLATE("TEN"));
	fWords[19] = new LabelView("w20", B_TRANSLATE("ELEVEN"));
	fWords[20] = new LabelView("w21", B_TRANSLATE("TWELVE"));
	fWords[21] = new LabelView("w22", B_TRANSLATE("O'CLOCK")); 

	BLayoutBuilder::Group<>(this, B_VERTICAL, 0)
		.SetInsets(B_USE_WINDOW_SPACING, B_USE_WINDOW_SPACING,
				B_USE_WINDOW_SPACING, B_USE_WINDOW_SPACING)
		.AddGroup(B_HORIZONTAL, 0) /*
			.SetInsets(0, B_USE_WINDOW_SPACING,
				B_USE_WINDOW_SPACING, B_USE_WINDOW_SPACING)
			.AddGroup(new BGroupView(B_VERTICAL, B_USE_ITEM_SPACING))
				.Add(message)*/
				.Add(fWords[0])
				.AddGlue()
				.Add(fWords[1])
				.AddGlue()
				.Add(fWords[2])
		.End()
		.AddGroup(B_HORIZONTAL, 0) 
				.Add(fWords[3])
				.AddStrut(B_USE_SMALL_SPACING)//.AddGlue()
				.Add(fWords[4])

		.End()
		.AddGroup(B_HORIZONTAL, 0)
				.Add(fWords[5])
				.AddGlue()
				.Add(fWords[6])
				.AddGlue()
				.Add(fWords[7])
		.End()
		.AddGroup(B_HORIZONTAL, 0) 
				.Add(fWords[8])
				.AddGlue()
				.Add(fWords[9])
				.AddGlue()
				.Add(fWords[10])
		.End()
		.AddGroup(B_HORIZONTAL, 0) 
				.Add(fWords[11])
				.AddGlue()
				.Add(fWords[12])
				.AddGlue()
				.Add(fWords[13])
		.End()
		.AddGroup(B_HORIZONTAL, 0) 
				.Add(fWords[14])
				.AddGlue()
				.Add(fWords[15])
				.AddGlue()
				.Add(fWords[16])
		.End()
		.AddGroup(B_HORIZONTAL, 0) 
				.Add(fWords[17])
				.AddGlue()
				.Add(fWords[18])
				.AddGlue()
				.Add(fWords[19])
		.End()
		.AddGroup(B_HORIZONTAL, 0) 
				.Add(fWords[20])
				.AddGlue()
				.Add(fWords[21])
		.End()
		.AddGroup(B_HORIZONTAL, 0) 
				.AddGlue()
				.Add(fDragger = new BDragger(this))
		.End()
	.End();
	SetViewColor(fBackgroundColor);
	fDragger->SetExplicitMinSize(BSize(kDraggerSize, kDraggerSize));
	fDragger->SetExplicitMaxSize(BSize(kDraggerSize, kDraggerSize));	

}


void
ClockView::Update() {
	
	BTime now = BTime::CurrentTime(B_LOCAL_TIME);
	// Compute the string
	
	for (int i = 0; i < 22; i++)
		fWords[i]->SetLights(false);
		
	int32 hours = now.Hour();
	int32 minutes = now.Minute();
	
	//cout << hours << minutes << endl;

	fWords[words_ITIS]->SetLights(true);
	
	
	int32 minute =  now.Minute();
			
	if (minute >= 55) 
		fWords[words_FIVE2]->SetLights(true);
	else
	if (minute >= 50)
		fWords[words_TEN2]->SetLights(true);
	else
	if (minute >= 45)
		fWords[words_QUARTER]->SetLights(true);
	else
	if (minute >= 40) {
		fWords[words_TWENTY]->SetLights(true);
	}else
	if (minute >= 35) {
		fWords[words_TWENTY]->SetLights(true);
		fWords[words_FIVE2]->SetLights(true);
	}
	else	
	if (minute >= 30) {
		fWords[words_HALF]->SetLights(true);
	}
	else	
	if (minute >= 25) {
		fWords[words_TWENTY]->SetLights(true);
 		fWords[words_FIVE2]->SetLights(true);
	}
	else
	if (minute >= 20) {
		fWords[words_TWENTY]->SetLights(true);
	}
	else
	if (minute >= 15) {
		fWords[words_QUARTER]->SetLights(true);
	}
	else
	if (minute >= 10)
		fWords[words_TEN2]->SetLights(true);
	else
	if (minute >= 5)
		fWords[words_FIVE2]->SetLights(true);
	else
	if (minute >= 0)
		fWords[words_OCLOCK]->SetLights(true);
	
	if (minute < 30 && minute >=5) {
		fWords[words_PAST]->SetLights(true);	
	}
	else
	if (minute >= 35)
	{	
		hours++;
		fWords[words_TO]->SetLights(true);
	}
	
	if (hours >= 12)
		hours %= 12;
	switch (hours) {
		case 1 : fWords[words_ONE]->SetLights(true);
			break;
		case 2 : fWords[words_TWO]->SetLights(true);
			break;
		case 3 : fWords[words_THREE]->SetLights(true);
			break;
		case 4 : fWords[words_FOUR]->SetLights(true);
			break;
		case 5 : fWords[words_FIVE]->SetLights(true);
			break;
		case 6 : fWords[words_SIX]->SetLights(true);
			break;
		case 7 : fWords[words_SEVEN]->SetLights(true);
			break;
		case 8 : fWords[words_EIGHT]->SetLights(true);
			break;
		case 9 : fWords[words_NINE]->SetLights(true);
			break;
		case 10 : fWords[words_TEN]->SetLights(true);
			break;
		case 11 : fWords[words_ELEVEN]->SetLights(true);
			break;
		case 0 : fWords[words_TWELVE]->SetLights(true);
			break;
	}
	
	if (!fWords[words_OCLOCK]->Lights() 
		&& !fWords[words_HALF]->Lights()
		&& !fWords[words_QUARTER]->Lights())
	fWords[words_MINUTES]->SetLights(true);
	
	Invalidate();
}


BArchivable*
ClockView::Instantiate(BMessage* archive)
{
	if (!validate_instantiation(archive, "ClockView"))
		return NULL;

	return new ClockView(archive);
}


ClockView::ClockView(BMessage* archive)
	:
	BView(archive),
	fReplicated(true)
{
	_ApplyState(archive);
	fWords[0] =  dynamic_cast<LabelView *>(FindView("w1"));
	fWords[1] =  dynamic_cast<LabelView *>(FindView("w2"));
	fWords[2] =  dynamic_cast<LabelView *>(FindView("w3"));
	fWords[3] =  dynamic_cast<LabelView *>(FindView("w4"));
	fWords[4] =  dynamic_cast<LabelView *>(FindView("w5"));
	fWords[5] =  dynamic_cast<LabelView *>(FindView("w6"));
	fWords[6] =  dynamic_cast<LabelView *>(FindView("w7"));
	fWords[7] =  dynamic_cast<LabelView *>(FindView("w8"));
	fWords[8] =  dynamic_cast<LabelView *>(FindView("w9"));
	fWords[9] =  dynamic_cast<LabelView *>(FindView("w10"));
	fWords[10] =  dynamic_cast<LabelView *>(FindView("w11"));
	fWords[11] =  dynamic_cast<LabelView *>(FindView("w12"));
	fWords[12] =  dynamic_cast<LabelView *>(FindView("w13"));
	fWords[13] =  dynamic_cast<LabelView *>(FindView("w14"));
	fWords[14] =  dynamic_cast<LabelView *>(FindView("w15"));
	fWords[15] =  dynamic_cast<LabelView *>(FindView("w16"));
	fWords[16] =  dynamic_cast<LabelView *>(FindView("w17"));
	fWords[17] =  dynamic_cast<LabelView *>(FindView("w18"));
	fWords[18] =  dynamic_cast<LabelView *>(FindView("w19"));
	fWords[19] =  dynamic_cast<LabelView *>(FindView("w20"));
	fWords[20] =  dynamic_cast<LabelView *>(FindView("w21"));
	fWords[21] =  dynamic_cast<LabelView *>(FindView("w22")); 
}


status_t
ClockView::_ApplyState(BMessage* archive)
{
	rgb_color *color;
	ssize_t colorsize;
	status_t status;
	const char * fontFamily;
	const char * fontStyle;

	float fontSize;
	
	BFont font(be_plain_font);

	status = archive->FindString("fontFamily", &fontFamily);
	if (status == B_OK)
		status = archive->FindString("fontStyle", &fontStyle);
	if (status == B_OK)
		font.SetFamilyAndStyle(fontFamily, fontStyle);
	status = archive->FindFloat("fontSize", &fontSize);
		if (status == B_OK)
			font.SetSize(fontSize);
	SetFont(&font);	
	
	status = archive->FindData("backgroundColor", B_RGB_COLOR_TYPE, (const void **)&color, &colorsize);
	fBackgroundColor = (status == B_NO_ERROR) ? *color : (fReplicated ? B_TRANSPARENT_COLOR : ui_color(B_PANEL_BACKGROUND_COLOR));

	status = archive->FindData("textColor", B_RGB_COLOR_TYPE, (const void **)&color, &colorsize);
	fTextColor = (status == B_NO_ERROR) ? *color : ui_color(B_PANEL_TEXT_COLOR);

	return B_OK;
}


status_t
ClockView::Archive(BMessage* into, bool deep) const
{
	status_t status;

	status = BView::Archive(into, true);
	if (status < B_OK)
		return status;

	status = into->AddString("add_on", kSignature);
	if (status < B_OK)
		return status;

	status = SaveState(into, deep);
	if (status < B_OK)
		return status;

	return B_OK;
}


status_t
ClockView::SaveState(BMessage* into, bool deep) const
{
	BFont font;
	GetFont(&font);
	
	font_family fontFamily;
	font_style fontStyle;
	
	font.GetFamilyAndStyle(&fontFamily, &fontStyle);
	float fontSize = font.Size();	
	status_t status;
	status = into->AddString("fontFamily", fontFamily);
		if (status != B_OK)
			return status;
	status = into->AddString("fontStyle", fontStyle);
		if (status != B_OK)
			return status;
	status = into->AddFloat("fontSize", fontSize);
		if (status != B_OK)
			return status;

	if (!IsDefaultColor() || fReplicated) {
		status = into->AddData("textColor", B_RGB_COLOR_TYPE, &fTextColor, sizeof(rgb_color));
		if (status != B_OK)
			return status;
		status = into->AddData("backgroundColor", B_RGB_COLOR_TYPE, &fBackgroundColor, sizeof(rgb_color));
		if (status != B_OK)
			return status;
	}
	return B_OK;
}


void
ClockView::AttachedToWindow()
{
	BMessenger view(this);
	BMessage autoUpdateMessage(kAutoUpdateMessage);
	fAutoUpdate = new BMessageRunner(view,  &autoUpdateMessage, (bigtime_t) 10 * 1000 * 1000);
	view.SendMessage(new BMessage(kUpdateMessage));
	BView::AttachedToWindow();
}


void
ClockView::AllAttached()
{
	BView::AllAttached();
	SetTextColor(fTextColor);
	if (!_SupportTransparent() && fBackgroundColor == B_TRANSPARENT_COLOR)
			fBackgroundColor = ui_color(B_PANEL_BACKGROUND_COLOR);
	SetBackgroundColor(fBackgroundColor);
	
	// Set the font for all words
	// TODO Improve this set 2 time
	BFont font;
	GetFont(&font);
	SetFontSize(font.Size());
	SetFontStyle(&font, B_FONT_FAMILY_AND_STYLE);

}


bool
ClockView::_SupportTransparent() {
	return 	fReplicated && Parent() && (Parent()->Flags() & B_DRAW_ON_CHILDREN) != 0;
}


void
ClockView::MessageReceived(BMessage *msg)
{
	// msg->PrintToStream();
	if (msg->WasDropped()) {
		rgb_color* color = NULL;
		ssize_t size = 0;

		// Disabled for now
		if (false && msg->FindData("RGBColor", (type_code)'RGBC', (const void**)&color,
				&size) == B_OK) {
			BPoint point;
			uint32	buttons;
			GetMouse(&point, &buttons, true);
			BMenuItem* item;
			BPopUpMenu* popup = new BPopUpMenu("PopUp", false);
			popup->AddItem(item = new BMenuItem(B_TRANSLATE("Background"), new BMessage('BACC')));
			popup->AddItem(item = new BMenuItem(B_TRANSLATE("Text"), new BMessage('TEXC')));
			popup->AddSeparatorItem();
			popup->AddItem(item = new BMenuItem(B_TRANSLATE("Default"), new BMessage('DEFT')));
			item->SetEnabled(!IsDefaultColor());
			if (_SupportTransparent()) {
				popup->AddItem(item = new BMenuItem(B_TRANSLATE("Transparent"), new BMessage('TRAS')));
				item->SetEnabled(ViewColor() != B_TRANSPARENT_COLOR);
			}
			ConvertToScreen(&point);
			item = popup->Go(point);

			if (item && item->Message()->what == 'BACC')
				SetBackgroundColor(*color);

			if (item && item->Message()->what == 'TEXC')
				SetTextColor(*color);

			if (item && item->Message()->what == 'DEFT') {
				SetBackgroundColor(ui_color(B_PANEL_BACKGROUND_COLOR));
				SetTextColor(ui_color(B_PANEL_TEXT_COLOR));
			}
			if (item && item->Message()->what == 'TRAS') {
				SetBackgroundColor(B_TRANSPARENT_COLOR);
			}
			return;
		}
	}

	switch (msg->what) {
	case kUpdateMessage:
	case kAutoUpdateMessage:
		Update();
		break;
	case B_ABOUT_REQUESTED: {
		BAlert *alert = new BAlert(B_TRANSLATE("About WorldClock"),
			B_TRANSLATE("WordClock (The Replicant version)"),
			B_TRANSLATE("OK"));
		alert->SetFlags(alert->Flags() | B_CLOSE_ON_ESCAPE);
		alert->Go();
		break;
	}
	default:
		BView::MessageReceived(msg);
	}
}


void
ClockView::SetTextColor(rgb_color color)
{
	fTextColor = color;
	SetHighColor(color);
	return;
}


void
ClockView::SetBackgroundColor(rgb_color color)
{
	fBackgroundColor = color;
	SetViewColor(color);
	for (int32 i = 0; i < 22; i++) {
		fWords[i]->SetViewColor(color);
	}
	return;
	fDragger->Invalidate();
}


bool
ClockView::IsDefaultColor() const
{
	return fBackgroundColor == ui_color(B_PANEL_BACKGROUND_COLOR)
		&& fTextColor == ui_color(B_PANEL_TEXT_COLOR);
}
