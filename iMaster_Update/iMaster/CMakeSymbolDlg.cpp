// CMakeSymbolDlg.cpp: 구현 파일
//

#include "pch.h"
#include "iMaster.h"
#include "MainFrm.h"
#include "CMakeSymbolDlg.h"
#include "afxdialogex.h"
#include "CFeatureEdit_AddDlg.h"
#include "CFeatureAdd_Line_SubDlg.h"
#include "CFeatureAdd_Pad_SubDlg.h"
#include "CFeatureAdd_Arc_SubDlg.h"

// CMakeSymbolDlg 대화 상자

IMPLEMENT_DYNAMIC(CMakeSymbolDlg, CDialogEx)

CMakeSymbolDlg::CMakeSymbolDlg(enum eMakeSymbolMode eMode, CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_EDIT_MAKE_SYMBOL, pParent),
	m_eMode(eMode)
{
}

CMakeSymbolDlg::~CMakeSymbolDlg()
{
}

void CMakeSymbolDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_SHAPE_DATA, m_Spread);

	DDX_Control(pDX, IDC_RADIO_SYMBOL_CIRCLE, m_btnSymbol[enumBtnCircle]);
	DDX_Control(pDX, IDC_RADIO_SYMBOL_SQUARE, m_btnSymbol[enumBtnSquare]);
	DDX_Control(pDX, IDC_RADIO_SYMBOL_RECT, m_btnSymbol[enumBtnRectangle]);
	DDX_Control(pDX, IDC_RADIO_SYMBOL_ROUNDED_RECT, m_btnSymbol[enumBtnRounded_Rectangle]);
	DDX_Control(pDX, IDC_RADIO_SYMBOL_CHAMFERED_RECT, m_btnSymbol[enumBtnChamfered_Rectangle]);

	DDX_Control(pDX, IDC_RADIO_SYMBOL_OVAL, m_btnSymbol[enumBtnOval]);
	DDX_Control(pDX, IDC_RADIO_SYMBOL_DIAMOND, m_btnSymbol[enumBtnDiamond]);
	DDX_Control(pDX, IDC_RADIO_SYMBOL_OCTAGON, m_btnSymbol[enumBtnOctagon]);
	DDX_Control(pDX, IDC_RADIO_SYMBOL_ROUND_DONUT, m_btnSymbol[enumBtnRound_Donut]);
	DDX_Control(pDX, IDC_RADIO_SYMBOL_SQUARE_DONUT, m_btnSymbol[enumBtnSqaure_Donut]);

	DDX_Control(pDX, IDC_RADIO_SYMBOL_HORIZONTAL_HEXAGON, m_btnSymbol[enumBtnHorizontal_Hexagon]);
	DDX_Control(pDX, IDC_RADIO_SYMBOL_VERTICAL_HEXAGON, m_btnSymbol[enumtBtnVertical_Hexagon]);
	DDX_Control(pDX, IDC_RADIO_SYMBOL_BUTTERFLY, m_btnSymbol[enumBtnButterfly]);
	DDX_Control(pDX, IDC_RADIO_SYMBOL_SQUARE_BUTTERFLY, m_btnSymbol[enumBtnSquare_Butterfly]);
	DDX_Control(pDX, IDC_RADIO_SYMBOL_TRIANGLE, m_btnSymbol[enumBtnTriangle]);

	DDX_Control(pDX, IDC_RADIO_SYMBOL_HALF_OVAL, m_btnSymbol[enumBtnHalf_Oval]);
	DDX_Control(pDX, IDC_RADIO_SYMBOL_ROUND_THERMAL_ROUNDED, m_btnSymbol[enumBtnSquare_Round_Thermal_Rounded]);
	DDX_Control(pDX, IDC_RADIO_SYMBOL_ROUND_THERMAL_SQUARED, m_btnSymbol[enumBtnSquare_Round_Thermal_Squared]);
	DDX_Control(pDX, IDC_RADIO_SYMBOL_SQUARE_THERMAL, m_btnSymbol[enumBtnSquare_Thermal]);
	DDX_Control(pDX, IDC_RADIO_SYMBOL_SQUARE_THERMAL_OPEN_CORNERS, m_btnSymbol[eunmBtnSquare_Thermal_Open_Corners]);

	DDX_Control(pDX, IDC_RADIO_SYMBOL_SQUARE_ROUND_THERMAL, m_btnSymbol[enumBtnSqaure_Round_Thermal]);
	DDX_Control(pDX, IDC_RADIO_SYMBOL_RECTANGULAR_THERMAL, m_btnSymbol[enumBtnRectangluar_Thermal]);
	DDX_Control(pDX, IDC_RADIO_SYMBOL_RECTANGULAR_THERMAL_OPEN_CORNERS, m_btnSymbol[enumBtnRectangular_Thermal_Open_Corners]);
	DDX_Control(pDX, IDC_RADIO_SYMBOL_ELLIPSE, m_btnSymbol[enumBtnEllipse]);
	DDX_Control(pDX, IDC_RADIO_SYMBOL_MOIRE, m_btnSymbol[enumBtnMoire]);

	DDX_Control(pDX, IDC_RADIO_SYMBOL_HOLE, m_btnSymbol[enumBtnHole]);




	

	DDX_Control(pDX, IDC_ICON__CIRCLE, m_Icon_R);
	DDX_Control(pDX, IDC_ICON__Square, m_Icon_S);
	DDX_Control(pDX, IDC_ICON__Rectangle, m_IconRect);
	DDX_Control(pDX, IDC_ICON__ROUND__RECT, m_Icon__RonudRect);
	DDX_Control(pDX, IDC_ICON__CHAMFERED__RECT, m_IconChamferedRect);
	DDX_Control(pDX, IDC_ICON__OVAL, m_IconOval);
	DDX_Control(pDX, IDC_ICON__DIAMOND, m_IconDiamond);
	DDX_Control(pDX, IDC_ICON__OCTAGON, m_IconOct);
	DDX_Control(pDX, IDC_ICON__ROUND__DONUT, m_IconDonut__R);
	DDX_Control(pDX, IDC_ICON__DONUT__S, m_IconDonut__S);
	DDX_Control(pDX, IDC_ICON__HEX__L, m_IconHex_L);
	DDX_Control(pDX, IDC_ICON__HEX__S, m_Icon_Hex_S);
	DDX_Control(pDX, IDC_ICON__BFR, m_Icon_BFR);
	DDX_Control(pDX, IDC_ICON__BFS, m_IconBFS);
	DDX_Control(pDX, IDC_ICON__TRI, m_Icon_TRI);
	DDX_Control(pDX, IDC_ICON__OVAL_H, m_Icon_Oval_H);
	DDX_Control(pDX, IDC_ICON__THR, m_Icon_THR);
	DDX_Control(pDX, IDC_ICON__THS, m_Icon_THS);
	DDX_Control(pDX, IDC_ICON__S__THS, m_Icon_S_THS);
	DDX_Control(pDX, IDC_ICON__S__THO, m_Icon_S_THO);
	DDX_Control(pDX, IDC_ICON__SR_THS, m_Icon_SR_THS);
	DDX_Control(pDX, IDC_ICON__RC__THS, m_Icon_RC_THS);
	DDX_Control(pDX, IDC_ICON__RC__THO, m_Icon_RC_THO);
	DDX_Control(pDX, IDC_ICON__EL, m_Icon_EL);
	DDX_Control(pDX, IDC_ICON__MOIRE, m_Icon_Moire);
	DDX_Control(pDX, IDC_ICON__HOLE, m_Icon_Hole);



}


