/* Address of the buffer to write to */
/* Bit offset to start writing to */
/* Address of the buffer to read from */
/* Bit offset to start reading from */

void bitcpy(void *dest, size_t write, const void *_src, size_t read, size_t count);
