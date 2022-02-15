#pragma once
#include "CRes.h"

#include "CFBXLoader.h"

class CStructuredBuffer;

struct tIndexInfo
{
    ComPtr<ID3D11Buffer> pIB;
    D3D11_BUFFER_DESC    tIBDesc;
    UINT				 iIdxCount;       
    void*                pIdxSysMem;
};


class CMesh :
    public CRes
{
private:  
    ComPtr<ID3D11Buffer>    m_pVB;
    D3D11_BUFFER_DESC       m_tVBDesc;
    UINT                    m_iVtxCount;
    void*                   m_pVtxSysMem;

    // �ϳ��� ���ؽ����ۿ� �������� �ε������۰� ����
    vector<tIndexInfo>		m_vecIdxInfo;
   
    // Animation3D ����
    vector<tMTAnimClip>		m_vecAnimClip;
    vector<vector<tMTBone>>	m_vecBones;

    vector<CStructuredBuffer*>      m_pBoneFrameData;   // ��ü �� ������ ����
    vector<CStructuredBuffer*>      m_pBoneOffset;	    // �� ���� offset ���
    vector<UINT>                    i_FrameCount;


    // Nav Mesh Data
    bool                    m_bIsNavMesh; // �׺� �޽� ����
    vector<NavMesh>         m_vecNavData;
public:
    static CMesh* CreateFromContainer(CFBXLoader& _loader, int _num = 0, bool _Nav = false);

    void Create(VTX* _pVTXSysmem, UINT _iVtxCount, UINT* _IdxSysmem, UINT _iIdxCount);    
    void Reset(VTX* _pVTXSysmem, UINT _iVtxCount, UINT* _IdxSysmem, UINT _iIdxCount);
    void UpdateData(UINT _iSubset);
    void render(UINT _iSubset);
    void render_particle(UINT _iParticleCount);
    void render_instancing(UINT _iSubset);
  

public:
    UINT GetSubsetCount() { return (UINT)m_vecIdxInfo.size(); }
    const vector<tMTBone>* GetBones(UINT _i) { return &m_vecBones[_i]; }
    UINT GetBoneCount(UINT _i) { return (UINT)m_vecBones[_i].size(); }
    UINT GetBoneSize() { return m_vecBones.size(); }
    const vector<tMTAnimClip>* GetAnimClip() { return &m_vecAnimClip; }
    bool IsAnimMesh() { return !m_vecAnimClip.empty(); }

    CStructuredBuffer* GetBoneFrameDataBuffer(UINT _i) { return m_pBoneFrameData[_i]; } // ��ü �� ������ ����
    CStructuredBuffer* GetBoneOffsetBuffer(UINT _i) { return  m_pBoneOffset[_i]; }	   // �� ���� offset ���	

    bool    IsNavMesh() { return m_bIsNavMesh;}
    vector<NavMesh>* GetNavData() { return &m_vecNavData; }
    void NavMeshDataTrans(Matrix _TransData);
public:
    virtual void Save(const wstring& _strRelativePath);
    virtual void Load(const wstring& _strFilePath);

public:
    CMesh();
    ~CMesh();
};

