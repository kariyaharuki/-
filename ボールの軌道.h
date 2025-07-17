#pragma once

class Ball {
private:
    float x, y;      // ボールの座標
    float vx, vy;    // ボールの速度
    float r;         // 半径

public:
    Ball();
    void Init(float px, float py);
    void Update(float delta_time);
    void Draw() const;
    float GetX() const;
    float GetY() const;
    float GetR() const;
    float GetVX() const;
    float GetVY() const;
    void SetSpeed(float newVx, float newVy);
    void ReflectX();
    void ReflectY();
};
