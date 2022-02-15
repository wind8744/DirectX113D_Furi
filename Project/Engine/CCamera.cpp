#include "pch.h"
#include "CCamera.h"

#include "CTransform.h"
#include "CDevice.h"
#include "CCore.h"

#include "CKeyMgr.h"

#include "CRenderMgr.h"
#include "CSceneMgr.h"
#include "CScene.h"
#include "CLayer.h"

#include "CMeshRender.h"
#include "CParticleSystem.h"
#include "CCollider2D.h"
#include "CAnimator2D.h"
#include "CAnimator3D.h"
#include "CTerrain.h"

#include "CMaterial.h"
#include "CGraphicsShader.h"

#include "CRenderMgr.h"
#include "CMRT.h"

#include "CInstancingBuffer.h"





CCamera::CCamera()
	: CComponent(COMPONENT_TYPE::CAMERA)
	, m_frustum(this)
	, m_eProjType(PROJ_TYPE::PERSPECTIVE)
	, m_vScale(Vec2(1.f, 1.f))
	, m_fFOV(XM_PI / 2.f)
	, m_tRay{}
	, m_fFar(1000.f)
	, m_iLayerCheck(0)
{
	m_frustum.init();

	POINT ptRes = CDevice::GetInst()->GetBufferResolution();
	m_vProjRange = Vec2((float)ptRes.x, (float)ptRes.y);
}

CCamera::~CCamera()
{
}

void CCamera::update()
{

}

void CCamera::finalupdate()
{
	CalViewMat();

	CalProjMat();

	CalRay();

	m_frustum.finalupdate();
	CRenderMgr::GetInst()->RegisterCamera(this);


}


void CCamera::SetLayerCheck(UINT _iLayerIdx, bool _bAdd)
{
	assert(!(_iLayerIdx >= MAX_LAYER));

	if (_bAdd)
	{
		m_iLayerCheck |= 1 << _iLayerIdx;
	}
	else
	{
		m_iLayerCheck &= ~(1 << _iLayerIdx);
	}
}

