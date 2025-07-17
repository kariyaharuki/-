#pragma once
class Scene {
public:
    virtual ~Scene() {}
    virtual void Init() = 0;
    virtual void Update(float delta_time) = 0;
    virtual void Draw() const = 0;
    virtual void End() = 0;
};
