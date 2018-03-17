#ifndef __BENAPHORE_H__
#define __BENAPHORE_H__

class Benaphore
{
public:
	Benaphore(int count = 10);
	~Benaphore();

void Lock(void);
void Unlock(void);

private:

	void *fSemaphore;
	long fFlag;
};

#endif