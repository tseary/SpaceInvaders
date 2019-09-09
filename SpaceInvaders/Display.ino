
int pinCS = 10; // Attach CS to this pin, DIN to MOSI and CLK to SCK (cf http://arduino.cc/en/Reference/SPI )
int numberOfHorizontalDisplays = 2;
int numberOfVerticalDisplays = 2;

Max72xxPanelBleh matrix = Max72xxPanelBleh(pinCS, numberOfHorizontalDisplays, numberOfVerticalDisplays);

void setupDisplay() {
	matrix.setIntensity(8); // Set brightness between 0 and 15
}

void drawDisplay() {
	drawDisplay(true);
}

void drawDisplay(uint8_t drawPlayer) {
	for (uint8_t y = SCREEN_H - 1; y < SCREEN_H; y--) {
		for (uint8_t x = SCREEN_W - 1; x < SCREEN_W; x--) {
			char c = field[x][y];
			if (x == xShot && y == yShot) {
				c = F_PLAYER_BULLET;
			} else if (drawPlayer != DRAW_PLAYER_INVISIBLE && isPlayer(x, y)) {
				c = (drawPlayer != DRAW_PLAYER_EMPTY) ? F_PLAYER : F_EMPTY;
			}

			matrix.drawPixel(x, y, c);
#ifdef PRINT_DISPLAY
			Serial.print(' ');
			Serial.print(c);
#endif
		}
#ifdef PRINT_DISPLAY
		Serial.println();
#endif
	}
	matrix.write(); // Send bitmap to display
#ifdef PRINT_DISPLAY
	Serial.println();
#endif
}