void CCamera::SortObject()
{
	for (auto& pair : m_mapInstGroup_D)
		pair.second.clear();
	for (auto& pair : m_mapInstGroup_F)
		pair.second.clear();


	m_vecParticle.clear();
	m_vecPostEffect.clear();
	m_vecCollider.clear();
	CScene* pCurScene = CSceneMgr::GetInst()->GetCurScene();


	/// <summary>
	/// ui 29 ���̤ä� ********************�߰�
	const vector<CGameObject*>& vecObj = pCurScene->GetLayer(29)->GetObjects();

	for (size_t j = 0; j < vecObj.size(); ++j)
	{
			m_vecUi.push_back(vecObj[j]);
	}
	/// </summary>


	for (UINT i = 0; i < MAX_LAYER; ++i)
	{
		/// <summary>
	/// ui 29����********************�߰�
		if (i == 29)
		{
			int t = 1;
			continue;
		}
		/// </summary>

		if (m_iLayerCheck & (1 << i))
		{
			const vector<CGameObject*>& vecObj = pCurScene->GetLayer(i)->GetObjects();
			

			for (size_t j = 0; j < vecObj.size(); ++j)
			{
				
				if (vecObj[j]->Collider2D())
				{
					m_vecCollider.push_back(vecObj[j]->Collider2D());
				}
				if (vecObj[j]->ParticleSystem())
				{
					m_vecParticle.push_back(vecObj[j]);
					continue;
				}


				// Terrain component �� ���� ���
				/*if (vecObj[j]->Terrain())
				{
					uInstID uID = {};
					uID.llID = vecObj[j]->Terrain()->GetInstID(0);

					map<ULONG64, vector<tInstObj>>::iterator iter = m_mapInstGroup_D->find(uID.llID);
					if (iter == m_mapInstGroup_D->end())
					{
						m_mapInstGroup_D->insert(make_pair(uID.llID, vector<tInstObj>{tInstObj{ vecObj[j], iMtrl }}));
					}
					else
					{
						iter->second.push_back(tInstObj{ vecObj[j], 0 });
					}
				}*/

				if (nullptr != vecObj[j]->ParticleSystem())
				{
					if (false == vecObj[j]->ParticleSystem()->IsActive())
					{
						continue;
					}
				}
				if (nullptr == vecObj[j]->MeshRender() || nullptr == vecObj[j]->MeshRender()->GetMesh())
					continue;

				// ����ü �׽�Ʈ(�̱���)
				if (vecObj[j]->IsFrustum())
				{
					Vec3 vWorldPos = vecObj[j]->Transform()->GetWorldPos();
					if (!m_frustum.CheckFrustum(vWorldPos))
					{
						continue;
					}
				}
				// ���׸��� ������ŭ �ݺ�
				UINT iMtrlCount = vecObj[j]->MeshRender()->GetMtrlCount();
				for (UINT iMtrl = 0; iMtrl < iMtrlCount; ++iMtrl)
				{
					// Material �� �����ϰ� ���� �ʰų�, Material �� ���� Shader �� �������� ���� ���¶�� Continue
					Ptr<CMaterial> pMtrl = vecObj[j]->MeshRender()->GetSharedMaterial(iMtrl);
					if (nullptr == pMtrl || pMtrl->GetShader() == nullptr)
						continue;

					// Shader �� POV �� ���� �ν��Ͻ� �׷��� �з��Ѵ�.
					map<ULONG64, vector<tInstObj>>* pMap = NULL;
					if (pMtrl->GetShader()->GetPOV() == SHADER_POV::DEFERRED)
						pMap = &m_mapInstGroup_D;
					else if (pMtrl->GetShader()->GetPOV() == SHADER_POV::FORWARD)
						pMap = &m_mapInstGroup_F;
					else if (pMtrl->GetShader()->GetPOV() == SHADER_POV::POSTEFFECT)
					{
						m_vecPostEffect.push_back(vecObj[j]);
					}
					else
					{
						assert(nullptr);
						continue;
					}

					uInstID uID = {};
					uID.llID = vecObj[j]->MeshRender()->GetInstID(iMtrl);

					// ID �� 0 �� ==> Mesh �� Material �� ���õ��� �ʾҴ�.
					if (0 == uID.llID || !pMap)
						continue;
					
					map<ULONG64, vector<tInstObj>>::iterator iter = pMap->find(uID.llID);
					if (iter == pMap->end())
					{
						pMap->insert(make_pair(uID.llID, vector<tInstObj>{tInstObj{ vecObj[j], iMtrl }}));
					}
					else
					{
						iter->second.push_back(tInstObj{ vecObj[j], iMtrl });
					}
				}
			}
		}
	}
}


