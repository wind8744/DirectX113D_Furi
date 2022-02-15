#include "pch.h"
#include "CUICam.h"

#include "CDevice.h"

#include "CSceneMgr.h"
#include "CScene.h"
#include "CLayer.h"
#include "CTransform.h"
#include "CDevice.h"
#include "CCore.h"

#include "CGameObject.h"
#include "CTransform.h"
#include "CMeshRender.h"

#include "CRenderMgr.h"
#include "CResMgr.h"
#include "CInstancingBuffer.h"
#include "CAnimator3D.h"
#include "CTerrain.h"

CUICam::CUICam()
{
}

CUICam::~CUICam()
{
}

void CUICam::SortUIObject()
{
	m_mapInstGroup_ui.clear();

	CScene* pCurScene = CSceneMgr::GetInst()->GetCurScene();

	UINT i = 29;
	//for (UINT i = 0; i < MAX_LAYER; ++i)
	{
		//if (m_iLayerCheck & (1 << i))
		{
			const vector<CGameObject*>& vecObj = pCurScene->GetLayer(i)->GetObjects();


			for (size_t j = 0; j < vecObj.size(); ++j)
			{
				// ���׸��� ������ŭ �ݺ�
				UINT iMtrlCount = vecObj[j]->MeshRender()->GetMtrlCount();
				for (UINT iMtrl = 0; iMtrl < iMtrlCount; ++iMtrl)
				{
					// Material �� �����ϰ� ���� �ʰų�, Material �� ���� Shader �� �������� ���� ���¶�� Continue
					Ptr<CMaterial> pMtrl = vecObj[j]->MeshRender()->GetSharedMaterial(iMtrl);
					if (nullptr == pMtrl || pMtrl->GetShader() == nullptr)
						continue;

					uInstID uID = {};
					uID.llID = vecObj[j]->MeshRender()->GetInstID(iMtrl);//GetInstShadowID(iMtrl);

					// ID �� 0 �� ==> Mesh �� Material �� ���õ��� �ʾҴ�.
					if (0 == uID.llID)
						continue;

					map<ULONG64, vector<tInstObj>>::iterator iter = m_mapInstGroup_ui.find(uID.llID);
					if (iter == m_mapInstGroup_ui.end())
					{
						m_mapInstGroup_ui.insert(make_pair(uID.llID, vector<tInstObj>{tInstObj{ vecObj[j], iMtrl }}));
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

void CUICam::render_ui()
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

	for (auto& pair : m_mapInstGroup_ui)
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
			Ptr<CMaterial> pMtrl = instObj.pObj->MeshRender()->GetSharedMaterial(0);
			if(pMtrl->GetShader()->GetPOV() == SHADER_POV::POSTEFFECT)
				CRenderMgr::GetInst()->CopyTarget();
			instObj.pObj->MeshRender()->render(instObj.iMtrlIdx);
		}
	}
}

void CUICam::finalupdate()
{
	//CalViewMat();
	////CalProjMat();
	//POINT ptRes = CCore::GetInst()->GetWndResolution();
	//m_matProj = XMMatrixOrthographicLH(m_vProjRange.x * m_vScale.x, m_vProjRange.y * m_vScale.y, 1.f, m_fFar);
	//m_matProjInv = XMMatrixInverse(nullptr, m_matProj);
	////CalRay();

	CalViewMat();

	CalProjMat();

	CalRay();
}

