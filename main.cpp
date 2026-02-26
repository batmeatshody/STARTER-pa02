// Winter'24
// Instructor: Diba Mirza
// Student name: 
#include <iostream>
#include <fstream>
#include <string>
#include <ctime>
#include <vector>
#include <cstring>
#include <algorithm>
#include <limits.h>
#include <iomanip>
#include <set>
#include <queue>
#include <sstream>
#include <map>
#include <chrono>
#include <tuple>
using namespace std;

#include "utilities.h"
#include "movies.h"

bool parseLine(string &line, string &movieName, double &movieRating);

int main(int argc, char** argv){
    if (argc < 2){
        cerr << "Not enough arguments provided (need at least 1 argument)." << endl;
        cerr << "Usage: " << argv[ 0 ] << " moviesFilename prefixFilename " << endl;
        exit(1);
    }

    ifstream movieFile (argv[1]);
 
    if (movieFile.fail()){
        cerr << "Could not open file " << argv[1];
        exit(1);
    }
  
    // Create an object of a STL data-structure to store all the movies
    MovieMap movies;

    string line, movieName;
    double movieRating;
    // Read each file and store the name and rating
    while (getline (movieFile, line) && parseLine(line, movieName, movieRating)){
            // Use std::string movieName and double movieRating
            // to construct your Movie objects
            // cout << movieName << " has rating " << movieRating << endl;
            // insert elements into your data structure
            movies[movieName] = movieRating;
    }

    movieFile.close();

    if (argc == 2){
            //print all the movies in ascending alphabetical order of movie names
            for (const auto& p : movies) {
                cout << p.first << ", " << fixed << setprecision(1) << p.second << endl;
            }
            return 0;
    }

    ifstream prefixFile (argv[2]);

    if (prefixFile.fail()) {
        cerr << "Could not open file " << argv[2];
        exit(1);
    }

    vector<string> prefixes;
    while (getline (prefixFile, line)) {
        if (!line.empty()) {
            prefixes.push_back(line);
        }
    }

    // Start timing for part 2
    auto start = chrono::high_resolution_clock::now();

    vector<tuple<string, string, double>> bestMovies; // prefix, name, rating

    //  For each prefix,
    for (const string& prefix : prefixes) {
        //  Find all movies that have that prefix and store them in an appropriate data structure
        auto it = movies.lower_bound(prefix);
        vector<pair<double, string>> candidates; // rating, name
        for (; it != movies.end() && it->first.substr(0, prefix.size()) == prefix; ++it) {
            candidates.emplace_back(it->second, it->first);
        }
        if (candidates.empty()) {
            cout << "No movies found with prefix " << prefix << endl;
        } else {
            // sort by rating desc, then name asc
            sort(candidates.begin(), candidates.end(), [](const auto& a, const auto& b) {
                if (a.first != b.first) return a.first > b.first;
                return a.second < b.second;
            });
            for (const auto& p : candidates) {
                cout << p.second << ", " << fixed << setprecision(1) << p.first << endl;
            }
            cout << endl;
            // Store the best
            bestMovies.emplace_back(prefix, candidates[0].second, candidates[0].first);
        }
    }

    // End timing
    auto end = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::milliseconds>(end - start);

    //  For each prefix,
    //  Print the highest rated movie with that prefix if it exists.
    for (const auto& b : bestMovies) {
        cout << "Best movie with prefix " << get<0>(b) << " is " << get<1>(b) << " with rating " << fixed << setprecision(1) << get<2>(b) << endl;
    }

    // For analysis, print the time if needed, but for submission, comment out
    // cout << "Time taken: " << duration.count() << " ms" << endl;

    return 0;
}

/*
Part 3a: Time Complexity Analysis

Assume n movies, m prefixes, k max movies per prefix, l max name length.

Building the map: O(n log n) time.

For each of m prefixes:
- lower_bound: O(log n)
- Iterating the range: O(k) to collect candidates
- Sorting candidates: O(k log k)

Total time: O(n log n + m log n + sum k log k)

Worst case, sum k = m * n if all movies match all prefixes, but in practice, sum k <= m * n, but typically less.

Since k <= n, O(k log k) <= O(n log n), so overall O(n log n + m log n)

Relevant parameters: n, m

Big-O: O((n + m) log n)

Measured runtimes on random datasets with prefix_large.txt (m=17576):

input_20_random.csv: 23 ms
input_100_random.csv: 12 ms
input_1000_random.csv: 17 ms
input_76920_random.csv: 79 ms

(Run on local machine, may vary on csil)

Part 3b: Space Complexity Analysis

Space: O(n) for the map storing n movies.

Vectors for candidates: O(k) per prefix, but reused, so O(max k) = O(n)

Prefixes: O(m l)

Total: O(n + m l)

Relevant: n, m, l

Big-O: O(n + m l)

Part 3c: Tradeoffs

I designed for low time complexity, targeting O((n+m) log n) time and O(n) space.

I achieved low space complexity as well, since O(n) is minimal for storing the data.

Low time was harder because optimizing for prefixes required an ordered structure like map, which has log n factors.
*/

bool parseLine(string &line, string &movieName, double &movieRating) {
    int commaIndex = line.find_last_of(",");
    movieName = line.substr(0, commaIndex);
    movieRating = stod(line.substr(commaIndex+1));
    if (movieName[0] == '\"') {
        movieName = movieName.substr(1, movieName.length() - 2);
    }
    return true;
}