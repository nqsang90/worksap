/**
 * Author: Nguyen Quang Sang
 * Email: nqsang90@gmail.com
 */
#include <iostream>
#include <cstdint>
using namespace std;

void clean_visited(int** visited, const int n, const int m);
int64_t max_points(int64_t** map, const int n, const int m, int i, int j, int** visited, int64_t** max_acum_point, int** reachability, bool just_west);
bool reachable(int64_t** map, const int n, const int m, int** reachability, int i, int j);

int main()  {
    int n, m;
    cin >> n;
    cin >> m;
    
    int64_t** map = new int64_t*[n];
	int** visited = new int*[n];
	// remember max_acum_point. -1 not reachable, -2 not determine, max score otherwise
	int64_t** max_acum_point = new int64_t*[n];
	// 0 unknown, -1 unreachable, 1 reachable. remember reachability
    int** reachability = new int*[n];

	for(int i = 0; i < m; ++i) {
		map[i] = new int64_t[m];
	    visited[i] = new int[m];
	    max_acum_point[i] = new int64_t[m];
	    reachability[i] = new int[m];
	}

	for (int i = 0; i < n; ++i) {
		for (int j = 0; j < m; ++j)
			cin >> map[i][j];
	}
	for (int i = 0; i < n; ++i) {
		for (int j = 0; j < m; ++j) {
			max_acum_point[i][j] = -2;
		}
	}

	// cout << "starting" << endl;
	int64_t mpoints = -1;
	for (int i = 0; i < n; ++i) {
		clean_visited(visited, m, n);
		int64_t cumalative = max_points(map, n, m, i, m-1, visited, max_acum_point, reachability, true);
		mpoints = max(mpoints, cumalative);
	}
	cout << mpoints << endl;


	// for (int i = 0; i < n; ++i) {
	// 	for (int j = 0; j < n; ++j)
	// 		cout << max_acum_point[i][j] << " ";
	// 	cout << endl;
	// }
	// clean up
	for(int i = 0; i < n; ++i) {
		delete[] map[i];
	    delete[] visited[i];
	    delete[] reachability[i];
	}
	delete[] map;
	delete[] visited;
	delete[] reachability;

	return 0;
}
void clean_visited(int** visited, const int n, const int m) {
	for (int i = 0; i < n; ++i) {
		for (int j = 0; j < m; ++j) 
			visited[i][j] = 0;
	}
}

/**
	We move backward from destination to have take advantage of subproblem, due to teleport.
	Return the max point if destination is any (i,j)
	Dynamic programming: remember max score and use it (only when first land on a column).
*/
int64_t max_points(int64_t** map, const int n, const int m, int i, int j, int** visited, int64_t** max_acum_point, int** reachability, bool just_west) {
	// cout << "at " << i << " " << j << endl;
    if (i >= 0 && i < n && j >= 0 && j < m && max_acum_point[i][j] != -2 && just_west) {
    	return max_acum_point[i][j];
    }

    // out of map
	if (i < 0 || i >= n || j < 0 || j >= m)
		return -1;
	// blocked way
	if (map[i][j] == -1) {
		// max_acum_point[i][j] = -1;
		return -1;
	}
	// visited
	if (visited[i][j] == true)
		return -1;
	visited[i][j] = true;

	// north
	int64_t north_points;
	if (i - 1 < 0) // a teleport
		north_points = reachable(map, n, m, reachability, (i-1+n) % n, j) ? 0 : -1;
	else
		north_points = max_points(map, n, m, i-1, j, visited, max_acum_point, reachability, false);
	// west
	int64_t west_points = max_points(map, n, m, i, j-1, visited, max_acum_point, reachability, true);
	// south
	int64_t south_points;
	if (i + 1 >= n)	// a teleport
		south_points = reachable(map, n, m, reachability, (i+1+n) % n, j) ? 0 : -1;
	else 
		south_points = max_points(map, n, m, i+1, j, visited, max_acum_point, reachability, false);
	visited[i][j] = false;

	// no way to go, and not at starting position. dead point
	// a base case
	if (north_points == west_points && west_points == south_points && south_points == -1 && j != 0) {
		// max_acum_point[i][j] = -1;
		return -1;
	}
	int64_t accum = max((int64_t)0, max(north_points, max(west_points, south_points))) + map[i][j];
	if (just_west)
		max_acum_point[i][j] = accum;
	return accum;
}
/**
	(i,j) reachable from left most column
	reachability map: 0 not known, -1 not reachable, 1 reachable.
	concept of dynamic programming and recursive
*/
bool reachable(int64_t** map, const int n, const int m, int** reachability, int i, int j) {
	// base case: out of map, blocked
	if (i < 0 || i >= n || j < 0 || j >= m || map[i][j] == -1) {
		return false;
	}
	// base case: left column
	if (j == 0 && map[i][j] != -1) {
		reachability[i][j] = true;
		return true;
	}

	// known
	if (reachability[i][j] != 0)
		return reachability[i][j-1] == 1 ? true : false;

	// left/upper/lower cell reachable
	if (reachability[i][j-1] == 1 
		|| reachability[(i-1+n) % n][j] == 1 
		|| reachability[(i+1+n) % n][j] == 1) {
		reachability[i][j] = 1;
		return true;
	}
	// none of them reachable
	else if (reachability[i][j-1] == -1 
		&& reachability[(i-1+n) % n][j] == -1 
		&& reachability[(i+1+n) % n][j] == -1) {
		reachability[i][j] = -1;
		return false;
	}
	// adjacent reachability unknown, find it out
	if (reachability[i][j] == 0) {
		reachability[i][j] = (reachable(map, n, m, reachability, i, j-1) 
								|| reachable(map, n, m, reachability, (i-1+n)%n, j) 
								|| reachable(map, n, m, reachability, (i+1+n)%n, j)) ? 1 : -1;
	}
	return reachability[i][j] == 1;
}
