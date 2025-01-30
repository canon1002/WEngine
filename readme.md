# 「Gladiator」 作品説明

### ゲーム概要
「Gladiator」は、3Dアクションゲームです。プレイヤーは剣を持ち、目の前に現れた敵と1vs1の戦いをし、勝つことを目的としたゲームです。

### ゲームの特色
- **敵の行動処理** : [BehaviorTree]を活かしたランダム性を極力排除した敵AI
- **アニメーション** : モーションブレンドを利用し、プレイヤーや敵が滑らかに動きます

# WEngine 詳細説明

## 概要
WEngineは、DirectX12のフレームワークを利用した自作ゲームエンジンです。
制作期間としては12/14日現在で13ヶ月程度です。

## 特徴
主な機能としては
- モーションブレンドなどアニメーションの操作を行える
- フレームレートクラスを用いたゲーム内速度の変更ができる

などがあり、アクションや演出を作りやすくなるようにしています

## 使用技術
- **プログラミング言語**: C++
- **ツール**: Visual Studio, Git
- **外部ライブラリ** DirectXTex,ImGui,ImNodes,assimp,json


## 作者
- 名前: 渡辺雄大
- 学校: 日本工学院専門学校
- メール: your.email@example.com
-  GitHubURL [WEngine & Gladiator](https://github.com/canon1002/WEngine)

## 外部リンク
[BehaviorTree]:(https://github.com/canon1002/WEngine/blob/master/App/AI/BehaviorTree/IBehavior.h)
