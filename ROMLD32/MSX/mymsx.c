#include <stdlib.h>
#include "mymsx.h"

#asm
WRSLT	EQU	0014H ;MAIN-ROM BIOS CALL WRSLT
CALSLT  EQU 001CH ;MAIN-ROM BIOS CALL CALSLT
ENASLT	EQU	0024H ;MAIN-ROM BIOS CALL ENASLT
CHGET	EQU	009FH ;MAIN-ROM BIOS CALL CHGET
CHPUT	EQU	00A2H ;MAIN-ROM BIOS CALL CHPUT
CLS		EQU 00C3H ;MAIN-ROM BIOS CALL CLS
EXPTBL  EQU 0FCC1H ; EXPTBL 
SYSTEMCALL	EQU	0F37DH ; DISK-BASIC SYSTEM CALL
CREATE_FILE	EQU	016H ;
SET_DMA		EQU	01AH ;
OPEN_FILE	EQU	00FH ;
RND_WRITE	EQU	026H ;
RND_READ    EQU 027H ;
CLOSE_FILE	EQU	010H ;
#endasm

char* input_buffer = NULL;

void msx_soft_reset() {
#asm
    JP      0000H
#endasm
}

void msx_jp(unsigned short int address) {
#asm
    LD      IX, 2
    ADD     IX, SP
    LD      H, (IX+1)
    LD      L, (IX)
    JP      (HL)
    RET
#endasm
}

void msx_chput(char c) {
#asm
    LD      IX, 2
    ADD     IX, SP
    LD      A, (IX)
    CALL    CHPUT
	RET
#endasm    
}

void msx_chput_CRLF() {
    msx_chput(0x0D);
    msx_chput(0x0A);
}

void msx_cls() {
#asm
    CALL    CLS
	RET
#endasm   
}

void msx_print(char[] str) {
    for(int i=0;i<256;i++) {
        if(str[i]==0) {
            return;
        }
        msx_chput(str[i]);
    }
}

void msx_println(char[] str) {
    msx_print(str);
    msx_chput_CRLF();
}

// 1文字入力
char msx_chget() {
#asm
    CALL    CHGET
    LD      H,0x00
    LD      L,A  
	RET
#endasm
}

// １行入力
char* msx_input() {
    msx_chput('?');
    if (input_buffer == NULL) {
        input_buffer = (char*)malloc(sizeof(char)*256);
    }
    for(int i=0;i<256;i++) {
        input_buffer[i]=0;
    }
    int index=0;
    while(index<256) {
        char s = msx_chget();
        if (s==0x0D) {
            msx_chput_CRLF();
            input_buffer[index]=0;
            return input_buffer;
        }
        if (s==0x08 && index>0) {
            index--;           
        }
        input_buffer[index++]=s;
        msx_chput(s);
    }
    return input_buffer;
}

// １行入力
char* msx_input_line_headless() {
    if (input_buffer == NULL) {
        input_buffer = (char*)malloc(sizeof(char)*256);
    }
    for(int i=0;i<256;i++) {
        input_buffer[i]=0;
    }
    int index=0;
    while(index<256) {
        char s = msx_chget();
        if (s==0x0D) {
            input_buffer[index]=0;
            return input_buffer;
        }
        input_buffer[index++]=s;
    }
    return input_buffer;
}

char* msx_input_with_prompt(char[] prompt) {
    msx_print(prompt);
    return msx_input();
    // return prompt;
}


void msx_enaslt(char slot,unsigned short int page) {
#asm
    ; 関数に渡された引数の値を取り出し
    LD      IX, 2
    ADD     IX, SP
    LD      A, (IX+2)
    LD      H, (IX+1)
    LD      L, (IX)
    CALL    ENASLT
	RET
#endasm
}

void msx_wrslt(char slot,unsigned short int address,char value) {
#asm
    ; 関数に渡された引数の値を取り出し
    LD      IX, 2
    ADD     IX, SP
    LD      A, (IX+3)
    LD      H, (IX+2)
    LD      L, (IX+1)
    LD      E, (IX)
    CALL    WRSLT
	RET
#endasm
}

void msx_calslt(unsigned short int slot_IY,unsigned short int address_IX) {
#asm
    ; 関数に渡された引数の値を取り出し
    LD      IX, 2
    ADD     IX, SP
    LD      H, (IX+3)
    LD      L, (IX+2)
    LD      D, (IX+1)
    LD      E, (IX)
    PUSH    HL
    POP     IY
    PUSH    DE
    POP     IX
    CALL    CALSLT
	RET
#endasm
}

