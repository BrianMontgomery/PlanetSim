#include "PSIMPCH.h"
#include "PSIMAssetLibs.h"

PSIMAssetLibraries* PSIMAssetLibraries::m_AssetLibraries = nullptr;
PSIMAssetLibrariesDestroyer PSIMAssetLibraries::destroyer;

//PSIMAssetLibrariesDestroyer Funcs
//--------------------------------------------------------------------------------------------------------------------------------


PSIMAssetLibrariesDestroyer::PSIMAssetLibrariesDestroyer(PSIMAssetLibraries *s)
{
	_singleton = s;
}

PSIMAssetLibraries* PSIMAssetLibraries::getAssetLibraries() {
	if (!m_AssetLibraries)
	{
		m_AssetLibraries = new PSIMAssetLibraries;
		destroyer.SetSingleton(m_AssetLibraries);
	}
	return m_AssetLibraries;
}

//PSIMAssetLibraries Funcs
//--------------------------------------------------------------------------------------------------------------------------------

PSIMAssetLibraries::PSIMAssetLibraries()
{
}

PSIMAssetLibraries::~PSIMAssetLibraries()
{
	PSIM_PROFILE_FUNCTION();
}