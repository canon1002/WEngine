#pragma once

#include<cstdint>

// プレイヤー陣営
const uint32_t kCollisionAttributePlayer = 0b1;
// 敵陣営
const uint32_t kCollisionAttributeEnemy = 0b1 << 1;
// プレイヤー陣営の弾
const uint32_t kCollisionAttributePlayerBullet = 0b1 << 2;
// 敵陣営の弾
const uint32_t kCollisionAttributeEnemyBullet = 0b1 << 3;
// 干渉可能な弾
const uint32_t kCollisionAttributeHitBullet = 0b1 << 4;