void CCamera::render_deferred()
{
	g_transform.matView = m_matView;
	g_transform.matViewInv = m_matViewInv;
	g_transform.matProj = m_matProj;

	for (auto& pair : m_mapSingleObj)
	{
		pair.second.clear();
	}

	tInstancingData tInstData = {};

	for (auto& pair : m_mapInstGroup_D)
	{
		// �׷� ������Ʈ�� ���ų�, ���̴��� ���� ���
		if (pair.second.empty())
			continue;

		// instancing ���� ���� �̸��̰ų�
		// Animation2D ������Ʈ�ų�(��������Ʈ �ִϸ��̼� ������Ʈ)
		// Shader �� Instancing �� �������� �ʴ°��
		if (pair.second.size() <= 1
			|| pair.second[0].pObj->Animator2D()
			|| pair.second[0].pObj->MeshRender()->GetSharedMaterial(pair.second[0].iMtrlIdx)->GetShader()->GetInstVS() == nullptr
			|| pair.second[0].pObj->Terrain()
			)
		{
			// �ش� ��ü���� ���� ���������� ��ȯ
			for (UINT i = 0; i < pair.second.size(); ++i)
			{
				map<INT_PTR, vector<tInstObj>>::iterator iter
					= m_mapSingleObj.find((INT_PTR)pair.second[i].pObj);

				if (iter != m_mapSingleObj.end())
					iter->second.push_back(pair.second[i]);
				else
				{
					m_mapSingleObj.insert(make_pair((INT_PTR)pair.second[i].pObj, vector<tInstObj>{pair.second[i]}));
				}
			}
			continue;
		}

		CGameObject* pObj = pair.second[0].pObj;
		Ptr<CMesh> pMesh = pObj->MeshRender()->GetMesh();
		Ptr<CMaterial> pMtrl = pObj->MeshRender()->GetSharedMaterial(pair.second[0].iMtrlIdx);

		// Instancing ���� Ŭ����
		CInstancingBuffer::GetInst()->Clear();

		int iRowIdx = 0;
		bool bHasAnim3D = false;
		for (UINT i = 0; i < pair.second.size(); ++i)
		{
			tInstData.matWorld = pair.second[i].pObj->Transform()->GetWorldMat();
			tInstData.matWV = tInstData.matWorld * m_matView;
			tInstData.matWVP = tInstData.matWV * m_matProj;

			if (pair.second[i].pObj->Animator3D())
			{
				pair.second[i].pObj->Animator3D()->UpdateData();
				tInstData.iRowIdx = iRowIdx++;
				CInstancingBuffer::GetInst()->AddInstancingBoneMat(pair.second[i].pObj->Animator3D()->GetFinalBoneMat());
				bHasAnim3D = true;
			}
			else
				tInstData.iRowIdx = -1;


			CInstancingBuffer::GetInst()->AddInstancingData(tInstData);
		}

		// �ν��Ͻ̿� �ʿ��� �����͸� ����(SysMem -> GPU Mem)
		CInstancingBuffer::GetInst()->SetData();

		if (bHasAnim3D)
		{
			int num = pObj->Animator3D()->GetCurClip();
			pMtrl->SetAnim3D(true); // Animation Mesh �˸���
			pMtrl->SetBoneCount(pMesh->GetBoneCount(num));
		}

		pMtrl->UpdateData(1);
		pMesh->render_instancing(pair.second[0].iMtrlIdx);

		// ����
		pMtrl->Clear();
	}

	// ���� ������
	for (auto& pair : m_mapSingleObj)
	{
		if (pair.second.empty())
			continue;

		for (auto& instObj : pair.second)
		{


			if (instObj.pObj->Terrain())
			{
				instObj.pObj->Terrain()->render();
			}
			else
			{
				instObj.pObj->MeshRender()->render(instObj.iMtrlIdx);
			}
		}
	}
}

