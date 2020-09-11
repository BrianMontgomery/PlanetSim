#pragma once

#include "PSIM/Graphics/Asset/Model.h"
#include "PSIM/Graphics/Asset/Texture.h"

//--------------------------------------------------------------------------------------------------------------------------------

class PSIMAssetLibraries {
public:
	static PSIMAssetLibraries *getAssetLibraries();
protected:
	PSIMAssetLibraries();
	virtual ~PSIMAssetLibraries();

	friend class PSIMAssetLibrariesDestroyer;

private:
	static PSIMAssetLibraries *m_AssetLibraries;
	static PSIMAssetLibrariesDestroyer destroyer;

public:
	ModelLibrary PSIM_ModelLibrary;
	TextureLibrary PSIM_TextureLibrary;
};

//--------------------------------------------------------------------------------------------------------------------------------

class PSIMAssetLibrariesDestroyer
{
public:
	PSIMAssetLibrariesDestroyer(PSIMAssetLibraries * = 0);
	~PSIMAssetLibrariesDestroyer() { delete _singleton; }
	void SetSingleton(PSIMAssetLibraries *s) { _singleton = s; }

private:
	PSIMAssetLibraries *_singleton;
};