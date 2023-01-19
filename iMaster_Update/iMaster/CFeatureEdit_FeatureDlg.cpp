// CFeatureEdit_FeatureDlg.cpp: 구현 파일
//

#include "pch.h"
#include "iMaster.h"
#include "CFeatureEdit_FeatureDlg.h"
#include "afxdialogex.h"


// CFeatureEdit_FeatureDlg 대화 상자

IMPLEMENT_DYNAMIC(CFeatureEdit_FeatureDlg, CDialogEx)

CFeatureEdit_FeatureDlg::CFeatureEdit_FeatureDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_EIDT_FEATURE, pParent)
{

}

CFeatureEdit_FeatureDlg::~CFeatureEdit_FeatureDlg()
{
}

void CFeatureEdit_FeatureDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CFeatureEdit_FeatureDlg, CDialogEx)
END_MESSAGE_MAP()


// CFeatureEdit_FeatureDlg 메시지 처리기
