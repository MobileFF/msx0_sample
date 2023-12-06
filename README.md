# msx0_sample

MSX0用に作成した主にIoT BASIC用のサンプルプログラムです。
一部のプログラムは外部のArduino/Raspberry Pi pico等と連携するため、BASIC以外のプログラムも含まれています。

## DSKCHG

IoT BASICでディスクイメージ（.DSKファイル）を入れ替えるサンプルプログラムです。
現状のMSX0ではディスクイメージを入れ替えたあとの１回目のディスクアクセスはうまく機能しないため、プログラム内でFILESコマンドを実行しエラーハンドリングした上でプログラムを終了しますので、プログラム終了後は普通にアクセスが可能です。

MSX0自体の起動用の設定ファイル(conf.txt)は書き換えていないと思いますので、再起動するとconf.txtに指定されたディスクイメージがセットされると思います。

## I2C_list

現在接続されているI2Cデバイス（PORT Aと、内部I2C接続されているもの）の一覧を表示します。

## I2C_test

PORT AのI2C通信の疎通確認のために作ったサンプルプログラムです。
I2Cスレーブが必要ですが、Arduinoをスレーブにする前提でArduino用のプログラムも置いてあります。

## JoyStick_to_MSX0

MSX(ATARI9ピン)規格のジョイスティックをMSX0に接続するためのArduinoシールド用プログラムです。
MSX0用のBASICプログラムは単なる動作確認用サンプルなので、なくても大丈夫です。
※シールドは自作する前提です

## WebAPI_sample

MSX0単体で、WebAPIにアクセスし、UTF8エンコードのJSONを取得して必要なノードの値だけを取り出し、Shift_JISコードに変換して表示するサンプルコードです。

## YM825_sample

ヤマハのFM音源チップYM825でドレミファソを繰り返し演奏させるサンプルです。
YM825はSPI通信でデータを送信しますが、MSX0は現状SPI通信はサポートしていないため、ArduinoでI2Cスレーブとして受け付けた内容を、SPI通信としてYMF825に送るようにしています。そのため、Arduinoが必要になります。