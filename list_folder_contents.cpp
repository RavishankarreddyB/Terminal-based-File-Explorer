#include <string>
#include <iostream>
#include <filesystem>
#include <cmath>
#include <algorithm>
#include <sys/stat.h>
#include <sys/types.h>

using namespace std;

char* file_permissions(char *fileName) {
	struct stat file_statistics;
	char *stats=(char*)malloc(sizeof(char)*10);
	if( stat(fileName, &file_statistics) == 0 ) {
		mode_t perms = file_statistics.st_mode;
		stats[0] = (perms & S_IRUSR) ? 'r' : '-';
		stats[1] = (perms & S_IWUSR) ? 'w' : '-';
		stats[2] = (perms & S_IXUSR) ? 'x' : '-';
		stats[3] = (perms & S_IRGRP) ? 'r' : '-';
        	stats[4] = (perms & S_IWGRP) ? 'w' : '-';
        	stats[5] = (perms & S_IXGRP) ? 'x' : '-';
        	stats[6] = (perms & S_IROTH) ? 'r' : '-';
        	stats[7] = (perms & S_IWOTH) ? 'w' : '-';
        	stats[8] = (perms & S_IXOTH) ? 'x' : '-';
        	stats[9] = '\0';
		return stats;
	}
	else
		return strerror(errno);
}

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

	char* perms = file_permissions(&fileName[0]);
	for(int i=0;i<9;i++)
		cout<<*(perms+i);
	cout<<"\t";

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
