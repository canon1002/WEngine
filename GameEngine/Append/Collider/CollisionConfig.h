#pragma once

#include<cstdint>


// 種別ID定義
enum class CollisionTypeId :uint32_t {
	kDefault,
	kPlayer,
	kPlayerWeapon,
	kPlayerBullet,
	kEnemy,
	kEnemyWeapon,
	kEnemyBullet,
};