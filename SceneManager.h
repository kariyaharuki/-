#pragma once
#include "Scene.h"

enum class SceneType { Title, Game, Result };

class SceneManager {
public:
    SceneManager();
    ~SceneManager();
    void ChangeScene(SceneType type);
    void Update(float delta_time);
    void Draw() const;
    SceneType GetCurrentSceneType() const { return currentType; }
    Scene* GetCurrentScene() const { return currentScene; }
private:
    Scene* currentScene;
    SceneType currentType;
};
