#include "VAT.hlsli"

ConstantBuffer<Material> gMaterial : register(b0);
ConstantBuffer<DirectionalLight> gDirectionalLight : register(b1);
ConstantBuffer<Camera> gCamera : register(b2);
ConstantBuffer<PointLight> gPointLight : register(b3);

Texture2D<float32_t4> gTexture : register(t2);
TextureCube<float32_t4> gEnviromentTexture : register(t3);

SamplerState gSampler : register(s1);

PixelShaderOutput main(VertexShaderOutput input) {
	PixelShaderOutput output;

	float32_t4 transformedUV = mul(float32_t4(input.texcoord, 0.0f, 1.0f), gMaterial.uvTransform);
	float32_t4 textureColor = gTexture.Sample(gSampler, transformedUV.xy);

	if (textureColor.a == 0.0) {
		discard;
	}
	if (textureColor.a <= 0.5) {
		discard;
	}
	if (output.color.a == 0.0) {
		discard;
	}

	//Lightingする場合
	float32_t Ndotl = dot(normalize(input.normal), -gDirectionalLight.direction);
	float32_t cos = pow(Ndotl * 0.5f + 0.5f, 2.0f);
	if (gMaterial.enableLighting == 1) {//HalfLambert
		output.color.rgb = gMaterial.color.rgb * textureColor.rgb * gDirectionalLight.color.rgb * cos * gDirectionalLight.intensity;
		output.color.a = gMaterial.color.a * textureColor.a;
	}
	else if (gMaterial.enableLighting == 2) {//PhongReflection
		float32_t3 toEye = normalize(gCamera.worldPosition - input.worldPosition);
		float32_t3 reflectLight = reflect(gDirectionalLight.direction, normalize(input.normal));
		float32_t RdotE = dot(reflectLight, toEye);
		float32_t specularPow = pow(saturate(RdotE), gMaterial.shininess);//反射強度
		//拡散反射
		float32_t3 diffuseDirectionalLight = gMaterial.color.rgb * textureColor.rgb * cos * gDirectionalLight.intensity;
		//鏡面反射
		float32_t3 specularDirectionalLight = gDirectionalLight.color.rgb * gDirectionalLight.intensity * specularPow * float32_t3(1.0f, 1.0f, 1.0f);

		output.color.rgb = diffuseDirectionalLight + specularDirectionalLight;
		output.color.a = gMaterial.color.a * textureColor.a;
	}
	else if (gMaterial.enableLighting == 3) {//BlinnPhongReflection
		float32_t3 specularColor = { 1.0f, 1.0f, 1.0f };//反射色

		//DirectionalLight
		float32_t3 toEye = normalize(gCamera.worldPosition - input.worldPosition);
		float32_t3 halfVector = normalize(-gDirectionalLight.direction + toEye);
		float32_t NdotH = dot(normalize(input.normal), halfVector);
		float32_t specularPow = pow(saturate(NdotH), gMaterial.shininess);//反射強度
		//拡散反射
		float32_t3 diffuseDirectionalLight = gMaterial.color.rgb * textureColor.rgb * cos * gDirectionalLight.intensity;
		//鏡面反射
		float32_t3 specularDirectionalLight = gDirectionalLight.color.rgb * gDirectionalLight.intensity * specularPow * specularColor;

		//PointLight
		float32_t3 pointLightDirection = normalize(input.worldPosition - gPointLight.position);
		float32_t distance = length(gPointLight.position - input.worldPosition);
		float32_t factor = pow(saturate(-distance / gPointLight.radius + 1.0f), gPointLight.decay);
		halfVector = normalize(-pointLightDirection + toEye);
		NdotH = dot(normalize(input.normal), halfVector);
		specularPow = pow(saturate(NdotH), gMaterial.shininess);//反射強度
		Ndotl = dot(normalize(input.normal), -gPointLight.position);
		cos = pow(Ndotl * 0.5f + 0.5f, 2.0f);
		//拡散反射
		float32_t3 diffusePointLight = gMaterial.color.rgb * textureColor.rgb * gPointLight.color.rgb * cos * gPointLight.intensity * factor;
		//鏡面反射
		float32_t3 specularPointLight = gMaterial.color.rgb * gPointLight.intensity * specularPow * gPointLight.color.rgb * factor;

		output.color.rgb = diffuseDirectionalLight + specularDirectionalLight + diffusePointLight + specularPointLight;
		output.color.a = gMaterial.color.a * textureColor.a;
	}
	else if (gMaterial.enableLighting == 4) {//Environment+
		float32_t3 specularColor = { 1.0f, 1.0f, 1.0f };//反射色

		//DirectionalLight
		float32_t3 toEye = normalize(gCamera.worldPosition - input.worldPosition);
		float32_t3 halfVector = normalize(-gDirectionalLight.direction + toEye);
		float32_t NdotH = dot(normalize(input.normal), halfVector);
		float32_t specularPow = pow(saturate(NdotH), gMaterial.shininess);//反射強度
		//拡散反射
		float32_t3 diffuseDirectionalLight = gMaterial.color.rgb * textureColor.rgb * cos * gDirectionalLight.intensity;
		//鏡面反射
		float32_t3 specularDirectionalLight = gDirectionalLight.color.rgb * gDirectionalLight.intensity * specularPow * specularColor;

		//PointLight
		float32_t3 pointLightDirection = normalize(input.worldPosition - gPointLight.position);
		float32_t distance = length(gPointLight.position - input.worldPosition);
		float32_t factor = pow(saturate(-distance / gPointLight.radius + 1.0f), gPointLight.decay);
		halfVector = normalize(-pointLightDirection + toEye);
		NdotH = dot(normalize(input.normal), halfVector);
		specularPow = pow(saturate(NdotH), gMaterial.shininess);//反射強度
		Ndotl = dot(normalize(input.normal), -gPointLight.position);
		cos = pow(Ndotl * 0.5f + 0.5f, 2.0f);
		//拡散反射
		float32_t3 diffusePointLight = gMaterial.color.rgb * textureColor.rgb * gPointLight.color.rgb * cos * gPointLight.intensity * factor;
		//鏡面反射
		float32_t3 specularPointLight = gMaterial.color.rgb * gPointLight.intensity * specularPow * gPointLight.color.rgb * factor;

		float32_t3 cameraToPosition = normalize(input.worldPosition - gCamera.worldPosition);
		float32_t3 reflectVector = reflect(cameraToPosition, normalize(input.normal));
		float32_t4 enviromentColor = gEnviromentTexture.Sample(gSampler, reflectVector);

		enviromentColor = enviromentColor * 0.1f;

		output.color.rgb = diffuseDirectionalLight + specularDirectionalLight + diffusePointLight + specularPointLight + enviromentColor.rgb;
		output.color.a = gMaterial.color.a * textureColor.a;
	}
	else {
		output.color = gMaterial.color * textureColor;
	}
	return output;
}