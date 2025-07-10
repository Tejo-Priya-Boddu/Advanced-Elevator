#include <REG51.H>
#include <stdio.h>

int p, q = 10, r = 10;  // Motor delay params

sbit FIRE = P1^3;
sbit BUZ = P1^5;
sbit Tamper = P1^4;

void buzzer_on();
void fire_detected();
void tamper_detected();
void buzzer_off();

void uart_init() {
    TMOD = 0x20;
    TH1 = 0xFD;
    SCON = 0x50;
    TR1 = 1;
    TI = 1;
}

void uart_send(char ch) {
    SBUF = ch;
    while (!TI);
    TI = 0;
}

void uart_send_string(char *str) {
    while (*str) {
        uart_send(*str++);
    }
}

void delay() {
    int i, j;
    for (i = 0; i < 500; i++) {
        for (j = 0; j < r; j++);
    }
}

int up(int b) {
    int i, j;
    for (i = 1; i <= b; i++) {
        for (j = 0; j <= 10; j++) {
            P2 = (P2 & 0x0F) | 0x10;
            delay();
            P2 = (P2 & 0x0F) | 0x20;
            delay();
            P2 = (P2 & 0x0F) | 0x40;
            delay();
            P2 = (P2 & 0x0F) | 0x80;
            delay();
        }
        P2 = p + i;
    }
    p += b;
    return b;
}

int down(int b) {
    int i, j;
    for (i = 1; i <= b; i++) {
        for (j = 0; j <= q; j++) {
            P2 = (P2 & 0x0F) | 0x80;
            delay();
            P2 = (P2 & 0x0F) | 0x40;
            delay();
            P2 = (P2 & 0x0F) | 0x20;
            delay();
            P2 = (P2 & 0x0F) | 0x10;
            delay();
        }
        P2 = p - i;
    }
    p -= b;
    return b;
}

int control(int a) {
    int difference;
    if (a > p) {
        difference = a - p;
        up(difference);
    }
    if (a < p) {
        difference = p - a;
        down(difference);
    }

     // Send audio trigger message via UART
    switch(a) {
        case 0: uart_send_string("GROUND FLOOR\r\n"); break;
        case 1: uart_send_string("FIRST FLOOR\r\n"); break;
        case 2: uart_send_string("SECOND FLOOR\r\n"); break;
        case 3: uart_send_string("THIRD FLOOR\r\n"); break;
        case 4: uart_send_string("FOURTH FLOOR\r\n"); break;
        case 5: uart_send_string("FIFTH FLOOR\r\n"); break;
        default: uart_send_string("UNKNOWN FLOOR\r\n");
    }

    return a;
}

void buzzer_on() {
    BUZ = 1;
}

void buzzer_off() {
    BUZ = 0;
}

void fire_detected() {
    if (FIRE == 0) {
        buzzer_on();
        uart_send_string("FIRE ALERT\r\n");
        control(0);
        buzzer_off();
        while (1);
    }
}

void tamper_detected() {
    if (Tamper == 0) {
        buzzer_on();
        uart_send_string("TAMPER ALERT\r\n");
        control(0);
        buzzer_off();
        
    }
}

void main(void) {
    int p1;
    p = 0;
    P2 = p;
    uart_init();

    while (1) {
        fire_detected();
        tamper_detected();
        
        if (P0 == 0x01) { p1 = 0; control(0); }  // Ground
        if (P0 == 0x02) { p1 = 1; control(1); }
        if (P0 == 0x04) { p1 = 2; control(2); }
        if (P0 == 0x08) { p1 = 3; control(3); }
        if (P0 == 0x10) { p1 = 4; control(4); }
        if (P0 == 0x20) { p1 = 5; control(5); }
    }
}