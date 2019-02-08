#include <memory>
#include <unordered_map>
#include <vector>

typedef unsigned long IDType;

class Shader;
class Animation;
class EngineComponent;
class Game;
class GameObject;
class Point;
class SoundManager;
class GameplayComponent;

typedef std::weak_ptr<Game> GameWPtr;
typedef std::weak_ptr<SoundManager> SoundManagerWPtr;
typedef std::weak_ptr<GameObject> GameObjectWPtr;

typedef std::shared_ptr<Game> GamePtr;
typedef std::shared_ptr<EngineComponent> EngineComponentPtr;
typedef std::shared_ptr<Shader> ShaderPtr;
typedef std::shared_ptr<Animation> AnimationPtr;
typedef std::shared_ptr<GameObject> GameObjectPtr;
typedef std::shared_ptr<GameplayComponent> GameplayComponentPtr;

typedef std::unordered_map<IDType, GameObjectPtr> ObjectsArr;

typedef std::vector<ObjectsArr*> GameObjectArrPtrVec;
typedef std::vector<const ObjectsArr*> GameObjectCArrPtrVec;


