#pragma once
#include "IBossState.h"
#include <memory>


class VitalityBossState : public IBossState
{
public: // -- 公開 メンバ関数 -- //

    // 初期化処理
    void Init(BossEnemy* boss)override;

    // 更新処理
    void Update()override;


private: // -- 非公開 メンバ変数 -- //

    // Behavior Treeの実行速度
    const int32_t kBehaviorRunCountMax = 90;
    int32_t mBehaviorRunCount;

    // ビヘイビアツリー
    //std::unique_ptr<SelectorNode> mRoot;

    // メンバ関数ポインタ
    //std::function<void()> mBehaviorUpdate;


};

