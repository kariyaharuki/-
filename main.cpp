#include "SceneManager.h"
#include "TitleScene.h"
#include "GameScene.h"
#include "ResultScene.h"
SceneManager sceneManager;

void MainLoop(float delta_time) {
    sceneManager.Update(delta_time);

    // シーン遷移
    if (sceneManager.GetCurrentSceneType() == SceneType::Title) {
        auto* scene = static_cast<TitleScene*>(sceneManager.GetCurrentScene());
        if (scene->startGame) sceneManager.ChangeScene(SceneType::Game);
    }
    else if (sceneManager.GetCurrentSceneType() == SceneType::Game) {
        auto* scene = static_cast<GameScene*>(sceneManager.GetCurrentScene());
        if (scene->isGameOver || scene->isClear) sceneManager.ChangeScene(SceneType::Result);
    }
    else if (sceneManager.GetCurrentSceneType() == SceneType::Result) {
        auto* scene = static_cast<ResultScene*>(sceneManager.GetCurrentScene());
        if (scene->retry) sceneManager.ChangeScene(SceneType::Title);
    }

    sceneManager.Draw();
}
