#include "Gm_main.h"
#include "Block.h"
#include "Ball.h"
#include "Bar.h"
#include "../dxe/dxe.h"

// -------------------- 設定 --------------------
#define BLOCK_ROWS 12
#define BLOCK_COLS 18
#define MAX_BLOCKS (BLOCK_ROWS * BLOCK_COLS)
#define MAX_BALLS 10

enum class SceneType { Title, Game, Result };
SceneType scene = SceneType::Title;

Block* blocks[MAX_BLOCKS] = { nullptr };
Ball balls[MAX_BALLS];
int ballCount = 1;
Bar bar;
int lastScore = 0;

// --- アイテムブロック（青色） ---
class ItemBlock : public Block {
public:
    ItemBlock(float x, float y, float w, float h) : Block(x, y, w, h) {}
    void Draw() const override {
        if (alive) {
            DrawBox(static_cast<int>(x), static_cast<int>(y), static_cast<int>(x + w), static_cast<int>(y + h), GetColor(50, 100, 255), TRUE);
        }
    }
};

// --- ハードブロック（硬い赤色） ---
class HardBlock : public Block {
public:
    HardBlock(float x, float y, float w, float h)
        : Block(x, y, w, h), hp(2) {
    }
    void Draw() const override {
        if (!alive) return;
        int color = (hp == 2) ? GetColor(255, 100, 100) : GetColor(200, 50, 50);
        DrawBox(static_cast<int>(x), static_cast<int>(y), static_cast<int>(x + w), static_cast<int>(y + h), color, TRUE);
    }
    bool IsHit(float ball_x, float ball_y, float ball_r) override {
        if (!alive) return false;
        if (Block::IsHit(ball_x, ball_y, ball_r)) {
            hp--;
            if (hp <= 0) alive = false;
            return true;
        }
        return false;
    }
private:
    int hp;
};

// -------------------- GameManager --------------------
GameManager& GameManager::Instance() {
    static GameManager instance;
    return instance;
}

GameManager::GameManager() : state(GameState::Playing), score(0) {}

void GameManager::Init() { 
    state = GameState::Playing; score = 0; 
}
void GameManager::Update(float delta_time) {}
void GameManager::Draw() {}
void GameManager::SetState(GameState s) { state = s; }
GameState GameManager::GetState() const { return state; }
void GameManager::Reset() { Init(); }
int GameManager::GetScore() const { return score; }
void GameManager::AddScore(int value) { score += value; }

// -------------------- ユーティリティ --------------------
float getBlockOffsetX(int block_cols, float block_w, float interval, int window_w) {
    return (window_w - (block_cols * block_w + (block_cols - 1) * interval)) / 2.0f;
}

// -------------------- 終了処理 --------------------
void gameEnd() {
    for (int i = 0; i < MAX_BLOCKS; ++i) {
        if (blocks[i]) {
            delete blocks[i];
            blocks[i] = nullptr;
        }
    }
}

// -------------------- 初期化 --------------------
void gameStart() {
    // 前回分の解放
    gameEnd();

    GameManager::Instance().Init();
    int idx = 0;
    float block_w = 28.0f;
    float block_h = 18.0f;
    float interval = 4.0f;
    float offset_x = getBlockOffsetX(BLOCK_COLS, block_w, interval, DXE_WINDOW_WIDTH);
    float offset_y = 40.0f;

    int blue_pattern[BLOCK_ROWS][BLOCK_COLS] = { 0 };
    for (int i = 0; i < BLOCK_ROWS; ++i) {
        for (int j = 0; j < BLOCK_COLS; ++j) {
            blue_pattern[i][j] = 0;
            if ((i + j) % 7 == 0 || (i % 3 == 0 && j % 5 == 0) || (i == 2 && (j == 3 || j == 13)) || (i == 6 && j % 4 == 1)) {
                blue_pattern[i][j] = 1;
            }
        }
    }

    for (int i = 0; i < BLOCK_ROWS; ++i) {
        for (int j = 0; j < BLOCK_COLS; ++j) {
            float x = offset_x + j * (block_w + interval);
            float y = offset_y + i * (block_h + interval);

            if (i == 0) {
                blocks[idx] = new HardBlock(x, y, block_w, block_h);
            }
            else if (blue_pattern[i][j]) {
                blocks[idx] = new ItemBlock(x, y, block_w, block_h);
            }
            else {
                blocks[idx] = new Block(x, y, block_w, block_h);
            }
            ++idx;
        }
    }
    ballCount = 1;
    balls[0].Init(DXE_WINDOW_WIDTH / 2, DXE_WINDOW_HEIGHT - 80);
    balls[0].SetSpeed(440.0f, -360.0f);
    for (int i = 1; i < MAX_BALLS; ++i) {
        balls[i].Init(-100, -100);
        balls[i].SetSpeed(440.0f, -360.0f);
    }
    bar.Init(DXE_WINDOW_WIDTH / 2 - 40, DXE_WINDOW_HEIGHT - 40);
}

