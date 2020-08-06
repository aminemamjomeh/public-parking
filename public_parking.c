#include <avr/io.h>
#define F_CPU 8000000
#include <util/delay.h>
#include <stdlib.h>
#include <avr/interrupt.h>
#include <avr/sfr_defs.h>

#define enable            4
#define registerselection 5

int second =0;
int minute =0;
int hour =0;

char second_string[2];
char minute_string[2];
char hour_string[2];
char capacity_string[2];

int index=0;
int capacity=16;
int arr[16][6] = {{-1,-1,-1,-1,-1,-1},{-1,-1,-1,-1,-1,-1},{-1,-1,-1,-1,-1,-1},{-1,-1,-1,-1,-1,-1},{-1,-1,-1,-1,-1,-1},{-1,-1,-1,-1,-1,-1},{-1,-1,-1,-1,-1,-1},{-1,-1,-1,-1,-1,-1},{-1,-1,-1,-1,-1,-1},{-1,-1,-1,-1,-1,-1},{-1,-1,-1,-1,-1,-1},{-1,-1,-1,-1,-1,-1},{-1,-1,-1,-1,-1,-1},{-1,-1,-1,-1,-1,-1},{-1,-1,-1,-1,-1,-1},{-1,-1,-1,-1,-1,-1}};
int diff [16][3] = {{-1,-1,-1},{-1,-1,-1},{-1,-1,-1},{-1,-1,-1},{-1,-1,-1},{-1,-1,-1},{-1,-1,-1},{-1,-1,-1},{-1,-1,-1},{-1,-1,-1},{-1,-1,-1},{-1,-1,-1},{-1,-1,-1},{-1,-1,-1},{-1,-1,-1},{-1,-1,-1},};
void send_a_command(unsigned char command)
{
	PORTB = command;
	PORTD &= ~ (1<<registerselection);
	PORTD |= 1<<enable;
	_delay_ms(3);
	PORTD &= ~ (1<<enable);
	PORTB = 0xFF;
}

void send_a_character(unsigned char character)
{
	PORTB = character;
	PORTD |= 1<<registerselection;
	PORTD |= 1<<enable;
	_delay_ms(3);
	PORTD &= ~ (1<<enable);
	PORTB = 0xFF;
}
void send_a_string(char *string_of_characters)
{
	while(*string_of_characters > 0)
	{
		send_a_character(*string_of_characters++);
	}
}


int main(){
	int flag =0;
	DDRA = 0x03;
	DDRB = 0xFF;
	DDRD = 0xF0;
	DDRC = 0x00;
	sei();
	TCCR1B |=(1<<CS12)|(1<<CS10)|(1<<WGM12);
	TCNT1 = 0;
	OCR1A = 7812;
	TIMSK |=(1<<OCIE1A);
	GICR |= (1<<INT0) | (1<<INT1);
	MCUCR = 0x0F;
	char second_string[2];
	char minute_string[2];
	char hour_string[2];

	char second_string_alarm[2];
	char minute_string_alarm[2];
	char hour_string_alarm[2];
	

	send_a_command(0x01); //Clear Screen 0x01 = 00000001
	//_delay_ms(50);
	send_a_command(0x38);
	//_delay_ms(50);
	send_a_command(0x0C);
	//_delay_ms(50);
	while(1){
		if(capacity == 0){
			PORTA = 0x02;
		}
		else{
			PORTA = 0x01;
		}
		
		itoa(hour/10,hour_string,10);
		send_a_string (hour_string);
		itoa(hour%10,hour_string,10);
		send_a_string (hour_string);
		send_a_string (":");
		send_a_command(0x80 + 3);
		//_delay_ms(50);
		
		itoa(minute/10,minute_string,10);
		send_a_string (minute_string);
		itoa(minute%10,minute_string,10);
		send_a_string (minute_string);
		send_a_string (":");
		send_a_command(0x80 + 6);
		//_delay_ms(50);

		itoa(second/10,second_string,10);
		send_a_string (second_string);
		itoa(second%10,second_string,10);
		send_a_string (second_string);

		send_a_command(0x80 + 10);
		//_delay_ms(50);
		
		send_a_string ("C");
		send_a_string (":");
		if(capacity<10){
		send_a_command(0x80 + 14);
		send_a_string(" ");
		
		}
		else{
		send_a_command(0x80 + 14);
		}
		//_delay_ms(50);
		itoa(capacity,capacity_string,10);
		send_a_string (capacity_string);
		
		
		
		
	}
	
}

ISR(INT0_vect){
	index = PINC;
	if(capacity != 0 && arr[index][0] ==-1){
		capacity = capacity - 1;
		
		
		arr[index][0] = hour;
		arr[index][1] = minute;
		arr[index][2] = second;
	}
	
}

ISR(INT1_vect){
	char SHOWALSEC [2];
	char SHOWALMIN [2];
	char SHOWALHOU [2];
	char index_string[2];
	
	index = PINC;

	if(capacity != 16 && arr[index][0] != -1)
	capacity = capacity + 1;
	
	itoa(index,index_string,10);
	if(arr[index][0] != -1){
		arr[index][3] = hour;
		arr[index][4] = minute;
		arr[index][5] = second;
	}
	
	differenceBetweenTimePeriod();
	
	if(arr[index][0] != -1){
	send_a_command(0x80 + 0x40 + 0);
	
	send_a_string ("TIME:");
	
	send_a_string(index_string);
	send_a_string(" ");
	send_a_command(0x80 + 0x40 + 8);
	
	itoa(diff[index][0]/10,SHOWALHOU,10);
	send_a_string(SHOWALHOU);
	itoa(diff[index][0]%10,SHOWALHOU,10);
	send_a_string(SHOWALHOU);
	send_a_command(0x80 + 0x40 +9);
	send_a_string (":");
	send_a_command(0x80 + 0x40 +10);

	itoa(diff[index][1]/10,SHOWALMIN,10);
	send_a_string(SHOWALMIN);
	itoa(diff[index][1]%10,SHOWALMIN,10);
	send_a_string(SHOWALMIN);
	send_a_command(0x80 + 0x40+ 12);
	send_a_string (":");
	send_a_command(0x80 + 0x40+ 14);
	
	itoa(diff[index][2]/10,SHOWALSEC,10);
	send_a_string(SHOWALSEC);
	itoa(diff[index][2]%10,SHOWALSEC,10);
	send_a_string(SHOWALSEC);
	}
	else{
		send_a_command(0x80 + 0x40 + 0);
		
		send_a_string ("INVALID ID      ");
	}
	
	arr[index][0] = -1;
	arr[index][1] = -1;
	arr[index][2] = -1;
	arr[index][3] = -1;
	arr[index][4] = -1;
	arr[index][5] = -1;
}

void differenceBetweenTimePeriod() {
	while (arr[index][5] < arr[index][2]) {
		--arr[index][4];
		arr[index][5] += 60;
	}
	diff[index][2] = arr[index][5] - arr[index][2] ;
	while (arr[index][4] < arr[index][1]) {
		--arr[index][3];
		arr[index][4] += 60;
	}
	diff[index][1] = arr[index][4] - arr[index][1];
	diff[index][0] = arr[index][3] - arr[index][0];
}

ISR(TIMER1_COMPA_vect){
	if (second<60)
	{
		second++;
	}
	if (second==60)
	{
		second=0;
		if (minute<60)
		{
			minute++;
		}
	}
	if (minute==60)
	{
		minute=0;
	}
}


