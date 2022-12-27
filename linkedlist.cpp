/*
   Class Name:

      CLinkedList

   Description:

      linked list FIFO  (first in, first out)
*/

#include "stdafx.h"
#include "linkedlist.h"

// constructor
CLinkedList::CLinkedList()
{
	First = Last = Cursor = NULL;
	Count = 0;
}

// destructor
CLinkedList::~CLinkedList()
{
	RemoveAll();
}

// determine if list is empty
bool CLinkedList::IsEmpty()
{
	return (Count == 0);
}

// add node to the end of the list
//
//           First                  Last                      Node
//           +----+     +----+     +----+                    +----+
//           |    |---->|    |---->|    |---->NULL           |    |----> NULL
// NULL <----|    |<----|    |<----|    |           NULL<----|    |
//           +----+     +----+     +----+                    +----+
//
void CLinkedList::Add(float vx, float vy, float sx, float sy, float t, float x, float y)
{
	LINKEDLIST *Node;

	Node = new LINKEDLIST;

	Node->vx   = vx;
	Node->vy   = vy;
	Node->sx   = sx;
	Node->sy   = sy;
	Node->t    = t;
	Node->x    = x;
	Node->y    = y;
	Node->Prev = Node->Next = NULL;

	if(IsEmpty())
	{
		First = Last = Node;
	}
	else
	{
		Node->Prev = Last;

		Last->Next = Node;
		Last       = Node;
	}

	Count++;
}

// remove the first node of the list
//
//           First                                  Last
//           +----+          +----+     +----+     +----+
//           |    |---->     |    |---->|    |---->|    |----> NULL
// NULL <----|    |     <----|    |<----|    |<----|    |
//           +----+          +----+     +----+     +----+
//
void CLinkedList::Remove()
{
	Remove(First);
}

// remove node from the list
//
//           First                                                  Last
//           +----+     +----+          +----+          +----+     +----+
//           |    |---->|    |---->     |    |---->     |    |---->|    |----> NULL
// NULL <----|    |<----|    |     <----|    |     <----|    |<----|    |
//           +----+     +----+          +----+          +----+     +----+
//                       Node1           Node            Node2
//
void CLinkedList::Remove(LINKEDLIST *Node)
{
	if (Node == NULL) return;

	LINKEDLIST *Node1, *Node2;

	Node1 = Node->Prev;
	Node2 = Node->Next;

	if (Node1 != NULL) Node1->Next = Node2;
	if (Node2 != NULL) Node2->Prev = Node1;

	if (Node == First) First = Node2;
	if (Node == Last)  Last  = Node1;

	delete Node;
	Count--;
}

// remove all nodes
void CLinkedList::RemoveAll()
{
	while(!IsEmpty()) Remove();
}

// move to first node
void CLinkedList::MoveFirst()
{
	Cursor = First;
}

// move to next node
void CLinkedList::MoveNext()
{
	Cursor = Cursor->Next;
}

// determine if end of list is reached
bool CLinkedList::IsEndOfQueue()
{
	return (Cursor == NULL);
}

// read item at current node
LINKEDLIST *CLinkedList::Read(float *vx, float *vy, float *sx, float *sy, float *t)
{
	*vx = Cursor->vx;
	*vy = Cursor->vy;
	*sx = Cursor->sx;
	*sy = Cursor->sy;
	*t  = Cursor->t;

	return Cursor;
}

// read item at current node
void CLinkedList::Read(float *x, float *y)
{
	*x = Cursor->x;
	*y = Cursor->y;
}

// update item at current node
void CLinkedList::Update(float x, float y, float t)
{
	Cursor->x = x;
	Cursor->y = y;
	Cursor->t = t;
}

// return total node of the list
unsigned int CLinkedList::GetCount()
{
	return Count;
}

//