//#include "../pch.h"
#include "pch.h"
#include "CSaveLayer_NSIS.h"
#include "../CDrawLayer.h"


#include <wrl.h>//Comptr
using namespace Microsoft::WRL;

#include "shlobj_core.h"

CSaveLayer_NSIS::CSaveLayer_NSIS()
{

}

CSaveLayer_NSIS::~CSaveLayer_NSIS()
{
	

}

UINT32 CSaveLayer_NSIS::SaveImage(IN const CString &strfilename, IN const double &dCamAngle,
	IN CStep *pStep, IN LayerSet_t* pLayerSet, CTypedPtrArray <CObArray, CStandardFont*>* pFontArr,
	CRotMirrArr *pRotMirrArr, IN const UserSetInfo_t *pUserSetInfo, IN const BOOL &bSaveRegion,
	IN const SaveMode eSaveMode, IN bool bExceptDummy, IN CString strLineArcWidth, double dAngle, bool bMirror)
{
	UINT32 nRet = RESULT_BAD;

	if (pStep == nullptr) return nRet;
	if (pLayerSet == nullptr) return nRet;
	if (m_pImageRender == nullptr) return nRet;

	COLORREF	color = pLayerSet->color;
	pLayerSet->color = RGB(255, 255, 255);

	int nLayerIndex = 0;//사용안함
	CLayer *pLayer = CDrawFunction::_GetLayerPrt(pStep->m_arrLayer, pLayerSet->strLayer, nLayerIndex);
	if (pLayer == nullptr)
		return nRet;

	if (m_ptSplitCount.x == 1 && m_ptSplitCount.y == 1)
	{
		_BeginDraw();
		nRet = _DrawRender(m_pImageRender, pLayer, pLayerSet, pFontArr, pRotMirrArr, TRUE, pUserSetInfo, dAngle, bMirror);

		_EndDraw();

		_SetSaveImage(strfilename, dCamAngle);
	}
	else
	{

	}
	
	

	pLayerSet->color = color;

	return RESULT_GOOD;
}

UINT32 CSaveLayer_NSIS::_DrawRender(ID2D1RenderTarget* pRender, IN  CLayer *pLayer, IN const LayerSet_t* pLayerSet,
	CTypedPtrArray <CObArray, CStandardFont*>* pFontArr, CRotMirrArr *pRotMirrArr, IN const BOOL &bNeedUpdate, 
	IN const UserSetInfo_t *pUserSetInfo, double dAngle, bool bMirror)
{
	if (pRender == nullptr) return RESULT_BAD;
	if (pLayer == nullptr) return RESULT_BAD;
	if (pLayerSet == nullptr) return RESULT_BAD;
	if (m_pDrawFunction == nullptr) return RESULT_BAD;

	//
	D2D1_POINT_2F fptCenter;
	fptCenter.x = (m_frtPictureBox.left + (m_frtPictureBox.right - m_frtPictureBox.left) / 2.f);//rtClient.Width() / 2.f;
	fptCenter.y = (m_frtPictureBox.top + (m_frtPictureBox.bottom - m_frtPictureBox.top) / 2.f);
	//D2D1::Matrix3x2F matTrans = m_matScaleView.TransformPoint(fptCenter);

	D2D1_POINT_2F fptPan = D2D1::Point2F(0.f, 0.f);
	_SetViewScale(pUserSetInfo, m_fSplitScale, fptPan, dAngle, bMirror);

	D2D1::Matrix3x2F matViewScale = D2D1::Matrix3x2F(m_matDrawTransform.m11, m_matDrawTransform.m12, m_matDrawTransform.m21, m_matDrawTransform.m22, 0, 0);
	D2D1_POINT_2F fptCenterT = matViewScale.TransformPoint(fptCenter);

	fptPan.x = (-1.0f*fptCenterT.x) + m_sizeSplitImage.width / 2.f;
	fptPan.y = (-1.0f*fptCenterT.y) + m_sizeSplitImage.height / 2.f;

	_SetViewScale(pUserSetInfo, m_fSplitScale, fptPan, dAngle, bMirror);

	pRender->SetTransform(m_matDrawTransform);
	//

/*	//Set Scale
	float fInnerSizeX = pRender->GetSize().width;
	float fInnerSizeY = pRender->GetSize().height;

	float fInnerScaleX = fabs(fInnerSizeX / (m_frtPictureBox.right - m_frtPictureBox.left));
	float fInnerScaleY = fabs(fInnerSizeY / (m_frtPictureBox.bottom - m_frtPictureBox.top));
	float fInnerScale = fInnerScaleX <= fInnerScaleY ? fInnerScaleX : fInnerScaleY;

	D2D1_POINT_2F fptCenter;
	fptCenter.x = (m_frtPictureBox.left + (m_frtPictureBox.right - m_frtPictureBox.left) / 2.f);//rtClient.Width() / 2.f;
	fptCenter.y = (m_frtPictureBox.top + (m_frtPictureBox.bottom - m_frtPictureBox.top) / 2.f);
	//D2D1::Matrix3x2F matTrans = m_matScaleView.TransformPoint(fptCenter);

	D2D1_POINT_2F fptPan = D2D1::Point2F(0.f, 0.f);
	_SetViewScale(pUserSetInfo, fInnerScale, fptPan, dAngle, bMirror);

	D2D1::Matrix3x2F matViewScale = D2D1::Matrix3x2F(m_matDrawTransform.m11, m_matDrawTransform.m12, m_matDrawTransform.m21, m_matDrawTransform.m22, 0, 0);
	D2D1_POINT_2F fptCenterT = matViewScale.TransformPoint(fptCenter);

	fptPan.x = (-1.0f*fptCenterT.x) + fInnerSizeX / 2.f;
	fptPan.y = (-1.0f*fptCenterT.y) + fInnerSizeY / 2.f;

	_SetViewScale(pUserSetInfo, fInnerScale, fptPan, dAngle, bMirror);

	pRender->SetTransform(m_matDrawTransform);
*/
	return m_pDrawFunction->OnMake_Layer(pRender, pLayer, pLayerSet, pFontArr, pRotMirrArr, bNeedUpdate, pUserSetInfo, m_eDrawLevel);
}

