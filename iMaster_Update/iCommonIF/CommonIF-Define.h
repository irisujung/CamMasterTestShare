#pragma once

#ifndef __COMMONIF_DEFINE_H__
#define __COMMONIF_DEFINE_H__

#include <climits>
#include <atlstr.h> 


enum class eStepRepeatType
{
    eOneStep = 0, eTwoStep, eThreeStep, eFourStep, eFiveStep,
    eSixStep = 5, eSevenStep, eEightStep, eNineStep, eTenStep, 
    eStepRepeatTypeNone
};

enum class LogMsgType
{
	EN_ERROR = 0,
	EN_WARNING,
	EN_INFORMATION
};


//언어 추가 22.07.20 DJ.J
enum class eLanguage { eKorea =0, eEnglish, LanguageNone};
// 키보드 방향키 추가 2022.08.05 KJH ADD
enum class eKeyDir { eUP = 0, eDown, eLeft, eRight, eNone};
enum class eUserSite { eLGIT = 0, eDAEDUCK, eSEM, UserSiteNone };
enum class eMachineType { eNSIS = 0, eAOI, eAVI, MachineTypeNone };
enum class ePlatFormType { eBGA = 0, eFCB, eHDI, PlatFormTypeNone };
enum class eProductType { eStrip = 0, ePanel = 1, eStripBatch, ProductTypeNone };
enum class eCamType { eODB = 0, eGerber, eDxf, CamTypeNone };
enum class eMasterLevel { eBlockLv = 0, ePanelLv, MasterLevelNone };
enum class eShowStepRepeatType { eShowJustOne = 0, eShowAll, ShowStepRepeatTypeNone };
enum class eRefRegionMode  { eFeature = 0, eProfile, RefRegionModeNone };
enum class eScanType { eLineScan = 0, eAreaScan, ScanTypeNone };
enum class eFovType { eHorizontalSequential = 0, eVerticalSequential, eHorizontalJigjag, eVerticalJigjag, FovTypeNone };
enum class eMouseTool { eZoom = 0, ePan, eUnit, eMeasure, eWorkRgn, eUserRgn, eMouseToolNone };
enum class eZoomType { eZoomIn = 0, eZoomOut, ZoomTypeNone };
enum class eCoordinateUnit { eMMToInch = 0, eInchToMM, CoordinateUnitNone };
enum class eProdSide { eTop = 0, eBot, ProdSideNone };
enum class eDrawLevel { eUnitLv = 0, eStripLv, ePanelLv, eSymbolLv, DrawLevelNone};
enum class eDrawOption { eAll = 0, eLineOnly, eArcOnly, eLineArcOnly, ePadOnly, eTextOnly, eSurfaceOnly, DrawOptionNone };
enum class eDirectionType { eHorizontal = 0, eVertical, eBoth, DirectionTypeNone };
//enum class eViewType { eAllVT = 0, ePanelVT, eStripVT, ViewTypeNone };
enum class eRecipeMode { eRecipe_Align_Panel = 0, 
						 eRecipe_MeasureNSIS_Panel,
						 eRecipe_MeasureSR_Panel, eRecipe_Monitor_Panel, RecipeModeNone };
enum class eProdProcess { ePreSR = 0, ePostSR, ProdProcessNone };

//for NSIS
enum class MeasureSide { top = 0, bot, MeasureSideNone };
enum class MeasureType {
	fm_p = 0, align, align_plating, //Align
	t_width, t_space, bf_width, bf_space,
	bol_width, bol_space, thickness, dimple, ball_width,
	bump_width, imp_space_A, imp_space_B, imp_width_A, imp_width_B,
	bol_width_x, bol_width_y, bol_space_x, bol_space_y, bol_pitch,
	via_width, r_depth, pad, ball_pitch,
	thickness_cu, thickness_pad,
	MeasureTypeNone
};

