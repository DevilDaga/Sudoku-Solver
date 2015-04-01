#include <stdio.h>
#include <windows.h>
#include <math.h>

class Node
{
public:
	int index, count;
	Node *prev, *next;

	Node ( )
	{
		count = index = 0;
		prev = next = NULL;
	}

	Node ( int index )
	{
		count = 0;
		this->index = index;
		prev = next = NULL;
	}

	Node *uncover ( )
	{
		next->prev = this;
		prev->next = this;
		return this;
	}

	Node *cover ( )
	{
		prev->next = next;
		next->prev = prev;
		return this;
	}

	void addNext ( Node *p )
	{
		p->next = this->next;
		p->prev = this;
		next = p;
		p->next->prev = p;
	}

	void addPrev ( Node *p )
	{
		p->prev = this->prev;
		p->next = this;
		prev = p;
		p->prev->next = p;
	}

	int remove ( )
	{
		if ( next == this )
			next = prev = NULL;
		cover ( );
		return index;
	}
};

class Head
{
public:
	Node *start, *end;
	int length;

	Head ( )
	{
		start = end = NULL;
		length = 0;
	}

	void insert ( int index )
	{
		Node *p = new Node ( index );
		length++;
		if ( !start )
		{
			start = end = p;
			start->next = end->next = p;
			start->prev = end->prev = p;
		}
		else
		{
			end->addNext ( p );
			end = end->next;
			end->next = start;
			start->prev = end;
		}
	}

	void insert ( Node *p )
	{
		int index = p->index;
		length++;
		if ( !start )
		{
			start = end = p;
			start->next = end->next = p;
			start->prev = end->prev = p;
		}
		else
		{
			//Node *q = start;
			//do
			//{
			//	if ( q->index > p->index )
			//	{
			//		q->addPrev ( p );
			//		return;
			//	}
			//	if ( q->index == p->index )
			//	{
			//		length--;
			//		return;
			//	}
			//	q = q->next;
			//} while ( q != start );
			//length--;
			//insert ( p->index );
			end->addNext ( p );
			end = end->next;
			end->next = start;
			start->prev = end;
		}
	}

	void sorted_insert ( Node *p )
	{
		int index = p->index;
		length++;
		if ( !start )
		{
			start = end = p;
			start->next = end->next = p;
			start->prev = end->prev = p;
		}
		else
		{
			Node *q = start;
			do
			{
				if ( q->index > p->index )
				{
					q->addPrev ( p );
					if ( q == start )
					{
						start = start->prev;
						end->next = start;
					}
					return;
				}
				//if ( q->index == p->index )
				//{
				//	length--;
				//	return;
				//}
				q = q->next;
			} while ( q != start );
			if ( q == start )
				insert ( p );
		}
	}

	Node *remove ( int index )
	{
		if ( !start )
			return NULL;
		length--;
		if ( start == end && start->index == index )
		{
			Node *temp = start;
			start = end = NULL;
			return temp;
		}
		Node *p = start;
		do
		{
			if ( index == p->index )
			{
				if ( p == end )
				{
					end = p->prev;
					start->prev = end;
				}
				if ( p == start )
				{
					start = start->next;
					end->next = start;
				}
				p->cover ( );
				return p;
			}
			p = p->next;
		} while ( p != start );
		length++;
		return NULL;
	}

	Node *removeLast ( )
	{
		if ( !start )
			return NULL;
		if ( start == end )
		{
			Node *temp = start;
			start = end = NULL;
			return temp;
		}
		Node *p = end;
		p->cover ( );
		end = end->prev;
		length--;
		return p;
	}

	Node *find ( int index )
	{
		Node *p = start;
		do
		{
			if ( index == p->index )
				return p;
			p = p->next;
		} while ( p->index <= index && p != start );
		return NULL;
	}

	Node *findByCount ( int count )
	{
		Node *p = start;
		do
		{
			if ( count == p->count )
				return p;
			p = p->next;
		} while ( p != start );
		return NULL;
	}

	Node *at ( int n )
	{
		if ( !start )
			return NULL;
		Node *p = start;
		for ( int i = 0; i < n % length; i++ )
			p = p->next;
		return p;
	}
};

#define DLXOLD				DancingLinks_old

class DancingLinks_old
{
public:
	int N, Nto2, Nto3, Nto4, Nto5, Nto6;
	Head *curRows, *curCols,
		solutionRows, discardedRows, discardedCols;
	bool **matrix;

