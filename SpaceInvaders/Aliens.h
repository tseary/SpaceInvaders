
const uint8_t ALIEN_COUNT = 3;

const uint8_t
ALIEN_A = 0,
ALIEN_B = 1,
ALIEN_C = 2;

const uint8_t ALIEN_NONE = 255;

const uint8_t TETRAMINO_SIZE = 4;
const uint16_t TETRAMINO_MASK = 0x000f;

// Geometry data for each alien shape
// Lower right is origin
// Every pair of bits indicates the colour/animation:
// 0b00 = black
// 0b01 = white for odd frames
// 0b10 = white for even frames
// 0b11 = white
const uint8_t ALIEN_SHAPES[ALIEN_COUNT] = {
	// C	[][]  [][]
	//		  []  []
	0b11110110,

	// B	[]      []
	//		  []  []
	0b10010110,

	// A	[][]  [][]
	//		[]      []
	0b11111001
};

const uint8_t ALIEN_NONE = 255;
