#pragma once

#include "CSingleton.h"

#include "Ptr.h"
#include "CTexture.h"
#include "CLight2D.h"
#include "CLight3D.h"
#include "CUICam.h" //ui************************************************************** �߰�

class CStructuredBuffer;
class CCamera;
class CMRT;
class CUICam; //ui************************************************************** �߰�

class CRenderMgr
	: public CSingleton<CRenderMgr>
{
	SINGLE(CRenderMgr)
private:
	vector<Ptr<CTexture>>	m_vecNoiseTex;
	int						m_iCurNoise;
	float					m_fAccTime;

	vector<CLight2D*>		m_vecLight2D;
	CStructuredBuffer* m_pLight2DBuffer;

	vector<CLight3D*>		m_vecLight3D;
	CStructuredBuffer* m_pLight3DBuffer;

	Ptr<CTexture>			m_pCopyTarget;		// swapchain backbuffer copy
	vector<CCamera*>		m_vecCam;
	CCamera* m_pToolCam;			// Client ���� ����

	CStructuredBuffer* m_pAnim2DBuffer;	// 2D Animation ���� ����

	CUICam* m_uicam;	//ui************************************************************** �߰�
	CMRT* m_arrMRT[(UINT)MRT_TYPE::END];


public:
	void init();
	void update();

	void render();

private:
	// Scene �� ���
	void render_play();
	void render_ui();//ui************************************************************** �߰�

	// ����, �Ͻ�����
	void render_tool();

	void render_shadowmap();

public:
	void RegisteruiCamera(CUICam* _pCam) { m_uicam = _pCam; }//ui************************************************************** �߰�
	void RegisterLight2D(CLight2D* _pLight2D) { m_vecLight2D.push_back(_pLight2D); }
	int RegisterLight3D(CLight3D* _pLight3D) { m_vecLight3D.push_back(_pLight3D);  return (int)m_vecLight3D.size() - 1; }
	void RegisterCamera(CCamera* _pCam) { m_vecCam.push_back(_pCam); }
	void RegisterToolCamera(CCamera* _pCam) { m_pToolCam = _pCam; }
	void CopyTarget(); // SwapChain backbuffer �� ��ó���� ���ҽ� �ؽ��Ŀ� �����Ѵ�.

	CStructuredBuffer* GetAnim2DBuffer() { return m_pAnim2DBuffer; }
	CMRT* GetMRT(MRT_TYPE _eType) { return m_arrMRT[(UINT)_eType]; }

	CCamera* GetCurCam();

private:
	void LightMerge();
	void UpdateLights(vector<tLightInfo>& _vec, CStructuredBuffer* _buffer, UINT _iRegisterNum);
	void CreateMRT();
	void ClearMRT();
};

