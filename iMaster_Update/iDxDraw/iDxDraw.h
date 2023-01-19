// iDxDraw.h : iDxDraw DLL의 주 헤더 파일
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'pch.h' before including this file for PCH"
#endif

#include "resource.h"		// 주 기호입니다.

#include "DxDraw/Def_Viewer.h"
#include "CDrawManager.h"
#include "CDrawFunction.h"
#include "CFeatureFilterFunction.h"
#include "CDrawMinimap.h"

#include "CDrawMakePoint.h"


// CiDxDrawApp
// 이 클래스 구현에 대해서는 iDxDraw.cpp를 참조하세요.
//

class CiDxDrawApp : public CWinApp
{
public:
	CiDxDrawApp();

// 재정의입니다.
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};
