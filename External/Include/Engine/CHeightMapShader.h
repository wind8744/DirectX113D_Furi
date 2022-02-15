#pragma once
#include "CComputeShader.h"

class CStructuredBuffer;

class CHeightMapShader :
    public CComputeShader
{
private:
    tMtrlInfo       m_tInfo;
    Ptr<CTexture>   m_pHeightMap;           // ���̰��� ���� �� �ؽ���
    Ptr<CTexture>   m_pBrushTex;            // ����� Brush Texture

    CStructuredBuffer* m_pPickingBuffer;    // ��ŷ�� ��ġ������ ����ִ� ����
    

public:
    void SetHeightMode(int _iUpDown) { m_tInfo.arrInt[2] = _iUpDown; }
    void SetHeightMap(Ptr<CTexture> _pTex) { m_pHeightMap = _pTex; }
    void SetBrushTex(Ptr<CTexture> _pTex) { m_pBrushTex = _pTex; }
    void SetCenterPos(CStructuredBuffer* _pPickPosBuffer) { m_pPickingBuffer = _pPickPosBuffer; }
    void SetBrushScale(float _fScale) { m_tInfo.arrFloat[0] = _fScale; }

private:
    virtual void UpdateData();
    virtual void Clear();

public:
    virtual void Excute();

public:
    CHeightMapShader();
    ~CHeightMapShader();

};

