#include "UIFactory.h"
#include "Externals/magic_enum/magic_enum.hpp"

std::shared_ptr<BaseUI> UIFactory::CreateUI(const std::string& type){

	// 生成したいUIのタイプを文字列で取得して
	// magic_enumで列挙型に変換する
	switch (magic_enum::enum_cast<UIType>(type).value())
	{
	case UIType::Base :

		// 通常UIを生成
		return std::make_shared<BaseUI>();
		break;

	case UIType::Fade:

		// フェードUIを生成
		return std::make_shared<FadeUI>();
		break;

	default:

		// 生成したいUIのタイプが見つからなかった場合は
		// エラーメッセージを表示する
		assert(0 && "UIのタイプが見つかりません");
		return nullptr;

		break;
	}

}
