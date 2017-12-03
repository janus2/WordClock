/*
	Licensed under the MIT license.
	Made for Haiku.
*/

#ifndef _MAINWINDOW_H_
#define _MAINWINDOW_H_


#include <Bitmap.h>
#include <Button.h>
#include <GridLayout.h>
#include <GridView.h>
#include <GroupView.h>
#include <MenuBar.h>
#include <Resources.h>
#include <String.h>
#include <StringView.h>
#include <View.h>
#include <Window.h>

#include "ClockView.h"

const BRect kDefaultMainWindowRect = BRect(150,150,0,0);
const uint32 kFontSize = 'FoSz';
const uint32 kFontStyle = 'FoSy';
const uint32 kFontFamily = 'FoFa';



class MainWindow : public BWindow {
public:
					MainWindow(void);
	virtual void	MessageReceived(BMessage* msg);
	virtual bool	QuitRequested();
	virtual void	MenusBeginning();
	
private:
	status_t		_LoadSettings(BMessage& settings);
	status_t		_SaveSettings();
	BMenuBar*		_PrepareMenuBar(void);
	void 			_SetFontStyle(const char* fontFamily, const char* fontStyle);
	
	ClockView*		fClockView;

	BRect			fMainWindowRect;	

	BMenu*				fFontMenu;
	BMenu*				fFontSizeMenu;
	BMenu*				fFontColorMenu;
	BMenuItem*			fCurrentFontItem;
	BMenuItem*			fCurrentStyleItem;
	
};

#endif // _MAINWINDOW_H_
