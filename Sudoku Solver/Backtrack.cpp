class Backtrack
{
public:
	int **grid, N, NSQR;

	Backtrack ( int **grid, int N )
	{
		this->grid = grid;
		this->N = N;
		NSQR = N * N;
	}

	inline bool checkRow ( int **grid, int val, int row )
	{
		for ( int i = 0; i < NSQR; i++ )
			if ( grid[ row ][ i ] == val )
				return false;
		return true;
	}

	inline bool checkCol ( int **grid, int val, int col )
	{
		for ( int i = 0; i < NSQR; i++ )
			if ( grid[ i ][ col ] == val )
				return false;
		return true;
	}

	inline bool checkBox ( int **grid, int val, int row, int col )
	{
		for ( int i = row; i < row + N; i++ )
			for ( int j = col; j < col + N; j++ )
				if ( grid[ i ][ j ] == val )
					return false;
		return true;
	}

#define ISSAFE(grid, val, row, col)			( ( (grid)[ (row) ][ (col) ] == 0 ) && \
											Backtrack::checkBox( (grid), (val), (row/N*N), (col/N*N) ) && \
											Backtrack::checkCol ( (grid), (val), (col) ) && \
											Backtrack::checkRow ( (grid), (val), (row) ) )

	bool getEmpty ( int **grid, int *row, int *col )
	{
		for ( int i = 0; i < NSQR; i++ )
			for ( int j = 0; j < NSQR; j++ )
				if ( grid[ i ][ j ] == 0 )
				{
			*row = i;
			*col = j;
			return true;
				}
		return false;
	}

	bool solve ( int **grid )
	{
		int row = 0, col = 0;
		if ( !getEmpty ( grid, &row, &col ) )
			return true;
		for ( int guess = 1; guess <= NSQR; guess++ )
			if ( ISSAFE ( grid, guess, row, col ) )
			{
			grid[ row ][ col ] = guess;
			if ( solve ( grid ) )
				return true;
			grid[ row ][ col ] = 0;
			}
		return false;
	}
#undef ISSAFE
};