/*
	Name:       SpaceInvaders.ino
	Created:	9/5/2019 10:38:01 PM
	Author:     SCROLLSAW\Thomas
*/

const uint8_t
SCREEN_WIDTH = 16,
SCREEN_HEIGHT = 16,
PLAYER_HEIGHT = 2,
SHIP_HEIGHT = 1,
SHIP_OFFSET = SCREEN_HEIGHT - SHIP_HEIGHT;

uint32_t monsterLayer[SCREEN_HEIGHT];
uint32_t playerLayer[PLAYER_HEIGHT];
uint32_t shipLayer[SHIP_HEIGHT];

bool moveLeft = false;
bool moveDown = false;

void setup() {
	Serial.begin(115200);
	while (!Serial);
	delay(500);
	Serial.println("Space Invaders");

	shipLayer[0] = 0b0000000000111000;

	monsterLayer[SCREEN_HEIGHT - 2] = 0b1101101101100000;
	monsterLayer[SCREEN_HEIGHT - 3] = 0b0101000101000000;
	monsterLayer[SCREEN_HEIGHT - 5] = 0b1000101000100000;
	monsterLayer[SCREEN_HEIGHT - 6] = 0b1101101101100000;

	playerLayer[1] = 0b0000100000000000;
	playerLayer[0] = 0b0001110000000000;

	printDisplay();
}

void loop() {
	// Move monsters
	if (!moveDown) {
		moveDown = !(moveLeft ? tryShiftMonstersLeft() : tryShiftMonstersRight());
	}
	if (moveDown) {
		shiftMonstersDown();
		moveDown = false;
		moveLeft = !moveLeft;
	}

	printDisplay();
	delay(1000);
}

bool tryShiftMonstersLeft() {
	for (uint8_t y = 0; y < SCREEN_HEIGHT; y++) {
		if (isMonster(SCREEN_WIDTH - 1, y)) {
			return false;
		}
	}
	for (uint8_t y = 0; y < SCREEN_HEIGHT; y++) {
		monsterLayer[y] <<= 1;
	}
	return true;
}

bool tryShiftMonstersRight() {
	for (uint8_t y = 0; y < SCREEN_HEIGHT; y++) {
		if (isMonster(0, y)) {
			return false;
		}
	}
	for (uint8_t y = 0; y < SCREEN_HEIGHT; y++) {
		monsterLayer[y] >>= 1;
	}
	return true;
}

void shiftMonstersDown() {
	for (uint8_t y = 1; y < SCREEN_HEIGHT; y++) {
		monsterLayer[y - 1] = monsterLayer[y];
	}
}

bool isSpaceship(uint8_t x, uint8_t y) {
	y -= SHIP_OFFSET;
	return y < SHIP_HEIGHT ? (shipLayer[y] & ((uint32_t)1 << x)) : false;
}

bool isMonster(uint8_t x, uint8_t y) {
	return y < SCREEN_HEIGHT ? (monsterLayer[y] & ((uint32_t)1 << x)) : false;
}

bool isPlayer(uint8_t x, uint8_t y) {
	return y < PLAYER_HEIGHT ? (playerLayer[y] & ((uint32_t)1 << x)) : false;
}
