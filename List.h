#ifndef __BLIST_H__
#define __BLIST_H__

#include "SupportDefs.h"

class BList
{
public:
	BList(int32 count = 20);
	BList(const BList& list);
virtual ~BList();

	bool	AddItem(void *item);
	bool	AddItem(void *item, int32 index);

	bool	AddList(const BList &list);
	bool	AddList(const BList &list, int32 index);

	int32	CountItems(void) const;
	// calls func for items 1 .. end or until the function returns true.
	void	DoForEach(bool (*func)(void *));
	void	DoForEach(bool (*func)(void *, void *), void *arg);
	void	*FirstItem(void) const;
	void	*LastItem(void) const;
	void	*ItemAt(int32 index) const;
	void	*Items(void) const;
	bool	HasItem(const void *item) const;
	int32	IndexOf(const void *item) const;
	bool	IsEmpty(void) const;
	void	MakeEmpty(void);

	bool	RemoveItem(const void *item);
	void	*RemoveItem(int32 index);
	bool	RemoveItems(int32 index, int32 count);

	void	SortItems(int (*func)(const void *, const void *));

	BList&	operator = (const BList &list);

private:

	void	**fData;
	int32	fCount;
	int32	fAlloc;
	int32	fAllocSize;
};


#endif