BEGIN_MESSAGE_MAP(CMakeSymbolDlg, CDialogEx)
	ON_CONTROL_RANGE(BN_CLICKED, IDC_RADIO_SYMBOL_CIRCLE, 1155, OnModeButtonClicked)
	ON_BN_CLICKED(IDC_BUTTON_APPLY, &CMakeSymbolDlg::OnBnClickedButtonApply)
	ON_BN_CLICKED(IDC_BUTTON_CANCEL, &CMakeSymbolDlg::OnBnClickedButtonCancel)
	ON_BN_CLICKED(IDC_BUTTON_PREVIEW, &CMakeSymbolDlg::OnBnClickedButtonPreview)
END_MESSAGE_MAP()


BOOL CMakeSymbolDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	_InitUI(m_eMode);
	_InitParaName();
	_InitSpread();

	((CButton*)GetDlgItem(IDC_RADIO_SYMBOL_CIRCLE))->SetCheck(TRUE);
	for (int i = IDC_RADIO_SYMBOL_CIRCLE + 1; i < 1199; i++)
	{
		CButton* pRadioButton = (CButton*)GetDlgItem(i);
		if (pRadioButton == nullptr) continue;

		pRadioButton->SetCheck(FALSE);
	}
	m_nSelectShape = 0;
	_UpdateSpread(m_nSelectShape);


	m_Icon_R.LoadBitmaps(IDB_CIRCLE, NULL, NULL, NULL);
	//m_Icon_R.SizeToContent;
	m_Icon_S.LoadBitmaps(IDB_SQUARE, NULL, NULL, NULL);
	//m_Icon_S.SizeToContent;
	m_IconRect.LoadBitmaps(IDB_RECT, NULL, NULL, NULL);
	//m_IconRect.SizeToContent;
	m_Icon__RonudRect.LoadBitmaps(IDB_ROUNDEDRECT, NULL, NULL, NULL);
	//m_Icon__RonudRect.SizeToContent;
	m_IconChamferedRect.LoadBitmaps(IDB_CHAMFEREDRECT, NULL, NULL, NULL);
	//m_IconChamferedRect.SizeToContent;

	m_IconOval.LoadBitmaps(IDB_OVAL, NULL, NULL, NULL);
	//m_IconOval.SizeToContent;
	m_IconDiamond.LoadBitmaps(IDB_DIAMOND, NULL, NULL, NULL);
	//m_IconDiamond.SizeToContent;
	m_IconOct.LoadBitmaps(IDB_OCTAGON, NULL, NULL, NULL);
	//m_IconOct.SizeToContent;
	m_IconDonut__R.LoadBitmaps(IDB_ROUND_DONUT, NULL, NULL, NULL);
	//m_IconDonut__R.SizeToContent;
	m_IconDonut__S.LoadBitmaps(IDB_SQUARE_DONUT, NULL, NULL, NULL);
	//m_IconDonut__S.SizeToContent;

	m_IconHex_L.LoadBitmaps(IDB_HORIZONTAL_HEXAGON, NULL, NULL, NULL);
	//m_IconHex_L.SizeToContent;
	m_Icon_Hex_S.LoadBitmaps(IDB_VERTICALHEXAGON, NULL, NULL, NULL);
	//m_Icon_Hex_S.SizeToContent;
	m_Icon_BFR.LoadBitmaps(IDB_BUTTERFLY, NULL, NULL, NULL);
	//m_Icon_BFR.SizeToContent;
	m_IconBFS.LoadBitmaps(IDB_SQUARE_BUTTERFLY, NULL, NULL, NULL);
	//m_IconBFS.SizeToContent;
	m_Icon_TRI.LoadBitmaps(IDB_TRIANGLE, NULL, NULL, NULL);
	//m_Icon_TRI.SizeToContent;

	m_Icon_Oval_H.LoadBitmaps(IDB_HALF_OVAL, NULL, NULL, NULL);
	//m_Icon_Oval_H.SizeToContent;
	m_Icon_THR.LoadBitmaps(IDB_ROUND_THERMAL_ROUNDED, NULL, NULL, NULL);
	//m_Icon_THR.SizeToContent;
	m_Icon_THS.LoadBitmaps(IDB_ROUND_THERMAL_SQUARED, NULL, NULL, NULL);
	//m_Icon_THS.SizeToContent;
	m_Icon_S_THS.LoadBitmaps(IDB_SQUARETHERMAL, NULL, NULL, NULL);
	//m_Icon_S_THS.SizeToContent;
	m_Icon_S_THO.LoadBitmaps(IDB_SQAURE_THERMAL_OPEN_CORNERS, NULL, NULL, NULL);
	//m_Icon_S_THO.SizeToContent;

	m_Icon_SR_THS.LoadBitmaps(IDB_SQUARE_ROUND_THERMAL, NULL, NULL, NULL);
	//m_Icon_SR_THS.SizeToContent;
	m_Icon_RC_THS.LoadBitmaps(IDB_RECTANGLULAR_THERMAL, NULL, NULL, NULL);
	//m_Icon_RC_THS.SizeToContent;
	m_Icon_RC_THO.LoadBitmaps(IDB_RECTANGLUAR_THERMAL_OPEN_CORNERS, NULL, NULL, NULL);
	//m_Icon_RC_THO.SizeToContent;
	m_Icon_EL.LoadBitmaps(IDB_ELLIPSE, NULL, NULL, NULL);
	//m_Icon_EL.SizeToContent;
	m_Icon_Moire.LoadBitmaps(IDB_MOIRE, NULL, NULL, NULL);
	//m_Icon_Moire.SizeToContent;

	m_Icon_Hole.LoadBitmaps(IDB_HOLE, NULL, NULL, NULL);
	//m_Icon_Hole.SizeToContent;


	return TRUE;  // return TRUE unless you set the focus to a control
				  // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

