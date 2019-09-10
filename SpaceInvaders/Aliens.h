
const uint8_t ALIEN_NONE = 255;
const uint8_t ALIEN_TYPES = 3, FRAMES = 2;

// Geometry data for each alien shape
// Lower right is origin (LSB)
const uint8_t ALIEN_SHAPES[ALIEN_TYPES][FRAMES] = {
	// Bottom row of aliens
	// [][]    [][]
	// []      []
	{0b1110, 0b1101},

	// [][]    [][]
	//   []    []
	{0b1101, 0b1110},

	// Top row of aliens
	//   []    []
	// []        []
	{0b0110, 0b1001}
};
