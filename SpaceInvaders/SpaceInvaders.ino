/*
	Name:       SpaceInvaders.ino
	Created:	9/5/2019 10:38:01 PM
	Author:     SCROLLSAW\Thomas
*/

const uint8_t
SCREEN_W = 16,
SCREEN_H = 16,
PLAYER_W2 = 1,
PLAYER_H = 2,
SPACESHIP_H = 1,
SHIP_OFFSET = SCREEN_H - SPACESHIP_H;

const uint8_t
ALIEN_ROWS = 3,
ALIEN_COLS = 4,
ALIEN_W = 2,
ALIEN_H = 2,
ALIEN_PITCH_X = 3,	// Horizontal distance from one alien to the next on the field
ALIEN_PITCH_Y = 3,	// Vertical distance from one alien to the next on the field
TOTAL_ALIENS = ALIEN_ROWS * ALIEN_COLS;

// The position of the lower-right alien on the field.
uint8_t xGridOrigin, yGridOrigin;

// The position of the player
uint8_t xPlayer;

// THe position of the (one and only) bullet
uint8_t xShot, yShot;

char field[SCREEN_W][SCREEN_H];

// Field symbols
const char
F_PLAYER = '@',
F_PLAYER_BULLET = '^',
F_ALIEN_BULLET = 'v',
F_SPACSHIP = '$',
F_SHIELD = '#',
F_ALIEN_0 = 'A';

// The field is the size of the screen and has one char per pixel.
// The char identifies what is on-screen.
// 
// field coordinates
//    151413121110 9 8 7 6 5 4 3 2 1 0
// 15  . . . . $ $ $ . . . . . . . . .
// 14  . . . . . . . . . . . . . . . .
// 13  . . . . . . . . . . . . . . . .
// 12  . . L L . K K . J J . I I . . .
// 11  . . L L . K K . J J . I I . . .
// 10  . . . . . . . . . . . . . . . .
//  9  . . H H . G G . F F . E E . . .
//  8  . . . H . G . . . F . E . . . .
//  7  . . . . . . . . . . . . . . . .
//  6  . . . ^ . C C . B B . A A . . .
//  5  . . . . . . C . B . . . A . . .
//  4  . . v . . . . . . . . . . . . .
//  3  . . . . . . . . . . . v . . . .
//  2  . . # # . . . # # . . . # # . .
//  1  . . . . . @ . . . . . . . . . .
//  0  . . . . @ @ @ . . . . . . . . .
// 

bool moveLeft = false;
bool moveDown = false;

void setup() {
	Serial.begin(115200);
	while (!Serial);
	delay(500);
	Serial.println("Space Invaders");
}

void loop() {

	newGame();
	printDisplay();

	const uint32_t moveAliensPeriodPer = 80;
	uint32_t moveAliensPeriod = TOTAL_ALIENS * moveAliensPeriodPer,
		moveAliensTime = millis() + moveAliensPeriod;
	uint32_t moveShotPeriod = 250,
		moveShotTime;

	uint8_t liveAliens = TOTAL_ALIENS;

	while (true) {
		bool draw = false;

		updateControl();

		// Move player
		if (isLClick()) {
			Serial.println("left");
			tryMovePlayerLeft();
			draw = true;
		}
		if (isRClick()) {
			Serial.println("right");
			tryMovePlayerRight();
			draw = true;
		}
		if (isDClick()) {
			Serial.println("pew!");
			shoot();
			moveShotTime = millis() + moveShotPeriod;
			draw = true;
		}

		// Move player shot
		if (isShotActive() && millis() >= moveShotTime) {
			moveShotUp();
			moveShotTime += moveShotPeriod;
			draw = true;
		}

		// Move aliens
		if (millis() >= moveAliensTime) {
			if (!moveDown) {
				moveDown = !(moveLeft ? tryShiftAliensLeft() : tryShiftAliensRight());
			}
			if (moveDown) {
				moveAliensDown();
				moveDown = false;
				moveLeft = !moveLeft;
			}

			moveAliensTime += moveAliensPeriod;
			draw = true;
		}

		// Check collisions
		if (isAlien(xShot, yShot)) {
			// The shot hit an alien
			liveAliens--;
			moveAliensPeriod -= moveAliensPeriodPer;
			clearShot();
		}

		if (draw) {
			printDisplay();
		}
	}
}

