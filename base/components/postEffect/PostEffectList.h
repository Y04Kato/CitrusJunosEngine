#pragma once
#include "DirectXCommon.h"

struct PostEffectList {
	int32_t isGrayScale;
	int32_t isVignetting;
	int32_t isSmoothing;
	int32_t isGaussian;
	int32_t isOutline;
};

class PostEffectLists {
public:
	static PostEffectLists* GetInstance();

	void Initialize();

	void Update();

	void SetTarget(const PostEffectList& postEffectList) { postEffectList_ = postEffectList; }
	const PostEffectList& GetPostEffectList() { return postEffectList_; }

private:
	PostEffectLists() = default;
	~PostEffectLists() = default;
	PostEffectLists(const PostEffectLists& obj) = default;
	PostEffectLists& operator=(const PostEffectLists& obj) = default;

	PostEffectList postEffectList_;
};