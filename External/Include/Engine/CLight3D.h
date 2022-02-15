#pragma once
#include "CComponent.h"

#include "CMesh.h"
#include "CMaterial.h"
#include "CMaterial.h"

class CGameObject;

class CLight3D :
    public CComponent
{
private:
    tLightInfo      m_info;

    Ptr<CMesh>      m_pMesh; // Lighting ���� �޽�
    Ptr<CMaterial>  m_pMtrl; // Lighting ���׸���

    int             m_iLightIdx; // RenderMgr �� ��� �� �ε���

    CGameObject*    m_pCamObj;

public:
    virtual void finalupdate();
    void render();
    void render_shadowmap();

public:
    void SetLightType(LIGHT_TYPE _eType);
    void SetDiffusePow(Vec3 _vDiffusePow) { m_info.color.vDiff = _vDiffusePow; }
    void SetSpecPow(Vec3 _vSpecPow) { m_info.color.vSpec = _vSpecPow; }
    void SetAmbPow(Vec3 _vAmbPow) { m_info.color.vAmb = _vAmbPow; }
    void SetRange(float _fRange);

    const tLightInfo& GetInfo(){return m_info;}
    LIGHT_TYPE GetLightType();
public:
    virtual void SaveToScene(FILE* _pFile);
    virtual void LoadFromScene(FILE* _pFile);
    CLONE(CLight3D);

public:
    CLight3D();
    CLight3D(const CLight3D&);
    ~CLight3D();
};

