#define __DELAY_BACKWARD_COMPATIBLE__
#include <avr/io.h>
#include <util/delay.h>
#include <string.h>
#include <stdlib.h>
#include "usart.h"

#define INPUT 0
#define OUTPUT 1
#define N 6

#define F_FACE PC2
#define R_FACE PC3
#define U_FACE PC4
#define B_FACE PC5
#define L_FACE PC6
#define D_FACE PC7

#define FRU_DIR PC0
#define BLD_DIR PC1

#define F_ENB PA0
#define R_ENB PA1
#define U_ENB PA2
#define B_ENB PA3
#define L_ENB PA4
#define D_ENB PA5

#define BUTTON PB2
#define LED PD7

#define DIR_PORT PORTC
#define STEP_PORT PORTC
#define ENB_PORT PORTA
#define BUTTON_PORT PORTB
#define LED_PORT PORTD

#define DIR_DDR DDRC
#define STEP_DDR DDRC
#define ENB_DDR DDRA
#define BUTTON_DDR DDRB
#define LED_DDR DDRD

#define BUTTON_PIN PINB

#define digitalWrite(port, pin, val) ((val == 0) ? (port &= ~(1 << pin)) : (port |= (1 << pin)))
#define digitalRead(port, pin) ((port & (1 << pin)))
#define pinMode(port, pin, mode) (digitalWrite(port, pin, mode))
#define pullUp(port, pin) (digitalWrite(port, pin, 1))

const int step_pins[N] = {F_FACE, R_FACE, U_FACE, B_FACE, L_FACE, D_FACE};
const int enb_pins[N] = {F_ENB, R_ENB, U_ENB, B_ENB, L_ENB, D_ENB};

int full_map(double input, double in_min, double in_max, double out_min, double out_max) {

  double slope = (double) (out_max - out_min) / (double) (in_max - in_min);
  double output = (double) out_min + slope * (double) (input - in_min);
  return (int)output;

}

void dissable_all() {
  for (int i = 0; i < N; i++)
    digitalWrite(ENB_PORT, enb_pins[i], 1);
}

void move_face(int face, int steps, int dir, int us_delay) {

  digitalWrite(ENB_PORT, enb_pins[face], 0);

  if (face < 3) {
    digitalWrite(DIR_PORT, FRU_DIR, dir);
  } else {
    digitalWrite(DIR_PORT, BLD_DIR, dir);
  }

  for (int i = 0; i < steps; i++) {
    digitalWrite(STEP_PORT, step_pins[face], 1);
    _delay_us(us_delay);
    digitalWrite(STEP_PORT, step_pins[face], 0);
    _delay_us(us_delay);
  }

  digitalWrite(ENB_PORT, enb_pins[face], 1);
}

void execute(char* moves) {

  char * move;
  move = strtok(moves," ,.-");

  int move_delay = atoi(move);
  move = strtok(NULL, " ,.-");
  int us_delay = atoi(move);
  move = strtok(NULL, " ,.-");

  while (move != NULL)
  {

    int face = 0;
    int steps = 50;
    int dir = 0;

    switch (move[0]) {
    case 'F':
      face = 0;
      break;
    case 'R':
      face = 1;
      break;
    case 'U':
      face = 2;
      break;
    case 'B':
      face = 3;
      break;
    case 'L':
      face = 4;
      break;
    case 'D':
      face = 5;
      break;
    }

    if (strlen(move) > 1) {
      if (move[1] == '\'') {
        dir = 1;
      } else if (move[1] == '2') {
        steps = 100;
      }
    }

    move_face(face, steps, dir, us_delay);

    move = strtok(NULL, " ,.-");

    _delay_ms(move_delay);
  }

}

void get_solution_and_solve() {

  char buf[256];
  char c;

  while (1) {
    int i = 0;
    c = USART0_receive();
    while (c != '#') {
      buf[i] = c;
      c = USART0_receive();
      i++;
    }
    buf[i] = 0;

    USART0_print(buf);
    USART0_print("\n");
    if (c == '#') {
      USART0_print("Ready!\n");
      while (1) {
        if ((digitalRead(BUTTON_PIN, BUTTON)) == 0) {
          digitalWrite(LED_PORT, LED, 1);
          execute(buf);
          digitalWrite(LED_PORT, LED, 0);
          break;
        }
      }
    }
  }
}

void init_others() {
  pinMode(BUTTON_DDR, BUTTON, INPUT);
  pullUp(BUTTON_PORT, BUTTON);

  pinMode(LED_DDR, LED, OUTPUT);

  digitalWrite(LED_PORT, LED, 0);
}

void init_drivers() {

  pinMode(DIR_DDR, FRU_DIR, OUTPUT);
  pinMode(DIR_DDR, BLD_DIR, OUTPUT);
  for (int i = 0; i < N; i++) {
    pinMode(STEP_DDR, step_pins[i], OUTPUT);
    pinMode(ENB_DDR, enb_pins[i], OUTPUT);
  }

  for (int i = 0; i < N; i++) {
    digitalWrite(STEP_PORT, step_pins[i], 0);
  }
  dissable_all();
}

int main(void) {

  USART0_init();
  init_others();
  init_drivers();

  get_solution_and_solve();

  while (1) ;

  return 0;
}
