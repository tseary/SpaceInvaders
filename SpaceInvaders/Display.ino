
void printDisplay() {
	for (uint8_t y = SCREEN_HEIGHT - 1; y < SCREEN_HEIGHT; y--) {
		for (uint8_t x = SCREEN_WIDTH - 1; x < SCREEN_WIDTH; x--) {
			if (isMonster(x, y)) {
				Serial.print("MM");
			} else if (isPlayer(x, y)) {
				Serial.print("^^");
			} else if (isSpaceship(x, y)) {
				Serial.print("/\\");
			} else {
				Serial.print(".'");
			}
		}
		Serial.println();
	}
	Serial.println();
}