void CSaveLayer_NSIS::_SetViewScale(IN const UserSetInfo_t *pUserSetInfo, IN const float &fScale, IN const D2D_POINT_2F &fptPan,
	double dAngle, bool bMirror)
{
	/*D2D1::Matrix3x2F matScale = D2D1::Matrix3x2F(fScale, 0, 0, fScale, 0, 0);

	D2D1::Matrix3x2F matRotate = D2D1::Matrix3x2F::Rotation(static_cast<float>(pUserSetInfo->dAngle),
		CDrawFunction::GetRectCenter(m_frtPictureBox));

	bool bIsMirror = pUserSetInfo->bMirror;

	if (m_eDrawLevel == eDrawLevel::eUnitLv)
	{
		matRotate = D2D1::Matrix3x2F::Rotation(static_cast<float>(dAngle),
			CDrawFunction::GetRectCenter(m_frtPictureBox));

		bIsMirror = bMirror;
	}

	D2D1::Matrix3x2F matTotal = matScale * matRotate;

	if (bIsMirror)
	{
		if (pUserSetInfo->mirrorDir == eDirectionType::eBoth)
		{
			m_matDrawTransform = D2D1::Matrix3x2F(matTotal.m11, matTotal.m12, matTotal.m21*1.0f, matTotal.m22*1.0f, fptPan.x, fptPan.y);
			m_matDrawTransform = D2D1::Matrix3x2F(m_matDrawTransform.m11 * -1.f, m_matDrawTransform.m12* -1.f, m_matDrawTransform.m21, m_matDrawTransform.m22, fptPan.x, fptPan.y);
		}
		else if (pUserSetInfo->mirrorDir == eDirectionType::eHorizontal)
		{
			m_matDrawTransform = D2D1::Matrix3x2F(matTotal.m11, matTotal.m12, matTotal.m21*1.0f, matTotal.m22*1.0f, fptPan.x, fptPan.y);
		}
		else if (pUserSetInfo->mirrorDir == eDirectionType::eVertical)
		{
			m_matDrawTransform = D2D1::Matrix3x2F(matTotal.m11 * -1.f, matTotal.m12* -1.f, matTotal.m21*-1.0f, matTotal.m22*-1.0f, fptPan.x, fptPan.y);
		}
		else
		{
			m_matDrawTransform = D2D1::Matrix3x2F(matTotal.m11, matTotal.m12, matTotal.m21*-1.0f, matTotal.m22*-1.0f, fptPan.x, fptPan.y);
		}
	}
	else
	{
		m_matDrawTransform = D2D1::Matrix3x2F(matTotal.m11, matTotal.m12, matTotal.m21*-1.0f, matTotal.m22*-1.0f, fptPan.x, fptPan.y);
	}*/

	D2D1::Matrix3x2F matScale = D2D1::Matrix3x2F(fScale, 0, 0, fScale, 0, 0);
	m_matDrawTransform = D2D1::Matrix3x2F(matScale.m11, matScale.m12, matScale.m21*-1.0f, matScale.m22*-1.0f, fptPan.x, fptPan.y);
}

UINT32 CSaveLayer_NSIS::_SetSaveImage(IN const CString &strfilename, IN const double &dAngle)
{
	if (strfilename.GetLength() < 1)
		return RESULT_BAD;

	fs::path fsPath;
	fsPath = std::string(CT2CA(strfilename));
	CString strPath = CA2CT(fsPath.parent_path().string().c_str());

	::SHCreateDirectoryEx(NULL, strPath, NULL);

	UINT32 iRet = _SaveImage(m_pWicBitmap, strfilename);

	
	//if (RESULT_GOOD == iRet)
	if (PathFileExists(strfilename))
	{
		//Load Image
		String strLoad = String(CT2CA(strfilename));
		Mat matLoadImg = imread(strLoad);

		//
		Mat matRotate;
		matLoadImg.copyTo(matRotate);


		if (dAngle != 0.)
		{
			if (dAngle == 90.)
			{
				cv::rotate(matLoadImg, matRotate, cv::ROTATE_90_CLOCKWISE);
			}
			else if (dAngle == 180.)
			{
				cv::rotate(matLoadImg, matRotate, cv::ROTATE_180);
			}
			else if (dAngle == 270.)
			{
				cv::rotate(matLoadImg, matRotate, cv::ROTATE_90_COUNTERCLOCKWISE);
			}
		}

		//Rotate (-dAngle)
		//Convert as 8-bit Image from 32-bit Bitmap
		Mat matOutput;
		cvtColor(matRotate, matOutput, COLOR_BGR2GRAY);

		//		Mat matBin;
		//		threshold(matOutput, matBin, 100, 255, THRESH_BINARY);

				//Save as 8-bit Image
		imwrite(strLoad, matOutput);		
	}

	return iRet;
}
