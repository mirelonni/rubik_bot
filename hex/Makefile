all: main.hex

main.hex: main.elf
	avr-objcopy  -j .text -j .data -O ihex $^ $@
	avr-size main.elf

main.elf: main.c usart.c
	avr-g++ -mmcu=atmega324p -DF_CPU=16000000 -Os -Wall -o $@ $^

clean:
	rm -rf main.elf main.hex
