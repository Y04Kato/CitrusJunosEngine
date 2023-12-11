#pragma once
#include <iostream>
#include <HAPI.h>
#include "CJEngine.h"
#include <string>
//
//static std::string getLastError();
//static std::string getLastCookError();
//static std::string getString(HAPI_StringHandle stringHandle);
//
//class CreateHoudini {
//public:
//	void Initialize();
//
//	void Update();
//
//	void Draw();
//
//	void Finalize();
//
//	void Loadhda(int argc, char** argv, const char* filePath);
//
//private:
//	DirectXCommon* dxCommon_;
//	CitrusJunosEngine* CJEngine_;
//
//	HAPI_Session session_;
//	bool bUseInProcess_ = false;
//
//	HAPI_CookOptions cookOptions_;
//
//	HAPI_AssetLibraryId assetLibId_;
//
//	const char* hdaFile_;
//
//private:
//	static void printCompleteNodeInfo(HAPI_Session& session, HAPI_NodeId nodeId,
//		HAPI_AssetInfo& assetInfo);
//	static void processGeoPart(HAPI_Session& session, HAPI_AssetInfo& assetInfo,
//		HAPI_NodeId objectNode, HAPI_NodeId geoNode,
//		HAPI_PartId partId);
//	static void processFloatAttrib(HAPI_Session& session, HAPI_AssetInfo& assetInfo,
//		HAPI_NodeId objectNode, HAPI_NodeId geoNode,
//		HAPI_PartId partId, HAPI_AttributeOwner owner,
//		std::string name);
//};