#include <wiringPi.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>

#define CLK 1
#define DIO 4

#define CMD_AUTOINCREASE 0x40
#define CMD_STARTADDRESS 0xC0
#define CMD_BRIGHTNESS 0x88
#define COLON_MASK 0x80

void delay(unsigned int time){
	int intime = time;
	while(intime--){
		for(int i=255;i>0;i--){ }
	}
}

void writeData(unsigned char data){
	unsigned char indata = data;
	for(int i=0;i<8;i++){
		digitalWrite(CLK,LOW);
		delay(1);
		if((indata & 0x01) == 0x01){
			digitalWrite(DIO,HIGH);
			delay(1);
		}else{
			digitalWrite(DIO,LOW);
			delay(1);
		}
		//printf("0x%02x\t",indata);
		indata = (indata >> 1);
		digitalWrite(CLK,HIGH);
		delay(1);
	}
	//printf("\n");
	digitalWrite(CLK,LOW);
	delay(1);
	digitalWrite(CLK,HIGH);
	delay(1);
	digitalWrite(CLK,LOW);
	delay(1);
}

void startDisp(){
	digitalWrite(CLK,HIGH);
	delay(1);
	digitalWrite(DIO,HIGH);
	delay(1);
	digitalWrite(DIO,LOW);
	delay(1);
	digitalWrite(CLK,LOW);
	delay(1);
}

void stopDisp(){
	digitalWrite(CLK,LOW);
	delay(1);
	digitalWrite(DIO,LOW);
	delay(1);
	digitalWrite(CLK,HIGH);
	delay(1);
	digitalWrite(DIO,HIGH);
	delay(1);
}

void disp(){
	startDisp();writeData(CMD_AUTOINCREASE);stopDisp();
	startDisp();writeData(CMD_STARTADDRESS);
	writeData(0x7f);
	writeData(0x7f);
	writeData(0x7f);
	writeData(0x7f);
	stopDisp();
	startDisp();writeData(CMD_BRIGHTNESS);stopDisp();
}

void disparray(const uint8_t* arr){
	startDisp();writeData(CMD_AUTOINCREASE);stopDisp();
	startDisp();writeData(CMD_STARTADDRESS);
	writeData(arr[0]);
	writeData(arr[1]);
	writeData(arr[2]);
	writeData(arr[3]);
	stopDisp();
	startDisp();writeData(CMD_BRIGHTNESS);stopDisp();
}

void decoder(const char* content, uint8_t* dest) {
    const uint8_t tableB[]={0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,0x7f,0x6f,0x77,0x7c,0x39,0x5e,0x79,0x71};
    char colon_flag = FALSE;
    int idx = 0;
    for (size_t i = 0; i < strlen(content); i++) {
        char c = content[i];
        if ('0' <= c && c <= '9') {
            dest[idx] = tableB[c - '0'];
            idx++;
        } 
        else if ('a' <= c && c <= 'f') {
            dest[idx] = tableB[10 + c - 'a'];
            idx++;
        }
        else if ('A' <= c && c <= 'F') {
            dest[idx] = tableB[10 + c - 'A'];
            idx++;
        }
        else if (c == ':') {
            colon_flag = TRUE;
        }
    }
    if (colon_flag) {
        for (int k = 0; k < 4; k++) {
            dest[k] |= COLON_MASK;
        }
    }
}


int main() {

    char buffer[100];
    uint8_t display_buf[4];
	wiringPiSetup();
	
	pinMode(CLK,OUTPUT);
	pinMode(DIO,OUTPUT);
	digitalWrite(CLK,LOW);
	digitalWrite(DIO,LOW);

	//disp();
    do {
        char* res = fgets(buffer, 100, stdin);
        if (res == NULL || strcmp(buffer, "quit\n") == 0) {
            pinMode(CLK,INPUT);
	        pinMode(DIO,INPUT);
            break;
        }
        decoder(buffer, display_buf);
        disparray(display_buf);
    } while(TRUE);
	return 0;
}
