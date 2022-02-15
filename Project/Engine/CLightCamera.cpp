#include "pch.h"
#include "CLightCamera.h"

#include "CDevice.h"

#include "CSceneMgr.h"
#include "CScene.h"
#include "CLayer.h"

#include "CGameObject.h"
#include "CTransform.h"
#include "CMeshRender.h"

#include "CResMgr.h"
#include "CInstancingBuffer.h"
#include "CAnimator3D.h"
#include "CTerrain.h"

CLightCamera::CLightCamera()
{
}

CLightCamera::~CLightCamera()
{
}

void CLightCamera::finalupdate()
{
	CalViewMat();

	CalProjMat();

	CalRay();
}

void CLightCamera::SortShadowObject()
{
	m_mapInstGroup_S.clear();

	CScene* pCurScene = CSceneMgr::GetInst()->GetCurScene();	

	for (UINT i = 0; i < MAX_LAYER; ++i)
	{
		if (m_iLayerCheck & (1 << i))
		{
			const vector<CGameObject*>& vecObj = pCurScene->GetLayer(i)->GetObjects();

			for (size_t j = 0; j < vecObj.size(); ++j)
			{
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

				if (!vecObj[j]->IsDynamicShdow() ||
					nullptr == vecObj[j]->MeshRender() || 
					nullptr == vecObj[j]->MeshRender()->GetMesh())
					continue;

				// ����ü �׽�Ʈ(�̱���)

				// ���׸��� ������ŭ �ݺ�
				UINT iMtrlCount = vecObj[j]->MeshRender()->GetMtrlCount();
				for (UINT iMtrl = 0; iMtrl < iMtrlCount; ++iMtrl)
				{
					// Material �� �����ϰ� ���� �ʰų�, Material �� ���� Shader �� �������� ���� ���¶�� Continue
					Ptr<CMaterial> pMtrl = vecObj[j]->MeshRender()->GetSharedMaterial(iMtrl);
					if (nullptr == pMtrl || pMtrl->GetShader() == nullptr)
						continue;
										
					uInstID uID = {};
					uID.llID = vecObj[j]->MeshRender()->GetInstShadowID(iMtrl);

					// ID �� 0 �� ==> Mesh �� Material �� ���õ��� �ʾҴ�.
					if (0 == uID.llID)
						continue;

					map<ULONG64, vector<tInstObj>>::iterator iter = m_mapInstGroup_S.find(uID.llID);
					if (iter == m_mapInstGroup_S.end())
					{
						m_mapInstGroup_S.insert(make_pair(uID.llID, vector<tInstObj>{tInstObj{ vecObj[j], iMtrl }}));
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

void CLightCamera::render_shadowmap()
{
	// ����İ� ��������� �������� ī�޶��� ������ ��ü�ص�
	g_transform.matView = m_matView;
	g_transform.matViewInv = m_matViewInv;
	g_transform.matProj = m_matProj;
	
	for (auto& pair : m_mapSingleObj)
	{
		pair.second.clear();
	}

	tInstancingData tInstData = {};

	for (auto& pair : m_mapInstGroup_S)
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
			|| pair.second[0].pObj->Terrain())
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
		Ptr<CMaterial> pMtrl = CResMgr::GetInst()->FindRes<CMaterial>(L"ShadowMapMtrl");

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
				instObj.pObj->MeshRender()->render_shadowmap(instObj.iMtrlIdx);
			}
		}
	}
}