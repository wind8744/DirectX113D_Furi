#ifndef _LIGHT
#define _LIGHT

#include "value.fx"
#include "func.fx"

// ========================
// Directional Light Shader
// mesh : rect mesh
// blend state : One - One
// depth stencil state : NoTest, NoWrite
// g_tex_0 : Normal Target
// g_tex_1 : Position Target
// g_int_0 : Light Index

#define DYNAMIC_SHADOW_MAP      g_tex_3 // 동적 쉐도우 맵
#define LightCamViewProjMat     g_mat_0 // 광원 시점으로의 View * Proj 행렬
// ========================
struct VS_DIR_IN
{
    float3 vPos : POSITION;
};

struct VS_DIR_OUT
{
    float4 vPosition : SV_Position;
};

struct PS_OUT
{
    float4 vDiffuse : SV_Target0;
    float4 vSpecular : SV_Target1;
};

VS_DIR_OUT VS_DirLight(VS_DIR_IN _in)
{
    VS_DIR_OUT output = (VS_DIR_OUT) 0.f;
    
    output.vPosition.xy = _in.vPos * 2.f;
    output.vPosition.zw = 1.f;
    
    return output;
}

PS_OUT PS_DirLight(VS_DIR_OUT _in)
{
    PS_OUT output = (PS_OUT) 0.f;
            
    float2 vScreenUV = _in.vPosition.xy / g_vResolution;
    
    float4 vViewNormal = g_tex_0.Sample(g_sam_0, vScreenUV);
    float3 vViewPosition = g_tex_1.Sample(g_sam_0, vScreenUV).xyz;
    
    if (vViewPosition.z == 0.f)
    {
        clip(-1);
    }     
    
    tLightColor lightColor = CalLight(g_int_0, vViewNormal.xyz, vViewPosition);
        
    // 그림자 판정    
    // 빛이 없으면 그림자 처리를 하지 않는다.
    if (dot(lightColor.vDiff, lightColor.vDiff) != 0.f)
    {
        float4 vWorldPos = mul(float4(vViewPosition.xyz, 1.f), g_matViewInv); // 메인카메라 view 역행렬을 곱해서 월드좌표를 알아낸다.
        float4 vShadowProj = mul(vWorldPos, LightCamViewProjMat); // 광원 시점으로 투영시킨 좌표 구하기
        float fDepth = vShadowProj.z / vShadowProj.w; // w 로 나눠서 실제 투영좌표 z 값을 구한다.(올바르게 비교하기 위해서)
    
        // 계산된 투영좌표를 UV 좌표로 변경해서 ShadowMap 에 기록된 깊이값을 꺼내온다.
        float2 vShadowUV = float2((vShadowProj.x / vShadowProj.w) * 0.5f + 0.5f
                           , (vShadowProj.y / vShadowProj.w) * -0.5f + 0.5f);
    
        if (0.01f < vShadowUV.x && vShadowUV.x < 0.99f
        && 0.01f < vShadowUV.y && vShadowUV.y < 0.99f)
        {
            float fShadowDepth = DYNAMIC_SHADOW_MAP.Sample(g_sam_0, vShadowUV).r;
      
            // 그림자인 경우 빛을 약화시킨다.
            // 투영시킨 값이랑, 기록된 값이 같은경우는 그림자가 생기면 안되는데, 실수오차가 발생할 수 있기 때문에 보정값을 준다.
            if (fShadowDepth != 0.f && (fDepth > fShadowDepth + 0.00001f))
            {
                lightColor.vDiff *= 0.3f;
                lightColor.vSpec = (float4) 0.f;
            }
        }
    }
    
    
    output.vDiffuse = lightColor.vDiff + lightColor.vAmb;
    output.vSpecular = lightColor.vSpec;
    if (0.f != vViewNormal.a)
    {
        float4 vSpecCoeff = decode(vViewNormal.a);
        
        output.vSpecular *= vSpecCoeff;
    }
    return output;    
}


// ========================
// Point Light Shader
// mesh : sphere mesh
// blend state : One - One
// depth stencil state : NoTest, NoWrite
// g_tex_0 : Normal Target
// g_tex_1 : Position Target
// g_int_0 : Light Index
// ========================
VS_DIR_OUT VS_PointLight(VS_DIR_IN _in)
{
    VS_DIR_OUT output = (VS_DIR_OUT) 0.f;
    
    output.vPosition = mul(float4(_in.vPos, 1.f), g_matWVP);
    return output;
}

PS_OUT PS_PointLight(VS_DIR_OUT _in)
{
    PS_OUT output = (PS_OUT) 0.f;
            
    float2 vScreenUV = _in.vPosition.xy / g_vResolution;
    
    float3 vViewNormal = g_tex_0.Sample(g_sam_0, vScreenUV).xyz;
    float3 vViewPosition = g_tex_1.Sample(g_sam_0, vScreenUV).xyz;
    
    if (vViewPosition.z == 0.f)
    {
        //output.vDiffuse = float4(1.f, 0.f, 0.f, 1.f);
        //return output;
        clip(-1);
    }    
    
    // deferred 때 그려진 물체의 viewPos 를 가져와서, 볼륨메쉬(Sphere) 의 Local 까지 따라서 이동했을때,
    // Local 공간(Sphere 의) 에서 원점까지 거리가 1 이내라면(구의 반지름 내 범위) sphere 안에 있던 좌표였다.
    float4 vWorldPos = mul(float4(vViewPosition, 1.f), g_matViewInv);
    float3 vLocalPos = mul(vWorldPos, g_matWorldInv).xyz;
    if (length(vLocalPos) >  1.f)
    {
        //output.vDiffuse = float4(0.f, 0.f, 1.f, 1.f);        
        clip(-1);
    }       
    
    //output.vDiffuse = float4(0.f, 1.f, 0.f, 1.f);
    
    tLightColor lightColor = CalLight(g_int_0, vViewNormal, vViewPosition);
    output.vDiffuse = lightColor.vDiff + lightColor.vAmb;
    output.vSpecular = lightColor.vSpec;
    
    return output;
}


// ==================
// Light Merge Shader
// mesh : rect
// g_tex_0 : Diffuse Target
// g_tex_1 : Diffuse Light Target
// g_tex_2 : Specular Light Target
// ==================
VS_DIR_OUT VS_LightMerge(VS_DIR_IN _in)
{
    VS_DIR_OUT output = (VS_DIR_OUT) 0.f;
    
    output.vPosition.xy = _in.vPos * 2.f;
    output.vPosition.zw = 1.f;
    
    return output;
}

float4 PS_LightMerge(VS_DIR_OUT _in) : SV_Target
{   
    float2 vScreenUV = _in.vPosition.xy / g_vResolution;
    
    float4 vDiffuseColor = g_tex_0.Sample(g_sam_0, vScreenUV);
    float4 vDiffuseLight = g_tex_1.Sample(g_sam_0, vScreenUV);
    float4 vSpeculLight = g_tex_2.Sample(g_sam_0, vScreenUV);
    
    float4 vColor = vDiffuseColor * vDiffuseLight + vSpeculLight;
    vColor.a = 1.f;
    
    return vColor;
}


#endif