# 自作エンジン及び「Gladiator」 作品説明

## プロジェクト概要

### 制作期間
- 自作エンジン :  16ヶ月
- ゲーム : 9ヶ月

### 制作人数
- 1人

**** 

### 制作環境 
- Visual Studio 2022 
- DirectX12

### 動作環境
- **プロセッサ** :	    Intel(R) Core(TM) i7-14700HX   2.10 GHz
- **エディション** :    Windows 11 Pro
- **バージョン** :	    24H2
- **GPU** :            NVIDIA GeForce RTX 4070 Laptop GPU

****

### 制作箇所

- Externalsを除く [App] [GameEngine] フォルダのソースコード 

**以下追記**

    ・GameEngine > Input > [Gamepad.h , Gamepad.cpp] は友人のコードを改良
    
    ・GameEngine > Editor > [NodeEditor] フォルダ内のBehaviorTree関連クラスは、現在改良中のため、BehaviorTreeクラスはAppフォルダ内の旧バージョンのものを使用中。
    
    [Shaders] フォルダの .Hlsl 及び .Hlsli は基本的に授業内容をそのまま実装


*****

### 参考書籍/Webサイト
**Behavior Tree + 敵AI挙動**
- BehaviorTree設計部分
    - [BehaviorTree.CPP 4.6](https://www.behaviortree.dev/docs/Intro)
    - [[UE5] ビヘイビアツリー(Behavior Tree)の使い方](https://historia.co.jp/archives/29055/)
-   敵AI行動原理(参考程度)
    - [スクウェア・エニックスのAI](https://www.borndigital.co.jp/book/9784862466013/)
    - [FINAL FANTASY XV の人工知能 - ゲームAIから見える未来 -](https://www.borndigital.co.jp/book/14172/) 

**※本ゲームではAIの深層・強化学習やモーション自動生成などは未実装**
    
****

## アピールポイント <コード>

### デザインパターンを使用

- **ファクトリメソッド**
    - シーン生成処理

- **ステートパターン**
    - ISceneクラスを基底クラスに持つ各Sceneクラス及びSceneManagerで使用
    - GameScene内の<開始/戦闘/敗北/勝利>といったフェーズの分割で利用

- **ポリモーフィズムの活用**
    - BehaviorTreeの各Nodeクラス(ほぼインターフェースに改良済み)

- **関数ポインタの利用**
    - GameScene内のフェーズ実行処理を関数ポインタにて実行
    - BehaviorTreeのCondition(条件)ノードの条件用の関数を保持するために使用


**** 

## アピールポイント <ゲーム&自作エンジン>

- **敵の行動処理** : [BehaviorTree]を活かしたランダム性を極力排除した敵AI

- **アニメーション** : モーションブレンドを利用し、プレイヤーや敵キャラが滑らかに動きます

- **エンジン機能** : フレームレートクラスを用いたゲーム内速度の変更ができる


****

## 操作説明
- ### コントローラー
    - **移動** 左スティック
    - **カメラ操作** 右スティック
    - **攻撃** Bボタン (コンボ回数 : 3回)
    - **バックステップ** Aボタン
    - **ローリング回避** 左スティック入力中にAボタン
    - **ロックオン** Yボタン

****

## 使用技術/外部ライブラリ
- **プログラミング言語**: C/C++
- **ツール**: Visual Studio, Git
- **外部ライブラリ** DirectXTex,ImGui,ImNodes,assimp,json,magic_enum

****

## 制作者
- 名前: 渡辺雄大
- 学校: 日本工学院専門学校
- メール: your.email@example.com
-  GitHubURL [WEngine & Gladiator](https://github.com/canon1002/WEngine)