#include <string>
#include <iostream>
#include <filesystem>
#include <cmath>
#include <algorithm>
#include <sys/stat.h>
#include <sys/types.h>

#include <ncurses.h>
#include <vector>

#include <fstream>

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
	// Initialize curses
	initscr();
	cbreak();

	clear();
	//int maxlines = LINES - 1;
	//int maxcols = COLS - 1;
	int curr_x_pos = 0, curr_y_pos = 0;

	//mvaddch(0, 0, '0');
	//mvaddch(maxlines, maxcols/2, '1');
	//mvaddch(0, maxcols, '2');
	//mvaddstr(maxlines, 0, "Press any key to quit");
	
	string current_path = filesystem::current_path();
	
	vector<string> fileNames;
    //range-based for loop
    for (const auto & entry : filesystem::directory_iterator(current_path)) {
	// to display full file path
	//cout << entry.path() << endl;
	
	filesystem::directory_entry de(entry.path());

	// to display just the name
	string fileName = entry.path().filename();
	
	fileNames.push_back(entry.path());
	
	char* perms = file_permissions(&fileName[0], entry);
	for(int i=0;*(perms+i) != '\0';i++)
		printw("%c",*(perms+i));
	printw("\t");

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
		//cout << fixed << setprecision( 2 ) << size;
		printw("%.2lf",size);
		if ( i > 0 )
			printw("%c",size_chart[i]);
			//cout << size_chart[i];
		//cout << "B\t";
		printw("B\t");
	}
	else
		printw("\t");
		//cout << "\t";

	struct stat file_stats;
        if( stat(fileName.c_str(), &file_stats) == 0) {
                time_t modifiedTime = file_stats.st_mtime;
                string time = asctime(localtime(&modifiedTime)); 
		time.pop_back(); 
		//cout << time << "\t";
		addstr(time.c_str());
		printw("\t");
        }
        else
		printw("some problem fetching file last modified time\t");
                //cout << "some problem fetching file last modified time\t";
	
	// to remove quotes while printing using cout
        fileName.erase(remove(fileName.begin(), fileName.end(), '\"'), fileName.end());
        //cout << fileName << endl;
	addstr(fileName.c_str());
	printw("\n");
    }
	refresh();
	move(curr_y_pos, curr_x_pos);
	int ch;
	keypad(stdscr, true);
	while((ch = getch()) != 'q') {
		if( ch == KEY_UP ) {
			curr_y_pos--;
			if(curr_y_pos < 1)
				curr_y_pos=0;
			move(curr_y_pos, curr_x_pos); 
			//printw("up arrow is pressed\n");
		}
		else if ( ch == KEY_DOWN ) {
			curr_y_pos++;
			if(curr_y_pos >= (int)fileNames.size())
				curr_y_pos=fileNames.size()-1;
			move(curr_y_pos, curr_x_pos);
			//printw("down arrow is pressed\n");
		}
		else if ( ch == '\n' ) {
			clear();
			ifstream inputFile(fileNames[curr_y_pos]);
			string line;
			if(inputFile.is_open()) {
				while( getline(inputFile, line) )
					printw("%s\n", (line.c_str()));
			}
		}
		//else if ( ch == KEY_RIGHT )
			//printw("right key is pressed\n");
		//else if ( ch == KEY_LEFT )
			//printw("left key is pressed\n");
		//else 
		//	printw("the key pressed is %c\n", ch);
		refresh();
	}
	//while((ch = getchar()) != 'q');
		//printw("%c",ch);
	noecho();
	endwin();
}