//for NSIS - SEM - Minimap
enum class eMinimapMode {
	Measure = 0, Thickness, MinimapModeNone
};

//for NSIS - SEM
enum class MeasureType_SEM {
	fm_p = 0, align, align_plating, //Align
	t_width, t_space, bf_width, bf_space,
	bol_width, bol_space, thickness, dimple, ball_width,
	bump_width, imp_space_A, imp_space_B, imp_width_A, imp_width_B,
	bol_width_x, bol_width_y, bol_space_x, bol_space_y, bol_pitch,
	via_width, r_depth, pad, ball_pitch,
	thickness_cu, thickness_pad, roughness,
	MeasureTypeNone
};

enum class MeasureType_SR_SEM {	
	probe = 0,	MeasureTypeNone
};

namespace DefMeasureType_SEM {
	const CString pName[static_cast<int>(MeasureType_SEM::MeasureTypeNone)] = {
		_T(""), _T(""), _T(""),
		_T("T_WIDTH"), _T("T_SPACE"), _T("BF_WIDTH"), _T("BF_SPACE"),
		_T("BOL_WIDTH"), _T("BOL_SPACE"), _T("THICKNESS"), _T("DIMPLE"), _T("BALL_WIDTH"),
		_T("BUMP_WIDTH"), _T("IMP_SPACE_A"), _T("IMP_SPACE_B"), _T("IMP_WIDTH_A"), _T("IMP_WIDTH_B"),
		_T("BOL_WIDTH_X"), _T("BOL_WIDTH_Y"), _T("BOL_SPACE_X"), _T("BOL_SPACE_Y"), _T("BOL_PITCH"),
		_T("VIA_WIDTH"), _T("R_DEPTH"), _T("PAD"), _T("BALL_PITCH"),
		_T("THICKNESS_CU"), _T("THICKNESS_PAD"), _T("ROUGHNESS")
	};
}

namespace DefMeasureType_SR_SEM {
	const CString pName[static_cast<int>(MeasureType::MeasureTypeNone)] = {
		_T("PROBE"), _T("NONE")
	};
}


namespace DefMeasureSide {
	const CString pName[static_cast<int>(MeasureSide::MeasureSideNone)] = {
		_T("TOP"),
		_T("BOT"),
	};
}

namespace DefMeasureType {
	const CString pName[static_cast<int>(MeasureType::MeasureTypeNone)] = {
		_T(""), _T(""), _T(""),
		_T("T_WIDTH"), _T("T_SPACE"), _T("BF_WIDTH"), _T("BF_SPACE"),
		_T("BOL_WIDTH"), _T("BOL_SPACE"), _T("THICKNESS"), _T("DIMPLE"), _T("BALL_WIDTH"),
		_T("BUMP_WIDTH"), _T("IMP_SPACE_A"), _T("IMP_SPACE_B"), _T("IMP_WIDTH_A"), _T("IMP_WIDTH_B"),
		_T("BOL_WIDTH_X"), _T("BOL_WIDTH_Y"), _T("BOL_SPACE_X"), _T("BOL_SPACE_Y"), _T("BOL_PITCH"),
		_T("VIA_WIDTH"), _T("R_DEPTH"), _T("PAD"), _T("BALL_PITCH"),
		_T("THICKNESS_CU"), _T("THICKNESS_PAD"), 
	};
}

enum class eRefLayer
{
	EN_PATTERN_LAYER = 0,
	EN_SR_LAYER,
	EN_DOUBLE_SR_LAYER,
	EN_VIA_LAYER,
	EN_VIA2_LAYER,
	EN_DRILL_LAYER = 5,
	EN_DRILL2_LAYER,
	EN_MASK_LAYER,
	EN_UNIT_OUT_LAYER,
	EN_INSIDE_PATTERN_LAYER,
	EN_MEASURE_LAYER,
	EN_SR_DAM_LAYER,
	EN_AU_LAYER,
	EN_ETCH_LAYER,
	EN_USER_LAYER_1,
	EN_USER_LAYER_2,
	EN_USER_LAYER_3,
	EN_PROFILE_LAYER,
	EN_MEASURE_SR_LAYER,