// CMakeSymbolDlg 메시지 처리기


CString CMakeSymbolDlg::GetSymbolName()
{
	return m_strSymbol;
}

vector<double> CMakeSymbolDlg::GetSymbolPara()
{
	return m_vecPara_mm;
}

int				CMakeSymbolDlg::GetType()
{
	return m_nSelectShape;
}

BOOL CMakeSymbolDlg::_MakeResult()
{
	if (m_nSelectShape < 0) return FALSE;

	_UpdatePara();
	int nParaCount = DefSymbol::pParaCount[m_nSelectShape];
	if (nParaCount != static_cast<int>(m_vecPara_mm.size())) return FALSE;

	m_strSymbol = _T("");



	BOOL bRet = TRUE;
	SymbolName eSybolName = static_cast<SymbolName>(m_nSelectShape);

	m_strSymbol = CDrawFunction::GetSymbolName(eSybolName, m_vecPara_mm);
	if (m_strSymbol == _T(""))
	{
		m_vecPara_mm.resize(0);
		bRet = FALSE;
	}

	/*
	switch (eSybolName)
	{
	case SymbolName::r:
		m_strSymbol.Format(_T("%s%.3lf"), DefSymbol::pName[m_nSelectShape], CJobFile::MMToInch(m_vecPara_mm[0]) * 1000);
		break;
	case SymbolName::s:
		m_strSymbol.Format(_T("%s%.3lf"), DefSymbol::pName[m_nSelectShape], CJobFile::MMToInch(m_vecPara_mm[0]) * 1000);
		break;
	case SymbolName::rect:
		m_strSymbol.Format(_T("%s%.3lfx%.3lf"), DefSymbol::pName[m_nSelectShape], CJobFile::MMToInch(m_vecPara_mm[0]) * 1000, CJobFile::MMToInch(m_vecPara_mm[1]) * 1000);
		break;
	default:
		m_strSymbol = _T("");
		m_vecPara_mm.resize(0);
		bRet = FALSE;
		break;
	}
	*/

	return bRet;
}

