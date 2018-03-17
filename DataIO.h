#ifndef __BDATAIO_H__
#define __BDATAIO_H__

#include "SupportDefs.h"


#ifndef SEEK_SET
#define SEEK_SET	0
#define SEEK_CUR	1
#define SEEK_END	2
#endif

class BDataIO
{
public:
	BDataIO();
virtual ~BDataIO();

virtual size_t Read(void *buffer, size_t numBytes) = 0;
virtual size_t Write(const void *buffer, size_t numBytes) = 0;

};

class BPositionIO: public BDataIO
{
public:
	BPositionIO();
virtual ~BPositionIO();

virtual size_t ReadAt(off_t position, void *buffer, size_t numBytes) = 0;
virtual size_t WriteAt(off_t position, const void *buffer, size_t numBytes) = 0;

virtual off_t Position(void) const = 0;
virtual off_t Seek(off_t position, int32 mode) = 0;

virtual status_t SetSize(off_t numBytes)
	{
		return B_ERROR;
	}
};

class BMemoryIO: public BPositionIO
{
public:
	BMemoryIO(const void *buffer, size_t numBytes);
	BMemoryIO(void *buffer, size_t numBytes);
	virtual ~BMemoryIO() {}

virtual size_t Read(void *buffer, size_t numBytes) ;
virtual size_t ReadAt(off_t position, void *buffer, size_t numBytes);

virtual size_t Write(const void *buffer, size_t numBytes);
virtual size_t WriteAt(off_t position, const void *buffer, size_t numBytes);

virtual off_t Position(void) const;
virtual off_t Seek(off_t position, int32 mode);


private:
	void *fBuffer;
	size_t fSize;
	off_t fPosition;
	bool fReadOnly;
};


class BMallocIO: public BPositionIO
{
public:
	BMallocIO(void);
virtual ~BMallocIO(void);

const void *Buffer(void) const;
size_t BufferLength(void) const;


virtual size_t Read(void *buffer, size_t numBytes) ;
virtual size_t ReadAt(off_t position, void *buffer, size_t numBytes);

virtual size_t Write(const void *buffer, size_t numBytes);
virtual size_t WriteAt(off_t position, const void *buffer, size_t numBytes);

virtual off_t Position(void) const;
virtual off_t Seek(off_t position, int32 mode);

void SetBlockSize(size_t blockSize);
virtual status_t SetSize(off_t numBytes);

private:

	char *fBuffer;
	uint32 fBlockSize;
	uint32 fLength;
	uint32 fPosition;
	uint32 fAlloc;
};

#endif