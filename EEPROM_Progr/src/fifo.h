/*
 * fifo.h
 *
 * Created: 21.04.2020 17:35:47
 *  Author: Sebastian
 */ 


#ifndef FIFO_H_
#define FIFO_H_

#define BUFFER_FAIL     0
#define BUFFER_SUCCESS  1

#define BUFFER_SIZE 200
#define BUFFER_THREASHOLD_UPPER 150
#define  BUFFER_THREASHOLD_LOWER 20

struct Buffer
{
	uint8_t data[BUFFER_SIZE];
	uint8_t *read;
	uint8_t *write;
	uint8_t counter;
};

struct Buffer buffer;

uint8_t BufferIn(uint8_t);
uint8_t BufferOut(uint8_t*);
void setupFIFO(void);


void setupFIFO()
{
	buffer.read = buffer.data;
	buffer.write = buffer.data;
	buffer.counter = 0;
}

// write 1 byte into buffer
//
// Returns:
//     BUFFER_FAIL       buffer full
//     BUFFER_SUCCESS    byte written to buffer
//
uint8_t BufferIn(uint8_t byte)
{
	//if (buffer.write >= BUFFER_SIZE)
	//  buffer.write = 0; // erhöht sicherheit

	if ( ( buffer.write + 1 == buffer.read ) ||
	( buffer.read == buffer.data && buffer.write + 1 == buffer.data + (BUFFER_SIZE - 1)  ) )
	return BUFFER_FAIL;

	//write data byte
	*buffer.write = byte;

	//increment write position and wrap around if reaching end of array
	buffer.write++;
	if (buffer.write >= buffer.data+(BUFFER_SIZE-1))
		buffer.write = buffer.data;
		
	//increment fill counter
	buffer.counter++;

	return BUFFER_SUCCESS;
}

//
// read single byte from buffer
//
// Returns:
//     BUFFER_FAIL       buffer is empty, no byte could be read
//     BUFFER_SUCCESS    successfully returned a byte
//
uint8_t BufferOut(uint8_t *pByte)
{
	if (buffer.read == buffer.write)
	return BUFFER_FAIL;

	*pByte = *buffer.read;

	//increment read position and wrap if end of array
	buffer.read++;
	if (buffer.read >= (buffer.data+(BUFFER_SIZE-1)))
		buffer.read = buffer.data;
		
	//decrement counter
	buffer.counter--;

	return BUFFER_SUCCESS;
}


#endif /* FIFO_H_ */