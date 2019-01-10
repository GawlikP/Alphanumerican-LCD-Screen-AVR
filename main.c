/*
 * alphanumerican.c
 *
 * Created: 06.01.2019 16:37:49
 * Author : mo6ni
 */ 
#define  F_CPU 16000000UL

#define SHIFT_LEFT 0x04
#define SHIFT_RIGHT 0x14
#define COLLISION_CHARACTER 't'
#define UC unsigned char 
#define RW  0x01
#define RS  0x02
#define E 0x04
#define GRAVITY  0.3f
#define JUMP_SPEED 0.9f

#define PLAYER_CHAR 'x'
#define PLAYER_CHAR_HIGH 'X'

#include <avr/io.h>
#include <util/delay.h>

typedef struct{
	char *ch;
	unsigned char length;
}Str;
typedef struct{
	float y;
	unsigned char state;
	float vy;
}Player;


unsigned char Set_lengt(Str str){
	unsigned char l=0;
	for(;;){
		if(str.ch[l]=='\0')break;
		l++;
	}
	return l;
}

void lcd_cmd(unsigned char cmd){
	PORTD = cmd;
	PORTC &=~(RW);
	PORTC &=~(RS);
	PORTC |=(0x04);
	_delay_ms(10);
	PORTC &=~(E);
}

void lcd_data(unsigned char data)
{
	PORTD = data;
	PORTC |=(RW);
	PORTC &=~(RS);
	PORTC |=(E);
	_delay_ms(10);
	PORTC &=~(E);
}
void lcd_init(){
	lcd_cmd(0x38); // 8-bit mode
	// lcd_cmd(0x28) // 4 bit mode 
	lcd_cmd(0x0e); // Blincking cursor
	lcd_cmd(0x06); // INC cursor 
	lcd_cmd(0x01); // Clear screen 
	lcd_cmd(0x80); // First line 
}
void Show_str(Str str){
	unsigned char i =0;
	lcd_cmd(0x80);
	for(;i<str.length;i++){
		if(i==16) lcd_cmd(0xc0);
		if(str.ch[i]=='\n') lcd_cmd(0xc0);
		else lcd_data(str.ch[i]);
	}
}

void Put_char(unsigned char ch, unsigned char x, unsigned char y){
	if(x <0 || x > 18) return;
	if(y == 1)lcd_cmd(0x80);
	if(y == 0)lcd_cmd(0xc0);
	unsigned char i;
	for(i=0;i<x;i++){
		if(i >15) break;
		lcd_cmd(SHIFT_RIGHT);
	}
	lcd_data(ch);
}

unsigned char Check_collisions(unsigned char collisions[5], Player player){
	unsigned char i=0;
	for(;i<5;i++){
		if(collisions[i] == 0 && player.y < 1.0f) return 1;
	}
	return 0;
}

int main(void)
{
	DDRD = 0xff;
		   //7643210
	DDRC = 0b0000111;
	DDRB = 0x02;
	lcd_init();
    /* Replace with your application code */
   // PORTB ^= 0x02;

	

	Player player;
	player.y = 0;
	player.vy = 0;
	unsigned char collisons[5];
	
	char game_state = 'm';
	unsigned char i = 0;


	for(;i<5;i++){
		collisons[i]=i*9+18;
	}
	
	Str Welcome_screen;
	Welcome_screen.ch = "Welcome to game\n Press button to start";
	Welcome_screen.length = Set_lengt(Welcome_screen);
	Str Game_Over;
	Game_Over.ch = "Game Over\n Press to continue";
	Game_Over.length = Set_lengt(Game_Over);
	
	while (1) 
    {
		lcd_cmd(0x01);
		if(!(PINB & (1<<PINB0))){
			if(game_state == 'm'){
				game_state = 'g';	
			}
			if(game_state == 'g'){
				if(player.vy == 0)player.vy += JUMP_SPEED;
			}
			if(game_state == 'o'){
				game_state = 'm';
				for(i=0;i<5;i++){
					collisons[i]=i*9+18;
				}
			}
		}
		if(game_state == 'm'){
			Show_str(Welcome_screen);
		}
		else if(game_state == 'g'){
			if(player.y >0)player.vy -= GRAVITY;
			if(player.y <0){ player.vy =0; player.y =0;}
			player.y += player.vy;
			if(player.y < 0.5f)Put_char(PLAYER_CHAR,0,0);
			else if(player.y > 0.5f && player.y < 1.0f)Put_char(PLAYER_CHAR_HIGH,0,0);
			else if(player.y > 1.0f && player.y < 1.5f)Put_char(PLAYER_CHAR,0,1);
			else if(player.y > 1.5f)Put_char(PLAYER_CHAR_HIGH,0,1);
			//Put_char(PLAYER_CHAR,0,0);
			for(i=0;i<5;i++) {
				Put_char(COLLISION_CHARACTER,collisons[i],0);
				if(Check_collisions(collisons,player) == 1) game_state = 'o';
				if(collisons[i] <-1) collisons[i]=20;
				collisons[i]--;		
			}
			//for(i=0;i<10;i++) collisons[i]--;	
			
		}
		else if(game_state == 'o'){
			Show_str(Game_Over);
			_delay_ms(10);
		}
		
		_delay_ms(1);
		
	}
		
}

