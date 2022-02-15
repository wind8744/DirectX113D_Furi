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

#define DYNAMIC_SHADOW_MAP      g_tex_3 // ���� ������ ��
#define LightCamViewProjMat     g_mat_0 // ���� ���������� View * Proj ���
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
        
    // �׸��� ����    
    // ���� ������ �׸��� ó���� ���� �ʴ´�.
    if (dot(lightColor.vDiff, lightColor.vDiff) != 0.f)
    {
        float4 vWorldPos = mul(float4(vViewPosition.xyz, 1.f), g_matViewInv); // ����ī�޶� view ������� ���ؼ� ������ǥ�� �˾Ƴ���.
        float4 vShadowProj = mul(vWorldPos, LightCamViewProjMat); // ���� �������� ������Ų ��ǥ ���ϱ�
        float fDepth = vShadowProj.z / vShadowProj.w; // w �� ������ ���� ������ǥ z ���� ���Ѵ�.(�ùٸ��� ���ϱ� ���ؼ�)
    
        // ���� ������ǥ�� UV ��ǥ�� �����ؼ� ShadowMap �� ��ϵ� ���̰��� �����´�.
        float2 vShadowUV = float2((vShadowProj.x / vShadowProj.w) * 0.5f + 0.5f
                           , (vShadowProj.y / vShadowProj.w) * -0.5f + 0.5f);
    
        if (0.01f < vShadowUV.x && vShadowUV.x < 0.99f
        && 0.01f < vShadowUV.y && vShadowUV.y < 0.99f)
        {
            float fShadowDepth = DYNAMIC_SHADOW_MAP.Sample(g_sam_0, vShadowUV).r;
      
            // �׸����� ��� ���� ��ȭ��Ų��.
            // ������Ų ���̶�, ��ϵ� ���� �������� �׸��ڰ� ����� �ȵǴµ�, �Ǽ������� �߻��� �� �ֱ� ������ �������� �ش�.
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
    
    // deferred �� �׷��� ��ü�� viewPos �� �����ͼ�, �����޽�(Sphere) �� Local ���� ���� �̵�������,
    // Local ����(Sphere ��) ���� �������� �Ÿ��� 1 �̳����(���� ������ �� ����) sphere �ȿ� �ִ� ��ǥ����.
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