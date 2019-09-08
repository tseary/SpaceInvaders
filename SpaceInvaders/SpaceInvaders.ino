/*
	Name:       SpaceInvaders.ino
	Created:	9/5/2019 10:38:01 PM
	Author:     SCROLLSAW\Thomas
*/

#include <Max72xxPanelBleh.h>

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

// Player
uint8_t playerLives = 3;
// The position of the player
uint8_t xPlayer;

// The position of the (one and only) bullet
uint8_t xShot, yShot;

char field[SCREEN_W][SCREEN_H];

// Alien movement
uint32_t moveAliensPeriodPer;

// Field symbols
const char
F_EMPTY = '\0',
F_PLAYER = '@',
F_PLAYER_BULLET = '^',
F_ALIEN_BULLET = 'v',
F_SPACESHIP = '$',
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

	setupDisplay();
}

void loop() {

	do {
		newGame();

		printDisplay();
		drawDisplay();

		uint32_t moveAliensPeriod = TOTAL_ALIENS * moveAliensPeriodPer,
			moveAliensTime = millis() + moveAliensPeriod;
		uint32_t moveShotPeriod = 200,
			moveShotTime;

		uint8_t liveAliens = TOTAL_ALIENS;

		bool roundWin = false,
			roundLose = false;

		do {
			// TODO
			//newRound();

			bool draw = false;

			updateControl();

			// Move player
			if (isLClick()) {
				tryMovePlayerLeft();
				draw = true;
			}
			if (isRClick()) {
				tryMovePlayerRight();
				draw = true;
			}
			if (isDClick() && isShotReady()) {
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
					moveDown = !(moveLeft ? tryMoveAliensLeft() : tryMoveAliensRight());
				}
				if (moveDown) {
					// Lose if the aliens reach the bottom
					roundLose = !tryMoveAliensDown();
					moveDown = false;
					moveLeft = !moveLeft;
				}

				moveAliensTime += moveAliensPeriod;
				draw = true;
			}

			// Check collisions
			if (isAlien(xShot, yShot)) {
				// The shot hit an alien
				removeAlien(xShot, yShot);

				liveAliens--;
				moveAliensPeriod -= moveAliensPeriodPer;
				clearShot();

				// Win if all aliens are dead
				roundWin = !liveAliens;
			}
			if (isAlien(xPlayer - 1, 0) || isAlien(xPlayer, 0) ||
				isAlien(xPlayer + 1, 0) || isAlien(xPlayer, 1)) {
				// Alien is touching player
				roundLose = true;
			}

			if (draw) {
				printDisplay();
				drawDisplay();
			}

		} while (!roundWin && !roundLose);

		// Speed up after each round
		if (roundWin) {
			moveAliensPeriodPer *= 0.8;
		} else if (roundLose) {
			// TODO Reset the aliens position, but don't replace the ones already killed

			// Lose a life
			playerLives--;
		}

	} while (playerLives > 0);

	// TODO Game over
}

void newGame() {
	// Reset the aliens
	// Start with the grid of aliens touching the left side of the field...
	xGridOrigin = SCREEN_W - ((ALIEN_COLS - 1) * ALIEN_PITCH_X + ALIEN_W);
	// ..and as close as possible to the top of the screen, leaving space for spaceships.
	yGridOrigin = SCREEN_H - ((ALIEN_ROWS - 1) * ALIEN_PITCH_Y + ALIEN_H) - SPACESHIP_H;

	// Clear the field
	memset(field, F_EMPTY, SCREEN_W * SCREEN_H);

	// Reset the player
	playerLives = 3;
	xPlayer = SCREEN_W / 2;
	clearShot();

	// Reset timing
	moveAliensPeriodPer = 80;

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

/******************************************************************************
 * Player Shot
 ******************************************************************************/

void shoot() {
	xShot = xPlayer;
	yShot = PLAYER_H;
}

void clearShot() {
	yShot = SCREEN_H;
}

bool isShotActive() {
	return yShot < SCREEN_H;
}

bool isShotReady() {
	return yShot >= SCREEN_H / 2;
}

bool moveShotUp() {
	yShot++;
}

/******************************************************************************
 * Aliens
 ******************************************************************************/

void removeAlien(uint8_t xField, uint8_t yField) {
	// Get the symbol that represents this alien
	char c = field[xField][yField];

	// TODO Reduce loop bounds to alien size
	for (uint8_t y = 0; y < SCREEN_H; y++) {
		for (uint8_t x = 0; x < SCREEN_W; x++) {
			if (field[x][y] == c) field[x][y] = F_EMPTY;
		}
	}
}

bool tryMoveAliensLeft() {
	for (uint8_t y = 0; y < SCREEN_H; y++) {
		if (isAlien(SCREEN_W - 1, y)) return false;
	}
	for (uint8_t y = 0; y < SCREEN_H; y++) {
		for (uint8_t x = SCREEN_W - 2; x < SCREEN_W; x--) {
			if (isAlien(x, y)) {
				field[x + 1][y] = field[x][y];
				field[x][y] = F_EMPTY;
			}
		}
	}
	return true;
}

bool tryMoveAliensRight() {
	for (uint8_t y = 0; y < SCREEN_H; y++) {
		if (isAlien(0, y)) return false;
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

bool tryMoveAliensDown() {
	for (uint8_t x = 1; x < SCREEN_W; x++) {
		if (isAlien(x, 0)) return false;
	}
	for (uint8_t y = 0; y < SCREEN_H - 1; y++) {
		for (uint8_t x = 0; x < SCREEN_W; x++) {
			if (isAlien(x, y)) {
				if (y > 0) field[x][y - 1] = field[x][y];
				field[x][y] = '\0';
			}
		}
	}
	return true;
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

/******************************************************************************
 * Identify Objects on the Field
 ******************************************************************************/

bool isAlien(uint8_t x, uint8_t y) {
	return x < SCREEN_W && y < SCREEN_H &&
		(uint8_t)(field[x][y] - F_ALIEN_0) < TOTAL_ALIENS;
}

// This function defines the shape of the player:
// P = player origin, @ = part of player ship
// 
//   @
// @ P @
bool isPlayer(uint8_t x, uint8_t y) {
	return abs(x - xPlayer) <= PLAYER_W2 && y == 0 ||
		x == xPlayer && y < PLAYER_H;
}

bool isSpaceship(uint8_t x, uint8_t y) {
	return field[x][y] == F_SPACESHIP;
}

/******************************************************************************
 * Animations
 ******************************************************************************/

void animatePlayerExplode() {
	
}