void CMakeSymbolDlg::_UpdatePara()
{//Spread -> Para Data;

	UserSetInfo_t* pstUserSetInfo = GetUserSetInfo();
	if (pstUserSetInfo == nullptr) return;

	int nParaCount = DefSymbol::pParaCount[m_nSelectShape];
	int nStartRow = 3;
	int nEndRow = nStartRow + nParaCount;
	int nStartColumn = enumCol_Data;

	m_vecPara_mm.clear();
	m_vecPara_Mil.clear();

	if (nParaCount <= 0) return;

	m_vecPara_mm.resize(nParaCount);
	m_vecPara_Mil.resize(nParaCount);

	CString strTemp;
	for (int i = nStartRow; i < nEndRow; i++)
	{
		strTemp = m_Spread.GetText(enumCol_Data, i);

		if (pstUserSetInfo->coordUnit == eCoordinateUnit::eMMToInch)
		{
			m_vecPara_Mil[i - nStartRow] = _ttof(strTemp);
			m_vecPara_mm[i - nStartRow] = CJobFile::MilToMM(m_vecPara_Mil[i - nStartRow]);
		}
		else
		{
			m_vecPara_mm[i - nStartRow] = _ttof(strTemp);
			m_vecPara_Mil[i - nStartRow] = CJobFile::MMToMil(m_vecPara_mm[i - nStartRow]);
		}
	}
}

