#include <avr/io.h>
#include <util/delay.h>

extern char _binary_main_lua_start;
extern size_t _binary_main_lua_size;

#define F_CPU 16000000UL
#define BAUD 9600

#include <util/setbaud.h>

void uart_init(void){
	UBRR0H = UBRRH_VALUE;
	UBRR0L = UBRRL_VALUE;

#if USE_2X
	UCSR0A |= _BV(U2X0);
#else
	UCSR0A &= ~(_BV(U2X0));
#endif

	UCSR0C = _BV(UCSZ01) | _BV(UCSZ00); /* 8-bit data */
	UCSR0B = _BV(RXEN0) | _BV(TXEN0);   /* Enable RX and TX */
}

void uart_putchar(char c){
	loop_until_bit_is_set(UCSR0A, UDRE0); /* Wait until data register empty. */
	UDR0 = c;
}

char uart_getchar(void){
	loop_until_bit_is_set(UCSR0A, RXC0); /* Wait until data exists. */
	return UDR0;
}

void uart_putchar_stdout(char c, FILE* stream){
	if(c == '\n'){
		uart_putchar_stdout('\r', stream);
	}

	loop_until_bit_is_set(UCSR0A, UDRE0); /* Wait until data register empty. */
	UDR0 = c;
}

char uart_getchar_stdin(FILE* stream){
	loop_until_bit_is_set(UCSR0A, RXC0); /* Wait until data exists. */
	return UDR0;
}

void lua_hold(void){
	//We can't just 'exit' the program here, so instead we wait forever.
	while(1){
		_delay_ms(1000);
	}
}

int main(void){
	uart_init();

	FILE uart_output = FDEV_SETUP_STREAM(uart_putchar_stdout, NULL, _FDEV_SETUP_WRITE);
	FILE uart_input = FDEV_SETUP_STREAM(NULL, uart_getchar_stdin, _FDEV_SETUP_READ);
	stdout = &uart_output;
	stderr = &uart_output;
	stdin = &uart_input;
	
	//Set PORTB for output
	DDRB = 0xFF;

	lua_State* L = luaL_newstate();
	luaopen_base(L);
	luaopen_debug(L);
	luaopen_math(L);
	luaopen_string(L);
	luaopen_table(L);
	luaopen_utf8(L);

	int status = luaL_loadbuffer(L, &_binary_main_lua_start, _binary_main_lua_size, "main.lua");
	if(status){
		printf("Couldn't load file: %s\n", lua_tostring(L, -1));
		lua_hold();
	}

    status = lua_pcall(L, 0, LUA_MULTRET, 0);
	if(status){
		printf("Failed to run script: %s\n", lua_tostring(L, -1));
		lua_hold();
	}
	
	while(1){
		//Set PORTB.6 to HIGH
		PORTB = 0x20;

		_delay_ms(1000);

		PORTB = 0x00;

		_delay_ms(1000);
	}

	return 1;
}
