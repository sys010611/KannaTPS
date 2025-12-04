# 3D TPS ゲーム開発プロジェクト

## 基本情報

* ジャンル：3D、TPS
* 開発人数：1人
* 開発環境：Unreal Engine 5、GitHub、Blender

---

## 使用技術

* ビヘイビアツリー AI
* Environment Query System（EQS）
* Level Streaming
* Aim Offset
* Layered Blend Animation
* Root Motion

---
<img width="788" height="444" alt="image" src="https://github.com/user-attachments/assets/1199cb3b-c012-4a54-aeba-bd1e304672fc" />
<img width="763" height="514" alt="image" src="https://github.com/user-attachments/assets/fbd3e9d1-df99-47f9-ab05-c0bc9ff061d7" />
<img width="763" height="429" alt="image" src="https://github.com/user-attachments/assets/1242532a-0bcd-4fd5-98e1-5784073994e9" />

---

## プロジェクト概要

ポストプロセスを用いてピクセル単位のライティングを極端に表現することで、トゥーンシェーディングを実装しました。

Level Streaming を活用し、建物を背景にしたマップの最適化を実施しました。
プレイヤーの移動に合わせて隣接するフロアがロードされ、プレイ中は現在のフロアのみがアクティブとなるように構成し、**平均 FPS を約 30 向上**させました。

敵キャラクターの遮蔽物利用や射撃 AI の実装には、**EQS** と **ビヘイビアツリー**を使用しました。
プレイヤーキャラクターの自然な動きを実現するために、Blend Space、Additive Animation、Aim Offset など、Unreal Engine のアニメーション機能を積極的に活用しています。

また、敵キャラクターのモデリングにおけるテクスチャペインティングや、銃のマガジン・スライド部分のリギングは **Blender** で制作し、Unreal Engine にインポートしました。

---