// スロットの配置を元に戻す
// RESTORE_SLOT:
// 	POP AF       ; -2 AF(PRIMARY SLOT STATUS)
// 	OUT (0A8H),A
// 	POP AF       ; -1 AF(SECONDARY SLOT STATUS)
// 	LD	(0FFFFH),A
// 	RET
void msx_restore_slot(char pri,char sec) {
	outp(0xA8,pri);
	bpoke(0xFFFF,sec);
}

char* msx_create_fcb(char drive,char* filename) {
	char* fcb = NULL;
	fcb = (char*)malloc(sizeof(char)*37);
	if (fcb == NULL) {
		return fcb;
	}
	for(int i=0;i<36;i++) {
		fcb[i] = 0;
	}

	// ドライブ名をセット
	fcb[0] = drive;

    // ファイル名領域をスペースで埋めておく
    for(int i=0;i<11;i++) {
        fcb[i+1] = 0x20;
    }
	// ファイル名をセット
    int ext = -1;
	for(int i=0;i<12;i++) {
		if(filename[i]==0) {
			break;
		}
        if(filename[i]=='.') {
            ext = 0;
            continue;
        }
        if (ext==-1) {
            fcb[i+1] = filename[i];
        } else {
            fcb[9+ext] = filename[i];
            ext++;
        }
	}
	// 拡張子はROM固定
	// fcb[9]  = 'R';
	// fcb[10] = 'O';
	// fcb[11] = 'M';

	return fcb;
}

char* msx_assign_memory_to_fcb(unsigned short int address,char drive,char* filename) {
	char* fcb = address;

	for(int i=0;i<36;i++) {
		fcb[i] = 0;
	}

	// ドライブ名をセット
	fcb[0] = drive;

    // ファイル名領域をスペースで埋めておく
    for(int i=0;i<11;i++) {
        fcb[i+1] = 0x20;
    }
	// ファイル名をセット
    int ext = -1;
	for(int i=0;i<12;i++) {
		if(filename[i]==0) {
			break;
		}
        if(filename[i]=='.') {
            ext = 0;
            continue;
        }
        if (ext==-1) {
            fcb[i+1] = filename[i];
        } else {
            fcb[9+ext] = filename[i];
            ext++;
        }
	}
	// 拡張子はROM固定
	// fcb[9]  = 'R';
	// fcb[10] = 'O';
	// fcb[11] = 'M';

	return fcb;
}


// CREATE_FILE
// LD   	DE,FCB ; FCBの先頭アドレス
// LD 		C, CREATE_FILE
// CALL 	SYSTEMCALL
int msx_create_file(char* fcb) {
#asm
    ; 関数に渡された引数の値を取り出し
    LD      IX, 2
    ADD     IX, SP
    LD      D, (IX+1)
    LD      E, (IX)
	LD 		C, CREATE_FILE
	CALL 	SYSTEMCALL
	LD		H,0
	LD		L,A
	RET
#endasm
}

// DO_OPEN_FILE:
// 	LD	DE,FCB
// 	LD	C,OPEN_FILE
// 	CALL SYSTEMCALL
int msx_open_file(char* fcb) {
#asm
    ; 関数に渡された引数の値を取り出し
    LD      IX, 2
    ADD     IX, SP
    LD      D, (IX+1)
    LD      E, (IX)
	LD 		C, OPEN_FILE
	CALL SYSTEMCALL
	LD		H,0
	LD		L,A
#endasm
}

char* msx_create_dma_buffer(int record_size) {
	char* buf = NULL;
	buf = (char*)malloc(sizeof(char)*record_size);
	return buf;
}

char* msx_assign_memory_to_dma_buffer(unsigned short int address) {
	char* buf = address;
	return buf;
}

// ;転送先アドレスの設定
// SET_DMA_ADDRESS:
// 	LD	DE,FCB+37   ; DEは転送先(FCB+37＝FCBの直後?)
// 	LD	C,SET_DMA   ; Cはシステムコール番号(01AH)
// 	CALL SYSTEMCALL
void msx_set_dma_address(char* dma_buffer) {
#asm
    ; 関数に渡された引数の値を取り出し
    LD      IX, 2
    ADD     IX, SP
    LD      D, (IX+1)
    LD      E, (IX)
	LD		C, SET_DMA
    CALL    SYSTEMCALL
#endasm
}

