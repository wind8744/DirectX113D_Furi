#include "pch.h"
#include "CNavMeshManager.h"

#include <Engine\CScene.h>
#include <Engine\CLayer.h>
#include <Engine\CMesh.h>
#include <Engine/CMeshRender.h>
#include <Engine\CSceneMgr.h>
CNavMeshManager::CNavMeshManager():CScript((int)SCRIPT_TYPE::NAVMESHMANAGER)
{
}

CNavMeshManager::~CNavMeshManager()
{
}

void CNavMeshManager::update()
{
}

void CNavMeshManager::awake()
{
	NavMeshVector();
}

void CNavMeshManager::NavMeshVector()
{
	CScene* pCurScene = CSceneMgr::GetInst()->GetCurScene();

	const vector<CGameObject*>& vecObj = pCurScene->GetLayer(0)->GetObjects(); //���� ��� ������Ʈ�� ��ȯ
	for (size_t i = 0; i < vecObj.size(); ++i)
	{
		if (vecObj[i]->MeshRender())
		{
			if (vecObj[i]->MeshRender()->GetMesh()->IsNavMesh()) //�׺�޽��϶���
			{
				//�׺�޽��� ��Ʈ���� �� �ҷ��ͼ�
				//�������� ������, ȸ���� �̵��� ������.(���)
				vecObj[i]->MeshRender()->SetNavMeshTrans(); 
				
				m_NavMeshVector.push_back(vecObj[i]->MeshRender()->GetMesh()->GetNavData()); //�׹̸޽������͸� ���Ϳ� ����
			}
		}
	}

}