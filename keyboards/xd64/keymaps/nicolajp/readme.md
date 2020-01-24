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
|Fn+LSpace|無変換・英数入力|
|Fn+RSpace|変換・親指シフト入力|

kbd106_ansi.dllが必要

ANSI配列  
OEM_3 1 ... 0 OEM_MINUS OEM_PLUS BACKSPACE  
Q ... P OEM_4 OEM_6 OEM_5,OEM_102
A ... L OEM_1 OEM_7 ENTER
Z ... > OEM_2

JIS配列
JP_ZHTG 1 ... 0 OEM_MINUS OEM_7 OEM_5 BACKSPACE
Q ... P OEM_3 OEM_4 ENTER
A ... L OEM_PLUS OEM_1 OEM_6 ENTER
Z ... > OEM_2 OEM_102

kbd106_ansi.dll map

| VK | ANSI | JIS |
| OEM_MINUS | -, _ | -, = |
| OEM_PLUS | =, + | ;, + |
| OEM_4 | ［, ｛ | ［, ｛ |
| OEM_6 | ］, ｝ | ］, ｝ |
| OEM_5 | backslash, pipe | yen, pipe |
| OEM_102 | backslash, pipe | backslash, _ |
| OEM_1 | ;, : | :, * |
| OEM_7 | ', " | ^, ~ |
| OEM_2 | /, ? | /, ? |


|無変換 |KC_MHEN KC_INT5 |
|変換 |KC_HENK KC_INT4 |
|カタカナ ひらがな ローマ字 |KC_KANA KC_INT2|
|半角 全角 漢字 |KC_ZKHK KC_GRAVE|


https://github.com/eswai/qmk_firmware/ を元にXD64に移植しました。
