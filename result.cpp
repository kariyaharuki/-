#include "ResultScene.h"
#include "../dxe/dxe.h"

void ResultScene::Update(float delta_time) {
    if (CheckHitKey(KEY_INPUT_RETURN)) retry = true;
}
void ResultScene::Draw() const {
    DrawFormatString(320, 220, GetColor(255, 255, 0), "リザルト画面");
    DrawFormatString(280, 260, GetColor(255, 255, 0), "Enterキーでタイトルへ戻る");
}
