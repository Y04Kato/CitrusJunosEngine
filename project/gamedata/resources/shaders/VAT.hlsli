struct VertexShaderOutput {
	float32_t4 position : SV_POSITION;
	float32_t2 texcoord : TEXCOORD0;
	float32_t3 normal : NORMAL0;
	float32_t3 worldPosition : POSITION0;
};

struct VertexShaderInput {
	float32_t4 position : POSITION0;
	float32_t2 texcoord : TEXCOORD0;
	float32_t3 normal : NORMAL0;
};

struct PixelShaderOutput {
	float32_t4 color : SV_TARGET0;
};

struct TransformationMatrix {
	float32_t4x4 matWorld;
	float32_t4x4 WorldInverseTranspose;
};

struct ViewProjectionMatrix {
	float32_t4x4 view;
	float32_t4x4 projection;
	float32_t3 camera;
};

struct VATData {
	float32_t AnimationTimeOffset;
	float32_t VatAnimFps;
	float32_t VatAnimLength;
};

struct Material {
	float32_t4 color;
	int32_t enableLighting;
	float32_t4x4 uvTransform;
	float32_t shininess;
};

struct DirectionalLight {
	float32_t4 color;
	float32_t3 direction;
	float32_t intensity;
};

struct Camera {
	float32_t3 worldPosition;
};

struct PointLight {
	float32_t4 color;
	float32_t3 position;
	float32_t intensity;
	float32_t radius;
	float32_t decay;
	float32_t padding[2];
};