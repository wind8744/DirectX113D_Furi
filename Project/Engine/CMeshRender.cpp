#include "pch.h"
#include "CMeshRender.h"

#include "CResMgr.h"

#include "CGameObject.h"
#include "CTransform.h"
#include "CAnimator2D.h"
#include "CAnimator3D.h"
#include "CScript.h"
CMeshRender::CMeshRender()
	:CComponent(COMPONENT_TYPE::MESHRENDER)
{
}

CMeshRender::~CMeshRender()
{
}

void CMeshRender::update()
{
}

void CMeshRender::finalupdate()
{
}

void CMeshRender::render()
{
	Transform()->UpdateData();
		
	if (Animator2D())
	{
		Animator2D()->UpdateData();
	}

	// Animation Ȯ��
	if (Animator3D())
	{
		// ������� ����
		Animator3D()->UpdateData();

		for (size_t i = 0; i < m_vecMtrl.size(); ++i)
		{
			if (nullptr == m_vecMtrl[i])
				continue;
			int num = Animator3D()->GetCurClip();
			m_vecMtrl[i]->SetAnim3D(true); // Animation Mesh �˸���
			m_vecMtrl[i]->SetBoneCount(Animator3D()->GetBoneCount(num));
		}
	}

	for (size_t i = 0; i < m_vecMtrl.size(); ++i)
	{
		if (nullptr == m_vecMtrl[i] || nullptr == m_vecMtrl[i]->GetShader())
			continue;

		m_vecMtrl[i]->UpdateData();

		m_pMesh->UpdateData((UINT)i);
		m_pMesh->render((UINT)i);

		m_vecMtrl[i]->Clear();
	}

	if (Animator2D())
	{
		CAnimator2D::Clear();
	}

	// ����	
	if (Animator3D())
	{
		Animator3D()->ClearData();		
	}
}

void CMeshRender::render(UINT _iMtrl)
{
	if (nullptr == m_vecMtrl[_iMtrl])
	{
		return;
	}

	Transform()->UpdateData();

	if (Animator2D())
	{
		Animator2D()->UpdateData();
	}

	// Animation Ȯ��
	if (Animator3D())
	{
		// ������� ����
		Animator3D()->UpdateData();
		int num = Animator3D()->GetCurClip();
		m_vecMtrl[_iMtrl]->SetAnim3D(true); // Animation Mesh �˸���
		m_vecMtrl[_iMtrl]->SetBoneCount(Animator3D()->GetBoneCount(num));
	}
	
	m_vecMtrl[_iMtrl]->UpdateData();

	m_pMesh->UpdateData((UINT)_iMtrl);
	m_pMesh->render((UINT)_iMtrl);

	m_vecMtrl[_iMtrl]->Clear();
	
	if (Animator2D())
	{
		CAnimator2D::Clear();
	}

	// ����	
	if (Animator3D())
	{
		Animator3D()->ClearData();
	}
	if (GetObj()->GetName() == L"Sector")
	{
		GetObj()->GetScript()->UpdateData();
	}
}


void CMeshRender::render_shadowmap()
{
	if (nullptr == m_pMesh || m_vecMtrl.empty())
		return;

	// ��ġ ��� ���� ������Ʈ
	Transform()->UpdateData();

	Ptr<CMaterial> pMtrl = CResMgr::GetInst()->FindRes<CMaterial>(L"ShadowMapMtrl");

	// Animation Ȯ��
	if (Animator3D())
	{
		int num = Animator3D()->GetCurClip();
		// ������ 3D Animation �� ������ �˸�
		pMtrl->SetAnim3D(true);
		pMtrl->SetBoneCount(Animator3D()->GetBoneCount(num));

		// ������� ����
		Animator3D()->UpdateData();
	}

	// ����� ������ŭ ShadowMap �ۼ�
	for (UINT i = 0; i < m_pMesh->GetSubsetCount(); ++i)
	{
		if (nullptr == m_vecMtrl[i])
			continue;

		pMtrl->UpdateData();
		m_pMesh->UpdateData(i);
		m_pMesh->render(i);
	}

	// ����	
	if (Animator3D())
	{
		Animator3D()->ClearData();
		pMtrl->SetAnim3D(false);
	}
}

