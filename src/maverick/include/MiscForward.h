#pragma once

#include <memory>
#include <map>
#include <vector>

typedef unsigned long IDType;

class Shader;
class Animation;
class EngineComponent;
class Game;
class GameObject;
class Player;
class Point;
class SoundManager;
class GameplayComponent;
class GameServices;
class DlgManager;

typedef std::weak_ptr<Game> GameWPtr;
typedef std::weak_ptr<SoundManager> SoundManagerWPtr;
typedef std::weak_ptr<GameObject> GameObjectWPtr;
typedef std::weak_ptr<GameServices> GameServicesWPtr;
typedef std::weak_ptr<Animation> AnimationWPtr;
typedef std::weak_ptr<Player> PlayerWPtr;

typedef std::shared_ptr<Game> GamePtr;
typedef std::shared_ptr<DlgManager> DlgManagerPtr;
typedef std::shared_ptr<GameServices> GameServicesPtr;
typedef std::shared_ptr<EngineComponent> EngineComponentPtr;
typedef std::shared_ptr<Shader> ShaderPtr;
typedef std::shared_ptr<Animation> AnimationPtr;
typedef std::shared_ptr<GameObject> GameObjectPtr;
typedef std::shared_ptr<GameplayComponent> GameplayComponentPtr;

typedef std::map<IDType, GameObjectPtr> ObjectsArr;

typedef std::vector</*make me const please? */ObjectsArr*> GameObjectArrPtrVec;
typedef std::vector<const ObjectsArr*> GameObjectCArrPtrVec;


