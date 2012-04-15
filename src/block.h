#include "2DGraphics.h"

class Block : public CObject {
public:
	bool enabled;

	Block() : CObject() { enabled = false; }
};