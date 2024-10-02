/**
 * @file PipeLineList.h
 * @brief 各種パイプラインをまとめた部分
 * @author KATO
 * @date 未記録
 */

#pragma once

enum class PipelineType {
	Standard3D,
	Standard2D,
	Particle,
	Skinning,
	SkyBox,
	VertexAnimationTexture,
	PostProcess,
	Grayscale,
	Vignette,
	Smoothing,
	Gaussian,
	Outline,
	RadialBlur,
	MaskTexture,
	Random,
	LensDistortion,
	Scanlines,
	HSV,
};
