
#ifndef MOS6522_H
#define MOS6522_H

class Memory;

/**
	@author Anders Piniesjö <pugo@pugo.org>
*/
class MOS6522
{
public:
	MOS6522(Memory* memory);
	~MOS6522();

	void reset();
	short exec();
	
private:
	Memory* memory;
};

#endif // MOS6502_H
