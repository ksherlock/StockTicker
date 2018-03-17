#include "List.h"
#include <string.h>


BList::BList(int32 count)
{
	if (count < 1) count = 20;
	fAlloc = fAllocSize = count;
	fCount = 0;
	fData = new void *[fAlloc];
	for (register int32 i = 0; i < fAlloc; i++)
		fData[i] = 0;
}

BList::BList(const BList &list)
{
	register int32 i;

	fCount = list.fCount;
	fAlloc = list.fAlloc;
	fAllocSize = list.fAllocSize;
	fData = new void *[fAllocSize];

	for (i = 0; i < fCount; i++)
		fData[i] = list.fData[i];
	for ( ; i < fAllocSize; i++)
		fData[i] = NULL;
}

BList::~BList()
{
	if (fData) delete []fData; 
}

BList& BList::operator =(const BList& list)
{
	register int32 i;

	if (this == &list) return *this;

	if (fData) delete []fData;
	fAlloc = list.fAlloc;
	fAllocSize = list.fAllocSize;
	fCount = list.fCount;

	fData = new void *[fAlloc];

	for (i = 0; i < fCount; i++)
	{
		fData[i] = list.fData[i];
	}
	for ( ; i < fAlloc; i++)
	{
		fData[i] = NULL;
	}

	return *this;
}


int32 BList::CountItems(void) const
{
	return fCount;
}
bool BList::IsEmpty(void) const
{
	return (fCount == 0);
}

int32 BList::IndexOf(const void *item) const
{
	for (register int32 i = 0; i < fCount; i++)
		if (fData[i] == item) return i;

	return -1;
}

bool BList::HasItem(const void *item) const
{
	for (register int32 i = 0; i < fCount; i++)
		if (fData[i] == item) return true;

	return false;
}

void *BList::ItemAt(int32 index) const
{
	if (index >= fCount || index < 0) return NULL;
	return fData[index];
}

void BList::MakeEmpty(void)
{
	for (register int32 i = 0; i < fCount; i++)
		fData[i] = NULL;

	fCount = 0;
}

void BList::DoForEach(bool (*func)(void *))
{
	for (register int32 i = 0; i < fCount; i++)
	{
		if ((func)(fData[i])) return;
	}
}

void BList::DoForEach(bool (*func)(void *, void *), void *arg)
{
	for (register int32 i = 0; i < fCount; i++)
	{
		if ((func)(fData[i], arg)) return;
	}
}

void *BList::FirstItem(void) const
{
	return fData[0];
}
void *BList::LastItem(void) const
{
	return fCount ? fData[fCount-1] : NULL;
}
void *BList::Items(void) const
{
	return (void *)fData;
}

void BList::SortItems(int (*func)(const void *, const void *))
{
	//void *newArray[fCount];

	// do a standard bubble sort
	for (int32 i = 0; i < fCount; i++)
	{
		void *item = fData[i];
		for (int32 j = i + 1; j < fCount; j++)
		{
			int cmp = (func)(item, fData[j]);	// beos does &fData ??
			if (cmp > 0) // second is before first
			{
				item = fData[j];	// new lowest item;
				fData[j] = fData[i];
				fData[i] = item;
			}
		}
	}
}

bool BList::AddItem(void *item, int32 index)
{
	if (index < 0 || index > fCount) return false;

	// need more memory.
	if (fCount == fAlloc)
	{
		void **temp = new void *[fAlloc + fAllocSize];
		if (temp == NULL) return false;
		fAlloc += fAllocSize;

		// copy items before the index
		for (register int32 i = 0; i < index; i++)
		{
			temp[i] = fData[i];
		}

		temp[i++] = item;

		// copy ones after the index
		for ( ; i < fCount; i++)
		{
			temp[i] = fData[i - 1]; 
		}

		// null out the newly allocated memory

		for ( ; i < fAllocSize; i++)
		{
			temp[i] = NULL;
		}
		delete []fData;
		fData = temp;
		fCount++;
		return true;
	}
	else
	{
		// move everything at index -> index + 1
		for (register int32 i = index; i < fCount; i++)
		{
			fData[i + 1] = fData[i];
		}
		fData[index] = item;
		fCount++;
	}

	return true;
}

bool BList::AddItem(void *item)
{
	if (fCount == fAlloc)
	{
		void **temp = new void *[fAlloc + fAllocSize];
		if (temp == NULL) return false;
		fAlloc += fAllocSize;

		for (register int32 i = 0; i < fCount; i++)
		{
			temp[i] = fData[i];
		}
		for ( ; i < fAllocSize; i++)
		{
			temp[i] = NULL;
		}
		delete []fData;
		fData = temp;
	}
	fData[fCount++] = item;
	return true;
}

bool BList::RemoveItem(const void *item)
{
int32 index;

	index = IndexOf(item);
	if (index < 0) return false;
	RemoveItem(index);
	return true;
}

void *BList::RemoveItem(int32 index)
{
	void *data;

	if (index < 0 || index >= fCount) return NULL;

	fCount--;
	data = fData[fCount];

	if (index != fCount)
	{
		for (register int32 i = index; i < fCount; i++)
		{
			fData[i] = fData[i + 1];
		}
	}
	fData[fCount] = NULL;
	return data;
}

/*
 *
 * shortcut for (i = 0; i < count; i++) RemoveItem(index + i);
 *
 */
bool BList::RemoveItems(int32 index, int32 count)
{
	int32 oldCount = fCount;
	register int32 i;

	if (index < 0 || index >= fCount) return false;


	if (count + index >= fCount) fCount = index;
	else fCount -= count;

	for (i = index; i < fCount; i++)
	{
		fData[i] = fData[i + count];
	}
	for ( ; i < oldCount; i++)
	{
		fData[i] = NULL;
	}
	return true;
}