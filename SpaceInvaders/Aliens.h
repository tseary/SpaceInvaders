
const uint8_t ALIEN_TYPES = 3;

const uint8_t ALIEN_NONE = 255;

// Geometry data for each alien shape
// Lower right is origin
// Every pair of bits indicates the colour/animation:
const uint8_t
P_BLK = 0b00,
P_ODD = 0b01,
P_EVN = 0b10,
P_WHT = 0b11;

const uint8_t
BITS_PER_PIXEL = 2,
BITS_PER_ROW = 2 * BITS_PER_PIXEL,
P_MASK = ~(0xff << BITS_PER_PIXEL);

const uint8_t ALIEN_SHAPES[ALIEN_TYPES] = {
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
