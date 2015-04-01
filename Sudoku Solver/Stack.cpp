#include <intsafe.h>

class Stack
{
public:
	int *data, size;

	Stack ( )
	{
		data = new int[ 0xFFFFF ];
		size = -1;
	}

	bool isEmpty ( )
	{
		return ( size == -1 );
	}

	void push ( int data )
	{
		this->data[ ++size ] = data;
	}

	int top ( )
	{
		if ( !isEmpty ( ) )
			return data[ size ];
		else
			return INT_MAX;
	}

	int pop ( )
	{
		if ( !isEmpty ( ) )
			return data[ size-- ];
		else
			return INT_MAX;
	}

	int size ( )
	{
		return size + 1;
	}
};