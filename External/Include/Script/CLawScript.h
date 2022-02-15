#pragma once
#include <Engine\CScript.h>
class CPrefab;
class CGameObject;
class CCameraAnimaitorScript;

enum class Pattern
{
    START,
    LOOP,
    PLAY,
    END,
    NONE
};

enum class LAWCAM
{
    CAM_FAR,   //ž��
    CAM_CLOSE, //�ٰŸ�
};

enum class LAWSKILL
{
    //���� melee
    ATK_SWIPE_SLASH_COMBO_A,  //0
    //ATK_SWIPE_SLASH_COMBO_B,  
    ATK_SWIPE_SLASH_COMBO_C,//1
    ATK_SWIPE_SLASH_COMBO_D,

    ATK_GUNSHOT,  //3
    ATK_GUNSHOTSWIPE,
    ATK_ANGULAR_WAVE,
    //==================================== phase 2
    ATK_WIDE_SLASH_RTOL,
    ATK_WIDE_SLASH_STICK,
    ATK_YOYO,
    //==================================== phase 3
    ATK_CIRCLE_SHOOT,//9
    ATK_GRAB,  //10

    NONE
};

enum class LAWSTATE
{
    IDLE,
    MOVE,
    ATTACK,

    HIT_RECEIVED, //�±�
    KNOCKDOWN,  // ->   STANDUP
    GUARD,
    REFLECTBULLET, // �и�����_��
    //TAUNT, //����
    CHANGE_PHASE,
    RUNAWAY, //����

    PARRY_ATK, //�и�����

    REGEN, //����

    STUN, //����
    WEAKENED, //��ħ
    BEGIN_FIGHT,
    MELEEATK,
    GRAB, //���

    NONE
};

enum class LAWDIR
{
    UP,
    DOWN,
    RIGHT,
    LEFT,
    NONE
};

