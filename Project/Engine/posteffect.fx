#ifndef _POSTEFFECT
#define _POSTEFFECT

#include "value.fx"

// ==============================
// PostEffect Shader
// g_tex_0 : Target Copy Texture
// ==============================
float randomNoise(float2 seed)
{
    return frac(sin(dot(seed * floor(g_AccTime * 30.0), float2(127.1, 311.7))) * 43758.5453123);
}
   
float randomNoise(float seed)
{
    return randomNoise(float2(seed, 1.0));
}

float luminance(half3 color)
{
    return dot(color, half3(0.222, 0.707, 0.071));
}

float intensity(in float4 color)
{
    return sqrt((color.x * color.x) + (color.y * color.y) + (color.z * color.z));
}
half3 ColorAdjustment_Contrast_V3(float3 In, half3 ContrastFactor, float Contrast)
{
    half3 Out = (In - ContrastFactor) * Contrast + ContrastFactor;
    return Out;
}
struct VTX_IN
{
    float3 vLocalPos : POSITION;
    float2 vUV : TEXCOORD;    
};

struct VTX_OUT
{
    float4 vPosition : SV_Position;
    float2 vUV : TEXCOORD;
};

VTX_OUT VS_PostEffect(VTX_IN _in)
{
    VTX_OUT output = (VTX_OUT) 0.f;
    
    output.vPosition = mul(float4(_in.vLocalPos, 1.f), g_matWVP);
    output.vUV = _in.vUV;
    
    return output;
}


float4 PS_PostEffect(VTX_OUT _in) : SV_Target
{
    //return float4(1.f, 0.f, 0.f, 1.f);
    
        //float4 vColor = (float4) 0.f;
       
    //// ȣ��� �ȼ��� ȭ�� ��ü ���� UV
    //float2 vFullUV = _in.vPosition.xy / g_vResolution;    
    
    //vFullUV.x = vFullUV.x + cos(vFullUV.y * 6.28 + g_AccTime * 2.f) * 0.1f;
    
    //vColor = g_tex_0.Sample(g_sam_0, vFullUV);  
       
    //return vColor;
    
    
    //ripple effect
    if (g_int_3 == 0)
    {
        float2 vFullUV = _in.vPosition.xy / g_vResolution;
        float2 tc = g_tex_0.Sample(g_sam_0, vFullUV);
        float2 p = -1.0 + 2.0 * tc;
        float len = length(p);
        float2 uv = tc + (p / len) * cos(len * 12.0 - g_AccTime * 4.0) * 0.03;
        float3 col = g_tex_0.Sample(g_sam_0, uv).xyz;
        return float4(col, 1.0);
    }
    else if (g_int_3 ==1)
    { // Glich line
        float2 uv = _in.vPosition.xy / g_vResolution;
      
        float2 blockLayer1 = floor(uv * float2(2, 16));
      
        float lineNoise = pow(randomNoise(blockLayer1), 8) * 5 - pow(randomNoise(5.1379), 7.1) * 2;
      
        float4 colorR = g_tex_0.Sample(g_sam_0, uv);
        float4 colorG = g_tex_0.Sample(g_sam_0, uv + float2(lineNoise * 0.05 * randomNoise(5.0), 0));
        float4 colorB = g_tex_0.Sample(g_sam_0, uv - float2(lineNoise * 0.05 * randomNoise(31.0), 0));
      
        float4 result = float4(float3(colorR.r, colorG.g, colorB.b), colorR.a + colorG.a + colorB.a);
        result = lerp(colorR, result, 1);
      
        return result;
    }
    else if (g_int_3 == 2)
    {
        //�� ���� ����
        float2 uv = _in.vPosition.xy / g_vResolution;
        const half3 cyanfilter = float3(0.0, 1.30, 1.0);
        const half3 magentafilter = float3(1.0, 0.0, 1.05);
        const half3 yellowfilter = float3(1.6, 1.6, 0.05);
        const half2 redorangefilter = float2(1.05, 0.620); // RG_
        const half2 greenfilter = float2(0.30, 1.0); // RG_
        const half2 magentafilter2 = magentafilter.rb; // R_B


        half4 color = g_tex_0.Sample(g_sam_0, uv);

        half3 balance = 1.0 / ((1, 1, 1) * 1);

        half negative_mul_r = dot(redorangefilter, color.rg * balance.rr);
        half negative_mul_g = dot(greenfilter, color.rg * balance.gg);
        half negative_mul_b = dot(magentafilter2, color.rb * balance.bb);

        half3 output_r = negative_mul_r.rrr + cyanfilter;
        half3 output_g = negative_mul_g.rrr + magentafilter;
        half3 output_b = negative_mul_b.rrr + yellowfilter;

        half3 result = output_r * output_g * output_b;
        return float4(lerp(color.rgb, result.rgb, 0.1), 1.0);
    }
    else if (g_int_3 == 3)
    {
    //Color filter
    
        float2 uv = _in.vPosition.xy / g_vResolution;
        float4 finalColor = g_tex_0.Sample(g_sam_0, uv);
        float4 _Contrast = float4(1.f, 0.f, 0.f, 0.3f);
    
        finalColor.rgb = ColorAdjustment_Contrast_V3(finalColor.rgb, half3(_Contrast.x, _Contrast.y, _Contrast.z), 1 - (_Contrast.w));
        return finalColor;
    }
    else if (g_int_3 == 4)
    {
        //���� ���� 
        half2 pixelSize = float2(1 / g_vResolution.x, 1 / g_vResolution.y);
        pixelSize *= 1.5f;
    
        float2 uv = _in.vPosition.xy / g_vResolution;
    
        float4 blur = g_tex_0.Sample(g_sam_0, uv + half2(pixelSize.x, -pixelSize.y));
        blur += g_tex_0.Sample(g_sam_0, uv + half2(-pixelSize.x, -pixelSize.y));
        blur += g_tex_0.Sample(g_sam_0, uv + half2(pixelSize.x, pixelSize.y));
        blur += g_tex_0.Sample(g_sam_0, uv + half2(-pixelSize.x, pixelSize.y));
        blur *= 0.25;
        float4 sceneColor = g_tex_0.Sample(g_sam_0, uv);
        return sceneColor + (sceneColor - blur) * 2.5;
    }
    else if (g_int_3 == 5)
    {
        float2 uv = _in.vPosition.xy / g_vResolution;
        float vigntSharpness = 0.2f;
        float vigntIndensity = 0.5f;
        float4 color = float4(0.7, 0, 0, 0.8);
        float4 sceneColor = g_tex_0.Sample(g_sam_0, uv);
        float indensity = distance(uv.xy, float2(0.5, 0.5)); //center�� uv��ǥ �Ÿ���
        
        // 0~1������ �ε巴�� ���ϴ� Hermite ������ ���� 0.8���� ������ 0���� 
        indensity = smoothstep(0.8, vigntSharpness * 0.799, indensity * (vigntIndensity + vigntSharpness));
      
        float3 finalColor = lerp(color.rgb, sceneColor.rgb, indensity); //����
     
        return float4(finalColor.rgb, color.a);
    }
    
    return float4(0.f, 0.f, 0.f, 0.f);
    /*
    
    */
    
    /*
    
    */
    
    /*
   
    */
    
    // �¾����� ȭ�� ����� ȿ��
    
    
    
}






#endif