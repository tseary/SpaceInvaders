
void printDisplay() {
	for (uint8_t y = SCREEN_H - 1; y < SCREEN_H; y--) {
		for (uint8_t x = SCREEN_W - 1; x < SCREEN_W; x--) {
			char c = '.';
			if (x == xShot && y == yShot) {
				c = F_PLAYER_BULLET;
			} else if (abs(x - xPlayer) <= PLAYER_W2 && y == 0 ||
				x == xPlayer && y < PLAYER_H) {
				c = F_PLAYER;
			} else if (field[x][y]) {
				c = field[x][y];
			}

			Serial.print(' ');
			Serial.print(c);
		}
		Serial.println();
	}
	Serial.println();
}
