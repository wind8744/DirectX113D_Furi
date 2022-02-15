#include "pch.h"
#include "TestScene.h"

#include <Engine\CScene.h>
#include <Engine\CResMgr.h>
#include <Engine\CGameObject.h>
#include <Engine\CTransform.h>
#include <Engine\CMeshRender.h>
#include <Engine\CCamera.h>
#include <Engine\CAnimator2D.h>
#include <Engine\CCollider2D.h>
#include <Engine\CParticleSystem.h>
#include <Engine\CLight2D.h>
#include <Engine\CLight3D.h>
#include <Engine\CTerrain.h>
#include <Engine\CUICam.h>
#include <Engine\CTexture.h>
#include <Engine\cmeshdata.h>
#include <Engine\CPathMgr.h>
#include <Engine\CCollisionMgr.h>
#include <Engine\CCopyShaderCS.h>
#include <Engine\CSceneMgr.h>
#include <Engine\CRenderMgr.h>

#include <Script\CHpbarScript.h>
#include <Script\CPlayerScript.h>
#include <Script\CMonsterScript.h>
#include <Script\CMissileScript.h>
#include <Script\CSkyBoxScript.h>
#include <Script\CAnimatorScript.h>
#include <Script\CCameraScript.h>
#include <Script\CLawScript.h>
#include <Script\CCameraAnimaitorScript.h>
#include <Script\CNavMeshManager.h>
#include <Script\CColliderCheck.h>
#include <Script\CWallScript.h>
#include <Script\CWiseScript.h>
#include <Script/CWallAxisScript.h>
#include <Script\CStartScript.h>
#include <Script\CStart2Script.h>
#include <Script\CSecneChangeScript.h>
#include <Script/CSetPlayerBosPos.h>
#include <Script\CEffectScript.h>
#include "CSaveLoadMgr.h"


