#pragma once

class Bar {
public:
    Bar();
    void Init(float px, float py);
    void Update(float delta_time);
    void Draw() const;

    float GetX() const;
    float GetY() const;
    float GetW() const;
    float GetH() const;

private:
    float x, y, w, h;
    float speed;
};
