#pragma once

#include "../iODB/iODB.h"
#include "CDrawFunction.h"

class __declspec(dllexport) CFeatureData : public CObject
{
public:

	enum {
		enumEditPointSize = 5,//

		enumEditLineCount = 2,//Start, End
		enumEditPadCount = 0,//Pad Center
		enumEditArcCount = 3,//Arc Start, End, Center
		//Surface�� ObOe, OsOc�� ������� ����
	};

public:

	CFeatureData();
	~CFeatureData();

	void SetTp(int nLater, int nFeatureIndex, CFeatureZ *in_pFeature, int in_nMaskIndex);
	void SetLine(int nLayer, int nFeatureIndex, CFeatureL *in_pFeature, int in_nMaskIndex);
	void SetPad(int nLayer, int nFeatureIndex, CFeatureP *in_pFeature, int in_nMaskIndex);
	void SetArc(int nLayer, int nFeatureIndex, CFeatureA *in_pFeature, int in_nMaskIndex);
	void SetText(int nLayer, int nFeatureIndex, CFeatureT *in_pFeature, int in_nMaskIndex);
	void SetSurface(int nLayer, int nFeatureIndex, CFeatureS *in_pFeature, int in_nMaskIndex);//���� : Highlight�Ǿ� �ִ� ObOe ������ �Է� ����
	void SetMove(IN const double &dMoveX_mm, IN const double &dMoveY_mm);
	void Reset();

	void UpdateEditPoint();//���� m_pFeature�� ������ m_vecEditPoint�� ������Ʈ�Ѵ�.

	UINT32 MoveEditPoint(IN const int nPointIndex, IN const double &dMoveX, IN const double &dMoveY);
	
	UINT32 Resize_Apply(IN const float &fSizePersent);


	int m_nLayer;
	int m_nFeatureIndex;

	

	CFeature *m_pFeature = nullptr;

	int m_nMaskIndex;

	//edit Point
		//���콺�� �����ϴ� ����Ʈ ����
		//pSymbol->vcParam ���� + Move Point Size(Pad Center or Line S, E, or Arc S, E, C...)
	vector<RECTD> m_vecEditPoint;  
	const int m_nPointSize = enumEditPointSize;//Zoom�� �����Ǿ� ����
	//

private :
	RECTD _GetRect(IN const CPointD &dptCenter, IN const double &dHalfWidth, IN const double &dHalfHeight);
	
	UINT32 _SetEditPoint(CFeature *pFeature, vector<RECTD> &vecEditPoint);

	UINT32 _SetEditPoint_Line(CFeatureL *pFeatureL, vector<RECTD> &vecEditPoint);
	UINT32 _SetEditPoint_Pad(CFeatureP *pFeatureP, vector<RECTD> &vecEditPoint);
	UINT32 _SetEditPoint_Arc(CFeatureA *pFeatureA, vector<RECTD> &vecEditPoint);
	UINT32 _SetEditPoint_Surface(CFeatureS *pFeatureS, vector<RECTD> &vecEditPoint);

	UINT32 _SetEditPoint_Pad_Rect(CFeatureP *pFeatureP, vector<RECTD> &vecEditPoint);
	UINT32 _SetEditPoint_Pad_Square(CFeatureP *pFeatureP, vector<RECTD> &vecEditPoint);

	//Move
	UINT32 _MoveEditPoint_Line(CFeatureL *pFeatureL, IN const int nPointIndex, IN const double &dMoveX, IN const double &dMoveY);
	UINT32 _MoveEditPoint_Pad(CFeatureP *pFeatureP, IN const int nPointIndex, IN const double &dMoveX, IN const double &dMoveY);
	UINT32 _MoveEditPoint_Arc(CFeatureA *pFeatureA, IN const int nPointIndex, IN const double &dMoveX, IN const double &dMoveY);
	UINT32 _MoveEditPoint_Surface(CFeatureS *pFeatureS, IN const int nPointIndex, IN const double &dMoveX, IN const double &dMoveY);

	UINT32 _MoveEditPoint_Pad_Rect(CFeatureP *pFeatureP, IN const int nPointIndex, IN const double &dMoveX, IN const double &dMoveY);
	UINT32 _MoveEditPoint_Pad_Square(CFeatureP *pFeatureP, IN const int nPointIndex, IN const double &dMoveX, IN const double &dMoveY);
};