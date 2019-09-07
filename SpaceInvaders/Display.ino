
void printDisplay() {
	for (uint8_t y = SCREEN_HEIGHT - 1; y < SCREEN_HEIGHT; y--) {
		for (uint8_t x = SCREEN_WIDTH - 1; x < SCREEN_WIDTH; x--) {
			char c = field[x][y];
			Serial.print(' ');
			Serial.print(c ? c : '.');
		}
		Serial.println();
	}
	Serial.println();
}
