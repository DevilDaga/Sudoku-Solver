#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <windows.h>
#include <algorithm>
#include "Stack.cpp"

using namespace std;

class DLX
{
public:
	bool **matrix, *cols, *rows;
	int N, Nto2, Nto3, Nto4, Nto5, Nto6,
		maxRows, maxCols,
		colCount, rowCount, *count;
	Stack
		solutionRows, discardedRows, discardedCols;

	DLX ( int **grid, int n )
	{
		N = n;
		Nto6 = ( Nto5 = ( Nto4 = ( Nto3 = ( Nto2 = N * N ) * N ) * N ) * N ) * N;
		maxRows = rowCount = Nto6;
		maxCols = colCount = 4 * Nto4;
		matrix = new bool *[ maxRows ];
		count = new int[ maxCols ];
		for ( int i = 0; i < maxCols; i++ )
			count[ i ] = Nto2;
		for ( int i = 0; i < maxRows; i++ )
		{
			matrix[ i ] = new bool[ maxCols ];
			memset ( matrix[ i ], false, maxCols * sizeof false );
			matrix[ i ][ retNb ( i ) ] =
			matrix[ i ][ retRw ( i ) ] =
			matrix[ i ][ retCl ( i ) ] =
			matrix[ i ][ retBx ( i ) ] = true;
		}
		cols = new bool[ maxCols ];
		memset ( cols, true, maxCols * sizeof true );
		rows = new bool[ maxRows ];
		memset ( rows, true, maxCols * sizeof true );
		int *garbage = new int;
		for ( int i = 0; i < Nto2; i++ )
			for ( int j = 0; j < Nto2; j++ )
				if ( grid[ i ][ j ] )
					addToSolution (
					getIndex ( grid[ i ][ j ], i, j ),
					garbage, garbage
					);
	}

	inline int retNb ( int index )
	{
		return index / Nto2;
	}

	inline int retRw ( int index )
	{
		return Nto4 + ( index / Nto4 * Nto2 ) + ( index % Nto2 );
	}

	inline int retCl ( int index )
	{
		return 2 * Nto4 + index % Nto4;
	}

	inline int retBx ( int index )
	{
		return 3 * Nto4 + ( ( index / Nto5 * Nto3 ) + ( ( ( index / Nto3 ) % N ) * Nto2 ) + ( index % Nto2 ) );
	}

	inline int getIndex ( int num, int row, int col )
	{
		return ( num - 1 + col * Nto2 + row * Nto4 );
	}

	void increment ( int index )
	{
		if ( cols[ index ] )
			count[ index ]++;
	}

	void decrement ( int index )
	{
		if ( cols[ index ] )
			count[ index ]--;
	}

	void coverRow ( int index )
	{
		decrement ( retNb ( index ) );
		decrement ( retRw ( index ) );
		decrement ( retCl ( index ) );
		decrement ( retBx ( index ) );
		rows[ index ] = false;
	}

	void uncoverRow ( int index )
	{
		increment ( retNb ( index ) );
		increment ( retRw ( index ) );
		increment ( retCl ( index ) );
		increment ( retBx ( index ) );
		rows[ index ] = true;
	}

	void coverCol ( int index, int *coveredRows, int *coveredCols )
	{
		if ( !cols[ ( index ) ] )
			return;
		discardedCols.push ( ( index ) );
		cols[ index ] = false;
		( *coveredCols )++;
		colCount--;
		for ( int i = 0; i < maxRows; i++ )
		{
			if ( !rows[ i ] )
				continue;
			if ( matrix[ i ][ index ] )
			{
				coverRow ( i );
				discardedRows.push ( i );
				( *coveredRows )++;
				rowCount--;
			}
		}
	}

	void addToSolution ( int index, int *coveredRows, int *coveredCols )
	{
		rows[ index ] = false;
		solutionRows.push ( index );
		rowCount--;
		coverCol ( retNb ( index ), coveredRows, coveredCols );
		coverCol ( retRw ( index ), coveredRows, coveredCols );
		coverCol ( retCl ( index ), coveredRows, coveredCols );
		coverCol ( retBx ( index ), coveredRows, coveredCols );
	}

	void coverWrapper ( int index, int occurence, int *coveredRows, int *coveredCols )
	{
		for ( int i = 0; i < maxRows; i++ )
		{
			if ( !rows[ i ] )
				continue;
			if ( matrix[ i ][ index ] )
			{
				occurence--;
				if ( !occurence )
				{
					addToSolution ( i, coveredRows, coveredCols );
					return;
				}
			}
		}
	}

	int getMinCol ( int *n )
	{
		int *arr = new int[ maxCols ];
		memcpy ( arr, count, maxCols * sizeof ( int ) );
		sort ( arr, arr + maxCols );
		//if ( arr[ 0 ] == 0 )
		//	return -1;
		for ( int k = ( *n ) - 1; k < maxCols; k++ )
			for ( int i = 0; i < maxCols; i++ )
				if ( cols[ i ] && count[ i ] == arr[ k ] )
				{
					*n = k + 1;
					return i;
				}
		return -1;
	}

	bool solve ( )
	{
		static int call_counter = 0;
		//char *indent = new char[ ++call_counter + 1 ];
		//memset ( indent, ' ', call_counter * sizeof ' ' );
		//indent[ call_counter ] = NULL;
		if ( !rowCount )
			return true;
		int j = 1;
		for ( ; j <= colCount; )
		{
			int index = getMinCol ( &j ),
				m = count[ index ];
			j++;
			if ( index == -1 )
				__debugbreak ( );
			if ( m == 0 )
				return false;
			for ( int k = 1; k <= m; k++ )
			{
				int coveredRows = 0, coveredCols = 0;
				//if ( k == 2 )
				//	__debugbreak ( );
				coverWrapper ( index, k, &coveredRows, &coveredCols );
				printf ( "%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\n", ++call_counter, rowCount, colCount, solutionRows.top ( ), m, k, coveredRows, coveredCols );
				//if ( coveredCols != 4 )
				//	__debugbreak ( );
				if ( solve ( ) )
					return true;
				printf ( "\tUncover:\t%d\t%d\n", index, solutionRows.top ( ) );
				while ( coveredCols )
				{
					cols[ discardedCols.pop ( ) ] = true;
					colCount++;
					coveredCols--;
				}
				while ( coveredRows )
				{
					uncoverRow ( discardedRows.pop ( ) );
					rowCount++;
					coveredRows--;
				}
				rows[ solutionRows.pop ( ) ] = true;
				rowCount++;
			}
		}
		return false;
	}

	void fill_grid ( int **grid )
	{
		int index;
		while ( !solutionRows.isEmpty ( ) )
		{
			index = solutionRows.pop ( );
			grid[ index / Nto4 ][ ( index % Nto4 ) / Nto2 ] = 1 + ( index % Nto2 );
		}
	}
};