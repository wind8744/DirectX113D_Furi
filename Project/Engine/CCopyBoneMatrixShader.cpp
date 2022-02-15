#include "pch.h"
#include "CCopyBoneMatrixShader.h"

#include "CDevice.h"
#include "CConstBuffer.h"
#include "CStructuredBuffer.h"

CCopyBoneMatrixShader::CCopyBoneMatrixShader()
	: m_tInfo{}
	, m_pSrcBuffer(nullptr)
	, m_pDestBuffer(nullptr)
{
}

CCopyBoneMatrixShader::~CCopyBoneMatrixShader()
{
}

void CCopyBoneMatrixShader::UpdateData()
{
	// �ʿ��� ����� ����	
	static const CConstBuffer* pMtrlBuffer = CDevice::GetInst()->GetCB(CB_TYPE::MATERIAL);
	pMtrlBuffer->SetData(&m_tInfo);
	pMtrlBuffer->UpdateData((UINT)PIPELINE_STAGE::PS_COMPUTE);

	// ����ȭ���� ����
	m_pSrcBuffer->UpdateData(14);
	m_pDestBuffer->UpdateDataRW(0);
}

void CCopyBoneMatrixShader::Clear()
{
	// ������ ����ȭ���� Ŭ����	
	m_pSrcBuffer->Clear();
	m_pDestBuffer->ClearRW();
}

void CCopyBoneMatrixShader::Excute()
{
	int iBoneCount = m_tInfo.arrInt[0];
	int iThreadX = iBoneCount / 1024 + 1;
	Dispatch(iThreadX, 1, 1);
}