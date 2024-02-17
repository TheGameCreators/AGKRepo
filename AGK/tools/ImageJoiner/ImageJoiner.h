// ImageJoiner.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// CImageJoinerApp:
// See ImageJoiner.cpp for the implementation of this class
//

class CImageJoinerApp : public CWinApp
{
public:
	CImageJoinerApp();

// Overrides
	public:
	virtual BOOL InitInstance();

	IDirect3D9 *pD3D;
	IDirect3DDevice9 *pDevice;

// Implementation

	DECLARE_MESSAGE_MAP()
};

extern CImageJoinerApp theApp;