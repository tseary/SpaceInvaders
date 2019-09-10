/*
	Name:       SpaceInvaders.ino
	Created:	9/5/2019 10:38:01 PM
	Author:     SCROLLSAW\Thomas
*/

#include <Max72xxPanelBleh.h>
#include "Aliens.h"

//#define PRINT_DISPLAY

const uint8_t
SCREEN_W = 16,
SCREEN_H = 16,
PLAYER_W2 = 1,
PLAYER_H = 2,
SPACESHIP_W = 3,
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

// Drawing options
const uint8_t
DRAW_PLAYER_NORMAL = 1,
DRAW_PLAYER_INVISIBLE = 0,
DRAW_PLAYER_EMPTY = 2;

// The position of the lower-right alien on the field.
uint8_t xGridOrigin, yGridOrigin;
// One bit per alien, indicating alive or dead.
uint32_t liveAliens;

// Player
uint8_t playerLives = 3;
// The position of the player
uint8_t xPlayer;

// The position of the (one and only) bullet
uint8_t xShot, yShot;

// Alien movement
uint32_t moveAliensPeriodPer;
bool moveLeft, moveDown;
bool oddFrame;

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

void setup() {
	Serial.begin(115200);
	while (!Serial);
	delay(500);
	Serial.println("Space Invaders");

	setupDisplay();
}

void loop() {

	newGame();
	playGame();

}

void newGame() {
	// Reset the player
	playerLives = 3;
	xPlayer = SCREEN_W / 2;

	// Reset timing
	moveAliensPeriodPer = 75;
}

void newRound() {
	// Clear the shot
	clearShot();

	// Reset the aliens
	liveAliens = ~(0xffffffff << TOTAL_ALIENS);
	// Start with the grid of aliens touching the left side of the screen...
	xGridOrigin = SCREEN_W - ((ALIEN_COLS - 1) * ALIEN_PITCH_X + ALIEN_W);
	// ..and as close as possible to the top of the screen, leaving space for spaceships.
	yGridOrigin = SCREEN_H - ((ALIEN_ROWS - 1) * ALIEN_PITCH_Y + ALIEN_H) - SPACESHIP_H;
	moveDown = false;
	moveLeft = false;

	oddFrame = false;
}

void playGame() {
	do {
		newRound();

		drawDisplay();
#ifdef PRINT_DISPLAY
		printDisplay();
#endif

		uint32_t moveAliensPeriod = TOTAL_ALIENS * moveAliensPeriodPer,
			moveAliensTime = millis() + moveAliensPeriod;
		uint32_t moveShotPeriod = 200,
			moveShotTime;

		bool roundWin = false,
			roundLose = false;

		do {
			Serial.print("moveAliensPeriodPer = ");
			Serial.println(moveAliensPeriodPer);

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

				oddFrame = !oddFrame;

				moveAliensTime += moveAliensPeriod;
				draw = true;
			}

			// Check collisions
			if (isAlien(xShot, yShot)) {
				// The shot hit an alien
				killAlien(getAlienAt(xShot, yShot));

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
				drawDisplay();
#ifdef PRINT_DISPLAY
				printDisplay();
#endif
			}

		} while (!roundWin && !roundLose);

		// Speed up after each round
		if (roundWin) {
			moveAliensPeriodPer *= 0.8;
		} else if (roundLose) {
			// TODO Reset the aliens position, but don't replace the ones already killed

			// Lose a life
			playerLives--;
			animatePlayerExplode();
		}

		// Round end delay
		delay(1000);

	} while (playerLives > 0);

	// TODO Game over
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

bool tryMoveAliensLeft() {
	for (uint8_t y = 0; y < SCREEN_H; y++) {
		if (isAlien(SCREEN_W - 1, y)) return false;
	}
	xGridOrigin++;
	return true;
}

bool tryMoveAliensRight() {
	for (uint8_t y = 0; y < SCREEN_H; y++) {
		if (isAlien(0, y)) return false;
	}
	xGridOrigin--;
	return true;
}

bool tryMoveAliensDown() {
	for (uint8_t x = 1; x < SCREEN_W; x++) {
		if (isAlien(x, 0)) return false;
	}
	yGridOrigin--;
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
	return getAlienAt(x, y) != ALIEN_NONE;
}

// TODO Optimize with bitwise operations instead of multiplication/division
uint8_t getAlienAt(uint8_t x, uint8_t y) {
	// Return if point is off the field
	if (!(x < SCREEN_W && y < SCREEN_H)) return ALIEN_NONE;

	// TODO return ALIEN_NONE if not an alien (or dead)

	// Pixel coordinates, referenced to the grid origin
	uint8_t xOnFieldGrid = x - xGridOrigin;
	uint8_t yOnFieldGrid = y - yGridOrigin;
	// Grid coordinates
	uint8_t xOnGrid = xOnFieldGrid / ALIEN_PITCH_X;
	uint8_t yOnGrid = yOnFieldGrid / ALIEN_PITCH_Y;

	// Return if point is outside the grid
	if (!(xOnGrid < ALIEN_COLS && yOnGrid < ALIEN_ROWS)) return ALIEN_NONE;

	// Get the index
	uint8_t alienIndex = xOnGrid + yOnGrid * ALIEN_COLS;

	// Return if this alien is dead
	if (!isAlienAlive(alienIndex)) return ALIEN_NONE;

	// Now do bitmap comparison
	uint8_t xFrame = xOnFieldGrid % ALIEN_PITCH_X;
	uint8_t yFrame = yOnFieldGrid % ALIEN_PITCH_Y;

	// Return if the pixel is out of bounds
	if (!(xFrame < ALIEN_W && yFrame < ALIEN_H)) return ALIEN_NONE;

	// Get the shape
	uint8_t alienType = yOnGrid;
	uint8_t alienShape = ALIEN_SHAPES[alienType];

	// Get the pixel within the shape
	alienShape >>= BITS_PER_ROW * yFrame;
	alienShape >>= BITS_PER_PIXEL * xFrame;
	uint8_t pixel = alienShape & P_MASK;

	// Compare to the current animation frame
	bool itsAnAlien =
		(pixel == P_WHT) ||
		(pixel == P_EVN && !oddFrame) ||
		(pixel == P_ODD && oddFrame);

	// Return the index or none
	return itsAnAlien ? alienIndex : ALIEN_NONE;
}

bool isAlienAlive(uint8_t alienIndex) {
	return liveAliens & ((uint32_t)1 << alienIndex);
}

void killAlien(uint8_t alienIndex) {
	liveAliens &= ~((uint32_t)1 << alienIndex);
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
	if (y < SCREEN_H - SPACESHIP_H) return false;
	return false;	// TODO
}

/******************************************************************************
 * Animations
 ******************************************************************************/

void animatePlayerExplode() {
	const uint16_t BLINK_DELAY = 166;
	for (uint8_t i = 0; i < 6; i++) {
		drawDisplay((i & 0x01) ? DRAW_PLAYER_NORMAL : DRAW_PLAYER_EMPTY);
		delay(BLINK_DELAY);
	}
	drawDisplay(DRAW_PLAYER_INVISIBLE);
}