void CMeshRender::render_shadowmap(UINT _iMtrl)
{
	if (nullptr == m_pMesh || nullptr == m_vecMtrl[_iMtrl])
		return;

	// ��ġ ��� ���� ������Ʈ
	Transform()->UpdateData();

	Ptr<CMaterial> pMtrl = CResMgr::GetInst()->FindRes<CMaterial>(L"ShadowMapMtrl");

	// Animation Ȯ��
	if (Animator3D())
	{
		int num = Animator3D()->GetCurClip();
		// ������ 3D Animation �� ������ �˸�
		pMtrl->SetAnim3D(true);
		pMtrl->SetBoneCount(Animator3D()->GetBoneCount(num));

		// ������� ����
		Animator3D()->UpdateData();
	}
		
	pMtrl->UpdateData();
	m_pMesh->UpdateData(_iMtrl);
	m_pMesh->render(_iMtrl);

	if (Animator3D())
	{
		Animator3D()->ClearData();
		pMtrl->SetAnim3D(false);
	}
}

void CMeshRender::SetMesh(Ptr<CMesh> _pMesh)
{
	m_pMesh = _pMesh;

	int iResizeCount = 1;

	if (nullptr != m_pMesh)
		iResizeCount = m_pMesh->GetSubsetCount();

	vector<Ptr<CMaterial>> vec;
	vec.resize(iResizeCount);
	m_vecMtrl.swap(vec);
}

void CMeshRender::SetMaterial(Ptr<CMaterial> _pMtrl, UINT _iIdx)
{
	if (m_vecMtrl.size() <= _iIdx)
		assert(nullptr);

	m_vecMtrl[_iIdx] = _pMtrl;
}

ULONG64 CMeshRender::GetInstID(UINT _iMtrlIdx)
{
	if (m_pMesh == NULL || m_vecMtrl[_iMtrlIdx] == NULL)
		return 0;

	uInstID id{ (UINT)m_pMesh->GetID(), (WORD)m_vecMtrl[_iMtrlIdx]->GetID(), (WORD)_iMtrlIdx };
	return id.llID;
}

ULONG64 CMeshRender::GetInstShadowID(UINT _iMtrlIdx)
{
	if (m_pMesh == NULL || m_vecMtrl[_iMtrlIdx] == NULL)
		return 0;

	Ptr<CMaterial> pMtrl = CResMgr::GetInst()->FindRes<CMaterial>(L"ShadowMapMtrl");

	uInstID id{ (UINT)m_pMesh->GetID(), (WORD)pMtrl->GetID(), (WORD)_iMtrlIdx };
	return id.llID;
}

Ptr<CMaterial> CMeshRender::GetCloneMaterial(UINT _iIdx)
{
	m_vecMtrl[_iIdx] = m_vecMtrl[_iIdx]->Clone();

	return m_vecMtrl[_iIdx];
}

void CMeshRender::SetNavMeshTrans()
{
	
	Matrix matWorld = Transform()->GetWorldMat(); //�׺�޽��� ��Ʈ���� �� �ҷ��ͼ�
	m_pMesh->NavMeshDataTrans(matWorld); //�������� ������, ȸ���� �̵��� ������.(���)
}

void CMeshRender::SaveToScene(FILE* _pFile)
{
	CComponent::SaveToScene(_pFile);		
	
	SaveResRefInfo(m_pMesh, _pFile);

	int iMtrlSize = (int)m_vecMtrl.size();
	fwrite(&iMtrlSize, sizeof(int), 1, _pFile);

	for (int i = 0; i < (int)m_vecMtrl.size(); ++i)
	{	
		SaveResRefInfo(m_vecMtrl[i], _pFile);	
	}	
}

void CMeshRender::LoadFromScene(FILE* _pFile)
{
	CComponent::LoadFromScene(_pFile);
		
	LoadResRefInfo(m_pMesh, _pFile);	
	
	int iMtrlSize = 0;
	fread(&iMtrlSize, sizeof(int), 1, _pFile);

	m_vecMtrl.resize(iMtrlSize);

	for (int i = 0; i < iMtrlSize; ++i)
	{		
		LoadResRefInfo(m_vecMtrl[i], _pFile);		
	}
}