	EN_MAX_LAYER_NUM
};

enum class ODB_ERROR_CODE : int
{
	MASTER_RESTART = -1,
	NONE = 0,
	READ_FAIL = 1,
	GET_MEASUER_FAIL = 2,
	ZERO_MEASURE_LIST = 3,
	GET_LAYER_FAIL = 4,
	EXTRACT_ODB_FAIL = 5,
	MAKE_ODB_FAIL = 6,
	EXIST_ODB_FAIL = 7,
	CHANGE_MODE_FAIL = 8,
	ODB_COPY_FILE = 9,
};

#define APPLICATION_TITLE                   _T("iMaster")
#define PATTERN_LAYER_TXT					_T("PATTERN")
#define SR_LAYER_TXT						_T("SR")
#define DOUBLE_SR_LAYER_TXT					_T("DoubleSR")
#define VIA_LAYER_TXT						_T("VIA")
#define VIA2_LAYER_TXT						_T("VIA2")
#define DRILL_LAYER_TXT						_T("DRILL")
#define DRILL2_LAYER_TXT					_T("DRILL2")
#define MASK_LAYER_TXT						_T("MASK")
#define UNIT_OUT_LAYER_TXT					_T("OUT")
#define INSIDE_PATTERN_LAYER_TXT			_T("InsidePATTERN")
#define SR_DAM_LAYER_TXT					_T("DAM")
#define AU_LAYER_TXT						_T("AU")
#define ETCH_LAYER_TXT						_T("ETCH")
#define USER_LAYER1_TXT						_T("USER#1")
#define USER_LAYER2_TXT						_T("USER#2")
#define USER_LAYER3_TXT						_T("USER#3")
#define PROFILE_LAYER_TXT					_T("PROFILE")
#define ALIGN_SYMBOL1_TXT					_T("SR_NEW_MANUAL_EXPO")
#define ALIGN_SYMBOL2_TXT					_T("SR_MANUAL_EXPO")
#define NULL_LAYER_TXT						_T("")
#define ALIGN_SYMBOL3_TXT					_T("SR_AUTO_EXPO")
#define ALIGN_FEATURE_TXT					_T("r47.244")
#define CROSS_POINT_SYMBOL_TXT				_T("construct_inc")

#define MEASURE_LAYER_TXT					_T("MEASURE")
#define PANEL_ALIGN_TXT						_T("ALIGN")
#define PANEL_ALIGN_PLATING_TXT				_T("ALIGN_PLATING")
#define UNIT_ALIGN_TXT_P					_T("FM_P")
#define UNIT_ALIGN_TXT_N					_T("FM_N")
#define THICKNESS_LAYER_TXT					_T("MEASURE_SR")

#define TGZ_TOOL_PATH						L"\\Bin\\"
#define JOB_DATA_PATH						L"\\Job\\"

#define PRODUCT_INFO_FILE					L"ProductInfo.xml"
#define GRABINSP_INFO_FILE					L"GrabInspInfo.xml"
#define CELL_INFO_FILE						L"CellInfo.xml"

#define WAIT_BEFORE_DESTORY					1000			//ms
#define MIN_BTN_STEP_NUM					5
#define MAX_BTN_STEP_NUM					200
#define MAX_LAYER_NUM						20
#define MAKE_LAYER_MARGIN					10
#define MAX_VIEW_SCALE						10000
#define MAX_CELL_NUM_LINE					250
#define MASTER_IMAGE_MARGIN					10
#define MAX_DIB_SIZE						46260
#define MAX_DIB_MEM_SIZE					(1.98)
#define SWATH_SPLIT_NUM						4				//510mm 기준
#define MAX_3D_MODE_NUM						9
#define MAX_2D_MODE_NUM						6
#define NSIS_FOV_X							880
#define NSIS_FOV_Y							640
#define UNKNOWN_ANGLE						1000.0
#define CROSS_POINT_EDGES					12

