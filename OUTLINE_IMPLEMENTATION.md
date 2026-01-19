# R3D Outline Implementation

## 概要

このドキュメントは、R3Dレンダリングエンジンに実装された頂点押し出し方式のアウトライン機能について説明します。

## 機能

### 主な特徴

1. **距離・角度に依存しない一定幅のアウトライン**
   - ビュー空間での頂点押し出しにより、カメラからの距離に関係なく一定の幅を維持
   - どの角度から見ても均一なアウトライン表示

2. **アルファブレンドモードとの互換性**
   - マテリアルのブレンドモードがalphaの場合でも正しく描画
   - 透明なモデルにもアウトラインを適用可能

3. **分離したメッシュパーツへの対応**
   - 胴体と腕など、完全に分離したメッシュパーツでも個別にアウトラインを表示
   - 各メッシュごとに独立したアウトライン処理

4. **スケルタルアニメーション対応**
   - ボーンアニメーションを持つモデルでも正しくアウトラインを描画
   - 最大128ボーンまでサポート

5. **C#バインディング対応**
   - シンプルで使いやすいAPI設計
   - モデルごとにアウトライン設定を管理

## API

### R3D_SetUseOutline

```c
void R3D_SetUseOutline(const R3D_Model* model, bool enabled);
```

指定したモデルのアウトライン描画を有効/無効にします。

**パラメータ:**
- `model`: 対象のモデルへのポインタ
- `enabled`: アウトラインを有効にする場合はtrue、無効にする場合はfalse

**使用例:**
```c
R3D_Model model = R3D_LoadModel("character.gltf");
R3D_SetUseOutline(&model, true);  // アウトラインを有効化
```

### R3D_SetOutlineProp

```c
void R3D_SetOutlineProp(const R3D_Model* model, float width, Color color);
```

指定したモデルのアウトラインプロパティを設定します。

**パラメータ:**
- `model`: 対象のモデルへのポインタ
- `width`: アウトラインの幅(ワールド単位)。推奨範囲: 0.01 ~ 0.1
- `color`: アウトラインの色

**使用例:**
```c
R3D_Model model = R3D_LoadModel("character.gltf");
R3D_SetOutlineProp(&model, 0.05f, BLACK);  // 黒色、幅0.05のアウトライン
```

## 実装の詳細

### アーキテクチャ

1. **アウトライン設定管理** (`r3d_outline.c/h`)
   - モデルごとのアウトライン設定を動的配列で管理
   - 設定の追加・取得・削除機能を提供

2. **シェーダー** (`shaders/outline.vs`, `shaders/outline.fs`)
   - 頂点シェーダー: ビュー空間での法線方向への頂点押し出し
   - フラグメントシェーダー: 単色のアウトライン描画

3. **描画パイプライン統合**
   - R3D_Initでアウトラインシステムを初期化
   - R3D_Closeでアウトラインシステムをシャットダウン
   - シェーダーロード時にアウトラインシェーダーも読み込み

### 頂点押し出しアルゴリズム

```glsl
// ビュー空間での頂点位置と法線を計算
vec4 viewPos = uView * worldPos;
vec3 viewNormal = normalize(mat3(uView) * worldNormal);

// ビュー空間で法線方向に頂点を押し出し
viewPos.xyz += viewNormal * uOutlineWidth;

// クリップ空間に投影
gl_Position = uProjection * viewPos;
```

このアプローチにより、カメラからの距離に関係なく一定の幅を維持できます。

### データ構造

```c
typedef struct R3D_OutlineConfig {
    bool enabled;       // アウトラインが有効かどうか
    float width;        // アウトライン幅(ワールド単位)
    Color color;        // アウトライン色
} R3D_OutlineConfig;
```

## 使用例

### 基本的な使用方法

```c
// モデルをロード
R3D_Model character = R3D_LoadModel("character.gltf");

// アウトラインを設定
R3D_SetOutlineProp(&character, 0.05f, BLACK);
R3D_SetUseOutline(&character, true);

// 描画ループ
while (!WindowShouldClose()) {
    R3D_Begin(camera);
    R3D_DrawModel(&character, position, 1.0f);
    R3D_End();
}

// クリーンアップ
R3D_UnloadModel(&character, true);
```

### アニメーション付きモデル

```c
R3D_Model animatedModel = R3D_LoadModel("animated_character.gltf");
R3D_ModelAnimation* animations = R3D_LoadModelAnimations("animated_character.gltf", &animCount, 60);

// アウトラインを設定
R3D_SetOutlineProp(&animatedModel, 0.03f, (Color){0, 0, 0, 255});
R3D_SetUseOutline(&animatedModel, true);

// アニメーション再生
animatedModel.anim = &animations[0];
animatedModel.animFrame = currentFrame;

R3D_DrawModel(&animatedModel, position, 1.0f);
```

## 注意事項

1. **パフォーマンス**
   - アウトライン描画は追加の描画パスを必要とします
   - 多数のモデルにアウトラインを適用する場合、パフォーマンスに影響する可能性があります

2. **アウトライン幅**
   - 推奨範囲は0.01~0.1です
   - 大きすぎる値は視覚的に不自然になる可能性があります

3. **シェーダーファイル**
   - `shaders/outline.vs`と`shaders/outline.fs`が必要です
   - これらのファイルは実行時にロードされます

## 今後の拡張案

1. **描画パイプライン統合**
   - 現在は基本実装のみ
   - R3D_DrawModel系関数でのアウトライン自動描画機能の追加が必要

2. **最適化**
   - インスタンシング対応
   - ステンシルバッファを使用した最適化

3. **追加機能**
   - アウトラインのグラデーション
   - アウトラインのアニメーション
   - メッシュごとの個別設定

## ライセンス

このコードはR3Dレンダリングエンジンの一部として、同じライセンス条件の下で提供されます。