	DancingLinks_old ( int n )
	{
		N = n;
		Nto2 = N * N;
		Nto3 = Nto2 * N;
		Nto4 = Nto3 * N;
		Nto5 = Nto4 * N;
		Nto6 = Nto5 * N;
		int rowCount = Nto6,
			colCount = 4 * Nto4;
		curRows = new Head ( );
		curCols = new Head ( );
		matrix = new bool *[ rowCount ];
		for ( int i = 0; i < rowCount; i++ )
		{
			curRows->insert ( i );
			matrix[ i ] = new bool[ colCount ];
			for ( int j = 0; j < colCount; j++ )
				matrix[ i ][ j ] = false;
		}
		curCols->insert ( 0 );
		Node *cur2 = curCols->start;
		for ( int i = 1; i < colCount; i++ )
		{
			cur2->count = Nto2;
			curCols->insert ( i );
			cur2 = cur2->next;
		}
		cur2->count = Nto2;
		for ( int i = 0; i < rowCount; i++ )
			setRow ( i );
	}

#define ROWOFFSET			Nto4
#define COLOFFSET			2 * Nto4
#define BOXOFFSET			3 * Nto4

	inline int retNb ( int index )
	{
		return index / Nto2;
	}

	inline int retRw ( int index )
	{
		return ( index / Nto4 * Nto2 ) + ( index % Nto2 );
	}

	inline int retCl ( int index )
	{
		return index % Nto4;
	}

	inline int retBx ( int index )
	{
		return ( ( index / Nto5 * Nto3 ) + ( ( ( index / Nto3 ) % N ) * Nto2 ) + ( index % Nto2 ) );
	}

	inline int getIndex ( int num, int row, int col )
	{
		return ( num - 1 + col * Nto2 + row * Nto4 );
	}

	inline void setRow ( int index )
	{
		matrix[ index ][ retNb ( index ) ] = true;
		matrix[ index ][ ROWOFFSET + retRw ( index ) ] = true;
		matrix[ index ][ COLOFFSET + retCl ( index ) ] = true;
		matrix[ index ][ BOXOFFSET + retBx ( index ) ] = true;
	}

	inline void decrement ( Node *p )
	{
		if ( p )
		{
			p->count--;
			//if ( p->index == 1219 /*&& p->count < 7*/ )
			//	__debugbreak ( );
		}
	}

	inline void increment ( Node *p )
	{
		if ( p )
		{
			p->count++;
			//if ( p->index == 1219 )
			//	__debugbreak ( );
		}
	}

	void coverRow ( int index )
	{
		//Node *p = curRows->find ( index )->cover( );
		//if ( p == curRows->start )
		//	curRows->start = p->next;
		//if ( p == curRows->end )
		//	curRows->end = p->prev;
		decrement ( curCols->find ( retNb ( index ) ) );
		decrement ( curCols->find ( ROWOFFSET + retRw ( index ) ) );
		decrement ( curCols->find ( COLOFFSET + retCl ( index ) ) );
		decrement ( curCols->find ( BOXOFFSET + retBx ( index ) ) );
		//return p;
	}

	void uncoverRow ( Node *p )
	{
		//p->uncover ( );
		increment ( curCols->find ( retNb ( p->index ) ) );
		increment ( curCols->find ( ROWOFFSET + retRw ( p->index ) ) );
		increment ( curCols->find ( COLOFFSET + retCl ( p->index ) ) );
		increment ( curCols->find ( BOXOFFSET + retBx ( p->index ) ) );
	}

	void coverCol ( Node *p, int *covered_rows, int *covered_cols )
	{
		if ( !p )
			return;
		Node *q = curRows->start;
		int  i, limit = curRows->length;
		//discardedCols.insert ( p->cover ( ) );
		discardedCols.insert ( curCols->remove ( p->index ) );
		( *covered_cols )++;
		for ( int k = 0; k < limit; k++ )
		{
			i = q->index;
			q = q->next;
			if ( matrix[ i ][ p->index ] )
			{
				//discardedRows.insert ( coverRow ( i ) );
				coverRow ( i );
				discardedRows.insert ( curRows->remove ( i ) );
				( *covered_rows )++;
			}
		}
		if ( p == curCols->start )
			curCols->start = p->next;
		if ( p == curCols->end )
			curCols->end = p->prev;
	}

	void addToSolution ( int index, int *covered_rows, int *covered_cols )
	{
		solutionRows.insert ( curRows->remove ( index ) );
		coverCol ( curCols->find ( retNb ( index ) ), covered_rows, covered_cols );
		coverCol ( curCols->find ( ROWOFFSET + retRw ( index ) ), covered_rows, covered_cols );
		coverCol ( curCols->find ( COLOFFSET + retCl ( index ) ), covered_rows, covered_cols );
		coverCol ( curCols->find ( BOXOFFSET + retBx ( index ) ), covered_rows, covered_cols );
	}

	void coverColHelper ( Node *p, int occurence, int *covered_rows, int *covered_cols )
	{
		Node *q = curRows->start;
		while ( true )
		{
			if ( matrix[ q->index ][ p->index ] )
				if ( occurence )
					occurence--;
				else
					break;
			q = q->next;
			if ( q == curRows->start )
				__debugbreak ( );
		}
		addToSolution ( q->index, covered_rows, covered_cols );
	}

