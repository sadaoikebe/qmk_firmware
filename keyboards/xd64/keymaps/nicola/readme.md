# XD64 VER3.0 split spacebar 用 親指シフトキーマップ (NICOLA配列)

ソフトウェアをインストールせずにキーボードのみで親指シフトするためのキーマップです。

## PCB XD64 VER3.0 を想定
一般的な60% keyboardの配列で、最下段のAltとSpaceを変更する  
| Alt 1.25U | Space Convex 6.25U |  
↓  
| Alt 1.5U | Space Convex 3U | Space Convex 3U |  
Alt 1.5U のキーはキーがたくさん入っているキートップセットなら含まれている。  Convex 3UはMINILA用のものが使える。2つ必要なので一つは単独で買う必要がありそう。

### 主な機能

|キー|機能|
|----|----|
|LSpace|スペース|
|RSpace|スペース|
|Fn+LSpace|英数字入力へ切り替えと同時に Ctrl+Shift+F13|
|Fn+RSpace|親指シフト入力へ切り替えと同時に Ctrl+Shift+F14|

PC側に以下の設定が必要  
Ctrl+Shift+F13 = IME OFF  
Ctrl+Shift+F14 = IME ON  
※デフォルトのトグルだとモードずれが発生するため実用的でない。この設定だけは必要  

https://github.com/eswai/qmk_firmware/ を元にXD64に移植しました。