void CreateTestScene()
{
	CScene* pCurScene = CSceneMgr::GetInst()->GetCurScene();

	CGameObject* pObj = nullptr;

	//(22.1.3)
	CResMgr::GetInst()->Load<CTexture>(L"flak1", L"texture\\particle\\flak1.png");
	CResMgr::GetInst()->Load<CTexture>(L"flak2", L"texture\\particle\\flak2.png");
	CResMgr::GetInst()->Load<CTexture>(L"eeeeeee", L"texture\\particle\\eeeeeee.png");
	CResMgr::GetInst()->Load<CTexture>(L"Fog", L"texture\\particle\\Smoke.jpg");
	CResMgr::GetInst()->Load<CTexture>(L"Hit", L"texture\\particle\\hit.png");
	CResMgr::GetInst()->Load<CTexture>(L"elec2", L"texture\\particle\\elec2.png");
	CResMgr::GetInst()->Load<CTexture>(L"Star1", L"texture\\particle\\Star1.png");
	CResMgr::GetInst()->Load<CTexture>(L"Particle", L"texture\\particle\\Particle.png");

	Ptr<CTexture> pTexture = CResMgr::GetInst()->Load<CTexture>(L"Plane", L"texture\\player.bmp");
	Ptr<CTexture> pSmokeTex = CResMgr::GetInst()->Load<CTexture>(L"Smoke", L"texture\\smokeparticle.png");
	Ptr<CTexture> pAnimAtlas = CResMgr::GetInst()->Load<CTexture>(L"Link", L"texture\\link_0.png");
	Ptr<CTexture> pBackgroundTex = CResMgr::GetInst()->Load<CTexture>(L"Background", L"texture\\Background.png");
	Ptr<CTexture>  pTexture4 = CResMgr::GetInst()->Load<CTexture>(L"arene3", L"texture\\FBXTexture\\123123.png");
	Ptr<CTexture> pTexture2 = CResMgr::GetInst()->Load<CTexture>(L"arene1", L"texture\\FBXTexture\\arene.jpg");
	Ptr<CTexture> pTexture3 = CResMgr::GetInst()->Load<CTexture>(L"arene2", L"texture\\FBXTexture\\arene.png");
	Ptr<CTexture> pTexture5 = CResMgr::GetInst()->Load<CTexture>(L"cailloux_normals", L"texture\\cailloux_normals.png");
	Ptr<CTexture> pTexture6 = CResMgr::GetInst()->Load<CTexture>(L"MainCharacter_EMIT", L"texture\\MainCharacter_EMIT.png");
	Ptr<CTexture> pTexture7 = CResMgr::GetInst()->Load<CTexture>(L"Body_EMIT", L"texture\\Body_EMIT.png");
	Ptr<CTexture> pTexture8 = CResMgr::GetInst()->Load<CTexture>(L"The_Law_EMIT", L"texture\\The_Law_EMIT.png");
	Ptr<CTexture> pSkyBoxTex = CResMgr::GetInst()->Load<CTexture>(L"FS000_Day_01_Sunless", L"texture\\skybox\\FS000_Day_01_Sunless.png");
	Ptr<CTexture> pSkyBoxTexArr = CResMgr::GetInst()->Load<CTexture>(L"LawSkyBox", L"texture\\skybox\\LawSkyBox.dds");
	Ptr<CMaterial> pMtrl = CResMgr::GetInst()->FindRes<CMaterial>(L"SkyBoxMtrl");
	pMtrl->SetData(SHADER_PARAM::TEX_0, pSkyBoxTex.Get());
	pMtrl->SetData(SHADER_PARAM::TEXCUBE_0, pSkyBoxTexArr.Get());

	pSkyBoxTex = CResMgr::GetInst()->Load<CTexture>(L"FS000_Day_01_Sunless", L"texture\\skybox\\FS000_Day_01_Sunless.png");
	pSkyBoxTexArr = CResMgr::GetInst()->Load<CTexture>(L"SkyBoxWise", L"texture\\skybox\\SkyBoxWise.dds");
	pMtrl = CResMgr::GetInst()->FindRes<CMaterial>(L"SkyBoxMtr2");
	pMtrl->SetData(SHADER_PARAM::TEX_0, pSkyBoxTex.Get());
	pMtrl->SetData(SHADER_PARAM::TEXCUBE_0, pSkyBoxTexArr.Get());
	CResMgr::GetInst()->Load<CTexture>(L"SkyBox_Cube_Dawn", L"texture\\skybox\\SkyDawn.dds");

	// =============
	// Camera Object
	// =============

	pObj = new CGameObject;
	pObj->SetName(L"Camera_Root");
	pObj->AddComponent(new CTransform);
	pObj->Transform()->SetLocalPos(Vec3(0.f, 1500.f, 0.f));
	pObj->Transform()->SetLocalRotX(1.0f);
	pObj->AddComponent(new CCameraScript);

	CGameObject* pObj1 = new CGameObject;
	pObj1->SetName(L"Camera_bn");
	pObj1->AddComponent(new CTransform);
	pObj1->AddComponent(new CCameraAnimaitorScript);
	CCameraAnimaitorScript* CA_Anim = (CCameraAnimaitorScript*)pObj1->GetScript(L"CCameraAnimaitorScript");

	CGameObject* pObj2 = new CGameObject;
	pObj2->SetName(L"Camera_Child");
	pObj2->AddComponent(new CTransform);
	pObj2->AddComponent(new CCameraAnimaitorScript);
	CA_Anim->SetbnChild(pObj2);
	CA_Anim->LoadClipData(L"CamearaClip\\CameraClipData.Camera");

	CGameObject* pObj3 = new CGameObject;
	pObj3->SetName(L"Camera Object");
	pObj3->AddComponent(new CTransform);
	pObj3->AddComponent(new CCamera);

	pObj3->Camera()->SetProjType(PROJ_TYPE::PERSPECTIVE);
	pObj3->Camera()->SetFar(1000000.f);

	pObj3->Camera()->SetLayerAllCheck();
	CScript::AddChild(pObj, pObj1, 0);
	CScript::AddChild(pObj1, pObj2, 1);
	CScript::AddChild(pObj2, pObj3, 2);

	pCurScene->AddObject(pObj, 0);

	// ===============
	// 3D Light Object
	// ===============
	pObj = new CGameObject;
	pObj->SetName(L"Directional Light");
	pObj->AddComponent(new CTransform);
	pObj->AddComponent(new CLight3D);
	pObj->AddComponent(new CSetPlayerBosPos);
	pObj->Light3D()->SetLightType(LIGHT_TYPE::DIR);
	pObj->Light3D()->SetDiffusePow(Vec3(1.f, 1.f, 1.f));
	pObj->Light3D()->SetSpecPow(Vec3(0.3f, 0.3f, 0.3f));
	pObj->Light3D()->SetAmbPow(Vec3(0.1f, 0.1f, 0.1f));
	pObj->Light3D()->SetRange(10000000.f);
	pObj->Transform()->SetLocalPos(Vec3(0, 1000.f, 2000.f));
	pObj->Transform()->SetLocalRot(Vec3(2.618f, 0.f, 0.f));


	CSetPlayerBosPos* SetPlayerBosPos = (CSetPlayerBosPos*)pObj->GetScript(L"CSetPlayerBosPos");
	pCurScene->AddObject(pObj, 0);

	pObj = new CGameObject;
	pObj->SetName(L"SceneChagne");
	pObj->AddComponent(new CTransform);
	pObj->AddComponent(new CSecneChangeScript);

	pCurScene->AddObject(pObj, 0);
	CSecneChangeScript* SceneChage = (CSecneChangeScript*)pObj->GetScript(L"CSecneChangeScript");

	// =============
	// SkyBox Object
	// =============
	pObj = new CGameObject;
	pObj->SetName(L"LawSkyBox");
	pObj->AddComponent(new CTransform);
	pObj->AddComponent(new CMeshRender);
	pObj->AddComponent(new CSkyBoxScript);

	pObj->MeshRender()->SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"CubeMesh"));
	pObj->MeshRender()->SetMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"SkyBoxMtrl"), 0);

	((CSkyBoxScript*)pObj->GetScript())->SetBoxType();
	((CSkyBoxScript*)pObj->GetScript())->SetBoxType();
	pCurScene->AddObject(pObj, 0);

	SceneChage->ChangeObjPushObj(pObj, ObjList::Law);

	pObj = new CGameObject;
	pObj->SetName(L"WiseSkyBox");
	pObj->AddComponent(new CTransform);
	pObj->AddComponent(new CMeshRender);
	pObj->AddComponent(new CSkyBoxScript);

	pObj->MeshRender()->SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"CubeMesh"));
	pObj->MeshRender()->SetMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"SkyBoxMtr2"), 0);

	((CSkyBoxScript*)pObj->GetScript())->SetBoxType();
	((CSkyBoxScript*)pObj->GetScript())->SetBoxType();
	pCurScene->AddObject(pObj, 0);

	SceneChage->ChangeObjPushObj(pObj, ObjList::Wise);

	// ===============
	// NavManager Object
	// ===============
	/*
	pObj = new CGameObject;
	pObj->SetName(L"NavDataManager");

	pObj->AddComponent(new CTransform);
	pObj->AddComponent(new CTerrain);
	pObj->AddComponent(new CNavMeshManager);
	CNavMeshManager* NavManager = (CNavMeshManager*)pObj->GetScript(L"CNavMeshManager");
	pCurScene->AddObject(pObj, 0);
	*/

	// ============
	// FBX Loading
	// ============
	// ========
	// FBX Load
	// ========
	{

		Ptr<CMeshData> pMeshData = nullptr;

		pMeshData = CResMgr::GetInst()->Load<CMeshData>(L"//meshdata\\PlayerEnd.mdat", L"meshdata\\PlayerEnd.mdat");

		pObj = pMeshData->Instantiate();
		pObj->SetName(L"Player");
		pObj->AddComponent(new CCollider2D);
		pObj->Collider2D()->SetCollider2DType(COLLIDER2D_TYPE::CIRCLE);
		pObj->SetFrustumCheck(true);
		pObj->SetDynamicShadow(true);
		pObj->AddComponent(new CAnimatorScript);
		pObj->AddComponent(new CPlayerScript);
		pObj->Transform()->SetLocalPos(Vec3(0.f, 0.f, -200.f));
		pObj->Transform()->SetLocalScale(Vec3(100.f, 100.f, 100.f));

		CPlayerScript* Plyer = (CPlayerScript*)pObj->GetScript(L"CPlayerScript");
		//Plyer->SetNaviManager(NavManager);
		Ptr<CMaterial> pSMtrl = CResMgr::GetInst()->FindRes<CMaterial>(L"PlayerMaterial_mat00");
		pSMtrl->SetData(SHADER_PARAM::TEX_3, pTexture6.Get());
		SetPlayerBosPos->SetPlayer(pObj);

		pCurScene->AddObject(pObj, 1);
		SceneChage->ChangeObjPushObj(pObj, ObjList::Law);
		SceneChage->ChangeObjPushObj(pObj, ObjList::Wise);
		SceneChage->SetPlayer(pObj);
		{

			pMeshData = CResMgr::GetInst()->Load<CMeshData>(L"//meshdata\\Law_end.mdat", L"meshdata\\Law_end.mdat");


			pObj = pMeshData->Instantiate();
			pObj->SetName(L"Law");
			pObj->SetDynamicShadow(true);
			pObj->AddComponent(new CCollider2D);
			pObj->AddComponent(new CAnimatorScript);
			pObj->AddComponent(new CLawScript);
			pObj->Collider2D()->SetCollider2DType(COLLIDER2D_TYPE::CIRCLE);

			pObj->Transform()->SetLocalPos(Vec3(0.f, 0.f, 900.f));
			pObj->Transform()->SetLocalScale(Vec3(100.f, 100.f, 100.f));
			pObj->Transform()->SetLocalRot(Vec3(0.f, XM_PI, 0.f));
			SetPlayerBosPos->SetBos(pObj);
			pSMtrl = CResMgr::GetInst()->FindRes<CMaterial>(L"The_Law_COL0");
			pSMtrl->SetData(SHADER_PARAM::TEX_3, pTexture8.Get());
			pCurScene->AddObject(pObj, 2);
			SceneChage->ChangeObjPushObj(pObj, ObjList::Law);
			SceneChage->SetLaw(pObj);
		}

		{



			CResMgr::GetInst()->Load<CMeshData>(L"//meshdata\\Wise_ShieldPillar.mdat", L"meshdata\\Wise_ShieldPillar.mdat");
			pMeshData = CResMgr::GetInst()->Load<CMeshData>(L"//meshdata\\Wise.mdat", L"meshdata\\Wise.mdat");

			pObj = pMeshData->Instantiate();
			pObj->SetName(L"Wise");
			pObj->SetDynamicShadow(true);
			pObj->AddComponent(new CCollider2D);
			pObj->AddComponent(new CWiseScript);
			pObj->Collider2D()->SetCollider2DType(COLLIDER2D_TYPE::CIRCLE);

			pObj->Transform()->SetLocalPos(Vec3(0.f, 0.f, 900.f));
			pObj->Transform()->SetLocalScale(Vec3(100.f, 100.f, 100.f));
			pObj->Transform()->SetLocalRot(Vec3(0.f, XM_PI, 0.f));

			pSMtrl = CResMgr::GetInst()->FindRes<CMaterial>(L"Wise_Body0");
			pSMtrl->SetData(SHADER_PARAM::TEX_3, pTexture7.Get());
			//Player->SetTarGetObj(pObj);

			pCurScene->AddObject(pObj, 2);
			SceneChage->ChangeObjPushObj(pObj, ObjList::Wise);
			SceneChage->SetWise(pObj);
			if (1)
			{
				// ==============
				// Wall Manager1
				// ==============
				CGameObject* pCObj = new CGameObject;
				pCObj->SetName(L"WallAxis1");

				pCObj->AddComponent(new CTransform);
				pCObj->AddComponent(new CWallAxisScript);

				pCObj->Transform()->SetLocalPos(Vec3(0.f, 0.f, 0.f));
				pCObj->Transform()->SetLocalScale(Vec3(1.f, 1.f, 1.f));

				pCurScene->AddObject(pCObj, 2);
				CScript::AddChild(pObj, pCObj, 0);

				// ==============
				// Wall Manager2
				// ==============
				CGameObject* pCObj2 = new CGameObject;
				pCObj2->SetName(L"WallAxis2");

				pCObj2->AddComponent(new CTransform);
				pCObj2->AddComponent(new CWallAxisScript);

				pCObj2->Transform()->SetLocalPos(Vec3(0.f, 0.f, 0.f));
				pCObj2->Transform()->SetLocalScale(Vec3(1.f, 1.f, 1.f));

				CWallAxisScript* pScript = (CWallAxisScript*)pCObj2->GetScript(L"CWallAxisScript");
				pScript->SetClockwise(false);

				pCurScene->AddObject(pCObj2, 2);
				CScript::AddChild(pObj, pCObj2, 1);

				// ==============
				// Wall Object 1
				// ==============
				pMeshData = CResMgr::GetInst()->Load<CMeshData>(L"//meshdata\\Wise_ShieldPanel.mdat", L"meshdata\\Wise_ShieldPanel.mdat");

				int iCount = 0;
				for (float Angle = 0; Angle < XM_2PI; Angle += (XM_2PI / 10))
				{
					CGameObject* pCCObj = pMeshData->Instantiate();
					pCCObj->SetName(L"Wall");
					pCCObj->SetDynamicShadow(true);

					pCCObj->AddComponent(new CCollider2D);
					pCCObj->AddComponent(new CWallScript);

					Vec2 vDir;
					vDir.x = cos(Angle) * 3.6f;
					vDir.y = sin(Angle) * 3.6f;

					pCCObj->Transform()->SetLocalPos(Vec3(vDir.x, 0.f, vDir.y));
					pCCObj->Transform()->SetLocalScale(Vec3(0.01f, 0.01f, 0.01f));

					vDir.Normalize();
					pCCObj->Transform()->SetLocalRot(Vec3(0.f, -Angle, 0.f));

					pCCObj->Collider2D()->SetvOffsetPos(Vec2(0.f, 100.f));
					pCCObj->Collider2D()->SetvOffsetScale(Vec2(200.f, 200.f));
					pCCObj->Collider2D()->SetCollider2DType(COLLIDER2D_TYPE::CIRCLE);

					pCurScene->AddObject(pCCObj, 5);

					CScript::AddChild(pCObj, pCCObj, ++iCount);
				}

				// ==============
				// Wall Object 3
				// ==============
				for (float Angle = 0; Angle < XM_2PI; Angle += (XM_2PI / 24))
				{
					CGameObject* pCCObj = pMeshData->Instantiate();
					pCCObj->SetName(L"Wall");
					pCCObj->SetDynamicShadow(true);

					pCCObj->AddComponent(new CCollider2D);
					pCCObj->AddComponent(new CWallScript);

					Vec2 vDir;
					vDir.x = cos(Angle) * 8.8f;
					vDir.y = sin(Angle) * 8.8f;

					pCCObj->Transform()->SetLocalPos(Vec3(vDir.x, 0.f, vDir.y));
					pCCObj->Transform()->SetLocalScale(Vec3(0.01f, 0.01f, 0.01f));

					vDir.Normalize();
					pCCObj->Transform()->SetLocalRot(Vec3(0.f, -Angle, 0.f));

					pCCObj->Collider2D()->SetvOffsetScale(Vec2(200.f, 200.f));
					pCCObj->Collider2D()->SetCollider2DType(COLLIDER2D_TYPE::CIRCLE);

					pCurScene->AddObject(pCCObj, 5);
					CScript::AddChild(pCObj, pCCObj, ++iCount);
				}

				// ==============
				// Wall Object 2
				// ==============
				iCount = 0;
				for (float Angle = 0; Angle < XM_2PI; Angle += (XM_2PI / 18))
				{
					CGameObject* pCCObj = pMeshData->Instantiate();
					pCCObj->SetName(L"Wall");
					pCCObj->SetDynamicShadow(true);

					pCCObj->AddComponent(new CCollider2D);
					pCCObj->AddComponent(new CWallScript);

					Vec2 vDir;
					vDir.x = cos(Angle) * 6.2f;
					vDir.y = sin(Angle) * 6.2f;

					pCCObj->Transform()->SetLocalPos(Vec3(vDir.x, 0.f, vDir.y));
					pCCObj->Transform()->SetLocalScale(Vec3(0.01f, 0.01f, 0.01f));

					vDir.Normalize();
					pCCObj->Transform()->SetLocalRot(Vec3(0.f, -Angle, 0.f));

					pCCObj->Collider2D()->SetvOffsetScale(Vec2(200.f, 200.f));
					pCCObj->Collider2D()->SetCollider2DType(COLLIDER2D_TYPE::CIRCLE);

					pCurScene->AddObject(pCCObj, 5);
					CScript::AddChild(pCObj2, pCCObj, ++iCount);
				}
			}

		}
	}

	/*
	pObj = new CGameObject;
	pObj->SetName(L"c");
	pObj->AddComponent(new CTransform);
	pObj->AddComponent(new CColliderCheck);
	pObj->AddComponent(new CCollider2D);
	pObj->Transform()->SetLocalPos(Vec3(0.f, 0.f, 0.f));
	pObj->Transform()->SetLocalScale(Vec3(3000.f, 0.f, 3000.f));
	pCurScene->AddObject(pObj, 2);
	*/

	// =====================
	// Map
	// =====================

	Ptr<CMeshData> pMeshData = nullptr;
	pMeshData = CResMgr::GetInst()->Load<CMeshData>(L"//meshdata\\CombinedMesh-MultiMeshBaker.mdat", L"meshdata\\CombinedMesh-MultiMeshBaker.mdat");
	pObj = pMeshData->Instantiate();
	pObj->SetName(L"Map");
	pObj->SetFrustumCheck(true);
	pObj->SetDynamicShadow(true);
	pObj->Transform()->SetLocalPos(Vec3(0.f, -400.f, -6350.f));
	pObj->Transform()->SetLocalScale(Vec3(1.f, 1.f, 1.f));
	pObj->Transform()->SetLocalRotX(0.f);
	pCurScene->AddObject(pObj, 0);
	SceneChage->ChangeObjPushObj(pObj, ObjList::Law);

	pMeshData = CResMgr::GetInst()->Load<CMeshData>(L"//meshdata\\Map3_2.mdat", L"meshdata\\Map3_2.mdat");
	pObj = pMeshData->Instantiate();
	pObj->SetName(L"Map2");
	pObj->SetFrustumCheck(true);
	pObj->SetDynamicShadow(true);
	pObj->Transform()->SetLocalPos(Vec3(0.f, 0.f, -18000.f));
	pObj->Transform()->SetLocalScale(Vec3(1.f, 1.f, 1.f));
	pObj->Transform()->SetLocalRotY(-1.57f);
	pCurScene->AddObject(pObj, 0);
	SceneChage->ChangeObjPushObj(pObj, ObjList::Wise);

	Ptr<CMaterial> pSMtrl1 = CResMgr::GetInst()->FindRes<CMaterial>(L"_1_mat00");
	pSMtrl1->SetData(SHADER_PARAM::TEX_1, pTexture4.Get());
	pSMtrl1 = CResMgr::GetInst()->FindRes<CMaterial>(L"_1_mat70");
	pSMtrl1->SetData(SHADER_PARAM::TEX_0, pSmokeTex.Get());


	
	// ================
	// uiCamera Object
	// ================
	pObj = new CGameObject;
	pObj->SetName(L"uiCamera");
	pObj->AddComponent(new CTransform);
	pObj->AddComponent(new CUICam);

	pObj->Transform()->SetLocalPos(Vec3(0.f, 0.f, 0.f));
	pObj->Camera()->SetProjType(PROJ_TYPE::ORTHOGRAPHIC);
	pObj->Camera()->SetFar(1000.f);
	//pObj->Camera()->SetLayerAllCheck();
	CRenderMgr::GetInst()->RegisteruiCamera((CUICam*)pObj->Camera());
	pCurScene->AddObject(pObj, 0);

	// =========
	// hp ui  
	// =========
	pObj = new CGameObject;
	pObj->SetName(L"ui");
	pObj->AddComponent(new CTransform);
	pObj->AddComponent(new CHpbarScript);

	pCurScene->AddObject(pObj, 1);
	SceneChage->ChangeObjPushObj(pObj, ObjList::Hpbar);
	SceneChage->SetHpbar(pObj);

	// === main 1 obj ===
	pObj = new CGameObject;
	pObj->SetName(L"main");
	pObj->AddComponent(new CTransform);
	pObj->AddComponent(new CMeshRender);
	pObj->AddComponent(new CStartScript);

	pObj->Transform()->SetLocalPos(Vec3(0.f, 0.f, 10.f));
	pObj->Transform()->SetLocalScale(Vec3(1600.f, 900.f, 1.f));
	pObj->Transform()->SetLocalRot(Vec3(0.f, 0., 0.f));

	pObj->MeshRender()->SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"RectMesh"));
	pObj->MeshRender()->SetMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"Std2DMtrl"), 0);  // ++ ½¦ÀÌ´õ °íÄ¥°Í

	Ptr<CTexture> pmain = CResMgr::GetInst()->Load<CTexture>(L"main", L"texture\\ui\\main.png");
	pObj->MeshRender()->GetSharedMaterial(0)->SetData(SHADER_PARAM::TEX_0, pmain.Get());
	pCurScene->AddObject(pObj, 29);
	SceneChage->ChangeObjPushObj(pObj, ObjList::Start);
	// === main 2 obj ===
	pObj = new CGameObject;
	pObj->SetName(L"main2");
	pObj->AddComponent(new CTransform);
	pObj->AddComponent(new CMeshRender);
	pObj->AddComponent(new CStart2Script);

	pObj->Transform()->SetLocalPos(Vec3(0.f, 0.f, 10.f));
	pObj->Transform()->SetLocalScale(Vec3(1600.f, 900.f, 1.f));
	pObj->Transform()->SetLocalRot(Vec3(0.f, 0., 0.f));

	pObj->MeshRender()->SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"RectMesh"));
	pObj->MeshRender()->SetMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"Std2DMtrl"), 0);  // ++ ½¦ÀÌ´õ °íÄ¥°Í
	pCurScene->AddObject(pObj, 29);
	SceneChage->ChangeObjPushObj(pObj, ObjList::Change);

	// =======
	// Effect
	// =======
	//Ptr<CMaterial> pSMtrl1 = CResMgr::GetInst()->FindRes<CMaterial>(L"_1_mat00");
	//pSMtrl1->SetData(SHADER_PARAM::TEX_1, pTexture4.Get());

	//pSMtrl1 = CResMgr::GetInst()->FindRes<CMaterial>(L"_1_mat70");
	//pSMtrl1->SetData(SHADER_PARAM::TEX_0, pSmokeTex.Get());
	
	pObj = CResMgr::GetInst()->FindRes<CPrefab>(L"PostEffect_Vignette")->Instantiate();
	pObj->Transform()->SetLocalScale(Vec3(0.f, 0.f, 0.f));
	pCurScene->AddObject(pObj,29);

	pObj = CResMgr::GetInst()->FindRes<CPrefab>(L"PostEffect_GlichLine")->Instantiate();
	pObj->Transform()->SetLocalScale(Vec3(0.f, 0.f, 0.f));
	pCurScene->AddObject(pObj, 29);
	// ====================
	//  Fog Effect
	// ====================
	
	CGameObject* pFog = new CGameObject;
	pFog->SetName(L"FogEffect");
	pFog->AddComponent(new CTransform);
	pFog->AddComponent(new CParticleSystem);
	pFog->Transform()->SetLocalPos(Vec3(0.f, 100.f, -100.f)); //À§Ä¡Á¶Àý
	pFog->ParticleSystem()->SetPaticleName(L"Fog");
	pFog->ParticleSystem()->SetShaderIdx(1);
	pFog->ParticleSystem()->SetiMaxCount(100);
	pFog->ParticleSystem()->SetvCreateRange(Vec4(5000.f, 10.f, 5000.f, 0.f)); //¹üÀ§Á¶Àý
	pFog->ParticleSystem()->SetvStartColor(Vec4(0.3f, 0.2f, 0.2f, 0.5f));
	pFog->ParticleSystem()->SetvEndColor(Vec4(0.27f, 0.224f, 0.2f, 0.5f));
	pFog->ParticleSystem()->SetfMinSpeed(1);
	pFog->ParticleSystem()->SetfMaxSpeed(2);
	pFog->ParticleSystem()->SetvStartScale(Vec3(750.f, 0.f, 0.f));
	pFog->ParticleSystem()->SetvEndScale(Vec3(800.f, 0.f, 0.f));
	pFog->ParticleSystem()->SetDirection(0);
	pFog->ParticleSystem()->SetfMinLifeTime(200.f);
	pFog->ParticleSystem()->SetfMaxLifeTime(300.f);
	pFog->ParticleSystem()->SetfFrequency(0.01f);
	pCurScene->AddObject(pFog, 2);


	// Collision Check
	CCollisionMgr::GetInst()->CollisionCheck(1, 2);
	CCollisionMgr::GetInst()->CollisionCheck(2, 3);
	CCollisionMgr::GetInst()->CollisionCheck(1, 4);
	CCollisionMgr::GetInst()->CollisionCheck(3, 4);
	CCollisionMgr::GetInst()->CollisionCheck(1, 5);
	CCollisionMgr::GetInst()->CollisionCheck(3, 5);
	//CCollisionMgr::GetInst()->CollisionCheck(2, 11);
	// Scene Save
	CSaveLoadMgr::SaveScene(pCurScene, L"scene\\TestScene.scene");
}

#if(1)	
#else
#endif