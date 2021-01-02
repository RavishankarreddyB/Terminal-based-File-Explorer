#include <string>
#include <iostream>
#include <filesystem>

#include <algorithm>

using namespace std;

int main()
{
    string current_path = filesystem::current_path();

    //range-based for loop
    for (const auto & entry : filesystem::directory_iterator(current_path)) {
	// to display full file path
	//cout << entry.path() << endl;

	// to display just the name
	string fileName = entry.path().filename();

	// to remove quotes while printing using cout
	fileName.erase(remove(fileName.begin(), fileName.end(), '\"'), fileName.end());
	cout << fileName << endl;
    }
}
