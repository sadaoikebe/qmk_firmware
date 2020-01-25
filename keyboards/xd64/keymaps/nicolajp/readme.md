# XD64 VER3.0 ANSI split spacebar 用 親指シフトキーマップ (NICOLA配列)

キーボード側で親指シフト出力するためのキーマップです。  
一部の記号を入力するためJISキーボードとしてふるまいます。  
キーボードライブラリを変更します。  

### ライブラリ

https://github.com/sadaoikebe/Windows-driver-samples/tree/master/input/layout/fe_kbds/jpn/ansi106

kbd106_ansi.dll を C:\Windows\System32 にコピー
kbd106_ansi.reg を import

### キーマップ

スキャンコードとVKコードの対応は以下の通り  

ANSI Layout
![ANSI Layout](ansi_layout.svg "ANSI Layout")

JIS Layout
![JIS Layout](jis_layout.svg "JIS Layout")

JIS Layout SW layer on ANSI Layout HW
![ANSI_106 Layout](ansi_106_layout.svg "ANSI_106 Layout")

| VK | ANSI | JIS |
|----|----|----|
| OEM_MINUS | -, _ | -, = |
| OEM_PLUS | =, + | ;, + |
| OEM_4 | ［, ｛ | ［, ｛ |
| OEM_6 | ］, ｝ | ］, ｝ |
| OEM_5 | \\, \| | \\, \| |
| OEM_102 | \\, \| | \\, _ |
| OEM_1 | ;, : | :, * |
| OEM_7 | ', " | ^, ~ |
| OEM_2 | /, ? | /, ? |
| OEM_3 | \`, ~ | \`, @ |

|KEY|CODE|
|----|----|
|無変換 |KC_MHEN KC_INT5 |
|変換 |KC_HENK KC_INT4 |
|カタカナ ひらがな ローマ字 |KC_KANA KC_INT2|
|半角 全角 漢字 |KC_ZKHK KC_GRAVE|

## 情報元

https://github.com/eswai/qmk_firmware/ を元にXD64に移植しました。
https://bsakatu.net/doc/how-to-input-japanese-more-convinient-with-us-layout/ こちらの知見をもとに kbd106_ansi.dll を作成、ただし英数キーとかな「む」キーなど一部必要性に応じて変更。
