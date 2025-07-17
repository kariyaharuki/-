#pragma once

// ゲーム状態
enum class GameState {
    Playing,
    GameOver,
    Clear
};

class GameManager {
public:
    static GameManager& Instance();

    void Init();
    void Update(float delta_time);
    void Draw();
    void SetState(GameState state);
    GameState GetState() const;
    void Reset();

    int GetScore() const;
    void AddScore(int value);

private:
    GameManager();
    GameManager(const GameManager&) = delete;
    GameManager& operator=(const GameManager&) = delete;

    GameState state;
    int score;
};

// ゲーム用関数
void gameStart();
void gameMain(float delta_time);
void gameEnd();