#ifndef MIN
#define	MIN(a,b)							((a>b)?(b):(a))
#endif

#define	MIN3(a,b,c)							MIN(MIN(a,b),c)
#define	MIN4(a,b,c,d)						MIN(MIN(a,b),MIN(c,d))
#define	MIN5(a,b,c,d,e)						MIN(MIN(a,b),MIN3(c,d,e))

#ifndef MAX
#define MAX(a,b)							((a>b)?(a):(b))
#endif

#define	MAX3(a,b,c)							MAX(MAX(a,b),c)
#define	MAX4(a,b,c,d)						MAX(MAX(a,b),MAX(c,d))
#define	MAX5(a,b,c,d,e)						MAX(MAX(a,b),MAX3(c,d,e))

#ifndef ABS
#define	ABS(a)								((a<0)?-(a):(a))
#endif

#define	Round(a)							(a)<0? (int)(a-0.5) : (int)(a+0.5)
#define	SWAP(a,b,t)							{t=a;a=b;b=t;}
#define	EPSILON_DELTA1						1.0E-6				//float에서 round-off가 날 수 있을 정도의 작은 수..	
#define	EPSILON_DELTA2						1.0E-12				//double에서 round-off가 날 수 있을 정도의 작은 수..
#define RoundReal(a)						((floor((a * 10.0) + 0.5)) / 10)

#define	PIF									3.141592653589793f		//float
#define	PID									3.1415926535897932384626433832795 //double
#ifndef	PI
	#define	PI				(PID)
// #else 
// 	#define	PI				(PIF)
#endif

#define DeviceXAxis(InputX,ViewScale,ViewOffX,ImgOffX)   \
	Round(((InputX)+(ImgOffX))*(ViewScale)+(ViewOffX))

#define DeviceYAxis(InputY,ViewScale,ViewOffY,ImgOffY,WholeYSize)  \
	Round((WholeYSize)-(((InputY)+(ImgOffY))*(ViewScale)+(ViewOffY)))

#define WHITE_COLOR							RGB(255,255,255)
#define GRAY_COLOR							RGB(128,128,128)
#define BLACK_COLOR							RGB(0,0,0)
#define MAGENTA_COLOR						RGB(255,0,255)
#define BLUE_COLOR							RGB(0,0,255)
#define RED_COLOR							RGB(255,0,0)
#define GREEN_COLOR							RGB(0,255,0)
#define DARKGRAY_COLOR						RGB(50,50,50)
#define YELLOW_COLOR						RGB(255,255,0)
#define LIGHTGRAY_COLOR						RGB(192,192,192)
#define	PURPLE_COLOR						RGB(127,0,255)

#define UNIT_STRING							_T("UNIT")
#define STRIP_STRING						_T("STRIP")
#define QUAD_STRING							_T("QUAD")
#define PANEL_STRING						_T("PANEL")
#define NULL_STRING							_T("")
#define FRONT_SIDE_TEXT						_T("F")
#define BACK_SIDE_TEXT						_T("B")
#define ALL_SIDE_TEXT						_T("All")
#define POSITIVE_TEXT						_T("+")
#define NEGATIVE_TEXT						_T("-")
#define OPTIC_2D_TEXT						_T("2D")
#define OPTIC_3D_TEXT						_T("3D")
#define NSIS_ZOOM_MODE_1					_T("x1.50")
#define NSIS_ZOOM_MODE_2					_T("x1.00")
#define NSIS_ZOOM_MODE_3					_T("x0.50")
#define NSIS_ZOOM_MODE_4					_T("x0.25")
#define NSIS_MAG_MODE_1						_T("x50.0")
#define NSIS_MAG_MODE_2						_T("x20.0")
#define NSIS_MAG_MODE_3						_T("x10.0")
#define NSIS_MAG_MODE_4						_T("x5.0")
#define NSIS_MAG_MODE_5						_T("x2.5")

