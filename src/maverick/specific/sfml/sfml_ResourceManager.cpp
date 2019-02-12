#include "sfml_ResourceManager.h"

#include "Log.h"
#include "CfgStatic.h"
#include "Shader.h"

#include "sfml_TextureAtlas.h"
#include "sfml_Sound.h"

static const TextureRect emptyTexRect = TextureRect();

std::shared_ptr<sfml_ResourceManager>& sfml_ResourceManager::instancePtr() {
	static std::shared_ptr<sfml_ResourceManager> instancePtr {nullptr};
	return instancePtr;
}

sfml_ResourceManager* sfml_ResourceManager::Inst() {

	auto& ptr = instancePtr();
	if (ptr == nullptr) {
		ptr.reset(new sfml_ResourceManager());
	}

	return ptr.get();
}

void sfml_ResourceManager::ResetInst() {
	
	auto& ptr = instancePtr();
	if (ptr != nullptr) {
		ptr.reset();
	}
}

sfml_ResourceManager::sfml_ResourceManager() {
	Log::Inst()->PutMessage("ResourceManager::ResourceManager");
}

sfml_ResourceManager::~sfml_ResourceManager() {
	Log::Inst()->PutMessage("~ResourceManager");
}

bool sfml_ResourceManager::LoadAtlas(const std::string& tName, const std::string& descName) {

	TextureAtlasPtr atlasPtr = std::make_shared<TextureAtlas>();
	const bool loaded = atlasPtr->load(descName, tName);

	if (!loaded) {
		return false;
	}

	const TexturePtr& atlasTexPtr = AddTexture(tName);

	if (atlasTexPtr == nullptr) {
		return false;
	}

	_atlases[tName] = atlasPtr;
	return true;
}

ShaderPtr sfml_ResourceManager::AddShader(const std::string& sName) {

	const auto& shIt = _shaders.find(sName);

	if (shIt != _shaders.end()) {
		const ShaderPtr foundShPtr = shIt->second;
		Log::Inst()->PutMessage("ResourceManager::AddShader: shader already added " + sName);
		return foundShPtr;
	}

	const std::string& resPref = CfgStatic::getResourceDir();

	ShaderPtr shPtr = Shader::Create(sName);	

	const bool loadOk = shPtr->Load(resPref + sName + CfgStatic::vertExt, resPref + sName + CfgStatic::fragExt);
	if (!loadOk) {
		Log::Inst()->PutErr("ResourceManager::addShader error: unable to load " + sName);
		return ShaderPtr();
	}

	_shaders[sName] = shPtr;
	Log::Inst()->PutMessage("ResourceManager::addShader: shader added successfully: " + sName);

	return shPtr;
}

ShaderPtr sfml_ResourceManager::GetShader(const std::string& sName, const bool onlyTry) {

	const auto& shIt = _shaders.find(sName);

	if (shIt == _shaders.end()) {

		if (!onlyTry) {
			Log::Inst()->PutErr("ResourceManager::GetShader error, not found sound " + sName);
		}
		return ShaderPtr();
	}

	return shIt->second;
}

TexturePtr sfml_ResourceManager::AddTexture(const std::string& tName) {

	const auto& texIt = _textures.find(tName);

	if (texIt != _textures.end()) {
		const TexturePtr foundTexPtr = texIt->second;
		Log::Inst()->PutMessage("ResourceManager::addTexture: texture already added " + tName);
		return foundTexPtr;
	}

	TexturePtr texPtr = std::make_shared<sfml_Texture>();
	const bool loadOk = texPtr->loadFromFile(CfgStatic::getResourceDir() + tName);
	if (!loadOk) {
		Log::Inst()->PutErr("ResourceManager::AddTexture error: unable to load " + tName);
		return TexturePtr();
	}

	_textures[tName] = texPtr;
	Log::Inst()->PutMessage("ResourceManager::AddTexture: texture added successfully: " + tName);

	return texPtr;
}

TextureRect sfml_ResourceManager::GetTexture(const std::string& tName, const bool onlyTry) {

	const auto& tIt = _textures.find(tName);

	if (tIt != _textures.end()) {
		const TexturePtr& ptr = tIt->second;
		const auto tSize = ptr->getTex()->getSize();

		const Rect fullTexRect(0.f, 0.f, float(tSize.x), float(tSize.y));
		const TextureRect textureRect { ptr, fullTexRect };

		return textureRect;

	} else { // not a standalone texture, will look in atlases

		for (const auto& atlIt : _atlases) {
			const TextureAtlas& atlas = *atlIt.second;
			const auto& smallRect = atlas.getRect(tName);

			if (smallRect != Rect()) { // found!

				const std::string& bigTexName = atlas.getTextureName();
				const auto& texRectBigTex = GetTexture(bigTexName);

				if (texRectBigTex.texturePtr.lock() == nullptr) {
					Log::Inst()->PutErr("ResourceManager::GetTexture error, atlas may not be loaded yet: " + bigTexName);
					return emptyTexRect;
				}

				const TextureRect smallTextureRect{ texRectBigTex.texturePtr, smallRect };
				return smallTextureRect;
			}
		}
	}

	if (!onlyTry) {
		Log::Inst()->PutErr("ResourceManager::getTexture error, not found texture " + tName);
	}

	return emptyTexRect;
}

bool sfml_ResourceManager::RemoveTexture(const std::string& tName) {

	const auto& tIt = _textures.find(tName);
	if (tIt == _textures.end()) {
		return false;
	}
	
	_textures.erase(tIt);
	return true;
}