void CMakeSymbolDlg::_InitParaName()
{
	m_vecvecParaName.clear();

	UserSetInfo_t* pstUserSetInfo = GetUserSetInfo();
	if (pstUserSetInfo == nullptr) return;

	int nSymbolSize = static_cast<int>(SymbolName::SymbolNameNone);
	m_vecvecParaName.resize(nSymbolSize);

	for (int i = 0; i < nSymbolSize; i++)
	{
		m_vecvecParaName[i].resize(DefSymbol::pParaCount[i]);
	}

	CString strUnit = _T("(mm)");
	CString strAngle = _T("(°)");
	CString strNum = _T("(Number)");
	if (pstUserSetInfo->coordUnit == eCoordinateUnit::eMMToInch)
		strUnit = _T("(mil)");

	//추가 정의 필요
	for (int i = 0; i < nSymbolSize; i++)
	{
		SymbolName eSybolName = static_cast<SymbolName>(i);
		switch (eSybolName)
		{
		case SymbolName::r:
			m_vecvecParaName[i][0].Format(_T("Diameter\n%s"), strUnit);
			break;
		case SymbolName::s:
			m_vecvecParaName[i][0].Format(_T("Width\n%s"), strUnit);
			break;
		case SymbolName::rect:
			m_vecvecParaName[i][0].Format(_T("Width\n%s"), strUnit);
			m_vecvecParaName[i][1].Format(_T("Height\n%s"), strUnit);
			break;
		case SymbolName::rectxr:
			m_vecvecParaName[i][0].Format(_T("Width\n%s"), strUnit);
			m_vecvecParaName[i][1].Format(_T("Height\n%s"), strUnit);
			m_vecvecParaName[i][2].Format(_T("Radius\n%s"), strUnit);
			m_vecvecParaName[i][3].Format(_T("Conners\n%s"), strNum);
			break;
		case SymbolName::rectxc:
			m_vecvecParaName[i][0].Format(_T("Width\n%s"), strUnit);
			m_vecvecParaName[i][1].Format(_T("Height\n%s"), strUnit);
			m_vecvecParaName[i][2].Format(_T("Radius\n%s"), strUnit);
			m_vecvecParaName[i][3].Format(_T("Conners\n%s"), strNum);
			break;
		case SymbolName::oval:
			m_vecvecParaName[i][0].Format(_T("Width\n%s"), strUnit);
			m_vecvecParaName[i][1].Format(_T("Height\n%s"), strUnit);
			break;
		case SymbolName::di:
			m_vecvecParaName[i][0].Format(_T("Width\n%s"), strUnit);
			m_vecvecParaName[i][1].Format(_T("Height\n%s"), strUnit);
			break;
		case SymbolName::octagon:
			m_vecvecParaName[i][0].Format(_T("Width\n%s"), strUnit);
			m_vecvecParaName[i][1].Format(_T("Height\n%s"), strUnit);
			m_vecvecParaName[i][2].Format(_T("Side\n%s"), strUnit);
			break;
		case SymbolName::donut_r:
			m_vecvecParaName[i][0].Format(_T("OuterDiameter\n%s"), strUnit);
			m_vecvecParaName[i][1].Format(_T("InnerDiameter\n%s"), strUnit);
			break;
		case SymbolName::donut_s:
			m_vecvecParaName[i][0].Format(_T("OuterDiameter\n%s"), strUnit);
			m_vecvecParaName[i][1].Format(_T("InnerDiameter\n%s"), strUnit);
			break;
		case SymbolName::hex_l:
			m_vecvecParaName[i][0].Format(_T("OuterDiameter\n%s"), strUnit);
			m_vecvecParaName[i][1].Format(_T("InnerDiameter\n%s"), strUnit);
			m_vecvecParaName[i][2].Format(_T("Side\n%s"), strUnit);
			break;
		case SymbolName::hex_s:
			m_vecvecParaName[i][0].Format(_T("OuterDiameter\n%s"), strUnit);
			m_vecvecParaName[i][1].Format(_T("InnerDiameter\n%s"), strUnit);
			m_vecvecParaName[i][2].Format(_T("Side\n%s"), strUnit);
			break;
		case SymbolName::bfr:
			m_vecvecParaName[i][0].Format(_T("Diameter\n%s"), strUnit);
			break;
		case SymbolName::bfs:
			m_vecvecParaName[i][0].Format(_T("Size\n%s"), strUnit);
			break;
		case SymbolName::tri:
			m_vecvecParaName[i][0].Format(_T("Base\n%s"), strUnit);
			m_vecvecParaName[i][1].Format(_T("Height\n%s"), strUnit);
			break;
		case SymbolName::oval_h:
			m_vecvecParaName[i][0].Format(_T("Width\n%s"), strUnit);
			m_vecvecParaName[i][1].Format(_T("Height\n%s"), strUnit);
			break;
		case SymbolName::thr:
			m_vecvecParaName[i][0].Format(_T("OuterDimamter\n%s"), strUnit);
			m_vecvecParaName[i][1].Format(_T("InnerDiameter\n%s"), strUnit);
			m_vecvecParaName[i][2].Format(_T("Angle\n%s"), strAngle);
			m_vecvecParaName[i][3].Format(_T("Spoke\n%s"), strNum);
			m_vecvecParaName[i][4].Format(_T("Gap\n%s"), strUnit);
			break;
		case SymbolName::ths:
			m_vecvecParaName[i][0].Format(_T("OuterDimamter\n%s"), strUnit);
			m_vecvecParaName[i][1].Format(_T("InnerDiameter\n%s"), strUnit);
			m_vecvecParaName[i][2].Format(_T("Angle\n%s"), strAngle);
			m_vecvecParaName[i][3].Format(_T("Spoke\n%s"), strNum);
			m_vecvecParaName[i][4].Format(_T("Gap\n%s"), strUnit);
			break; 
		case SymbolName::s_ths:
			m_vecvecParaName[i][0].Format(_T("OuterSize\n%s"), strUnit);
			m_vecvecParaName[i][1].Format(_T("InnerSize\n%s"), strUnit);
			m_vecvecParaName[i][2].Format(_T("Angle\n%s"), strAngle);
			m_vecvecParaName[i][3].Format(_T("Spoke\n%s"), strNum);
			m_vecvecParaName[i][4].Format(_T("Gap\n%s"), strUnit);
			break;
		case SymbolName::s_tho:
			m_vecvecParaName[i][0].Format(_T("OuterSize\n%s"), strUnit);
			m_vecvecParaName[i][1].Format(_T("InnerDiameter\n%s"), strUnit);
			m_vecvecParaName[i][2].Format(_T("Angle\n%s"), strAngle);
			m_vecvecParaName[i][3].Format(_T("Spoke\n%s"), strNum);
			m_vecvecParaName[i][4].Format(_T("Gap\n%s"), strUnit);
			break;
		case SymbolName::sr_ths:
			m_vecvecParaName[i][0].Format(_T("OuterSize\n%s"), strUnit);
			m_vecvecParaName[i][1].Format(_T("InnerDiameter\n%s"), strUnit);
			m_vecvecParaName[i][2].Format(_T("Angle\n%s"), strAngle);
			m_vecvecParaName[i][3].Format(_T("Spoke\n%s"), strNum);
			m_vecvecParaName[i][4].Format(_T("Gap\n%s"), strUnit);
			break;
		case SymbolName::rc_ths:
			m_vecvecParaName[i][0].Format(_T("OuterSize\n%s"), strUnit);
			m_vecvecParaName[i][1].Format(_T("InnerDiameter\n%s"), strUnit);
			m_vecvecParaName[i][2].Format(_T("Angle\n%s"), strAngle);
			m_vecvecParaName[i][3].Format(_T("Spoke\n%s"), strNum);
			m_vecvecParaName[i][4].Format(_T("Gap\n%s"), strUnit);
			m_vecvecParaName[i][5].Format(_T("AirGap\n%s"), strUnit);
			break;
		case SymbolName::rc_tho:
			m_vecvecParaName[i][0].Format(_T("OuterWidth\n%s"), strUnit);
			m_vecvecParaName[i][1].Format(_T("OuterHeight\n%s"), strUnit);
			m_vecvecParaName[i][2].Format(_T("Angle\n%s"), strAngle);
			m_vecvecParaName[i][3].Format(_T("Spoke\n%s"), strNum);
			m_vecvecParaName[i][4].Format(_T("Gap\n%s"), strUnit);
			m_vecvecParaName[i][5].Format(_T("AirGap\n%s"), strUnit);
			break;
		case SymbolName::el:
			m_vecvecParaName[i][0].Format(_T("Width\n%s"), strUnit);
			m_vecvecParaName[i][1].Format(_T("Height\n%s"), strUnit);
			break;
		case SymbolName::moire:
			m_vecvecParaName[i][0].Format(_T("RingWidth\n%s"), strUnit);
			m_vecvecParaName[i][1].Format(_T("RingGap\n%s"), strUnit);
			m_vecvecParaName[i][2].Format(_T("Ring\n%s"), strNum);
			m_vecvecParaName[i][3].Format(_T("LineWidth\n%s"), strUnit);
			m_vecvecParaName[i][4].Format(_T("LineLength\n%s"), strUnit);
			m_vecvecParaName[i][5].Format(_T("LineAngle\n%s"), strAngle);
			break;
		case SymbolName::hole:
			m_vecvecParaName[i][0].Format(_T("HoleDiameter\n%s"), strUnit);
			/*
			m_vecvecParaName[i][1].Format(_T("PlatingStatus\n%s"), strUnit);
			m_vecvecParaName[i][2].Format(_T("+Tolerance\n"));
			m_vecvecParaName[i][3].Format(_T("-Tolerance\n"));
			*/
			break;
		case SymbolName::null:
			m_vecvecParaName[i][0].Format(_T("Extension\n%s"),strNum);
			break;

		default:
			break;
		}
	}
}