#define PANEL_STEP_ICON						_T("\\Bin\\Img\\panel.ico")
#define SHEET_STEP_ICON						_T("\\Bin\\Img\\sheet.ico")
#define OE_STEP_ICON						_T("\\Bin\\Img\\oe.ico")
#define UNIT_STEP_ICON						_T("\\Bin\\Img\\pcb.ico")
//#define BUTTON_NORMAL_IMAGE					_T("\\Bin\\Img\\ButtonNor.bmp")
#define BUTTON_NORMAL_IMAGE					_T("\\Bin\\Img\\ModelName3.bmp")
#define BUTTON_PRESSED_IMAGE				_T("\\Bin\\Img\\ButtonPressed.bmp")

#define EXTRACT_CAM_TXT						_T("Start to Extract CAM Data...")
#define NOT_COMPLETED_EXTRACT_CAM_TXT		_T("Please Extract CAM Data First...")
#define RE_EXTRACT_CAM_TXT					_T("Start to Re-Extract CAM Data...")
#define SUCCEED_EXTRACT_CAM_TXT				_T("Succeed to Extract CAM Data...")
#define FAILED_EXTRACT_CAM_TXT				_T("Failed to Extract CAM Data...")
#define LOAD_CAM_TXT						_T("Start to Load CAM Data...")
#define SUCCEED_LOAD_CAM_TXT				_T("Succeed to Load CAM Data...")
#define FAILED_LOAD_CAM_TXT					_T("Failed to Load CAM Data...")
#define LOAD_MATRIX_TXT						_T("Start to Load Matrix...")
#define LOAD_STEP_TXT						_T("Start to Load Step...")
#define LOAD_FONT_TXT						_T("Start to Load Font...")
#define LOAD_SYMBOL_TXT						_T("Start to Load Symbol...")
#define LOAD_LAYER_TXT						_T("Start to Load Layer...")
#define SUCCEED_LOAD_MISC_TXT				_T("Succeed to Load Misc...")
#define FAILED_LOAD_MISC_TXT				_T("Failed to Load Misc...") 
#define SUCCEED_LOAD_MATRIX_TXT				_T("Succeed to Load Matrix...")
#define FAILED_LOAD_MATRIX_TXT				_T("Failed to Load Matrix...") 
#define SUCCEED_LOAD_FONT_TXT				_T("Succeed to Load Font...")
#define FAILED_LOAD_FONT_TXT				_T("Failed to Load Font...")
#define SUCCEED_LOAD_USERSYMBOLS_TXT		_T("Succeed to Load Symbol...")
#define FAILED_LOAD_USERSYMBOLS_TXT			_T("Failed to Load Symbol...")
#define SUCCEED_LOAD_STEP_TXT				_T("Succeed to Load Step...")
#define FAILED_LOAD_STEP_TXT				_T("Failed to Load Step...")
#define SUCCEED_LOAD_LAYER_TXT				_T("Succeed to Load Layers...")
#define NOT_COMPLETED_LOAD_LAYER_TXT		_T("Please Load Layers First...")
#define FAILED_LOAD_LAYER_TXT				_T("Failed to Load Layers...")
#define SUCCEED_GET_STEP_INFO				_T("Succeed to Get Step Info...")
#define FAILED_GET_STEP_INFO				_T("Failed to Get Step Info...")
#define SUCCEED_GET_STEP_COUNT				_T("Succeed to Get Step Count...")
#define FAILED_GET_STEP_COUNT				_T("Failed to Get Step Count...")
#define SUCCEED_SET_STEP_INFO				_T("Succeed to Set Step Info...")
#define FAILED_SET_STEP_INFO				_T("Failed to Set Step Info...")
#define SET_CAM_TXT							_T("Start to Set Cam Info...")
#define SUCCEED_SET_CAM_TXT					_T("Succeed to Set Cam Info...")
#define FAILED_SET_CAM_TXT					_T("Failed to Set Cam Info...")
#define SET_LAYER_TXT						_T("Start to Set Layer Info...")
#define SUCCEED_SET_LAYER_INFO				_T("Succeed to Set Layer Info...")
#define FAILED_SET_LAYER_INFO				_T("Failed to Set Layer Info...")
#define SUCCEED_GET_LAYER_INFO				_T("Succeed to Get Layer Info...")
#define FAILED_GET_LAYER_INFO				_T("Failed to Get Layer Info...")
#define SUCCEED_GET_LAYER_COUNT				_T("Succeed to Get Layer Count...")
#define FAILED_GET_LAYER_COUNT				_T("Failed to Get Layer Count...")
#define SUCCEED_DISPLAY_CAM_INFO			_T("Succeed to Display Cam Info...")
#define FAILED_DISPLAY_CAM_INFO				_T("Failed to Display Cam Info...")
#define SUCCEED_DISPLAY_LAYER_INFO			_T("Succeed to Display Layer Info...")
#define FAILED_DISPLAY_LAYER_INFO			_T("Failed to Display Layer Info...")
#define SUCCEED_SET_COMP_INFO				_T("Succeed to Set Component Info...")
#define FAILED_SET_COMP_INFO				_T("Failed to Set Component Info...")
#define SUCCEED_GET_COMP_INFO				_T("Succeed to Get Component Info...")
#define FAILED_GET_COMP_INFO				_T("Failed to Get Component Info...")
#define SUCCEED_GET_COMP_COUNT				_T("Succeed to Get Component Count...")
#define FAILED_GET_COMP_COUNT				_T("Failed to Get Component Count...")
#define FAILED_MAKE_MASTER_TXT				_T("Failed to Make Master...")
#define SUCCEED_MAKE_MASTER_TXT				_T("Succeed to Make Master...")
#define START_SAVE_LAYER_TXT				_T("Start to Save Layer Data...")
#define SUCCEED_SAVE_LAYER_TXT				_T("Succeed to Save Layer Data...")
#define FAILED__SAVE_LAYER_TXT				_T("Failed to Save Layer Data...")

