# XD64 VER3.0 ANSI split spacebar 用 親指シフトキーマップ (NICOLA配列)

キーボード側で親指シフト出力するためのキーマップです。  
一部の記号を入力するためJISキーボードとしてふるまいます。  


## 配列
### 英数
 1  2  3  4  5    6  7  8  9  0  -  =  BS  
&nbsp;  q  w  e  r  t    y  u  i  o  p  [  ]  \  
&nbsp;&nbsp;   a  s  d  f  g    h  j  k  l  ;  ' ENTER  
&nbsp;&nbsp;&nbsp;    z  x  c  v  b    n  m  ,  .  /  
### そのまま押した場合
 １ ２ ３ ４ ５   ６ ７ ８ ９ ０ ー ＝ BS  
&nbsp;  。 か た こ さ   ら ち く つ ， 、 ； ￥  
&nbsp;&nbsp;   う し て け せ   は と き い ん BS ENTER  
&nbsp;&nbsp;&nbsp;    ． ひ す ふ へ   め そ ね ほ ・  
## 親指シフト時  ※ 8 9 0 は ANSI配列の括弧に合わせている
 ？ ／ ～ 「 」   ［ ］ ＊ （ ） ＿ ＋  
&nbsp;  ぁ え り ゃ れ   よ に る ま ぇ 「 」 ￥  
&nbsp;&nbsp;   を あ な ゅ も   み お の ょ っ ’  
&nbsp;&nbsp;&nbsp;    ぅ ー ろ や ぃ   ぬ ゆ む わ ぉ  
## クロスシフト時
 ？ ／ ～ 「 」   ［ ］ ＊ （ ） ＿ ＋   
&nbsp;   が だ ご ざ   ぱ ぢ ぐ づ ぴ   「 」 ￥  
&nbsp;&nbsp;   ヴ じ で げ ぜ   ば ど ぎ ぽ .. BS  
&nbsp;&nbsp;&nbsp;    .. び ず ぶ べ   ぷ ぞ ぺ ぼ ゛  
## 小指シフト
 ！ ＠ ＃ ＄ ％   ＾ ＆ ＊ （ ） ＿ ＋   
&nbsp;  Ｑ Ｗ Ｅ Ｒ Ｔ   Ｙ Ｕ Ｉ Ｏ Ｐ ｛ ｝ ｜  
&nbsp;&nbsp;   Ａ Ｓ Ｄ Ｆ Ｇ   Ｈ Ｊ Ｋ Ｌ ： BS  
&nbsp;&nbsp;&nbsp;    Ｚ Ｘ Ｃ Ｖ Ｂ   Ｎ Ｍ ＜ ＞ ？  

## スキャンコードとVKコードの対応

#### Shiftを外すもの
|KEY|ANSI Key|VK|JIS Keytop|
|---|---|---|---|
|SHIFT + JU_2|@|KC_LBRC| @, \` |
|SHIFT + JU_6|^|KC_EQL| ^, ~ |
|SHIFT + JU_SCLN|:| KC_QUOT | :, * |

#### Shiftを足すもの
|KEY|ANSI Key|VK|JIS Keytop|
|---|---|---|---|
|JU_EQL|+|SHIFT + KC_MINS| -, = |
|JU_QUOT|'|SHIFT + KC_7| 7, ' |
|JU_GRV|\`|SHIFT + KC_LBRC| @, \` |

#### Keycode置換 (No Shift)
|KEY|ANSI Key|VK|JIS Keytop|
|---|---|---|---|
|JU_BSLS|\\|KC_INT1| \\, _ |
|SHIFT + JU_BSLS|\| | SHIFT + KC_INT3 | \\, \| |

#### Keycode置換 (Shift)
|KEY|ANSI Key|VK|JIS Keytop|
|---|---|---|---|
|SHIFT + JU_7|&|SHIFT + KC_6| 6, & |
|SHIFT + JU_8|*|SHIFT + KC_QUOT| :, * |
|SHIFT + JU_9|(|SHIFT + KC_8| 8, ( |
|SHIFT + JU_0|)|SHIFT + KC_9| 9, ) |
|SHIFT + JU_MINS|_|SHIFT + KC_INT1| ロ, _ |
|SHIFT + JU_EQL|+|SHIFT + KC_SCLN| ;, + |
|SHIFT + JU_QUOT|"|SHIFT + KC_2| 2, " |
|SHIFT + JU_GRV|~|SHIFT + KC_EQL| ^, ~ |

#### Keyごと置換
|KEY|ANSI Key|VK|JIS Keytop|
|---|---|---|---|
|JU_LBRC|[, {|KC_RBRC| [, { |
|JU_RBRC|[, }|KC_NUHS| ], } |

|KEY|CODE|
|----|----|
|無変換 |KC_MHEN KC_INT5 |
|変換 |KC_HENK KC_INT4 |
|カタカナ ひらがな ローマ字 |KC_KANA KC_INT2|
|半角 全角 漢字 |KC_ZKHK KC_GRAVE|

## 情報元

https://github.com/eswai/qmk_firmware/ を元にXD64に移植しました。
[jtu_custom_keycodes](https://github.com/koktoh/jtu_custom_keycodes/tree/master/default) を参考にして layer DLL 不要としました。
