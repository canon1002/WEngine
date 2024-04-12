#include "TimedCall.h"

//コンストラクタ
TimeCall::TimeCall(std::function<void()> fire, uint32_t time) {
	//FireAndReset
	fire_ = fire;
	//kFireInterval
	time_ = time;
}

void TimeCall::Update() {
	if (isFinish_) {
		return;
	}
	time_--;
	if (time_ <= 0) {
		isFinish_ = true;
		//コールバック関数呼び出し
		fire_();

	}


}