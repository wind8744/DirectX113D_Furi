#pragma once

#include "define.h"

struct VTX
{
	Vec3 vPos;
	Vec4 vColor;	
	Vec2 vUV;

	Vec3 vTangent;	// ����
	Vec3 vNormal;	// ����
	Vec3 vBinormal; // ������

	Vec4 vWeights;	// �� ����ġ
	Vec4 vIndices;  // �� �ε���
};

struct tParticle
{
	Vec3	vWorldPos;
	Vec3	vWorldDir;

	float	m_fCurTime;
	float	m_fMaxLife;
	float   m_fSpeed;

	int		iAlive;

private:
	int     iPadding[2];
};

struct tParticleShared
{
	int iAliveCount;
	int iPadding[3];
};

struct tLightColor
{
	Vec4 vDiff; // Diffuse (���ݻ籤 ����)
	Vec4 vSpec; // �ݻ籤 ����
	Vec4 vAmb;  // Ambient (ȯ�汤)
};

struct tLightInfo
{
	tLightColor color;  // ���� ����

	Vec4		vDir;   // ������ ����
	Vec4		vWorldPos; // ������ ��ġ

	LIGHT_TYPE  eType;	// ���� Ÿ��		
	float		fRange; // ������ ����
	float		fAngle; // ���� ����
	
	int			iPadding;
};

struct tAnim2D
{
	Vec2	vLeftTop;
	Vec2	vStep;
	int		iAnimUse[4];
};

struct tRay
{
	Vec3 vDir;		// ������ ����
	Vec3 vPoint;	// �������� �� ��
};


// ===========
// Instancing
// ===========
union uInstID
{
	struct {
		UINT iMesh;
		WORD iMtrl;
		WORD iMtrlIdx;
	};
	ULONG64 llID;
};

class CGameObject;

struct tInstObj
{
	CGameObject* pObj;
	UINT			iMtrlIdx;
};

// =============
// Animation 3D
// =============
struct tFrameTrans
{
	Vec4	vTranslate;
	Vec4	vScale;
	Vec4	qRot;
};

struct tMTKeyFrame
{
	double	dTime;
	int		iFrame;
	Vec3	vTranslate;
	Vec3	vScale;
	Vec4	qRot;
};


struct tMTBone
{
	wstring				strBoneName;
	int					iDepth;
	int					iParentIndx;
	Matrix				matOffset;	// Offset ���(�� -> ��Ʈ ������ ���)
	Matrix				matBone;   // �̰� �Ⱦ�
	vector<tMTKeyFrame>	vecKeyFrame;
};

struct tMTAnimClip
{
	wstring			strAnimName;
	int				iStartFrame;
	int				iEndFrame;
	int				iFrameLength;

	double			dStartTime;
	double			dEndTime;
	double			dTimeLength;
	float			fUpdateTime; // �̰� �Ⱦ�

	FbxTime::EMode	eMode;
};



// ==============
// ������� ����ü
// ==============
struct tTransform
{
	Matrix	matWorld;
	Matrix	matView;
	Matrix	matProj;
	Matrix	matWV;
	Matrix  matWVP;

	Matrix	matWorldInv;
	Matrix	matViewInv;
};

struct tMtrlInfo
{
	Vec4 vDiff;
	Vec4 vSpec;
	Vec4 vAmb;
	Vec4 vEmis;

	int		arrInt[4];
	float	arrFloat[4];
	Vec2	arrVec2[4];
	Vec4	arrVec4[4];
	Matrix	arrMat[4];

	int     arrTexCheck[12];
	int		iAnim3D[4];		// Animation3D �߰����� ����
};

struct tGlobalValue
{
	Vec2 vResolution;
	Vec2 vNoiseResolution;
	float fDeltaTime;
	float fAcctime;

	UINT iLight2DCount;
	UINT iLight3DCount;
};
struct tMeshRink
{
	//string s_ParentNode;
	//string s_ParentMesh;
	string s_NodeName;
	string s_MeshName;
	//bool   b_ParentMesh;
	//string s_ChildNode;
	//string s_ChildMesh;
	//bool   b_ChildMesh;
	//Vec3   v_ChildPos;
	//Vec3   v_ChildScale;
	//Vec3   v_ChildRot;
};

// nav mesh
struct NavMesh
{
	Vec3 fPointPos[3]; // ���� ��ġ ����
	vector<UINT> vecAdjIdx; // ���� ��� �ﰢ�� idx
};

struct tSectorInfo
{
	float fAngle;
	float fDistance;
	int	  iPadding[2];
};