void newGame() {
	// Reset the aliens
	// Start with the grid of aliens touching the left side of the field...
	xGridOrigin = SCREEN_W - ((ALIEN_COLS - 1) * ALIEN_PITCH_X + ALIEN_W);
	// ..and as close as possible to the top of the screen, leaving space for spaceships.
	yGridOrigin = SCREEN_H - ((ALIEN_ROWS - 1) * ALIEN_PITCH_Y + ALIEN_H) - SPACESHIP_H;

	// Clear the field
	memset(field, '\0', SCREEN_W * SCREEN_H);

	// Reset the player
	xPlayer = SCREEN_W / 2;
	clearShot();

	// Draw the aliens
	for (uint8_t i = 0; i < ALIEN_COLS; i++) {
		for (uint8_t j = 0; j < ALIEN_ROWS; j++) {
			uint8_t x = xGridOrigin + i * ALIEN_PITCH_X,
				y = yGridOrigin + j * ALIEN_PITCH_Y;
			char c = F_ALIEN_0 + i + j * ALIEN_COLS;
			field[x][y] = c;
			field[x][y + 1] = c;
			field[x + 1][y + 1] = c;
		}
	}
}

void shoot() {
	xShot = xPlayer;
	yShot = PLAYER_H;
}

void clearShot() {
	xShot = SCREEN_W;
	yShot = PLAYER_H;
}

bool isShotActive() {
	return xShot < SCREEN_W;
}

bool moveShotUp() {
	yShot++;
}

bool tryShiftAliensLeft() {
	for (uint8_t y = 0; y < SCREEN_H; y++) {
		if (isAlien(SCREEN_W - 1, y)) {
			return false;
		}
	}
	for (uint8_t y = 0; y < SCREEN_H; y++) {
		for (uint8_t x = SCREEN_W - 2; x < SCREEN_W; x--) {
			if (isAlien(x, y)) {
				field[x + 1][y] = field[x][y];
				field[x][y] = '\0';
			}
		}
	}
	return true;
}

bool tryShiftAliensRight() {
	for (uint8_t y = 0; y < SCREEN_H; y++) {
		if (isAlien(0, y)) {
			return false;
		}
	}
	for (uint8_t y = 0; y < SCREEN_H; y++) {
		for (uint8_t x = 1; x < SCREEN_W; x++) {
			if (isAlien(x, y)) {
				field[x - 1][y] = field[x][y];
				field[x][y] = '\0';
			}
		}
	}
	return true;
}

void moveAliensDown() {
	for (uint8_t y = 0; y < SCREEN_H - 1; y++) {
		for (uint8_t x = 0; x < SCREEN_W; x++) {
			if (isAlien(x, y)) {
				if (y > 0) field[x][y - 1] = field[x][y];
				field[x][y] = '\0';
			}
		}
	}
}

bool tryMovePlayerLeft() {
	if (xPlayer < (SCREEN_W - 1 - PLAYER_W2)) {
		xPlayer++;
		return true;
	}
	return false;
}

bool tryMovePlayerRight() {
	if (xPlayer > PLAYER_W2) {
		xPlayer--;
		return true;
	}
	return false;
}

bool isSpaceship(uint8_t x, uint8_t y) {
	return field[x][y] == F_SPACSHIP;
}

bool isAlien(uint8_t x, uint8_t y) {
	return x < SCREEN_W && y < SCREEN_H &&
		(uint8_t)(field[x][y] - F_ALIEN_0) < TOTAL_ALIENS;
}

bool isPlayer(uint8_t x, uint8_t y) {
	return field[x][y] == F_PLAYER;
}