// ; 実メモリの内容をDMAに転送
// DMA_TRANSFER:
// 	LD	DE,DMA_ADDRESS
// 	LD	BC,RECORD_SIZE  ; DMA BLOCK SIZE(1024byte)
//  LD  HL,READ_ADDRESS
// 	LDIR                ; LDIRは(HL)の内容を(DE)にBCバイト分転送する
//                        ; BCを1減らし、HLとDEは1増やしながらループ
//                        ; BCが0になったら終了
void msx_ldir_mem2dma(char* dma_buffer,unsigned short int record_size,unsigned short int read_address) {
#asm
    ; 関数に渡された引数の値を取り出し
    LD      IX, 2
    ADD     IX, SP
	LD		D, (IX+5)
	LD		E, (IX+4)
	LD		B, (IX+3)
	LD		C, (IX+2)
    LD      H, (IX+1)
    LD      L, (IX)
	LDIR
#endasm
}

// DMAの内容を実メモリに転送
//  LD  HL,DMA_ADDRESS
// 	LD	DE,WRITE_ADDRESS
// 	LD	BC,RECORD_SIZE ; DMA BLOCK SIZE(1024byte)
// 	LDIR               ; LDIRは(HL)の内容を(DE)にBCバイト分転送する
//                      ; BCを1減らし、HLとDEは1増やしながらループ
//                        ; BCが0になったら終了
void msx_ldir_dma2mem(char* dma_buffer,unsigned short int record_size,unsigned short int read_address) {
#asm
    ; 関数に渡された引数の値を取り出し
    LD      IX, 2
    ADD     IX, SP
	LD		H, (IX+5)
	LD		L, (IX+4)
	LD		B, (IX+3)
	LD		C, (IX+2)
    LD      D, (IX+1)
    LD      E, (IX)
	LDIR
#endasm
}


// ; ファイルへ書き込む
// DO_SEQ_WRITE:
// 	PUSH HL         ; +3 HL(READ ADDRESS) 読み出しアドレスを退避
// 	LD	HL,1        ; 書き込むレコード数(1)
// 	LD	DE,FCB      ; FCBの先頭アドレス
// 	LD	C,RND_WRITE ; ランダムブロックアクセス書き込み(その2)
// 	CALL SYSTEMCALL ; EXECUTE SYSTEM CALL RND_WRITE
// 	OR A                ; 01H（失敗）だと01 OR 01で01になる(NZノンゼロ)
// 	JR NZ,DO_CLOSE_FILE ; 失敗したときはファイルを閉じる
int msx_file_write_rnd(char* fcb) {
#asm
    ; 関数に渡された引数の値を取り出し
    LD      IX, 2
    ADD     IX, SP
    LD      D, (IX+1)
    LD      E, (IX)
	LD		HL,01H      ; 書き込むレコード数(1)
	LD		C,RND_WRITE ; ランダムブロックアクセス書き込み(その2)
	CALL 	SYSTEMCALL ; EXECUTE SYSTEM CALL RND_WRITE
	LD		H,0
	LD		L,A
#endasm
}

// ランダムブロック読み出し
// DE	オープンされたFCBの先頭アドレス
// FCBのレコードサイズ	読み出すレコードサイズ
// FCBのランダムレコード	読み出しを開始するレコード
// HL	読み出すレコード数
int msx_file_read_rnd(char* fcb) {
#asm
    ; 関数に渡された引数の値を取り出し
    LD      IX, 2
    ADD     IX, SP
    LD      D, (IX+1)
    LD      E, (IX)
	LD		HL,01H     ; 書き込むレコード数(1)
	LD		C,RND_READ ; ランダムブロックアクセス書き込み(その2)
	CALL 	SYSTEMCALL ; EXECUTE SYSTEM CALL RND_WRITE
	LD		H,0
	LD		L,A
#endasm
}

// ファイルをクローズ		
// LD	DE,FCB       ; DEはFCBアドレス
// LD	C,CLOSE_FILE ; Cはシステムコール番号(010H)
// CALL SYSTEMCALL
int msx_file_close(char* fcb) {
#asm
    ; 関数に渡された引数の値を取り出し
    LD      IX, 2
    ADD     IX, SP
    LD      D, (IX+1)
    LD      E, (IX)
	LD		C,CLOSE_FILE
	CALL 	SYSTEMCALL ; EXECUTE SYSTEM CALL RND_WRITE
	LD		H,0
	LD		L,A
	RET
#endasm
}


