#include <iostream>
#include <sstream>
#include <string>
#include <time.h>
#include <thread>
#include <atomic>
#include <functional>
#include <algorithm>
#include <windows.h>
#include "DLX.cpp"

using namespace std;

#define ITERATE_N(x)				for ( int (x) = 0; (x) < NSQR; ++(x) )

int N, NSQR;

HANDLE hConsole;

void init ( int ***grid, int ele )
{
	*grid = new int*[ NSQR ];
	ITERATE_N ( i )
		( *grid )[ i ] = new int[ NSQR ];
	ITERATE_N ( i )
		ITERATE_N ( j )
		( *grid )[ i ][ j ] = ele;
}

void randomize ( int *arr )
{
	ITERATE_N ( i )
	{
		int t = 1 + rand ( ) % NSQR;
		ITERATE_N ( j )
			if ( arr[ j ] == t )
			{
			t = 0;
			break;
			}
		if ( t )
			arr[ i ] = t;
		else
			i--;
	}
}

void randomize_grid ( int **grid,
					  bool ** grid_state
					  )
{
	int *index = new int[ NSQR ],
		*value = new int[ NSQR ];
	srand ( (unsigned) time ( NULL ) );
	randomize ( index );
	randomize ( value );
	ITERATE_N ( i )
	{
		int row = rand ( ) % NSQR;
		grid[ index[ i ] - 1 ][ row ] = value[ i ];
		grid_state[ index[ i ] - 1 ][ row ] = true;
	}
}

void SetColour ( bool b_set,
						WORD colour_true = FOREGROUND_RED | FOREGROUND_BLUE,
						WORD colour_false = FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED
						)
{
	if ( b_set )
		SetConsoleTextAttribute ( hConsole, colour_true );
	else
		SetConsoleTextAttribute ( hConsole, colour_false );
}

void display ( int **grid,
			   bool **grid_state,
			   int row = -1,
			   int col = -1
			   )
{
	for ( int i = 0; i < NSQR * 4 + 2 * sqrt ( NSQR ); i++ )
		printf ( "_" );
	printf ( "\n\n" );
	ITERATE_N ( i )
	{
		ITERATE_N ( j )
		{
			SetColour ( i == row || j == col );
			if ( i == row && j == col )
				SetColour ( true, FOREGROUND_RED | FOREGROUND_INTENSITY );
			if ( grid[ i ][ j ] )
			{
				if ( grid_state[ i ][ j ] )
					SetColour ( i != row || j != col,
					FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY,
					FOREGROUND_RED | FOREGROUND_INTENSITY );
				else
					SetColour ( true,
					FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY );
				printf ( " %2d ", grid[ i ][ j ] );
			}
			else if ( i == row && j == col )
				printf ( "  \x00DB " );
			else
				printf ( "  - " );
			SetColour ( i == row );
			if ( ( j + 1 ) % N == 0 )
				printf ( " |" );
		}
		printf ( "\n" );
		SetColour ( false );
		if ( ( i + 1 ) % N == 0 )
			for ( int i = 0; i < NSQR * 4 + 2 * sqrt ( NSQR ); i++ )
				printf ( "_" );
		printf ( "\n\n" );
	}
}

void move_markers ( int *row, int *col,
					bool *input, bool *update, bool *clear,
					bool *clearAll, bool *stop, bool *busy )
{
	while ( true )
	{
		while ( *busy )
			this_thread::sleep_for ( chrono::milliseconds ( 10 ) );
		if ( GetAsyncKeyState ( VK_UP ) )
		{
			*row = ( *row - 1 + NSQR ) % NSQR;
			*update = true;
			Sleep ( 150 );
		}
		if ( GetAsyncKeyState ( VK_DOWN ) )
		{
			*row = ( *row + 1 ) % NSQR;
			*update = true;
			Sleep ( 150 );
		}
		if ( GetAsyncKeyState ( VK_LEFT ) )
		{
			*col = ( *col - 1 + NSQR ) % NSQR;
			*update = true;
			Sleep ( 150 );
		}
		if ( GetAsyncKeyState ( VK_RIGHT ) )
		{
			*col = ( *col + 1 ) % NSQR;
			*update = true;
			Sleep ( 150 );
		}
		if ( GetAsyncKeyState ( VK_SPACE )/* || GetAsyncKeyState( VK_RETURN )*/ )
		{
			*input = true;
			*update = true;
			Sleep ( 200 );
		}
		if ( GetAsyncKeyState ( VK_BACK ) )
		{
			*clear = true;
			*update = true;
			Sleep ( 50 );
		}
		if ( GetAsyncKeyState ( VK_DELETE ) )
		{
			*clearAll = true;
			*update = true;
			Sleep ( 50 );
		}
		if ( GetAsyncKeyState ( VK_ESCAPE ) )
		{
			*stop = true;
			*update = true;
			break;
		}
		this_thread::sleep_for ( chrono::milliseconds ( 5 ) );
	}
}

