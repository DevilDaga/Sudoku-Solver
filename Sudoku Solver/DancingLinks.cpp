class Node
{
public:
	unsigned row, col, count;
	Node *up, *down, *right, *left, *colHeader;

	inline Node ( unsigned row, unsigned col )
	{
		count = 0;
		this->row = row;
		this->col = col;
		up = down = left = right = colHeader = this;
	}

	inline void cover ( )
	{
		left->right = right;
		right->left = left;
		up->down = down;
		down->up = up;
	}

	inline void coverVertical ( )
	{
		up->down = down;
		down->up = up;
	}

	inline void coverHorizontal ( )
	{
		left->right = right;
		right->left = left;
	}

	inline void uncover ( )
	{
		left->right = right->left = up->down = down->up = this;
	}
};

class DancingLinks
{
public:
	unsigned m, n;
	Node *root, **colHeaders, **rowHeaders;

	inline DancingLinks ( unsigned m, unsigned n )
	{
		this->m = m;
		this->n = n;
		root = new Node ( m, n );
		colHeaders = new Node*[ n ];
		rowHeaders = new Node*[ m ];
		for ( unsigned i = 0; i < m; i++ )
			rowHeaders[ i ] = new Node ( i + 1, 0 );
		for ( unsigned i = 0; i < n; i++ )
			colHeaders[ i ] = new Node ( 0, i + 1 );
		colHeaders[ 0 ]->right = colHeaders[ 1 ];
		colHeaders[ n - 1 ]->left = colHeaders[ n - 2 ];
		rowHeaders[ 0 ]->down = rowHeaders[ 1 ];
		rowHeaders[ m - 1 ]->up = rowHeaders[ m - 2 ];
		root->right = colHeaders[ 0 ];
		root->left = colHeaders[ n - 1 ];
		root->down = rowHeaders[ 0 ];
		root->up = rowHeaders[ m - 1 ];
		root->uncover ( );
		for ( unsigned i = 1; i < m - 1; i++ )
		{
			rowHeaders[ i ]->up = rowHeaders[ i - 1 ];
			rowHeaders[ i ]->down = rowHeaders[ i + 1 ];
		}
		for ( unsigned i = 1; i < n - 1; i++ )
		{
			colHeaders[ i ]->left = colHeaders[ i - 1 ];
			colHeaders[ i ]->right = colHeaders[ i + 1 ];
		}
	}

	inline void insert ( unsigned row, unsigned col )
	{
		if ( col > n || row > m )
			return;
		Node *cell = new Node ( row, col ),
			*p = rowHeaders[ row - 1 ]->right,
			*q = colHeaders[ col - 1 ]->down;
		while ( p->col < col && p != rowHeaders[ row - 1 ] )
			p = p->right;
		while ( q->row < row && q != q->colHeader )
			q = q->down;
		if ( q->row == row && p->col == col )
			return;
		cell->right = p;
		cell->left = p->left;
		cell->down = q;
		cell->up = q->up;
		cell->colHeader = q->colHeader;
		cell->uncover ( );
		cell->colHeader->count++;
	}

	inline void coverRow ( Node *p )
	{
		Node *end = p;
		p = p->right;
		while ( p != end )					// Leave end be ?
		{
			p->coverVertical ( );
			if ( p->col )
				p->colHeader->count--;
			p = p->right;
		}
		p->colHeader->count--;
	}

	inline void uncoverRow ( Node *p )
	{
		Node *end = p;
		do
		{
			p->uncover ( );
			if ( p->col )
				p->colHeader->count++;
			p = p->right;
		} while ( p != end );
	}

	inline void uncoverCol ( Node *p )
	{
		Node *end = p;
		do
		{
			p->uncover ( );
			p = p->down;
		} while ( p != end );
		root->col++;
	}
};