[![.github/workflows/DebugBuild.yml](https://github.com/Y04Kato/CitrusJunosEngine/actions/workflows/DebugBuild.yml/badge.svg)](https://github.com/Y04Kato/CitrusJunosEngine/actions/workflows/DebugBuild.yml)
[![ReleaseBuild](https://github.com/Y04Kato/CitrusJunosEngine/actions/workflows/ReleaseBuild.yml/badge.svg)](https://github.com/Y04Kato/CitrusJunosEngine/actions/workflows/ReleaseBuild.yml)

# CitrusJunosEngine
## DirectX12を使用したゲームエンジンです
- 描画パイプラインの構築から実際のゲームシーンまで一通り制作しています

# 工夫点
## オブジェクト配置およびメッシュの同期機能(メッシュシンク)
- エンジンとHoudini間でTCP通信を用いてデータの送受信を行っています
![Screenshot of a comment on a GitHub issue showing an image, added in the Markdown, of an Octocat smiling and raising a tentacle.](Picture/preview1.png)
* [コード(受信)](Engine/base/components/utilities/dataReceipt/DataReceipt.cpp)
* [コード(解析&描画)](Engine/base/components/3d/Receipt3D.cpp)

* [コード(Houdini側ツール)](Tools/HoudiniPlugin/CitrusJunosEditor/CitrusJunosEditor.py)

## パラメータを摘出して操作する機能(パラメータシンク)
- 複数のノードを行き来する必要なく、必要なノードパラメータのみをまとめて操作できます
![Screenshot of a comment on a GitHub issue showing an image, added in the Markdown, of an Octocat smiling and raising a tentacle.](Picture/preview2.png)
* [コード(Houdini側ツール)](Tools/HoudiniPlugin/CitrusJunosEditor/CitrusJunosEditor.py)