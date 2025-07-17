#pragma once
#include "Scene.h"

class TitleScene : public Scene {
public:
    void Init() override {}
    void Update(float delta_time) override;
    void Draw() const override;
    void End() override {}
    bool startGame = false;
};
