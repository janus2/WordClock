/*
	Licensed under the MIT license.
	Made for Haiku.
*/

#include <AppKit.h>
#include <Bitmap.h>
#include <Catalog.h>
#include <FindDirectory.h>
#include <Font.h>
#include <GroupLayout.h>

#include <Menu.h>
#include <MenuBar.h>
#include <MenuItem.h>
#include <Path.h>
#include <TranslationUtils.h>

#include <stdio.h>

#include "MainWindow.h"

#undef B_TRANSLATION_CONTEXT
#define B_TRANSLATION_CONTEXT "MainWindow"

BMenuBar*
MainWindow::_PrepareMenuBar(void)
{
	BMenuBar *menubar = new BMenuBar("menu");
	// Add the "Font"-menu:
	fFontMenu = new BMenu(B_TRANSLATE("Font"));
	menubar->AddItem(fFontMenu);

	// "Size"-subMenu
	fFontSizeMenu = new BMenu(B_TRANSLATE("Size"));
	fFontSizeMenu->SetRadioMode(true);
	fFontMenu->AddItem(fFontSizeMenu);
	BMenuItem *menuItem;
	const int32 fontSizes[] = {8, 9, 10, 11, 12, 13, 14, 16, 18, 19, 20, 21, 24, 32, 36, 48, 72};
	for (uint32 i = 0; i < sizeof(fontSizes) / sizeof(fontSizes[0]); i++) {
		BMessage* fontMessage = new BMessage(kFontSize);
		fontMessage->AddFloat("size", fontSizes[i]);

		char label[64];
		snprintf(label, sizeof(label), "%" B_PRId32, fontSizes[i]);
		fFontSizeMenu->AddItem(menuItem = new BMenuItem(label, fontMessage));

		if (fontSizes[i] == (int32)be_plain_font->Size())
			menuItem->SetMarked(true);
	}

	fFontMenu->AddSeparatorItem();

	// Available fonts

	fCurrentFontItem = 0;
	fCurrentStyleItem = 0;

	BMenu* subMenu;
	int32 numFamilies = count_font_families();
	for (int32 i = 0; i < numFamilies; i++) {
		font_family family;
		if (get_font_family(i, &family) == B_OK) {
			subMenu = new BMenu(family);
			subMenu->SetRadioMode(true);
			fFontMenu->AddItem(new BMenuItem(subMenu,
				new BMessage(kFontFamily)));

			int32 numStyles = count_font_styles(family);
			for (int32 j = 0; j < numStyles; j++) {
				font_style style;
				uint32 flags;
				if (get_font_style(family, j, &style, &flags) == B_OK) {
					subMenu->AddItem(new BMenuItem(style,
						new BMessage(kFontStyle)));
				}
			}
		}
	}
	return menubar;
}


MainWindow::MainWindow()
	:
	BWindow(BRect(150, 150, 0, 0), B_TRANSLATE_SYSTEM_NAME("WordClock"),
		B_TITLED_WINDOW, B_NOT_RESIZABLE
		| B_NOT_ZOOMABLE | B_ASYNCHRONOUS_CONTROLS | B_QUIT_ON_WINDOW_CLOSE
		| B_AUTO_UPDATE_SIZE_LIMITS )
{
	BGroupLayout* root = new BGroupLayout(B_VERTICAL);
	root->SetSpacing(0);
	SetLayout(root);

	AddChild(_PrepareMenuBar());

	BMessage settings;
	_LoadSettings(settings);

	fClockView = new ClockView(BRect(0,0,100,100), &settings);
	AddChild(fClockView);
}


void
MainWindow::MessageReceived(BMessage *msg)
{
	switch (msg->what) {
	case kFontSize:
		{
			float fontSize;
			if (msg->FindFloat("size", &fontSize) == B_OK) {
				fClockView->SetFontSize(fontSize);				
			}
			fClockView->InvalidateLayout(true);
			break;
		}
	case kFontFamily:
		{
			const char* fontFamily = NULL;
			const char* fontStyle = NULL;
			void* ptr;
			if (msg->FindPointer("source", &ptr) == B_OK) {
				BMenuItem* item = static_cast<BMenuItem*>(ptr);
				fontFamily = item->Label();
			}

			BFont font;
			font.SetFamilyAndStyle(fontFamily, fontStyle);
			_SetFontStyle(fontFamily, fontStyle);
			break;
		}
		case kFontStyle:
		{
			const char* fontFamily = NULL;
			const char* fontStyle = NULL;
			void* ptr;
			if (msg->FindPointer("source", &ptr) == B_OK) {
				BMenuItem* item = static_cast<BMenuItem*>(ptr);
				fontStyle = item->Label();
				BMenu* menu = item->Menu();
				if (menu != NULL) {
					BMenuItem* super_item = menu->Superitem();
					if (super_item != NULL)
						fontFamily = super_item->Label();
				}
			}

			BFont font;
			font.SetFamilyAndStyle(fontFamily, fontStyle);
			_SetFontStyle(fontFamily, fontStyle);
			break;
		}
	default:
		BWindow::MessageReceived(msg);
	}
}