class CLawScript
    :public CScript
{
private:
    Ptr<CPrefab>             m_Attack1; //�Ѿ�
    Ptr<CPrefab>             m_Attack2; //�Ѿ�2
    Ptr<CPrefab>             m_Attack3; //��ä��
    Ptr<CPrefab>             m_Attack4; //yoyo
    Ptr<CPrefab>             m_Attack5; //slash
    Ptr<CPrefab>          m_Attack6; //ū ��ä�� ����
    Ptr<CPrefab>          m_Attack7; //���� ���� (�浹üx)
    CCameraAnimaitorScript* m_CameraAnim;

    LAWSTATE m_state;
    LAWSKILL m_skill;
    Pattern m_pattern;
    LAWDIR m_dir;

    bool m_HitRed;
    bool m_Hit;
    UINT m_HitCount;
    float m_HitTime;
    float m_HitRedTime;

    float m_ftime;
    float m_fMoveTime;
    float m_fAttackTime;
    float m_fmaxtime;
    float m_fspeed;

    //�ִ�
    CGameObject* m_AniObj;
    const vector<tMTAnimClip>* m_pVecClip;
    bool m_bFind;
    bool m_anim;

    //target
    float m_TarAngle;
    Vec2 m_TargetVec;
    LAWDIR m_TargetDir;

    //attack
    int m_AttackCount;
    bool m_Didattack;
    Vec3 m_PlayerPos;
    Vec3 m_PlayerPos2;
    Vec3 m_StartPos;
    float m_AttackSpeed;
    UINT m_BulletNum;
    float m_RotAngle;

    int m_RandSkill; //���õ� ��ų
    int m_Telpo; //1 tepo 0 move
    int m_TelpoCount;
    int m_iMeleeCount;

    float m_fMinDis; // ���� �̵��� �÷��̾���� �ּ� �Ÿ�

    bool m_CircleShootFlag;

    // HP
    float m_fLawHp;      //hp
    int m_Lawlife;      //life
    LAWCAM m_lawcam;   //����

    //idle
    bool bMoveBack; //idle �ڷ� �̵�
    bool bIsEndLaw;

    //phase
    UINT m_LastPhase;
    bool m_PlayerIsGuard; //���
    Vec2 m_ColCenterPos; //�浹ü �߽� ��ġ
    float m_fMaxDis;  //������ �߽ɿ��� ��� �� ���� �ִ� �Ÿ�

    UINT m_testskill;
    float m_Centerlength; //******************************** 9/4 ����
    void CalCenterPos();
    bool CalCenterPos2(Vec3 _TelPos);

    //test
    float m_testTardistance;

    CGameObject* portal_ring;
    bool         m_bPortalringOn;
public:
    LAWCAM GetLawCam() { return m_lawcam; } //����
    UINT GetiLife() { return (UINT)m_Lawlife; }
    float GetLawHp() { return m_fLawHp; }

    bool IsGuard()
    {
        if (m_state == LAWSTATE::GUARD || m_state == LAWSTATE::PARRY_ATK || m_state == LAWSTATE::REFLECTBULLET) { return true; }
        return false;
    }

    void HitReceived(); // �Ϲݰ���, ��������
    void FullChargedHitReceived(); //Ǯ���� ����
    void GunHitReceived(); //�Ѱ���
    bool IsStun() { if (m_state == LAWSTATE::STUN) { return true; } return false; }
    void FullChargedGunHitReceived();
    void SetMELEEATK()
    {
        if (m_state != LAWSTATE::MOVE)

            m_state = LAWSTATE::MELEEATK;
        m_pattern = Pattern::START;
        m_ftime = 0.f;
        m_anim = false;
    }

    void SetPlayerGuardFail() { m_PlayerIsGuard = false; }
    void SetPlayerGuardSucc() { m_PlayerIsGuard = true; }

public:
    virtual void awake();
    virtual void update();

private:

    //state
    void Attack();
    void Move();
    void Idle();

    void Setphase();
    void MakeRedLaw();
    Vec3 GetRandomLawPos(UINT _MinDis, UINT _MaxDis); //�÷��̾� ���� �Ÿ� 
    float CalTargetDis(Vec3 _TargetPos);
    void ChangeState();
    void LookAt(Vec3 _TargetPos);
    int PlayAnim(const wstring& _strName, bool _Repeat, Pattern _Nextpattern, LAWSTATE _curstate = LAWSTATE::NONE);//PlayAnim(int _idx, Pattern _ptn, LAWSTATE _state = LAWSTATE::NONE);
    void func(const vector<CGameObject*>& vecchild); //�ִ� Ŭ�� ã��

    //move
    void MOVE_WALK();
    void MOVE_RUN();
    void MOVE_DASH();
    void MOVE_TELPO();
    void MOVE_TELPO(Vec3 _Destination);
    void GAP_BACK(); // ���� �α�
    void GAP_CLOSER(); // ���� ������

    //�����Ҷ� ������ �̵�
    float m_fCloserTime;
    bool GAP_CLOSER(float _dis, float _time = 0.5);

    //attack
    //���Ÿ�
    void ATK_GUNSHOT();
    void ATK_GUNSHOT_SWIPE();
    void ATK_ANGULAR_WAVE();

    //���� Ư�� ��ġ��
    void ATK_CIRCLE_SHOOT();

    //�߰�
    void ATK_WIDE_SLASH_RTOL();
    void ATK_WIDE_SLASH_STICK();
    void ATK_YOYO();

    //����
    void ATK_SWIPE_SLASH_COMBO_A();
    void ATK_SWIPE_SLASH_COMBO_B();
    void ATK_SWIPE_SLASH_COMBO_C();
    void ATK_SWIPE_SLASH_COMBO_D();

    void ATK_GRAB(); //���

    //�и�����
    void ATK_PARRY();

public:
    virtual void OnCollisionEnter(CGameObject* _pOther);
    virtual void OnCollisionExit(CGameObject* _pOther);
    virtual void OnCollision(CGameObject* _pOther);

public:
    CLONE(CLawScript);

    virtual void SaveToScene(FILE* _pFile);
    virtual void LoadFromScene(FILE* _pFile);

    CLawScript();
    ~CLawScript();
};