void CCamera::render_forward()
{
	g_transform.matView = m_matView;
	g_transform.matViewInv = m_matViewInv;
	g_transform.matProj = m_matProj;

	for (auto& pair : m_mapSingleObj)
	{
		pair.second.clear();
	}

	tInstancingData tInstData = {};

	for (auto& pair : m_mapInstGroup_F)
	{
		// �׷� ������Ʈ�� ���ų�, ���̴��� ���� ���
		if (pair.second.empty())
			continue;

		// instancing ���� ���� �̸��̰ų�
		// Animation2D ������Ʈ�ų�(��������Ʈ �ִϸ��̼� ������Ʈ)
		// Shader �� Instancing �� �������� �ʴ°��
		if (pair.second.size() <= 1
			|| pair.second[0].pObj->Animator2D()
			|| pair.second[0].pObj->MeshRender()->GetSharedMaterial(pair.second[0].iMtrlIdx)->GetShader()->GetInstVS() == nullptr)
		{
			// �ش� ��ü���� ���� ���������� ��ȯ
			for (UINT i = 0; i < pair.second.size(); ++i)
			{
				map<INT_PTR, vector<tInstObj>>::iterator iter
					= m_mapSingleObj.find((INT_PTR)pair.second[i].pObj);

				if (iter != m_mapSingleObj.end())
					iter->second.push_back(pair.second[i]);
				else
				{
					m_mapSingleObj.insert(make_pair((INT_PTR)pair.second[i].pObj, vector<tInstObj>{pair.second[i]}));
				}
			}
			continue;
		}

		CGameObject* pObj = pair.second[0].pObj;
		Ptr<CMesh> pMesh = pObj->MeshRender()->GetMesh();
		Ptr<CMaterial> pMtrl = pObj->MeshRender()->GetSharedMaterial(pair.second[0].iMtrlIdx);

		// Instancing ���� Ŭ����
		CInstancingBuffer::GetInst()->Clear();

		int iRowIdx = 0;
		bool bHasAnim3D = false;
		for (UINT i = 0; i < pair.second.size(); ++i)
		{
			tInstData.matWorld = pair.second[i].pObj->Transform()->GetWorldMat();
			tInstData.matWV = tInstData.matWorld * m_matView;
			tInstData.matWVP = tInstData.matWV * m_matProj;

			if (pair.second[i].pObj->Animator3D())
			{
				pair.second[i].pObj->Animator3D()->UpdateData();
				tInstData.iRowIdx = iRowIdx++;
				CInstancingBuffer::GetInst()->AddInstancingBoneMat(pair.second[i].pObj->Animator3D()->GetFinalBoneMat());
				bHasAnim3D = true;
			}
			else
				tInstData.iRowIdx = -1;

			CInstancingBuffer::GetInst()->AddInstancingData(tInstData);
		}

		// �ν��Ͻ̿� �ʿ��� �����͸� ����(SysMem -> GPU Mem)
		CInstancingBuffer::GetInst()->SetData();

		if (bHasAnim3D)
		{
			int num = pObj->Animator3D()->GetCurClip();
			pMtrl->SetAnim3D(true); // Animation Mesh �˸���
			pMtrl->SetBoneCount(pMesh->GetBoneCount(num));
		}

		pMtrl->UpdateData(1);
		pMesh->render_instancing(pair.second[0].iMtrlIdx);
	}

	// ���� ������
	for (auto& pair : m_mapSingleObj)
	{
		if (pair.second.empty())
			continue;

		for (auto& instObj : pair.second)
		{
			instObj.pObj->MeshRender()->render(instObj.iMtrlIdx);
		}
	}

	// Particle Render
	for (size_t i = 0; i < m_vecParticle.size(); ++i)
	{
		m_vecParticle[i]->ParticleSystem()->render();

		//if (m_vecParticle[i]->Collider2D())
			//m_vecParticle[i]->Collider2D()->render();
	}
}

void CCamera::render_ui() //************************************************************* �߰�
{
	g_transform.matView = m_matView;
	g_transform.matProj = m_matProj;

	for (size_t i = 0; i < m_vecUi.size(); ++i)
	{
		//CRenderMgr::GetInst()->CopyTarget();

		m_vecUi[i]->MeshRender()->render(0);

		//if (m_vecPostEffect[i]->Collider2D())
			//m_vecPostEffect[i]->Collider2D()->render();
	}
}

void CCamera::render_posteffect()
{
	g_transform.matView = m_matView;
	g_transform.matProj = m_matProj;

	for (size_t i = 0; i < m_vecPostEffect.size(); ++i)
	{
		CRenderMgr::GetInst()->CopyTarget();

		m_vecPostEffect[i]->MeshRender()->render();

		//if (m_vecPostEffect[i]->Collider2D())
			//m_vecPostEffect[i]->Collider2D()->render();
	}
}

void CCamera::render_collider()
{
	g_transform.matView = m_matView;
	g_transform.matProj = m_matProj;

	for (size_t i = 0; i < m_vecCollider.size(); ++i)
	{

		m_vecCollider[i]->render();

		//if (m_vecPostEffect[i]->Collider2D())
			//m_vecPostEffect[i]->Collider2D()->render();
	}
}