bool checkRow ( int **grid, int val, int row )
{
	for ( int i = 0; i < NSQR; i++ )
		if ( grid[ row ][ i ] == val )
			return false;
	return true;
}

bool checkCol ( int **grid, int val, int col )
{
	for ( int i = 0; i < NSQR; i++ )
		if ( grid[ i ][ col ] == val )
			return false;
	return true;
}

bool checkBox ( int **grid, int val, int row, int col )
{
	for ( int i = row; i < row + N; i++ )
		for ( int j = col; j < col + N; j++ )
			if ( grid[ i ][ j ] == val )
				return false;
	return true;
}

#define ISSAFE(grid, val, row, col)			( ( (grid)[ (row) ][ (col) ] == 0 ) && \
											checkBox( (grid), (val), (row/N*N), (col/N*N) ) && \
											checkCol ( (grid), (val), (col) ) && \
											checkRow ( (grid), (val), (row) ) )

void input_grid ( int **grid, bool **grid_state, bool input_type = true )
{
	int row = 0, col = 0, val = -1;
	bool update = true, input = false, clear = false,
		clearAll = false, stop = false, busy = false;
	thread ( move_markers, &row, &col, &input, &update, &clear, &clearAll, &stop, &busy ).detach ( );
	while ( true )
	{
		system ( "cls" );
		printf ( "Press <SPACE> to input, <BCKSPC> to clear, <DEL> to clear ALL.\nPress <ESC> to solve.\n\n\n" );
		display ( grid, grid_state, row, col );
		while ( !update )
			Sleep ( 5 );
		if ( stop )
			break;
		if ( clear )
		{
			clear = false;
			grid[ row ][ col ] = 0;
			grid_state[ row ][ col ] = false;
		}
		if ( clearAll )
		{
			clearAll = false;
			for ( int i = 0; i < NSQR; i++ )
			{
				memset ( grid[ i ], 0, NSQR * sizeof grid[ 0 ][ 0 ] );
				memset ( grid_state[ i ], false, NSQR * sizeof grid_state[ 0 ][ 0 ] );
			}
		}
		if ( input )
		{
			COORD coord = {
				1 + col * 4 + col / 3 * 2,
				6 + row * 3 };
			SetConsoleCursorPosition ( hConsole, coord );
			busy = true;
			cin >> val;
			busy = false;
			if ( val < 0 || val > NSQR ||
				 !ISSAFE ( grid, val, row, col ) )
			{
				input = false;
				continue;
			}
			grid[ row ][ col ] = val;
			grid_state[ row ][ col ] = input_type;
			input = false;
		}
		update = false;
	}
	system ( "cls" );
}

void SetWindow ( int Width, int Height )
{
	_COORD coord;
	coord.X = Width;
	coord.Y = Height;

	_SMALL_RECT Rect;
	Rect.Top = 0;
	Rect.Left = 0;
	Rect.Bottom = Height - 1;
	Rect.Right = Width - 1;

	SetConsoleScreenBufferSize ( hConsole, coord );            // Set Buffer Size 
	SetConsoleWindowInfo ( hConsole, TRUE, &Rect );            // Set Window Size 
}

void main ( )
{
	int **grid;
	bool **grid_state;
	hConsole = GetStdHandle ( STD_OUTPUT_HANDLE );
	CONSOLE_HISTORY_INFO chi = { 0 };
	chi.cbSize = sizeof ( CONSOLE_HISTORY_INFO );
	SetConsoleHistoryInfo ( &chi );
	printf ( "Enter the size:\t" );
	cin >> N;
	NSQR = N * N;
	DLX dlx ( N );
	srand ( (UINT) time ( NULL ) );
	init ( &grid, 0 );
	init ( ( (int***) &grid_state ), false );
	randomize_grid ( grid, grid_state );
	input_grid ( grid, grid_state );
	int **grid_copy = new int*[ NSQR ];
	for ( int i = 0; i < NSQR; i++ )
	{
		grid_copy[ i ] = new int[ NSQR ];
		memcpy ( grid_copy[ i ], grid[ i ], NSQR * sizeof grid[ 0 ][ 0 ] );
	}
	auto t_start = chrono::high_resolution_clock::now ( ),
		t_end = chrono::high_resolution_clock::now ( );
	dlx.init ( grid );
	if ( dlx.solve ( ) )
	{
		dlx.fill_grid ( grid );
		t_end = chrono::high_resolution_clock::now ( );
		display ( grid, grid_state );
	}
	else
		printf ( "NOPE.\n" );
	cout << std::chrono::duration<double, std::milli> ( t_end - t_start ).count ( ) << " ms" << endl;
	cin.ignore ( '\n' );
}