// -------------------- 毎フレーム --------------------
void gameMain(float delta_time) {
    // --- タイトルシーン ---
    if (scene == SceneType::Title) {
        if (CheckHitKey(KEY_INPUT_RETURN)) {
            gameStart();
            scene = SceneType::Game;
        }
        // 描画
        DrawBox(0, 0, DXE_WINDOW_WIDTH, DXE_WINDOW_HEIGHT, GetColor(0, 0, 32), TRUE);
        DrawFormatString(static_cast<int>(DXE_WINDOW_WIDTH / 2 - 100), 220, GetColor(255, 255, 255), "ブロック崩し");
        DrawFormatString(static_cast<int>(DXE_WINDOW_WIDTH / 2 - 140), 260, GetColor(255, 255, 255), "Enterキーでスタート");
        dxe::DrawFpsIndicator({ 10, DXE_WINDOW_HEIGHT - 10 });
        return;
    }

    // --- ゲーム本編シーン ---
    if (scene == SceneType::Game) {
        bar.Update(delta_time);

        for (int n = 0; n < ballCount; ++n) {
            balls[n].Update(delta_time);

            // ボールとバー
            float bx = balls[n].GetX(), by = balls[n].GetY(), br = balls[n].GetR();
            float px = bar.GetX(), py = bar.GetY(), pw = bar.GetW(), ph = bar.GetH();
            if (bx + br > px && bx - br < px + pw && by + br > py && by - br < py + ph && by < py + ph) {
                // 反射角度に変化をつける
                float barCenter = px + pw / 2.0f;
                float hitPos = (bx - barCenter) / (pw / 2.0f);  // -1.0(左端)～0(中央)～+1.0(右端)
                float speed = sqrtf(balls[n].GetVX() * balls[n].GetVX() + balls[n].GetVY() * balls[n].GetVY());
                float angle = hitPos * (3.14159f / 3.0f); // ±60度
                float vx = speed * sinf(angle);
                float vy = -fabsf(speed * cosf(angle));    // 必ず上向き
                balls[n].SetSpeed(vx, vy);
                balls[n].Update(0.01f);
            }

            // ボールとブロック
            for (int i = 0; i < MAX_BLOCKS; ++i) {
                if (blocks[i] && blocks[i]->IsAlive() && blocks[i]->IsHit(bx, by, br)) {
                    if (dynamic_cast<ItemBlock*>(blocks[i]) != nullptr) {
                        if (ballCount < MAX_BALLS) {
                            balls[ballCount].Init(bx, by);
                            balls[ballCount].SetSpeed(440.0f, -360.0f);
                            ballCount++;
                        }
                    }
                    GameManager::Instance().AddScore(10);
                    balls[n].ReflectY();
                    break;
                }
            }

            // 画面外判定
            if (balls[n].GetY() > DXE_WINDOW_HEIGHT) {
                balls[n].Init(-100, -100);
            }
        }

        // 全ボールが画面外に消えたらGameOver
        bool allDead = true;
        for (int n = 0; n < ballCount; ++n) {
            if (balls[n].GetY() > 0 && balls[n].GetY() < DXE_WINDOW_HEIGHT) {
                allDead = false;
                break;
            }
        }
        if (allDead) {
            GameManager::Instance().SetState(GameState::GameOver);
            lastScore = GameManager::Instance().GetScore();
            scene = SceneType::Result;
        }

        // 全ブロック消滅判定
        bool clear = true;
        for (int i = 0; i < MAX_BLOCKS; ++i) {
            if (blocks[i] && blocks[i]->IsAlive()) { clear = false; break; }
        }
        if (clear) {
            GameManager::Instance().SetState(GameState::Clear);
            lastScore = GameManager::Instance().GetScore();
            scene = SceneType::Result;
        }

        // --- ゲーム描画 ---
        DrawBox(0, 0, DXE_WINDOW_WIDTH, DXE_WINDOW_HEIGHT, GetColor(0, 0, 32), TRUE);
        bar.Draw();
        for (int n = 0; n < ballCount; ++n) balls[n].Draw();
        for (int i = 0; i < MAX_BLOCKS; ++i) if (blocks[i]) blocks[i]->Draw();
        DrawFormatString(10, 10, -1, "Score: %d", GameManager::Instance().GetScore());
        dxe::DrawFpsIndicator({ 10, DXE_WINDOW_HEIGHT - 10 });
        return;
    }

    // --- リザルトシーン ---
    if (scene == SceneType::Result) {
        if (CheckHitKey(KEY_INPUT_RETURN)) {
            gameEnd();
            scene = SceneType::Title;
        }
        DrawBox(0, 0, DXE_WINDOW_WIDTH, DXE_WINDOW_HEIGHT, GetColor(0, 0, 32), TRUE);
        DrawFormatString(static_cast<int>(DXE_WINDOW_WIDTH / 2 - 100), 220, GetColor(255, 255, 0), "リザルト");
        DrawFormatString(static_cast<int>(DXE_WINDOW_WIDTH / 2 - 120), 260, GetColor(255, 255, 0), "SCORE: %d", lastScore);
        DrawFormatString(static_cast<int>(DXE_WINDOW_WIDTH / 2 - 140), 320, GetColor(255, 255, 255), "Enterキーでタイトルへ戻る");
        dxe::DrawFpsIndicator({ 10, DXE_WINDOW_HEIGHT - 10 });
        return;
    }
}
