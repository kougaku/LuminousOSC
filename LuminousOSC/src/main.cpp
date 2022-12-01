#include "ofMain.h"
#include "ofApp.h"

#ifdef TARGET_WIN32
// --------------------------------------------------------------------------------------------------
// for Windows
// リンカーの設定（すべてのオプション）でサブシステムを
// コンソールからWindowsに変更すること。

#include "../resource.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd) {

	ofSetupOpenGL(980, 480, OF_WINDOW);	// <-------- setup the GL context
	ofSetWindowTitle("LuminousOSC");

	// ウィンドウに表示されるアイコンの変更
	HWND hwnd = ofGetWin32Window();
	HICON hMyIcon = LoadIcon(hInstance, MAKEINTRESOURCE(MAIN_ICON));
	SendMessage(hwnd, WM_SETICON, ICON_BIG, (LPARAM)hMyIcon);
	
	ofRunApp(new ofApp());
}

#else
// --------------------------------------------------------------------------------------------------
// for other target

int  main() {
	ofSetupOpenGL(980, 480, OF_WINDOW);	// <-------- setup the GL context
	ofSetWindowTitle("LuminousOSC");

	// this kicks off the running of my app
	// can be OF_WINDOW or OF_FULLSCREEN
	// pass in width and height too:

	ofRunApp(new ofApp());
}
#endif




