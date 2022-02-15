#pragma once
#include <Engine\CScript.h>

class CPrefab;
class CSlashScript :
    public CScript
{
private:
    float m_fAtime;
    float m_fMaxtime;
    float m_fRange;
    Vec2 m_vDir;
    Vec3 m_TarPos;

    bool m_bFlag;

    UINT m_AttackNum;    //************************************** ����
    // 0 rtoL 1 ����1 2 ����2 3����3 '4�и�'

public:
    void SetAttackNum(UINT _attnum) { m_AttackNum = _attnum; }    //************************************** ����
    void SetTarPos(Vec3 _tarpos) { m_TarPos = _tarpos; }
    void SetMaxTime(float _time) { m_fMaxtime = _time; }
    void SetRange(float _range) { m_fRange = _range; }
public:
    virtual void update();
    virtual void awake();

public:
    CLONE(CSlashScript);

public:
    CSlashScript();
    ~CSlashScript();

    virtual void OnCollisionEnter(CGameObject* _pOther);
    virtual void OnCollisionExit(CGameObject* _pOther);
    virtual void OnCollision(CGameObject* _pOther);
};

