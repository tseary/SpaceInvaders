
int pinCS = 10; // Attach CS to this pin, DIN to MOSI and CLK to SCK (cf http://arduino.cc/en/Reference/SPI )
int numberOfHorizontalDisplays = 2;
int numberOfVerticalDisplays = 2;

Max72xxPanelBleh matrix = Max72xxPanelBleh(pinCS, numberOfHorizontalDisplays, numberOfVerticalDisplays);

void setupDisplay() {
	matrix.setIntensity(12); // Set brightness between 0 and 15
}

void drawDisplay() {
	for (uint8_t y = SCREEN_H - 1; y < SCREEN_H; y--) {
		for (uint8_t x = SCREEN_W - 1; x < SCREEN_W; x--) {
			char c = F_EMPTY;
			if (x == xShot && y == yShot) {
				c = F_PLAYER_BULLET;
			} else if (abs(x - xPlayer) <= PLAYER_W2 && y == 0 ||
				x == xPlayer && y < PLAYER_H) {
				c = F_PLAYER;
			} else if (field[x][y]) {
				c = field[x][y];
			}

			matrix.drawPixel(x, y, c);
		}
	}
	matrix.write(); // Send bitmap to display
}

void printDisplay() {
	for (uint8_t y = SCREEN_H - 1; y < SCREEN_H; y--) {
		for (uint8_t x = SCREEN_W - 1; x < SCREEN_W; x--) {
			char c = '.';
			if (x == xShot && y == yShot) {
				c = F_PLAYER_BULLET;
			} else if (isPlayer(x, y)) {
				c = F_PLAYER;
			} else if (field[x][y]) {
				c = field[x][y];
			}

			Serial.print(' ');
			Serial.print(c);
			matrix.drawPixel(x, y, HIGH);

		}
		Serial.println();
	}
	Serial.println();
}
