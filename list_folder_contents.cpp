#include <string>
#include <iostream>
#include <filesystem>
#include <cmath>
#include <algorithm>

using namespace std;

int main()
{
    string current_path = filesystem::current_path();

    //range-based for loop
    for (const auto & entry : filesystem::directory_iterator(current_path)) {
	// to display full file path
	//cout << entry.path() << endl;
	
	filesystem::directory_entry de(entry.path());
	if( de.is_directory() )
		cout<<"dir\t";
	else if( de.is_regular_file() )
		cout<<"file\t";

	// to display just the name
	string fileName = entry.path().filename();

	// to remove quotes while printing using cout
	fileName.erase(remove(fileName.begin(), fileName.end(), '\"'), fileName.end());
	cout << fileName << "\t";
	
	string size_chart = "BKMGTP"; // K - Kilobyte, M - Megabyte, G - Gigabyte, T - Terabyte, P - Petabyte;
	
	if( !de.is_directory() ) {
		cout << de.file_size() << "\t";
		// convert to human readable
		int i=0;
		double size = de.file_size();
		while(size >= 1024) {
			size=size/1024;
			i++;
		}
		cout << fixed << setprecision( 2 ) << size;
		if ( i > 0 )
			cout << size_chart[i];
		cout << "B" << endl;
	}
    }
}
