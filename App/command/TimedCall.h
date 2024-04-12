#pragma once
#include <functional>
#include <stdint.h>
#include <stdio.h>

class TimeCall {
public:
	//コンストラクタ
	TimeCall(std::function<void()> fire, uint32_t time);


	//更新
	void Update();



	//完了ならtrueを返す
	bool IsFinished() {
		return isFinish_;
	}





private:
	//コールバック
	//呼び出したい関数std::function
	std::function<void()> fire_;


	//残り時間
	int32_t time_;
	//完了フラグ
	bool isFinish_ = false;




};