void CCamera::CalViewMat()
{
	// View ���
	// View ��� �̵���Ʈ(ī�޶� ��ġ ��)
	Vec3 vCamPos = -Transform()->GetWorldPos();
	Matrix matTranslation = XMMatrixTranslation(vCamPos.x, vCamPos.y, vCamPos.z);

	// View ��� ȸ����Ʈ(��, ��, �� ������ ��ġ)
	Matrix matRot = XMMatrixIdentity();
	Vec3 vRight = Transform()->GetWorldDir(DIR_TYPE::RIGHT);
	Vec3 vUp = Transform()->GetWorldDir(DIR_TYPE::UP);
	Vec3 vFront = Transform()->GetWorldDir(DIR_TYPE::FRONT);

	matRot._11 = vRight.x;	matRot._12 = vUp.x; matRot._13 = vFront.x;
	matRot._21 = vRight.y;	matRot._22 = vUp.y;	matRot._23 = vFront.y;
	matRot._31 = vRight.z;	matRot._32 = vUp.z;	matRot._33 = vFront.z;

	m_matView = matTranslation * matRot;
	m_matViewInv = XMMatrixInverse(nullptr, m_matView);
}

void CCamera::CalProjMat()
{
	// Proj ���
	// FOV(file of view) �þ߰�
	// Aspect Ratio ��Ⱦ��
	if (PROJ_TYPE::PERSPECTIVE == m_eProjType)
	{
		POINT ptResolution = CDevice::GetInst()->GetBufferResolution();
		float fAR = (float)ptResolution.x / (float)ptResolution.y;
		m_matProj = XMMatrixPerspectiveFovLH(XM_PI / 4.f, fAR, 1.f, m_fFar);
	}
	else
	{
		POINT ptRes = CCore::GetInst()->GetWndResolution();
		m_matProj = XMMatrixOrthographicLH(m_vProjRange.x * m_vScale.x, m_vProjRange.y * m_vScale.y, 1.f, m_fFar);
	}
	m_matProjInv = XMMatrixInverse(nullptr, m_matProj);
}

void CCamera::CalRay()
{
	// SwapChain Ÿ���� ViewPort ����
	CMRT* pSwapChain = CRenderMgr::GetInst()->GetMRT(MRT_TYPE::SWAPCHAIN);
	D3D11_VIEWPORT tVP = {};
	pSwapChain->GetViewPort(&tVP);

	//  ���� ���콺 ��ǥ
	Vec2 vMousePos = CKeyMgr::GetInst()->GetMousePos();

	// ������ ī�޶��� ��ǥ�� �ݵ�� ������.
	m_tRay.vPoint = Transform()->GetWorldPos();

	// view space ������ ����
	m_tRay.vDir.x = ((((vMousePos.x - tVP.TopLeftX) * 2.f / tVP.Width) - 1.f) - m_matProj._31) / m_matProj._11;
	m_tRay.vDir.y = (-(((vMousePos.y - tVP.TopLeftY) * 2.f / tVP.Height) - 1.f) - m_matProj._32) / m_matProj._22;
	m_tRay.vDir.z = 1.f;

	// world space ������ ����
	m_tRay.vDir = XMVector3TransformNormal(m_tRay.vDir, m_matViewInv);
	m_tRay.vDir.Normalize();
}

void CCamera::SaveToScene(FILE* _pFile)
{
	CComponent::SaveToScene(_pFile);

	fwrite(&m_eProjType, sizeof(UINT), 1, _pFile);
	fwrite(&m_vScale, sizeof(Vec2), 1, _pFile);
	fwrite(&m_fFOV, sizeof(float), 1, _pFile);
	fwrite(&m_iLayerCheck, sizeof(UINT), 1, _pFile);
}

void CCamera::LoadFromScene(FILE* _pFile)
{
	CComponent::LoadFromScene(_pFile);

	fread(&m_eProjType, sizeof(UINT), 1, _pFile);
	fread(&m_vScale, sizeof(Vec2), 1, _pFile);
	fread(&m_fFOV, sizeof(float), 1, _pFile);
	fread(&m_iLayerCheck, sizeof(UINT), 1, _pFile);
}
