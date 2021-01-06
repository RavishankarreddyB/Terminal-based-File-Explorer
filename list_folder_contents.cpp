#include <string>
#include <iostream>
#include <filesystem>
#include <cmath>
#include <algorithm>
#include <sys/stat.h>
#include <sys/types.h>

using namespace std;

char* file_permissions(char *fileName, filesystem::directory_entry entry) {
	struct stat file_statistics;
	char *stats=(char*)malloc(sizeof(char)*11);
	if( stat(fileName, &file_statistics) == 0 ) {
		mode_t perms = file_statistics.st_mode;
		stats[0] = (entry.is_directory()) ? 'd' : entry.is_regular_file() ? '-' : '-';
		stats[1] = (perms & S_IRUSR) ? 'r' : '-';
		stats[2] = (perms & S_IWUSR) ? 'w' : '-';
		stats[3] = (perms & S_IXUSR) ? 'x' : '-';
		stats[4] = (perms & S_IRGRP) ? 'r' : '-';
        	stats[5] = (perms & S_IWGRP) ? 'w' : '-';
        	stats[6] = (perms & S_IXGRP) ? 'x' : '-';
        	stats[7] = (perms & S_IROTH) ? 'r' : '-';
        	stats[8] = (perms & S_IWOTH) ? 'w' : '-';
        	stats[9] = (perms & S_IXOTH) ? 'x' : '-';
        	stats[10] = '\0';
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

	// to display just the name
	string fileName = entry.path().filename();

	char* perms = file_permissions(&fileName[0], entry);
	for(int i=0;*(perms+i) != '\0';i++)
		cout<<*(perms+i);
	cout<<"\t";

	string size_chart = "BKMGTP"; // K - Kilobyte, M - Megabyte, G - Gigabyte, T - Terabyte, P - Petabyte;
	
	if( !de.is_directory() ) {
		//cout << de.file_size() << "\t";
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
		cout << "B\t";
	}
	else
		cout << "\t";

	struct stat file_stats;
        if( stat(fileName.c_str(), &file_stats) == 0) {
                time_t modifiedTime = file_stats.st_mtime;
                string time = asctime(localtime(&modifiedTime)); 
		time.pop_back(); 
		cout << time << "\t";
        }
        else
                cout << "some problem fetching file last modified time\t";
	
	// to remove quotes while printing using cout
        fileName.erase(remove(fileName.begin(), fileName.end(), '\"'), fileName.end());
        cout << fileName << endl;
    }
}