#define START_EDIT_MODE_TXT					_T("Start Edit Mode...")
#define END_EDIT_MODE_TXT					_T("End Edit Mode...")
#define START_EDIT_MP_LAYER_TXT				_T("Start Edit MP Layer ...")
#define START_EDIT_TP_LAYER_TXT				_T("Start Edit TP Later ...")
#define START_EDIT_RESIZE_TXT				_T("[RESIZE] Start...")
#define APPLY_EDIT_RESIZE_TXT				_T("[RESIZE] Apply...")
#define END_EDIT_RESIZE_TXT					_T("[RESIZE] End...")
#define START_EDIT_MOVE_TXT					_T("[MOVE] Start...")
#define APPLY_EDIT_MOVE_TXT					_T("[MOVE] Apply...")
#define END_EDIT_MOVE_TXT					_T("[MOVE] End...")
#define START_EDIT_DELETE_TXT				_T("[DELETE] Start...")
#define APPLY_EDIT_DELETE_TXT				_T("[DELETE] Apply...")
#define END_EDIT_DELETE_TXT					_T("[DELETE] End...")
#define START_EDIT_ROTATE_TXT				_T("[ROTATE] Start...")
#define APPLY_EDIT_ROTATE_TXT				_T("[ROTATE] Apply ...")
#define END_EDIT_ROTATE_TXT					_T("[ROTATE] End")
#define APPLY_EDOT_MIRROR_TXT				_T("[Mirror] Apply ...")
#define START_EDIT_ADD_TXT					_T("[ADD] Start...")
#define APPLY_EDIT_ADD_TXT					_T("[ADD] Apply...")
#define END_EDIT_ADD_TXT					_T("[ADD] End...")
#define START_EDIT_MASK_TXT					_T("[MASK_EDIT] Start...")
#define APPLY_EDIT_MASK_TXT					_T("[MASK_EDIT] Apply...")
#define END_EDIT_MASK_TXT					_T("[MASK_EDIT] End...")
#define START_EDIT_COPY_TXT					_T("[COPY] Start...")
#define APPLY_EDIT_COPY_TXT					_T("[COPY] Apply...")
#define END_EDIT_COPY_TXT					_T("[COPY] End...")
#define APPLY_EDIT_ATTRIBUTE_TXT			_T("[Attribute] Set ...")
#define APPLY_ADD_LINE_TXT					_T("[LIne] Add ...")
#define APPLY_ADD_ARC_TXT					_T("[Arc] Add ...")
#define APPLY_ADD_RECTAGNLE_TXT				_T("[Rectangle] Add ...")
#define APPLY_ADD_CIRCLE_TXT				_T("[Circle] Add ...")
#define APPLY_ADD_TP_TXT					_T("[TP] Add ...")
#define APPLY_ADD_SURFACE_TXT				_T("[Surface] Add ...")





