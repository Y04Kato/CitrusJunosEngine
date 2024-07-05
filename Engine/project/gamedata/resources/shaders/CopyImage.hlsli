struct VertexShaderOutput{
    float32_t4 position : SV_POSITION;
    float32_t2 texcoord : TEXCOORD0;
};

struct MaskData{
    float32_t maskThreshold;
    float32_t3 maskColor;
    float32_t3 edgeColor;
};

struct RandomData
{
    float32_t time;
};