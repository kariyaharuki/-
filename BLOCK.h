#pragma once

class Block {
public:
    Block(float x, float y, float w, float h);
    virtual ~Block() = default;

    virtual void Draw() const;
    virtual bool IsHit(float ball_x, float ball_y, float ball_r);

    bool IsAlive() const;
    void Destroy();

protected:
    float x, y, w, h;
    bool alive;
};
