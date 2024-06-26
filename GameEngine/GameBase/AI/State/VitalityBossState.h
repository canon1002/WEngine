#pragma once
#include "IBossState.h"

class VitalityBossState : public IBossState
{
public: // -- 公開 メンバ関数 -- //

    // 初期化処理
    void Init()override;

    // 更新処理
    void Update()override;

};

