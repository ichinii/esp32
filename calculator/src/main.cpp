#include <driver/gpio.h>
#include <driver/dac.h>
#include <WiFi.h>

#include <cmath>

enum Gpio {
	LedNum1 = 16,
	LedNum2 = 4,
	LedNum4 = 0,
	LedNum8 = 2,
	LedNum16 = 15,
	LedAns = 21,
	Input = 18,
	NextState = 19,
};

enum State {
	FirstNum,
	SecondNum,
	Ans,
	StatesCount,
};

// globals
State state;
int firstNum;
int secondNum;

void setup() {
	state = FirstNum;
	firstNum = 0;
	secondNum = 0;

	pinMode(LedNum1, OUTPUT);
	pinMode(LedNum2, OUTPUT);
	pinMode(LedNum4, OUTPUT);
	pinMode(LedNum8, OUTPUT);
	pinMode(LedNum16, OUTPUT);
	pinMode(LedAns, OUTPUT);

	pinMode(Input, INPUT);
	pinMode(NextState, INPUT);
}

void writeLeds() {
	int num = 0;
	switch (state) {
	case FirstNum:
		num = firstNum;
		break;
	case SecondNum:
		num = secondNum;
		break;
	case Ans:
		num = firstNum + secondNum;
		break;
	default:
		break;
	}

	auto output = [] (int value) { return value == 0 ? LOW : HIGH; };
	digitalWrite(LedNum1, output(num & 0x1));
	digitalWrite(LedNum2, output(num & 0x2));
	digitalWrite(LedNum4, output(num & 0x4));
	digitalWrite(LedNum8, output(num & 0x8));
	digitalWrite(LedNum16, output(num & 0x10));

	if (state == Ans)
		digitalWrite(LedAns, HIGH);
	else
		digitalWrite(LedAns, LOW);
}

void nextState() {
	if (state == Ans) {
		firstNum = 0;
		secondNum = 0;
	}

	state = static_cast<State>((state + 1) % StatesCount);
	writeLeds();
}

void increase() {
	switch (state) {
	case FirstNum:
		++firstNum;
		break;
	case SecondNum:
		++secondNum;
		break;
	default:
		break;
	}

	writeLeds();
}

template <int id>
bool justDown() {
	static bool pressing = false;
	bool down = digitalRead(id);
	bool just = !pressing && down;
	pressing = down;
	return just;
}

void loop() {
	delay(50);

	if (justDown<Input>())
		increase();

	if (justDown<NextState>())
		nextState();
}