void
MainWindow::_SetFontStyle(const char* fontFamily, const char* fontStyle)
{
	BFont font;
	// find out what the old font was
	font_family oldFamily;
	font_style oldStyle;
	fClockView->GetFont(&font);
	
	font.GetFamilyAndStyle(&oldFamily, &oldStyle);

	// clear that family's bit on the menu, if necessary
	if (strcmp(oldFamily, fontFamily)) {
		BMenuItem* oldItem = fFontMenu->FindItem(oldFamily);
		if (oldItem != NULL) {
			oldItem->SetMarked(false);
			BMenu* menu = oldItem->Submenu();
			if (menu != NULL) {
				oldItem = menu->FindItem(oldStyle);
				if (oldItem != NULL)
					oldItem->SetMarked(false);
			}
		}
	}

	font.SetFamilyAndStyle(fontFamily, fontStyle);
/*
	uint16 face = 0;

	if (!(font.Face() & B_REGULAR_FACE))
		face = font.Face();

	if (fBoldItem->IsMarked())
		face |= B_BOLD_FACE;

	if (fItalicItem->IsMarked())
		face |= B_ITALIC_FACE;

	font.SetFace(face);
*/	
	fClockView->SetFontStyle(&font, B_FONT_FAMILY_AND_STYLE);

	BMenuItem* superItem;
	superItem = fFontMenu->FindItem(fontFamily);
	if (superItem != NULL) {
		superItem->SetMarked(true);
		fCurrentFontItem = superItem;
	}

}


void
MainWindow::MenusBeginning()
{
	// update the font menu
	// unselect the old values
	if (fCurrentFontItem != NULL) {
		fCurrentFontItem->SetMarked(false);
		BMenu* menu = fCurrentFontItem->Submenu();
		if (menu != NULL) {
			BMenuItem* item = menu->FindMarked();
			if (item != NULL)
				item->SetMarked(false);
		}
	}
	
	BMenuItem* oldSizeItem = fFontSizeMenu->FindMarked();
	if (oldSizeItem != NULL)
		oldSizeItem->SetMarked(false);
	

	if (fCurrentStyleItem != NULL) {
		fCurrentStyleItem->SetMarked(false);
	}
	
	BFont font;
	fClockView->GetFont(&font);

	if ((int)font.Size() == font.Size()) {
		// select the current font size
		char fontSizeStr[16];
		snprintf(fontSizeStr, 15, "%i", (int)font.Size());
		BMenuItem* item = fFontSizeMenu->FindItem(fontSizeStr);
		if (item != NULL)
			item->SetMarked(true);
	}

	font_family family;
	font_style style;
	font.GetFamilyAndStyle(&family, &style);

	fCurrentFontItem = fFontMenu->FindItem(family);

	if (fCurrentFontItem != NULL) {
		fCurrentFontItem->SetMarked(true);
		BMenu* menu = fCurrentFontItem->Submenu();
		if (menu != NULL) {
			BMenuItem* item = menu->FindItem(style);
			fCurrentStyleItem = item;
			if (fCurrentStyleItem != NULL)
				item->SetMarked(true);
		}
	}

}

status_t
MainWindow::_LoadSettings(BMessage& m)
{
	BPath p;
	BFile f;

	if (find_directory(B_USER_SETTINGS_DIRECTORY, &p) != B_OK)
		return B_ERROR;
	p.Append(kSettingsFileName);

	f.SetTo(p.Path(), B_READ_ONLY);
	if (f.InitCheck() != B_OK)
		return B_ERROR;

	if (m.Unflatten(&f) != B_OK)
		return B_ERROR;

	if (m.what != kSettingsMessage)
		return B_ERROR;

	if (m.FindRect("fMainWindowRect", &fMainWindowRect) != B_OK)
		fMainWindowRect = kDefaultMainWindowRect;

	MoveTo(fMainWindowRect.LeftTop());

	return B_OK;
}


status_t
MainWindow::_SaveSettings()
{
	BPath p;
	BFile f;
	BMessage m(kSettingsMessage);

	fClockView->SaveState(&m);

	m.AddRect("fMainWindowRect", Frame());

	if (find_directory(B_USER_SETTINGS_DIRECTORY, &p) != B_OK)
		return B_ERROR;
	p.Append(kSettingsFileName);

	f.SetTo(p.Path(), B_WRITE_ONLY | B_ERASE_FILE | B_CREATE_FILE);
	if (f.InitCheck() != B_OK)
		return B_ERROR;

	if (m.Flatten(&f) != B_OK)
		return B_ERROR;

	return B_OK;
}


bool MainWindow::QuitRequested()
{
	_SaveSettings();
	return true;
}
