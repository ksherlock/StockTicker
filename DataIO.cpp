#include "DataIO.h"

#include <string.h>

BDataIO::BDataIO()
{
}
BDataIO::~BDataIO()
{
}
BPositionIO::BPositionIO()
{
}
BPositionIO::~BPositionIO()
{
}

BMemoryIO::BMemoryIO(void *buffer, size_t numBytes)
{
	fBuffer = buffer;
	fSize = numBytes;
	fPosition = 0;
	fReadOnly = false;
}

BMemoryIO::BMemoryIO(const void *buffer, size_t numBytes)
{
	fBuffer = (void *)buffer;
	fSize = numBytes;
	fPosition = 0;
	fReadOnly = true;
}

size_t BMemoryIO::Read(void *buffer, size_t numBytes)
{
	size_t cnt;
	
	cnt = fSize - fPosition; // max cnt;

	if (!cnt) return B_EOF;

	if (cnt > numBytes) cnt = numBytes;

	memcpy(buffer, (char *)fBuffer + fPosition, cnt);

	fPosition += cnt;

	return cnt;
}
size_t BMemoryIO::ReadAt(off_t position, void *buffer, size_t numBytes)
{
	size_t cnt;
	
	cnt = fSize - position; // max cnt;

	if (!cnt) return B_EOF;

	if (cnt > numBytes) cnt = numBytes;

	memcpy(buffer, (char *)fBuffer + position, cnt);

	return cnt;
}

size_t BMemoryIO::Write(const void *buffer, size_t numBytes)
{
size_t cnt;

	if (fReadOnly) return B_NOT_ALLOWED;
	
	cnt = fSize - fPosition; // max cnt;

	if (!cnt) return B_EOF;

	if (cnt > numBytes) cnt = numBytes;

	memcpy((char *)fBuffer + fPosition, buffer, cnt);

	fPosition += cnt;

	return cnt;
}
size_t BMemoryIO::WriteAt(off_t position, const void *buffer, size_t numBytes)
{
size_t cnt;

	if (fReadOnly) return B_NOT_ALLOWED;
	
	cnt = fSize - position; // max cnt;

	if (!cnt) return B_EOF;

	if (cnt > numBytes) cnt = numBytes;

	memcpy((char *)fBuffer + position, buffer, cnt);

	return cnt;
}

off_t BMemoryIO::Position(void) const
{
	return fPosition;
}

off_t BMemoryIO::Seek(off_t position, int32 mode)
{
	switch (mode)
	{
	case SEEK_SET: // offset from 0
		break;
	case SEEK_CUR:	// offset from current pos
		position += fPosition;
		break;
	case SEEK_END:	// offset from end (position must be -)
		position = fSize += position;
		break;
	default:
		position = fPosition;
	}

	if (position >= 0 && position < fSize) fPosition = position;

	return fPosition;
}


/*
 * BMallocIO class
 *
 */

BMallocIO::BMallocIO(void)
{
	fBuffer = NULL;
	fBlockSize = 256;
	fLength = 0;
	fAlloc = 0;
	fPosition = 0;
}
BMallocIO::~BMallocIO(void)
{
	if (fBuffer) delete[] fBuffer;
}

const void * BMallocIO::Buffer(void) const
{
	return fBuffer;
}

size_t BMallocIO::BufferLength(void) const
{
	return fLength;
}

void BMallocIO::SetBlockSize(size_t blockSize)
{
	if (blockSize > 0 ) fBlockSize = blockSize; 
}

// set the size of the allocated memory
status_t BMallocIO::SetSize(off_t numBytes)
{
off_t cnt;
uint32 pages;
char *tmp;

	if (numBytes < 0) return B_ERROR;

	pages = numBytes / fBlockSize;
	if (numBytes % fBlockSize) pages++;

	cnt = pages * fBlockSize;

	tmp = new char[cnt];

	if (tmp)
	{
		if (fBuffer) 
			memcpy(tmp, fBuffer, fLength < numBytes ? fLength : numBytes);

		fAlloc = cnt;
		fLength = numBytes;

		if (fBuffer) delete[] fBuffer;
		fBuffer = tmp;

		return B_OK;
	}
	return B_NO_MEMORY;
}

off_t BMallocIO::Position(void) const
{
	return fPosition;
}

off_t BMallocIO::Seek(off_t position, int32 mode)
{
	switch (mode)
	{
	case SEEK_SET: // offset from 0
		fPosition = position;
		break;
	case SEEK_CUR:	// offset from current pos
		fPosition += position;
		break;
	case SEEK_END:	// offset from end (position must be -)
		fPosition = fLength += position;
		break;
	default:
		fPosition = position;
	}

	return fPosition;
}

size_t BMallocIO::Read(void *buffer, size_t numBytes)
{
	size_t cnt;
	
	cnt = fLength - fPosition; // max cnt;

	if (!cnt) return B_EOF;

	if (cnt > numBytes) cnt = numBytes;

	memcpy(buffer, (char *)fBuffer + fPosition, cnt);

	fPosition += cnt;

	return cnt;
}

size_t BMallocIO::ReadAt(off_t position, void *buffer, size_t numBytes)
{
	size_t cnt;
	
	cnt = fLength - position; // max cnt;

	if (!cnt) return 0;

	if (cnt > numBytes) cnt = numBytes;

	memcpy(buffer, (char *)fBuffer + position, cnt);

	return cnt;
}


// if fPosition > eof, resizes buffer.
size_t BMallocIO::Write(const void *buffer, size_t numBytes)
{
status_t err;

	if (numBytes < 0) return B_ERROR;

	if (fPosition + numBytes > fLength)
	{
		err = SetSize(fPosition + numBytes);
		if (err != B_OK) return err;
	}

	memcpy(fBuffer + fPosition, buffer, numBytes);
	fPosition += numBytes;

	return numBytes;
}

size_t BMallocIO::WriteAt(off_t position, const void *buffer, size_t numBytes)
{
status_t err;

	if (numBytes < 0) return B_ERROR;

	if (position + numBytes > fLength)
	{
		err = SetSize(fPosition + numBytes);
		if (err != B_OK) return err;
	}

	memcpy(fBuffer + position, buffer, numBytes);
	return numBytes;
}