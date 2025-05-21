#include <stdio.h>
#include <stdlib.h>
#include "wiringx.h"
#include <unistd.h>

// 定义红外传感器引脚
#define IR_LEFT  26
#define IR_RIGHT 27

// 定义L298N控制引脚
#define IN1 0
#define IN2 1
#define IN3 17
#define IN4 16
#define ENA 13
#define ENB 12

// 定义PWM范围
#define PWM_RANGE 1000

int wiringSetup() {
    if (wiringXSetup("milkv_duo", NULL) == -1) {
        wiringXGC();
        return -1;
    }
    return 0;
}

void setup() {
    if (wiringSetup() != 0) {
        fprintf(stderr, "wiringXSetup failed\n");
        exit(EXIT_FAILURE);
    }

    // 设置红外传感器引脚为输入模式
    pinMode(IR_LEFT, PINMODE_INPUT);
    pinMode(IR_RIGHT, PINMODE_INPUT);

    // 设置L298N控制引脚为输出模式
    pinMode(IN1, PINMODE_OUTPUT);
    pinMode(IN2, PINMODE_OUTPUT);
    pinMode(IN3, PINMODE_OUTPUT);
    pinMode(IN4, PINMODE_OUTPUT);

    // 设置PWM周期和占空比
    wiringXPWMSetPeriod(ENA, PWM_RANGE);
    wiringXPWMSetPeriod(ENB, PWM_RANGE);
    wiringXPWMSetDuty(ENA, 50);
    wiringXPWMSetDuty(ENB, 50);
    wiringXPWMEnable(ENA, 1);
    wiringXPWMEnable(ENB, 1);
}

// 前进函数
void forward() {
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, HIGH);
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, HIGH);
    wiringXPWMSetDuty(ENA, 900);
    wiringXPWMSetDuty(ENB, 900);
}

// 左转函数
void turnRight() {
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, HIGH);
    digitalWrite(IN3, HIGH);
    digitalWrite(IN4, LOW);
    wiringXPWMSetDuty(ENA, 800);
    wiringXPWMSetDuty(ENB, 400);
}

// 右转函数
void turnLeft() {
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, HIGH);
    wiringXPWMSetDuty(ENA, 400);
    wiringXPWMSetDuty(ENB, 800);
}

// 停止函数
void stop() {
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, LOW);
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, LOW);
    wiringXPWMSetDuty(ENA, 0);
    wiringXPWMSetDuty(ENB, 0);
}

int main() {
    setup();

    while (1) {
        int leftValue = digitalRead(IR_LEFT);
        int rightValue = digitalRead(IR_RIGHT);
        
        if (leftValue == HIGH && rightValue == HIGH) {
            stop();
        } else if (leftValue == LOW && rightValue == HIGH) {
            // 左边未检测到黑线，右边检测到，右转
            turnLeft();
        } else if (leftValue == HIGH && rightValue == LOW) {
            // 左边检测到黑线，右边未检测到，左转
            turnRight();
        } else {
            // 左右都未检测到黑线，停止
            forward();
        }
    }

    return 0;
}