void CMakeSymbolDlg::_InitUI(enum eMakeSymbolMode eMode)
{
	m_eMode = eMode;

	switch (eMode)
	{
	default:
	case eMakeSymbolMode::enumMode_Pad:
	{
		for (int i = 0; i < enumBtnMax; i++)
		{
			m_btnSymbol[i].ShowWindow(SW_SHOW);
		}
		
// 			for (int j = 0; j < 1; j++)
// 			{
// 				m_IconSymbol[j].ShowWindow(SW_SHOW);
// 			}
			  		
	
	}
	break;
	case eMakeSymbolMode::enumMode_Line:
	{
		for (int i = 0; i < enumBtnMax; i++)
		 {
			m_btnSymbol[i].ShowWindow(SW_HIDE);
		 }
		 
		    m_btnSymbol[enumBtnCircle].ShowWindow(SW_SHOW);
		 	m_btnSymbol[enumBtnSquare].ShowWindow(SW_SHOW);
		 
			m_IconRect.ShowWindow(SW_HIDE);
			m_Icon__RonudRect.ShowWindow(SW_HIDE);
			m_IconChamferedRect.ShowWindow(SW_HIDE);
			m_IconOval.ShowWindow(SW_HIDE);
			m_IconDiamond.ShowWindow(SW_HIDE);
			m_IconOct.ShowWindow(SW_HIDE);
			m_IconDonut__R.ShowWindow(SW_HIDE);
			m_IconDonut__S.ShowWindow(SW_HIDE);
			m_IconHex_L.ShowWindow(SW_HIDE);
			m_Icon_Hex_S.ShowWindow(SW_HIDE);
			m_Icon_BFR.ShowWindow(SW_HIDE);
			m_IconBFS.ShowWindow(SW_HIDE);
			m_Icon_TRI.ShowWindow(SW_HIDE);
			m_Icon_Oval_H.ShowWindow(SW_HIDE);
			m_Icon_THR.ShowWindow(SW_HIDE);
			m_Icon_THS.ShowWindow(SW_HIDE);
			m_Icon_S_THS.ShowWindow(SW_HIDE);
			m_Icon_S_THO.ShowWindow(SW_HIDE);
			m_Icon_SR_THS.ShowWindow(SW_HIDE);
			m_Icon_RC_THS.ShowWindow(SW_HIDE);
			m_Icon_RC_THO.ShowWindow(SW_HIDE);
			m_Icon_EL.ShowWindow(SW_HIDE);
			m_Icon_Moire.ShowWindow(SW_HIDE);
			m_Icon_Hole.ShowWindow(SW_HIDE);
	}
	break;
	case eMakeSymbolMode::emumMode_Arc: 
	{
		for (int i = 0; i < enumBtnMax; i++)
		{
		 	m_btnSymbol[i].ShowWindow(SW_HIDE);
		}
		 	m_btnSymbol[enumBtnCircle].ShowWindow(SW_SHOW);
		 
			m_Icon_S.ShowWindow(SW_HIDE);
			m_IconRect.ShowWindow(SW_HIDE);
			m_Icon__RonudRect.ShowWindow(SW_HIDE);
			m_IconChamferedRect.ShowWindow(SW_HIDE);
			m_IconOval.ShowWindow(SW_HIDE);
			m_IconDiamond.ShowWindow(SW_HIDE);
			m_IconOct.ShowWindow(SW_HIDE);
			m_IconDonut__R.ShowWindow(SW_HIDE);
			m_IconDonut__S.ShowWindow(SW_HIDE);
			m_IconHex_L.ShowWindow(SW_HIDE);
			m_Icon_Hex_S.ShowWindow(SW_HIDE);
			m_Icon_BFR.ShowWindow(SW_HIDE);
			m_IconBFS.ShowWindow(SW_HIDE);
			m_Icon_TRI.ShowWindow(SW_HIDE);
			m_Icon_Oval_H.ShowWindow(SW_HIDE);
			m_Icon_THR.ShowWindow(SW_HIDE);
			m_Icon_THS.ShowWindow(SW_HIDE);
			m_Icon_S_THS.ShowWindow(SW_HIDE);
			m_Icon_S_THO.ShowWindow(SW_HIDE);
			m_Icon_SR_THS.ShowWindow(SW_HIDE);
			m_Icon_RC_THS.ShowWindow(SW_HIDE);
			m_Icon_RC_THO.ShowWindow(SW_HIDE);
			m_Icon_EL.ShowWindow(SW_HIDE);
			m_Icon_Moire.ShowWindow(SW_HIDE);
			m_Icon_Hole.ShowWindow(SW_HIDE);
	}
		break;
		 
	}
}

