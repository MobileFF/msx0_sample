void msx_soft_reset();
void msx_jp(unsigned short int address);

void msx_chput(char c);
void msx_chput_CRLF();
void msx_cls();

void msx_print(char[] str);
void msx_println(char[] str);

// 1文字入力
char msx_chget();

// １行入力（？表示あり）
char* msx_input();
// １行入力（エコーなし）
char* msx_input_line_headless();
// １行入力（プロンプト表示付き）
char* msx_input_with_prompt(char[] prompt);

// ENASLT
void msx_enaslt(char slot,unsigned short int page);

// WRSLT
void msx_wrslt(char slot,unsigned short int address,char value);

// CALSLT(インタースロットコール)
void msx_calslt(unsigned short int slot_IX,unsigned short int address);

// スロットの配置を元に戻す
void msx_restore_slot(char pri,char sec);

// FCB領域作成
char* msx_create_fcb(char drive,char* filename);

// ファイル作成
int msx_create_file(char* fcb);

// ファイルオープン
int msx_open_file(char* fcb);

// DMAバッファの領域作成
char* msx_create_dma_buffer(int record_size);

// 転送先アドレスの設定
void msx_set_dma_address(char* dma_buffer);

// 実メモリの内容をFCBに転送
void msx_ldir_mem2dma(char* dma_buffer,unsigned short int record_size,unsigned short int read_address);

// 実メモリの内容をFCBに転送
void msx_ldir_dma2mem(char* dma_buffer,unsigned short int record_size,unsigned short int write_address);

// ファイルへ書き込む
int msx_file_write_rnd(char* fcb);

// ランダムブロック読み出し
int msx_file_read_rnd(char* fcb);

// ファイルをクローズ		
int msx_file_close(char* fcb);
