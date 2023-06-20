#ifndef PROMAKE_CIRCULARBUFFER_H_INCLUDED
#define PROMAKE_CIRCULARBUFFER_H_INCLUDED

#include <inttypes.h>
#include <stddef.h>

#ifndef byte
#define byte uint8_t
#endif

// These values have to be interrelated
// To-Do: may we have just one? (BUFFERMASK)
#define __BUFFERSIZE__ 128
#define __BUFFERMASK__ 0x7F

class ProMake_CircularBufferManager
{
public:
	/** If there is spaceAvailable in the buffer, lets send a XON
	 */
	virtual void spaceAvailable();
};

class ProMake_CircularBuffer
{
private:
	// Buffer pointers.
	// head=tail means buffer empty
	// tail=head-1 means buffer full
	// tail=head+1 means just one char (pointed by head)
	// REMEMBER. head can be moved only by the main program
	// REMEMBER. tail can be moved only by the other thread (interrupts)
	// REMEMBER. head and tail can move only FORWARD
	volatile byte head; // First written one
	volatile byte tail; // Last written one.

	ProMake_CircularBufferManager *cbm; // Circular buffer manager

	// The buffer
	volatile byte theBuffer[__BUFFERSIZE__];

	/** Checks if a substring exists in the buffer
		@param reference	Substring
		@param thishead		Head
		@param thistail		Tail
		@param from			Initial byte position
		@param to			Final byte position
		@return true if exists, in otherwise return false
	 */
	bool locate(const char *reference, byte thishead, byte thistail, byte *from = 0, byte *to = 0);

public:
	/** Constructor
		@param mgr			Circular buffer manager
	 */
	ProMake_CircularBuffer(ProMake_CircularBufferManager *mgr = 0);

	// TO-DO.Check if this formule runs too at the buffer limit

	/** Get available bytes in circular buffer
		@return available bytes
	 */
	inline byte availableBytes() { return ((head - (tail + 1)) & __BUFFERMASK__); };

	/** Stored bytes in circular buffer
		@return stored bytes
	 */
	inline byte storedBytes() { return ((tail - head) & __BUFFERMASK__); };

	/** Write a character in circular buffer
		@param c			Character
		@return 1 if successful
	 */
	int write(char c);

	/** Returns a character and moves the pointer
		@return character
	 */
	char read();

	/** Returns a character but does not move the pointer.
		@param increment	Increment
		@return character
	 */
	char peek(int increment);

	/** Returns a pointer to the head of the buffer
		@return buffer with pointer in head
	*/
	inline char *firstString() { return (char *)theBuffer + head; };

	/** Go forward one string
		@return buffer with one string advance
	 */
	char *nextString();

	/** Flush circular buffer
	 */
	void flush();

	/** Get tail
		@return tail
	 */
	inline byte getTail() { return tail; };

	/** Get head
		@return head
	 */
	inline byte getHead() { return head; };

	// Only can be executed from the interrupt!
	/** Delete circular buffer to the end
		@param from			Initial byte position
	 */
	inline void deleteToTheEnd(byte from) { tail = from; };

	/** Checks if a substring exists in the buffer
		move=0, dont move, =1,put head at the beginning of the string, =2, put head at the end
		@param reference
		@return true if exists, in otherwise return false
	 */
	bool locate(const char *reference);

	/** Locates reference. If found, moves head (or tail) to the beginning (or end)
		@param reference
		@param movetotheend
		@param head
		@return true if successful
	 */
	bool chopUntil(const char *reference, bool movetotheend, bool head = true);

	/** Reads an integer from the head. Stops with first non blank, non number character
		@return integer from the head
	 */
	int readInt();

	// Caveat: copies the first bytes until buffer is full

	/** Extract a substring from circular buffer
		@param from			Initial byte position
		@param to			Final byte position
		@param buffer		Buffer for copy substring
		@param bufsize		Buffer size
		@return true if successful, false if substring does not exists
	 */
	bool extractSubstring(const char *from, const char *to, char *buffer, int bufsize);

	/** Retrieve all the contents of buffer from head to tail
		@param buffer
		@param bufsize
		@param SizeWritten
		@return true if successful
	 */
	bool retrieveBuffer(char *buffer, int bufsize, int &SizeWritten);

	/** Debug function to print the buffer after receiving data from the modem.
	 */
	void debugBuffer();

	/** Utility: dump character if printable, else, put in %x%
		@param c			Character
	 */
	static void printCharDebug(uint8_t c);
};

#endif