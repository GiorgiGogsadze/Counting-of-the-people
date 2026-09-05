
// Final Project.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'pch.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// CFinalProjectApp:
// See Final Project.cpp for the implementation of this class
//

class CFinalProjectApp : public CWinApp
{
public:
	CFinalProjectApp();

// Overrides
public:
	virtual BOOL InitInstance();

// Implementation

	DECLARE_MESSAGE_MAP()
};

extern CFinalProjectApp theApp;
