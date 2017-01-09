#include <avr/io.h>
#include <util/delay.h>

#include "avr-uart/uart.h"
#include "avr-uart/uart.c"

void lua_hold(lua_State* L){
	lua_close(L);

	uart0_puts("Closing serial\n\r");

	uart0_flush();

	exit(1);
	//We can't just 'exit' the program here, so instead we wait forever.
	//while(1){
	//	_delay_ms(1000);
	//}
}

int main(void){
	uart0_init(UART_BAUD_SELECT(9600,F_CPU));

	sei();

	FILE mystdout = FDEV_SETUP_STREAM(uart0_putc, NULL, _FDEV_SETUP_WRITE);
	stdout = &mystdout;

	uart0_puts("Test\n\r");

	/*
	while(1){
		PORTB = 0xFF;

		uart0_puts_P("Testing\n\r");

		_delay_ms(1000);

		PORTB = 0x00;

		_delay_ms(1000);
	}
	*/

	lua_State* L = luaL_newstate();
	luaopen_base(L);
	luaopen_debug(L);
	luaopen_math(L);
	luaopen_string(L);
	luaopen_table(L);

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
