# MSX0に接続されているI2Cデバイスのスレーブアドレスの一覧を表示するBASICプログラム <!-- omit in toc -->


## 能書き

MSX IoT BASICの`_IOTFIND()`関数を利用して、MSX0に接続されているI2Cデバイス（PORT Aに接続されたものと、内部I2Cで接続されたもの）の一覧を表示するBASICプログラムです。

内部I2Cデバイスに関しては、デバイスの種類も表示し、Faces/Battery Bottomのどちらを使っているかも下記の表に基づき判定して表示します。

内部デバイスの判定は、M5Stack Core2の標準に基づいてスレーブアドレスから判定しています。独自に内部I2Cに同じスレーブアドレスのI2Cスレーブを接続しても、標準のものとして判定しますのでご了承ください。

|スレーブアドレス|デバイスの種類 |内蔵場所|
|------------------------|----------------------|------------|
|0x08| Faces II KEYBOARD or JOYPAD| Faces II |
|0x34|AXP192| MSX0Stack本体|
|0x38|FT6336U|MSX0Stack本体|
|0x51|BM8563|MSX0Stack本体|
|0x68|MPU6886|Battery Bottom II|

## 実行例
![i2c_list_sample2](https://github.com/MobileFF/msx0_sample/assets/147289117/48b70b7d-ce19-407b-a4cd-48685053aa55)
![i2c_list_sample1](https://github.com/MobileFF/msx0_sample/assets/147289117/d5af438d-aa8c-4c3e-80c3-fdd9b7d13365)


## おことわり

上記件に関して、なにぶんにも、中の人は電子工作、電子回路については完全な素人なのでたいしてよくわかっていないことのほうが多いです。

記事の内容をお試しになる場合は、自己責任でお願いいたします。この記事にある通りのことを試してお手持ちの機器を破損しても中の人は一切の責任を負いません。

Arduinoは互換機含め様々入手経路があり、価格も安価なものがたくさんありますが、MSX0については、2023年10月時点で破損すると、基本的には一般販売開始を待つか、次のクラウドファンディングのリワード発送を待たなければいけなくなります。

このことに同意できない方は記事の内容を試されないようにお願いいたします。

以上