#define MENU_INSPECT_STRIP                  90601
#define MENU_NOT_INSPECT_STRIP				90602
#define MENU_INSPECT_UNIT					90603
#define MENU_NOT_INSPECT_UNIT				90604
#define MENU_INSPECT_SWATH					90605
#define MENU_NOT_INSPECT_SWATH				90606
#define MENU_INSPECT_CELL					90607
#define MENU_NOT_INSPECT_CELL				90608
#define MENU_VIEW_UNIT						90609
#define MENU_VIEW_STRIP						90610
#define MENU_VIEW_PANEL						90611
#define MENU_INSPECT_STRIP_ALL              90612
#define MENU_NOT_INSPECT_STRIP_ALL			90613
#define MENU_INSPECT_UNIT_ALL				90614
#define MENU_NOT_INSPECT_UNIT_ALL			90615
#define MENU_COPY_UNIT_TO_ALL_STRIP			90616

#define MENU_STEP_UNIT						90701

#define STR_MENU_INSPECT_STRIP				L"Inspect Strip"	
#define STR_MENU_NOT_INSPECT_STRIP			L"Skip Strip"	
#define STR_MENU_VIEW_UNIT					L"View Unit"	
#define STR_MENU_INSPECT_UNIT				L"Inspect Unit"	
#define STR_MENU_NOT_INSPECT_UNIT			L"Skip Unit"	
#define STR_MENU_VIEW_STRIP					L"View Strip"	
#define STR_MENU_VIEW_PANEL					L"View Panel"	
#define STR_MENU_INSPECT_SWATH				L"Inspect Swath"	
#define STR_MENU_NOT_INSPECT_SWATH			L"Skip Swath"	
#define STR_MENU_INSPECT_CELL				L"Inspect Cell"	
#define STR_MENU_NOT_INSPECT_CELL			L"Skip Cell"	
#define STR_MENU_INSPECT_STRIP_ALL			L"Inspect All Strips"	
#define STR_MENU_NOT_INSPECT_STRIP_ALL		L"Skip All Strips"	
#define STR_MENU_INSPECT_UNIT_ALL			L"Inspect All Units"	
#define STR_MENU_NOT_INSPECT_UNIT_ALL		L"Skip All Units"

#define STR_MENU_COPY_UNIT_TO_ALL_STRIP		L"Copy to All Strips"
#define STR_MENU_COPY_STRIP_TO_ALL_STRIP	L"Copy to All Strips"
#define STR_MENU_COPY_STRIP_TO_SELECTED_STRIP	L"Copy to Inspected Strips"



#endif