void CMakeSymbolDlg::_InitSpread()
{
	int nMaxCols = enumColMax - 1;

	m_Spread.Reset();

	// Max Col, Row 설정
	m_Spread.SetMaxCols(nMaxCols);
	m_Spread.SetMaxRows(1);

	// Font 설정
	//m_Spread.Set_Font(1, 1, nMaxCols, nMaxRows, _T("Malgun Gothic"), 10 * FONT_RATIO, TRUE);

	// Header Hide
	m_Spread.SetHeaderHide();

	// Header Display None

	// Text Display
	int nIndex = 1;

	m_Spread.SetText(enumCol_No, nIndex, _T("No."));
	m_Spread.SetText(enumCol_Name, nIndex, _T("Name"));
	m_Spread.SetText(enumCol_Data, nIndex, _T("Data"));
	nIndex = 2;

	CRect rcClient;
	m_Spread.GetClientRect(rcClient);


	//1 : 8 pixel
	//10 : 80 pixel
	//150 : 1200 pixel
	//Width 계산
	int nWidth = rcClient.Width() - 5;//Scroll bar Size
	double dWidthTemp;
	double dWidthSum = 0.0;
	for (int i = enumCol_No; i < enumColMax; i++)
	{
		switch (i)
		{
		case enumCol_No :
			dWidthTemp = nWidth * 0.1 / 8.;
			dWidthSum += dWidthTemp;
			break;
		case enumCol_Name:
			dWidthTemp = nWidth * 0.3 / 8.;
			dWidthSum += dWidthTemp;
			break;
		case enumCol_Data:
			dWidthTemp = nWidth / 8. - dWidthSum;
			break;
		default:
			break;
		}

		m_Spread.SetColWidth(i, dWidthTemp);
	}
	
	// Scrollbar Hide
	m_Spread.SetScrollBars(0/*ScrollBarsNone*/);

	/* 전체 텍스트 얼라인 Center로 - 주의 Static Text로 설정하기 때문에 Edit가 전부 Disable됨
	   호출 순서에 주의할것.*/
	m_Spread.Block_SetAlignTextCenter();


	//-------------------------------------------------------------------

	// 배경색 설정
	//Header
	m_Spread.Block_SetBackColor(1, nMaxCols, 1, 1, RGB(255, 250, 230));
	//
	m_Spread.Set_Font(1, 1, nMaxCols, 1, _T("Malgun Gothic"), 12 * 1, TRUE);

	// 고정모드 설정
	m_Spread.SetFixedMode();
	// 배경색 설정

	// 테두리 설정		
	m_Spread.SetCellBorder(1, 1, nMaxCols, 1, 1, RGB(112, 128, 144), 1);
	m_Spread.Set_AllBlockLock();

	m_Spread.SetGrayAreaBackColor(RGB(255, 255, 255));

	m_Spread.SetAllDataRowHeight(30);
}


