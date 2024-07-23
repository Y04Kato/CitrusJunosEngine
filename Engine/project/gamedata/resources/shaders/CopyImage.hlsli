struct VertexShaderOutput{
    float32_t4 position : SV_POSITION;
    float32_t2 texcoord : TEXCOORD0;
};

struct MaskData{
    float32_t maskThreshold;
    float32_t3 maskColor;
    float32_t3 edgeColor;
};

struct HSVMaterial{
    float32_t hue;
    float32_t saturation;
    float32_t value;
};

struct RandomData
{
    float32_t time;
};

struct ScanlineData
{
    float32_t scanlineIntensity;
    float32_t scanlineFrequency;
    float32_t time;
};