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

## RTC_sample

MSX0に内蔵されているRTC(BM8563)をIoT BASICから読み書きするサンプルコードです。

## WebAPI_sample

MSX0単体で、WebAPIにアクセスし、UTF8エンコードのJSONを取得して必要なノードの値だけを取り出し、Shift_JISコードに変換して表示するサンプルコードです。

## YM825_sample

ヤマハのFM音源チップYM825でドレミファソを繰り返し演奏させるサンプルです。
YM825はSPI通信でデータを送信しますが、MSX0は現状SPI通信はサポートしていないため、ArduinoでI2Cスレーブとして受け付けた内容を、SPI通信としてYMF825に送るようにしています。そのため、Arduinoが必要になります。

## serbinsv

USB接続したPCからMSX0へシリアルコンソール経由でファイルを転送するプログラム群です。

## ROMLOAD

BASICモードで、ディスクに格納された16KBのROMイメージをRAMにロードし、ROMカートリッジとして起動させるためのCプログラムです。

## ROMLD32

BASICモードで、ディスクに格納された32KBのROMイメージをRAMにロードし、ROMカートリッジとして起動させるためのCプログラムです。

## I2C_to_UART

MSX0からI2C通信で送信した内容をUARTに変換し外部デバイスに送信させるためのプログラム群です。

## UART_to_RemoteConsole

外部からUARTで受信した文字列をWiFi経由でリモートコンソールに送り、リモートコンソールからの応答をUARTに返信するためのプログラム（WeMos D1用のArduinoスケッチ）です。

クライアント側端末はポケコン(PB-1000)のBASICプログラムを掲載しています。

## VGMplay_I2C

MSX0からVGMデータファイルの内容をI2CでRaspberry Pi Picoに送信し、Raspberry Pi PicoがYM2151にVGMデータを送って演奏させるプログラムです。

## LTHPA

MSX IoT BASICのUART通信、I2C通信機能を利用して、GPS/温湿度センサー/気圧センサーから得た値をCSV形式でファイルに記録していくBASICプログラムです。

プログラムのファイル名(LTHPA.bas)は、Location(位置情報)/Temperature(温度)/Humidity(湿度)/Pressure(気圧)/Altitude(高度)の略称です。なんのひねりもなくてすいません。