void CMakeSymbolDlg::OnBnClickedButtonApply()
{
	if (_MakeResult() == TRUE)
	{
		CDialogEx::OnOK();
	}
	else
	{
		//파라미터가 잘못되었음
		AfxMessageBox(_T("파라미터가 잘못되었습니다."));
	}
}


void CMakeSymbolDlg::OnBnClickedButtonCancel()
{
	m_strSymbol = _T("");
	m_vecPara_mm.resize(0);

	CDialogEx::OnCancel();
}

BOOL CMakeSymbolDlg::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->wParam == VK_RETURN || pMsg->wParam == VK_ESCAPE) return TRUE;

	return CDialogEx::PreTranslateMessage(pMsg);
}


void CMakeSymbolDlg::OnModeButtonClicked(UINT32 nID)
{
	if (nID <= 0) return;

	int nIndex = nID - IDC_RADIO_SYMBOL_CIRCLE;
	if (nIndex < 0) return;
	if (nIndex > static_cast<int>(SymbolName::userdefined)) return;

	m_nSelectShape = nIndex;
	_UpdateSpread(m_nSelectShape);

}




void CMakeSymbolDlg::_UpdateSpread(int nType)
{
	UserSetInfo_t* pstUserSetInfo = GetUserSetInfo();
	if (pstUserSetInfo == nullptr) return;

	int nMaxRow = 1 + 1 + DefSymbol::pParaCount[nType];//Header + TypeName + ParaCount
	m_Spread.SetMaxRows(nMaxRow);

	int nRowIndex = 2;
	//Set Num
	CString strTemp;
	for (int i = nRowIndex; i <= nMaxRow; i++)
	{
		strTemp.Format(_T("%d"), i - 1);
		m_Spread.SetText(enumCol_No, i, strTemp);
	}

	//SetTypeName
	m_Spread.SetText(enumCol_Name, nRowIndex, _T("Type Name"));
	m_Spread.SetText(enumCol_Data, nRowIndex, DefSymbol::pTypeName[nType]);
	//

	nRowIndex = 3;
	int nParaIndex;
	switch (m_eMode)
	{
	default:
	case eMakeSymbolMode::enumMode_Pad:
	{
		for (int i = nRowIndex; i <= nMaxRow; i++)
		{
			nParaIndex = i - nRowIndex;
			m_Spread.SetText(enumCol_Name, i, m_vecvecParaName[nType][nParaIndex]);
		}
	}
	break;
	case eMakeSymbolMode::enumMode_Line:
	{
		for (int i = nRowIndex; i <= nMaxRow; i++)
		{
			nParaIndex = i - nRowIndex;
			if (pstUserSetInfo->coordUnit == eCoordinateUnit::eMMToInch)
				m_Spread.SetText(enumCol_Name, i, _T("Width\n(mil)"));
			else
				m_Spread.SetText(enumCol_Name, i, _T("Width\n(mm)"));
		}
	}
		break;
		case eMakeSymbolMode::emumMode_Arc:
		{
			for (int i = nRowIndex; i <= nMaxRow; i++)
			{
				nParaIndex = i - nRowIndex;
				if (pstUserSetInfo->coordUnit == eCoordinateUnit::eMMToInch)
					m_Spread.SetText(enumCol_Name, i, _T("Width\n(mil)"));
				else
					m_Spread.SetText(enumCol_Name, i, _T("Width\n(mm)"));
			}
		}

	}
	
	
	//Row가 변경되었으니, Lock을 재설정한다.
	//Editable FALSE
	m_Spread.Set_AllBlockLock(TRUE);
	m_Spread.Set_BlockLock(enumCol_Data, 3, enumCol_Data, nMaxRow, FALSE);//No. name Loch

	
}

void CMakeSymbolDlg::OnBnClickedSymbolPreview()
{
	if (m_pPadSubDlg->IsAllDataInput() == FALSE)
	{
		AfxMessageBox(_T("필수 데이터가 입력되지 않았습니다"));
		return;
	}

	/*if (m_AddDlg->_MakeFeature() == RESULT_GOOD)
	{
		SetAdd(m_AddDlg->GetLayerName(), m_vecFeatureData);

		_ClearFeatureData();
	}*/


}




void CMakeSymbolDlg::OnBnClickedButtonPreview()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}
