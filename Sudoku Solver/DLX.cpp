#include "DancingLinks.cpp"

class Stack
{
public:
	int size;
	Node **data;

	Stack ( int MAX = 1 )
	{
		data = new Node*[ MAX ];
		size = 0;
	}

	inline bool isEmpty ( )
	{
		return ( !size );
	}

	void push ( Node *data )
	{
		this->data[ size++ ] = data;
	}

	Node *pop ( )
	{
		if ( !isEmpty ( ) )
			return data[ --size ];
		else
			return nullptr;
	}
};

class DLX
{
public:
	int N, Nto2, Nto3, Nto4, Nto5, Nto6;
	DancingLinks *dl;
	Stack *solutionRows;

	inline int retNb ( int index )
	{
		return 1 + index / Nto2;
	}

	inline int retRw ( int index )
	{
		return 1 + Nto4 + ( index / Nto4 * Nto2 ) + ( index % Nto2 );
	}

	inline int retCl ( int index )
	{
		return 1 + 2 * Nto4 + index % Nto4;
	}

	inline int retBx ( int index )
	{
		return 1 + 3 * Nto4 + ( ( index / Nto5 * Nto3 ) + ( ( ( index / Nto3 ) % N ) * Nto2 ) + ( index % Nto2 ) );
	}

	inline int getIndex ( int num, int row, int col )
	{
		return ( num + col * Nto2 + row * Nto4 );
	}

	inline DLX ( int n )
	{
		N = n;
		Nto6 = ( Nto5 = ( Nto4 = ( Nto3 = ( Nto2 = N * N ) * N ) * N ) * N ) * N;
		dl = new DancingLinks ( Nto6, 4 * Nto4 );
		for ( int i = 1; i <= Nto6; i++ )
		{
			dl->insert ( i, retNb ( i - 1 ) );
			dl->insert ( i, retRw ( i - 1 ) );
			dl->insert ( i, retCl ( i - 1 ) );
			dl->insert ( i, retBx ( i - 1 ) );
		}
		solutionRows = new Stack ( Nto6 );
	}

	inline void init ( int **grid )
	{
		for ( int i = 0; i < Nto2; i++ )
			for ( int j = 0; j < Nto2; j++ )
				if ( grid[ i ][ j ] )
					addToSolution ( getIndex ( grid[ i ][ j ], i, j ) - 1 );
	}

	inline void coverHelper ( Node *p )
	{
		Node *q = p->down;
		while ( q != p )
		{
			if ( q != q->colHeader )
				dl->coverRow ( q );
			q = q->down;
		}
		q->colHeader->coverHorizontal ( );
		dl->root->col--;
	}

	inline void addToSolution ( int index )
	{
		addToSolution ( dl->rowHeaders[ index ]->right );
	}

	inline void addToSolution ( Node *p )
	{
		Node *rowHead = p;
		solutionRows->push ( p );
		do
		{
			if ( p->col )
				coverHelper ( p );
			else
				p->coverVertical ( );
			p = p->right;
		} while ( p != rowHead );
	}

	inline bool find ( int *visited, int n, int row )
	{
		for ( int i = 0; i < n; i++ )
			if ( visited[ i ] == row )
				return true;
		return false;
	}

	inline void uncover ( Node *p )
	{
		Node *end = p;
		int *visited = new int[ 4 * Nto2 ];
		int n = 0;
		do
		{
			if ( p->col )
			{
				dl->uncoverCol ( p );
				Node *q = p->colHeader->down;
				q->colHeader->count--;
				while ( q != q->colHeader )
				{
					if ( !find ( visited, n, q->row ) )
					{
						dl->uncoverRow ( q );
						visited[ n++ ] = q->row;
					}
					q = q->down;
				}
			}
			else
				p->uncover ( );
			p = p->right;
		} while ( p != end );
	}

	inline Node *getMinCol ( )
	{
		Node *min = dl->root->right;
		for ( Node *i = min->right; i != dl->root; i = i->right )
		{
			if ( i->count < min->count )
				min = i;
			if ( min->count <= 1 )
				return min;
		}
		return min;
	}

	inline bool solve ( )
	{
		if ( dl->root->down == dl->root )
			return true;
		Node *min = getMinCol ( );
		for ( Node *i = min->down; i != min; i = i->down )
		{
			addToSolution ( i );
			if ( solve ( ) )
				return true;
			uncover ( solutionRows->pop ( ) );
		}
		return false;
	}

	inline void fill_grid ( int **grid )
	{
		int index;
		while ( !solutionRows->isEmpty ( ) )
		{
			index = solutionRows->pop ( )->row - 1;
			grid[ index / Nto4 ][ ( index % Nto4 ) / Nto2 ] = 1 + ( index % Nto2 );
		}
	}
};