	void tempFunc ( )
	{
		//solutionRows.insert ( curRows->remove ( 8 ) );
		//solutionRows.insert ( curRows->remove ( 9 ) );
		//solutionRows.insert ( curRows->remove ( 10 ) );
		//solutionRows.insert ( curRows->remove ( 11 ) );
		//solutionRows.insert ( coverRow ( 8 ) );
		//solutionRows.insert ( coverRow ( 9 ) );
		//solutionRows.insert ( coverRow ( 10 ) );
		//solutionRows.insert ( coverRow ( 11 ) );
		debug_print ( );
		//printf ( "\n\n\n\n\n" );
		//while ( solutionRows.start )
		//	curRows->insert ( solutionRows.removeLast ( ) );
		//curRows->insert ( solutionRows.remove ( 8 ) );
		//curRows->insert ( solutionRows.remove ( 9 ) );
		//curRows->insert ( solutionRows.remove ( 10 ) );
		//curRows->insert ( solutionRows.remove ( 11 ) );
		//printf ( "%d\n", getMinCol ( 1 )->count );
		//printf ( "%d\n", getMinCol ( 2 )->count );
		//debug_print ( );
	}

	Node *getMinCol ( int n )
	{
		if ( n > curCols->length )
			return NULL;
		int *counts = new int[ curCols->length ];
		Node *p = curCols->start;
		for ( int i = 0; i < curCols->length; i++, p = p->next )
			counts[ i ] = p->count;
		for ( int i = 0; i < n; i++ )
			for ( int j = curCols->length - 1; j > i; j-- )
				if ( counts[ j - 1 ] > counts[ j ] )
				{
					int temp = counts[ j - 1 ];
					counts[ j - 1 ] = counts[ j ];
					counts[ j ] = temp;
				}
		return curCols->findByCount ( counts[ n - 1 ] );
	}

	bool solve ( )
	{
		if ( !curRows->length )
			return true;
			//if ( !curCols->length )
			//	return true;
			//else
			//	return false;
		Node *q = curCols->start;
		int limit = curCols->end->index;
		for ( int i = q->index, j = 1; i < curCols->end->index; j++, q = q->next )
		{
			i = q->index;
			Node *p = getMinCol ( j );
			if ( !p )
				return false;
			if ( p->count < 0 )
				__debugbreak ( );
			if ( !p->count )
				continue;
			for ( int k = 0; k < p->count; k++ )
			{
				int covered_rows = 0, covered_cols = 0;
				coverColHelper ( p, k, &covered_rows, &covered_cols );
				if ( solve ( ) )
					return true;
				while ( covered_cols )
				{
					p = discardedCols.removeLast ( );
					curCols->sorted_insert ( p );
					covered_cols--;
				}
				while ( covered_rows )
				{
					p = discardedRows.removeLast ( );
					curRows->sorted_insert ( p );
					uncoverRow ( p );
					covered_rows--;
				}
				curRows->sorted_insert ( solutionRows.removeLast ( ) );
			}
		}
		return false;
	}

	void fill_grid ( int **grid )
	{
		Node *p = solutionRows.start;
		do 
		{
			grid[ p->index / Nto4 ][ ( p->index % Nto4 ) / Nto2 ] = 1 + ( p->index % Nto2 );
			p = p->next;
		} while ( p != solutionRows.start );
	}

	bool solve ( int **grid )
	{
		int garbage;
		for ( int i = 0; i < Nto2; i++ )
			for ( int j = 0; j < Nto2; j++ )
				if ( grid[ i ][ j ] )
					addToSolution ( getIndex ( grid[ i ][ j ], i, j ), &garbage, &garbage );
		if ( !solve ( ) )
			return false;
		fill_grid ( grid );
		return true;
	}

	void debug_print ( )
	{
		Node *p = curRows->start;
		if ( p )
		{
			printf ( "DLX GRID:\n\n" );
			do
			{
				printf ( "%d:\t", p->index );
				for ( int j = 0; j < 4 * Nto4; j++ )
					printf ( "%d", (int) matrix[ p->index ][ j ] );
				printf ( "\n" );
				p = p->next;
			} while ( p != curRows->start );
		}
		printf ( "\n\nSolution GRID:\n\n" );
		p = solutionRows.start;
		if ( p )
		{
			do
			{
				printf ( "%d:\t", p->index );
				for ( int j = 0; j < 4 * Nto4; j++ )
					printf ( "%d", (int) matrix[ p->index ][ j ] );
				printf ( "\n" );
				p = p->next;
			} while ( p != solutionRows.start );
		}
	}
};