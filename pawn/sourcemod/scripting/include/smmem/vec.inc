#if defined _smmem_vec_included
	#endinput
#endif
#define _smmem_vec_included

#include <smmem>

#if 0

CUtlVector<Data> {
    Data *m_pMemory;		// +0	Ptr to items
    int m_nAllocationCount;	// +4	Amount of allocated space
    int m_nGrowSize;		// +8	Size by which memory grows
    int m_Size;				// +12	Number of items in vector
    Data *m_pElements;		// +16	Same as m_pMemory, used for debugging
}

#endif


stock CUtlVector operator+(CUtlVector oper1, int oper2)
{
	return oper1 + view_as< CUtlVector >(oper2);
}

stock CUtlVector operator+(int oper1, CUtlVector oper2)
{
	return view_as< CUtlVector >(oper1) + oper2;
}

methodmap CUtlVector
{
	public CUtlVector(any data)
	{
		return view_as< CUtlVector >(data);
	}	

	public static CUtlVector ToVector(any data)
	{
		return view_as< CUtlVector >(data);
	}

	property any m_pMemory
	{
		public get()
		{
			return Deref(this);
		}
		public set(const any val)
		{
			WriteVal(this, val);
		}
	}

	property int m_nAllocationCount
	{
		public get()
		{
			return Deref(this + 4);
		}
		public set(const int val)
		{
			WriteVal(this + 4, val);
		}
	}

	property int m_nGrowSize
	{
		public get()
		{
			return Deref(this + 8);
		}
		public set(const int val)
		{
			WriteVal(this + 8, val);
		}
	}

	property int m_Size
	{
		public get()
		{
			return Deref(this + 12);
		}
		public set(const int val)
		{
			WriteVal(this + 12, val);
		}
	}

	property any m_pElements
	{
		public get()
		{
			return Deref(this + 16);
		}
		public set(const any val)
		{
			WriteVal(this + 16, val);
		}
	}

	public int Count()
	{
		return this.m_Size;
	}

	public bool IsValidIndex(int i)
	{
		return (0 <= i < this.m_Size);
	}

	public static int InvalidIndex()
	{
		return -1;
	}

	// Ok this is actually wrong. Valve uses their own memory shit 
	// which makes me wonder if this will even work
	public void Grow(int alloccount, int size = 4)
	{
		this.m_nAllocationCount += alloccount;
		if (this.m_pMemory)
			this.m_pMemory = Realloc(this.m_pMemory, this.m_nAllocationCount * size);
		else this.m_pMemory = Malloc(this.m_nAllocationCount * size);
	}

	public void GrowVector(int num = 1, int size = 4)
	{
		if (this.m_Size + num > this.m_nAllocationCount)
			this.Grow(this.m_Size + num - this.m_nAllocationCount, size);
		this.m_Size += num;
	}

	public void ShiftElementsRight(int elem, int num = 1, int size = 4)
	{
		int numtomove = this.m_Size - elem - num;
		if (numtomove > 0 && num > 0)
			MemMove(this.m_pMemory + (elem + num) * size, this.m_pMemory + elem * size, numtomove * size);
	}

	public void ShiftElementsLeft(int elem, int num = 1, int size = 4)
	{
		int numtomove = this.m_Size - elem - num;
		if (numtomove > 0 && num > 0)
			MemMove(this.m_pMemory + elem * size, this.m_pMemory + (elem + num) * size, numtomove * size);
	}

	public void Construct(int elem, any src = 0, int size = 4)
	{
		WriteVal(this.m_pMemory + elem * size, src);
	}

	public any Element(int i, int size = 4)
	{
		return this.m_pMemory + i * size;
	}

	public any Get(int i, int size = 4)
	{
		return this.m_pMemory + i * size;
	}

	public any GetEx(int i, int size = 4)
	{
		return Deref(this.m_pMemory + i * size);
	}

	public any Head(int size = 4)
	{
		return this.Element(0, size);
	}

	public any Tail(int size = 4)
	{
		return this.Element(this.m_Size - 1, size);
	}

	public int InsertBefore(int elem, any src = 0, int size = 4)
	{
		this.GrowVector();
		this.ShiftElementsRight(elem, size);
		this.Construct(elem, src, size);
		return elem;
	}

	public int InsertAfter(int elem, any src = 0, int size = 4)
	{
		return this.InsertBefore(elem + 1, src, size);
	}

	public int AddToHead(any data = 0, int size = 4)
	{
		return this.InsertBefore(0, data, size);
	}

	public int AddToTail(any data = 0, int size = 4)
	{
		return this.InsertBefore(this.m_Size, data, size);
	}

	public int Find(any src, int size = 4)
	{
		int count = this.m_Size;
		for (int i = 0; i < count; ++i)
			if (this.Element(i, size) == src)
				return i;
		return -1;
	}

	public int FindEx(any src, int size = 4)
	{
		int count = this.m_Size;
		for (int i = 0; i < count; ++i)
			if (this.GetEx(i, size) == src)
				return i;
		return -1;
	}

	public void FillWithValue(any val, int size = 4)
	{
		int count = this.m_Size;
		for (int i = 0; i < count; ++i)
			WriteVal(this.m_pMemory + i * size, val);
	}

	public bool HasElement(any val, int size = 4)
	{
		return this.Find(val, size) >= 0;
	}

	public void Destruct(int elem, int size = 4)
	{
		#pragma unused elem
		#pragma unused size
		// Okay so freeing random shit is bad, I think we can all agree on that
		// Destruct calls the destructor which can be literally anything so do it yourself ig
//		WriteVal(this.m_pMemory + elem * size, 0);
	}

	public void FastRemove(int elem, int size = 4)
	{
		this.Destruct(elem);
		if (this.m_Size > 0)
		{
			if (elem != this.m_Size - 1)
				MemCopy(this.m_pMemory + elem * size, this.m_pMemory + (this.m_Size - 1) * size, size);
			--this.m_Size;
		}
	}

	public void Remove(int elem, int size = 4)
	{
		this.Destruct(elem, size);
		this.ShiftElementsLeft(elem, size);
		--this.m_Size;
	}

	public bool FindAndRemove(any src, int size = 4)
	{
		int elem = this.Find(src, size);
		if (elem != -1)
		{
			this.Remove(elem, size);
			return true;
		}
		return false;
	}

	public bool FindAndFastRemove(any src, int size = 4)
	{
		int elem = this.Find(src, size);
		if (elem != -1)
		{
			this.FastRemove(elem, size);
			return true;
		}
		return false;
	}

	public void RemoveAll(int size = 4)
	{
		for (int i = this.m_Size; --i >= 0;)
			this.Destruct(i, size);
		this.m_Size = 0;
	}

	public ArrayList ToArrayList(int size = 4)
	{
		ArrayList list = new ArrayList(ByteCountToCells(size));
		int len = this.m_Size;
		for (int i = 0; i < len; ++i)
			list.Push(this.Element(i));

		return list;
	}

	public void ToArray(any[] array)
	{
		int len = this.m_Size;
		for (int i = 0; i < len; ++i)
			array[i] = this.Element(i);
	}

	public void Set(int index, any src = 0, int size = 4, int offset = 0, NumberType numt = NumberType_Int32)
	{
		WriteVal(this.m_pMemory + index * size + offset, src, numt);
	}
}
