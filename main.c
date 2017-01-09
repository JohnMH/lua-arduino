#include <avr/io.h>
#include <util/delay.h>

//#define F_CPU 16000000UL
#define BAUD 9600

#include <util/setbaud.h>

FILE* stdout_orig;
FILE* stdin_orig;

uint8_t origR0H;
uint8_t origR0L;
uint8_t origR0C;
uint8_t origR0B;

void uart_init(void){
	origR0H = UBRR0H;
	origR0L = UBRR0L;

	UBRR0H = UBRRH_VALUE;
	UBRR0L = UBRRL_VALUE;

#if USE_2X
	UCSR0A |= _BV(U2X0);
#else
	UCSR0A &= ~(_BV(U2X0));
#endif

	origR0C = UCSR0C;
	origR0B = UCSR0B;

	UCSR0C = _BV(UCSZ01) | _BV(UCSZ00); /* 8-bit data */
	UCSR0B = _BV(RXEN0) | _BV(TXEN0);   /* Enable RX and TX */

	stdout_orig = stdout;
	stdin_orig = stdin;
}

void uart_close(void){
	UBRR0H = origR0H;
	UBRR0L = origR0L;

	UCSR0C = origR0C;
	UCSR0B = origR0B;

	stdout = stdout_orig;
	stdin = stdin_orig;
}

void uart_putchar(char c){
	//loop_until_bit_is_set(UCSR0A, TXC0); /* Wait until data register empty. */
	while(!(UCSR0A & (_BV(UDRE0))));
	UDR0 = c;
}

char uart_getchar(void){
	//loop_until_bit_is_set(UCSR0A, RXC0); /* Wait until data exists. */
	while(!(UCSR0A & (_BV(RXC0))));
	return UDR0;
}

void uart_putchar_stdout(char c, FILE* stream){
	if(c == '\n'){
		uart_putchar_stdout('\r', stream);
	}

	loop_until_bit_is_set(UCSR0A, TXC0); /* Wait until data register empty. */
	UDR0 = c;
}

char uart_getchar_stdin(FILE* stream){
	loop_until_bit_is_set(UCSR0A, RXC0); /* Wait until data exists. */
	return UDR0;
}

void lua_hold(lua_State* L){
	lua_close(L);

	puts("Closing serial");

	uart_close();

	//We can't just 'exit' the program here, so instead we wait forever.
	while(1){
		_delay_ms(1000);
	}
}

int main(void){
	DDRB = 0xFF;

	//uart_init();

	//FILE uart_output = FDEV_SETUP_STREAM(uart_putchar_stdout, NULL, _FDEV_SETUP_WRITE);
	//FILE uart_input = FDEV_SETUP_STREAM(NULL, uart_getchar_stdin, _FDEV_SETUP_READ);

	//stdout = &uart_output;
	//stderr = &uart_output;
	//stdin = &uart_input;

	//puts("Test");

	while(1){
		PORTB = 0xFF;

		//puts("Testing");

		_delay_ms(1000);

		PORTB = 0x00;

		_delay_ms(1000);
	}

	//Set PORTB for output
	//DDRB = 0xFF;

	lua_State* L = luaL_newstate();
	luaopen_base(L);
	luaopen_debug(L);
	luaopen_math(L);
	luaopen_string(L);
	luaopen_table(L);
	luaopen_utf8(L);

	int status = luaL_loadbuffer(L, "print(\"Hello, world.\")", 22, "main.lua");
	if(status){
		printf("Couldn't load file: %s\n", lua_tostring(L, -1));
		lua_hold(L);
	}

	status = lua_pcall(L, 0, LUA_MULTRET, 0);
	if(status){
		printf("Failed to run script: %s\n", lua_tostring(L, -1));
		lua_hold(L);
	}

	return 0;
}
