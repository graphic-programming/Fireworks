/*
   Class Name:

      CLinkedList

   Description:

      linked list FIFO  (first in, first out)
*/

#pragma once

typedef struct LINKEDLIST LINKEDLIST;

struct LINKEDLIST{
	float vx, vy, sx, sy, x, y;
	float t;
	LINKEDLIST *Prev, *Next;
};

class CLinkedList
{
private:
	LINKEDLIST *First,*Last,*Cursor;
	unsigned int Count;

public:
	CLinkedList();
	~CLinkedList();

	bool IsEmpty();
	void Add(float vx, float vy, float sx, float sy, float t, float x, float y);
	void Remove();
	void Remove(LINKEDLIST *Node);
	void RemoveAll();

	void MoveFirst();
	void MoveNext();
	bool IsEndOfQueue();
	LINKEDLIST *Read(float *vx, float *vy, float *sx, float *sy, float *t);
	void Read(float *x, float *y);
	void Update(float x, float y, float t);

	unsigned int GetCount();
};

