/*
 * fifo.h
 *
 * Created: 21.04.2020 17:35:47
 *  Author: Sebastian
 *
 *	Implementation of a software FIFO circular buffer
 */ 


#ifndef FIFO_H_
#define FIFO_H_

//buffer operation result keys
#define BUFFER_FAIL     0
#define BUFFER_SUCCESS  1

//buffer params
#define BUFFER_SIZE 100
#define BUFFER_THREASHOLD_UPPER 70
#define BUFFER_THREASHOLD_LOWER 10

//buffer structure def
struct Buffer
{
	uint8_t data[BUFFER_SIZE];
	uint8_t *read;
	uint8_t *write;
	uint8_t counter;
};

//global buffer var
struct Buffer buffer;

//prototypes
uint8_t BufferIn(uint8_t);
uint8_t BufferOut(uint8_t*);
void setupFIFO(void);

//set buffer r/w pointers to start of array
void setupFIFO()
{
	buffer.read = buffer.data;
	buffer.write = buffer.data;
	buffer.counter = 0;
}

// write 1 byte into buffer
uint8_t BufferIn(uint8_t byte)
{
	if ( ( buffer.write + 1 == buffer.read ) ||
	( buffer.read == buffer.data && buffer.write + 1 == buffer.data + (BUFFER_SIZE - 1)  ) )
	{
		//buffer full
		return BUFFER_FAIL;
	}

	//write data byte
	*buffer.write = byte;

	//increment write pointer and wrap around if reaching end of array
	buffer.write++;
	if (buffer.write >= buffer.data + (BUFFER_SIZE-1))
	{
		buffer.write = buffer.data;
	}
		
	//increment fill counter
	buffer.counter++;

	return BUFFER_SUCCESS;
}

// read single byte from buffer
uint8_t BufferOut(uint8_t *data)
{
	//check if empty
	if (buffer.read == buffer.write)
	{
		return BUFFER_FAIL;
	}
	
	//copy byte by value to detach from buffer
	*data = *buffer.read;

	//increment read position and wrap if end of array
	buffer.read++;
	if (buffer.read >= (buffer.data+(BUFFER_SIZE-1)))
	{
		buffer.read = buffer.data;
	}
		
	//decrement fill counter
	buffer.counter--;

	return BUFFER_SUCCESS;
}


#endif /* FIFO_H_ */