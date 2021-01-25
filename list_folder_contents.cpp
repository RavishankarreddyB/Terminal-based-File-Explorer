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

#include "commands.h"

using namespace std;

void file_permissions(const char *fileName, filesystem::directory_entry entry, string &permissions) {
	struct stat file_statistics;
	//char *stats=(char*)malloc(sizeof(char)*11);
	permissions="";
	if( stat(fileName, &file_statistics) == 0 ) {
		mode_t perms = file_statistics.st_mode;
		permissions += (entry.is_directory()) ? 'd' : entry.is_regular_file() ? '-' : S_ISLNK(file_statistics.st_mode) ? 'l' : '-';
		permissions += (perms & S_IRUSR) ? 'r' : '-';
		permissions += (perms & S_IWUSR) ? 'w' : '-';
		permissions += (perms & S_IXUSR) ? 'x' : '-';
		permissions += (perms & S_IRGRP) ? 'r' : '-';
        	permissions += (perms & S_IWGRP) ? 'w' : '-';
        	permissions += (perms & S_IXGRP) ? 'x' : '-';
        	permissions += (perms & S_IROTH) ? 'r' : '-';
        	permissions += (perms & S_IWOTH) ? 'w' : '-';
        	permissions += (perms & S_IXOTH) ? 'x' : '-';
	}
	//else
	//	 printw("%s\n", S_ISDIR(file_statistics.st_mode));
}

void display_saved_dir_data(vector<string> &displayStrings, int curr_y_pos) {
	//int size=(int)displayStrings.size();
	for(auto itr = displayStrings.begin(); itr != displayStrings.end() ;itr++)
		printw("%s", (*itr).c_str());
	refresh();
	move(curr_y_pos, 0);
}

WINDOW* fetch_and_display_current_directory(WINDOW *pad, vector<string> &fileNames, vector<string> &displayStrings, int *totalFilesDirs, string path="", bool noStatusBar=true) {
	string current_path, perms="", completeDisplay="";
	int contentsCount=0, height=0, width=0, count=0;
	getmaxyx(stdscr, height, width);
	if(path == "")
		current_path = filesystem::current_path();
	else
		current_path = path;

	for (const auto & entry : filesystem::directory_iterator(current_path))
		contentsCount++;
	if(contentsCount > *totalFilesDirs) {
		*totalFilesDirs = contentsCount;
		pad = newpad(contentsCount+2, width);	//+2 instead of +1 because of extra ../ that is to be printed at the first. In all the remaining cases in which count < height, a pad of height+1 is rendered, so no issue is faced.

		//wprintw(pad, "contentsCount = %d, height = %d\n", contentsCount, height); 
		scrollok(pad, true);
		keypad(pad, true);
		prefresh(pad, 0, 0, 0, 0, height-2, width-1);
	}
	else if(contentsCount <= height-1 && *totalFilesDirs >= height) {
		*totalFilesDirs = contentsCount;
		pad = newpad(height+1, width);
		scrollok(pad,true);
		keypad(pad, true);
		prefresh(pad, 0, 0, 0, 0, height-2, width-1);
	}
	//printw("parent path = %s, root path = %s\n", current_path.c_str(), filesystem::current_path().root_directory().c_str());
	
	fileNames.clear(); displayStrings.clear();
	//printw("%s\n", current_path.c_str());
    //range-based for loop

	if( current_path != "/") {
		wprintw(pad, "../\n");
		displayStrings.push_back("../\n");
	}
	//wprintw(pad, "count = %d\n", contentsCount);

    for (const auto & entry : filesystem::directory_iterator(current_path)) {
	completeDisplay="";
	// to display full file path
	//cout << entry.path() << endl;
	//printw("%s\n", entry.path().c_str());
	
	filesystem::directory_entry de(entry.path());

	// to display just the name
	string fileName = entry.path().filename();
	
	fileNames.push_back(entry.path());
	//printw("%s\n", fileName.c_str());	
	//file_permissions(&fileName[0], entry, perms);
	
	file_permissions(entry.path().c_str(), entry, perms);
	
	completeDisplay+=perms;

	for(int i=0; i < (int)perms.length();i++)
		wprintw(pad, "%c", perms[i]);
	wprintw(pad, "\t");
	completeDisplay+='\t';

	string size_chart = "BKMGTP", completeSize; // K - Kilobyte, M - Megabyte, G - Gigabyte, T - Terabyte, P - Petabyte;
	
	if( !de.is_directory() && perms.length() > 0 && perms[0] != 'l') {
		//cout << de.file_size() << "\t";
		// convert to human readable
		int i=0;
		double size = de.file_size();
		while(size >= 1024) {
			size=size/1024;
			i++;
		}
		//cout << fixed << setprecision( 2 ) << size;
		wprintw(pad, "%.2lf",size);
		completeSize+=to_string(size);
		if ( i > 0 ) {
			wprintw(pad, "%c",size_chart[i]);
			completeSize+=size_chart[i];
		}
			//cout << size_chart[i];
		//cout << "B\t";
		wprintw(pad, "B\t");
		completeSize+='B';
	}
	else {
		wprintw(pad, "\t");
		if(perms.length() > 0 && perms[0] != 'l')
		completeSize="directory";
		else
		completeSize="link";
		//cout << "\t";
	}
	
	completeDisplay+=completeSize;
	completeDisplay+='\t';

	
	struct stat file_stats;
        if( current_path != "/" && stat(entry.path().c_str(), &file_stats) == 0) {
                time_t modifiedTime = file_stats.st_mtime;
		string time = asctime(localtime(&modifiedTime));
		time.pop_back(); 
		//cout << time << "\t";
		waddstr(pad, time.c_str());
		completeDisplay+=time.c_str();
		wprintw(pad, "\t");
		completeDisplay+='\t';
        }
        //else
	//	wprintw(pad, "%s\t", S_ISDIR(file_stats.st_mode));
		//printw("some problem fetching file last modified time\t");
                //cout << "some problem fetching file last modified time\t";
	
	
	// to remove quotes while printing using cout
	fileName.erase(remove(fileName.begin(), fileName.end(), '\"'), fileName.end());
        //cout << fileName << endl;
	waddstr(pad, fileName.c_str());
	completeDisplay+=fileName.c_str();
	wprintw(pad, "\t %d\n", ++count);
	completeDisplay+='\n';
	displayStrings.push_back(completeDisplay);
    }
	//wmove(pad, height-1, 0);
	//wprintw(pad, "%s\n",current_path.c_str());
	wmove(pad, 0,0);
	prefresh(pad, 0, 0, 0, 0, height-2, width-1);
	
	if(noStatusBar) {
		WINDOW *status_bar = newwin(1,width, height-1, 0);
		wprintw(status_bar, "%s", current_path.c_str());
		wrefresh(status_bar);
	}

	return pad;
}

int main()
{
	// Initialize curses
	initscr();
	cbreak();

	noecho();
	clear();
	//int maxlines = LINES - 1;
	//int maxcols = COLS - 1;
	int curr_x_pos = 0, curr_y_pos = 0, ch, height, width, fileOpen=0, line_count=0, y_before_fileOpen=0, contentsCount=0;
	string FILE, command="";
	vector<string> fileNames, displayStrings;

	WINDOW *pad;
	getmaxyx(stdscr, height, width);
	
	for (const auto & entry : filesystem::directory_iterator(filesystem::current_path()))
		line_count++;

	if(line_count > height)
		pad=newpad(line_count+1, width);
	else
		pad=newpad(height+1, width);
	keypad(pad, true);
	keypad(stdscr, true);
	
	pad = fetch_and_display_current_directory(pad, fileNames, displayStrings, &line_count);
	prefresh(pad, contentsCount, 0, 0, 0, height-2, width-1);

	wmove(pad, curr_y_pos, curr_x_pos);
	keypad(stdscr, true);
	
	filesystem::path currPath = filesystem::current_path();
	
	ch=wgetch(pad);
	while(ch != 'q') {
/*
curr_y_pos var is used to keep track of present line number 
linesCount var is used to keep track of number of lines that are more than height of screen.
only after the curr_y_pos crosses height of screen, the screen is rolled up. otherwise only the cursor position is updated.
*/
		if( ch == KEY_UP ) {
			if(curr_y_pos != 0) {
                                if( curr_y_pos == contentsCount ) {
                                        curr_y_pos--;
                                        contentsCount--;
                                        prefresh(pad, contentsCount, 0, 0, 0, height-2, width-1);
                                }
                                else
                                        curr_y_pos--;
                        }
			wmove(pad, curr_y_pos, curr_x_pos);
			prefresh(pad, contentsCount, 0, 0,0, height-2, width-1);
		}
		else if ( ch == KEY_DOWN ) {
			if(curr_y_pos < height-2) {
				if(curr_y_pos < (int)fileNames.size())
					curr_y_pos++;
			}
			//else if(curr_y_pos == height-1 && contentsCount == line_count-1);
			else if(curr_y_pos == line_count);
                        else if(curr_y_pos != line_count && curr_y_pos == contentsCount+height-2) {
                                curr_y_pos++;
                                contentsCount++;
                                prefresh(pad, contentsCount, 0, 0, 0, height-2, width-1);
                        }
                        else if(curr_y_pos != contentsCount-1)
                                curr_y_pos++;
                
			wmove(pad, curr_y_pos, curr_x_pos);
			prefresh(pad,contentsCount, 0, 0,0, height-2, width-1);
		}
		else if ( ch == '\n' ) {
			//clear();
			if ( curr_y_pos > 0)
				FILE = fileNames[curr_y_pos-1];
			else {
				if(currPath != "/")
					FILE = currPath.parent_path();
				else
					FILE = currPath;
			}
			currPath = FILE;
			filesystem::directory_entry de(FILE);
			//filesystem::directory_entry de(fileNames[curr_y_pos]);
			//printw("entering if\n");
			if( de.is_directory() || curr_y_pos == 0 ) {
				//printw("%s\n", fileNames[curr_y_pos].c_str());
				wclear(pad);

				line_count=(line_count > height) ? line_count : height;
				if(curr_y_pos > 0)
					pad = fetch_and_display_current_directory(pad, fileNames, displayStrings, &line_count, fileNames[curr_y_pos-1]);
				else
					pad = fetch_and_display_current_directory(pad, fileNames, displayStrings, &line_count, FILE);
				curr_y_pos=0;
				contentsCount=0;
				//refresh();
				wmove(pad, curr_y_pos, curr_x_pos);
				prefresh(pad,contentsCount, 0, 0,0, height-2, width-1);
			}
			else {
//				fileOpen=1;
				string command = "", filePath = fileNames[curr_y_pos-1];
				for(int i=0; i<(int)filePath.length();i++) {
					if( filePath[i] == '/' || filePath[i] == '_' || filePath[i] == '.' || isalnum(filePath[i]) )
						command+=filePath[i];
					else if(filePath[i] == ' ' || filePath[i] == '(' || filePath[i] == ')')
						command=command+'\\'+filePath[i];
				}
				string comm="open "+command;
				//printw("%s\n", comm.c_str());
				int returnValue = system(comm.c_str());
				if (returnValue == -1 || WEXITSTATUS(returnValue) != 0)
					wprintw(pad, "problem in opening file: %s\n", command.c_str());
				//else 
				//	printw("%d\n", retValue);

				/*
				y_before_fileOpen=curr_y_pos;
				ifstream inputFileCount(fileNames[curr_y_pos-1]);
				string line;
			
				while (getline(inputFileCount, line))
					line_count++;
				inputFileCount.close();
				ifstream inputFile(fileNames[curr_y_pos-1]);			
			
				getmaxyx(stdscr, height, width);
			
				if(line_count > height)	
					pad=newpad(line_count+1, width);
				else
					pad=newpad(height+1, width);
				keypad(pad, true);
				//wprintw(pad, "height  = %d\n", line_count);	

				scrollok(pad, true);

				if(inputFile.is_open()) {
					while( getline(inputFile, line) ) {
						wprintw(pad, "%s\n", line.c_str());
					}
				}
				inputFile.close();
				curr_y_pos=0;
				wmove(pad, curr_y_pos, curr_x_pos);
				prefresh(pad, curr_y_pos, 0, 0, 0, height-1, width-1);	
				*/
			}
		}
		else if ( ch == 27 && fileOpen == 1) { //For detecting escape key
			fileOpen=0;
			delwin(pad);
			curr_y_pos=y_before_fileOpen;
			contentsCount=0;
			//display_saved_dir_data(displayStrings, curr_y_pos);
			for(auto itr = displayStrings.begin(); itr != displayStrings.end() ;itr++)
				printw("%s", (*itr).c_str());
			move(height-1, 0);
			printw("%s\n",FILE.c_str());
			refresh();
			move(curr_y_pos, 0);
		}
		else if( ch == 101 || ch == 69 ) {
                        //wclear(pad);
                        noecho();
                        WINDOW *status_bar = newwin(1,width, height-1, 0);
                        keypad(status_bar, true);
                        ch = wgetch(status_bar);
                        command="";
                        while( ch != 27) {
                                if(ch == '\n') {
                                        wclear(pad);
					wclear(status_bar);
                                        string goto_path = execute_command(status_bar, command);
                                        if(goto_path == "")
                                                pad = fetch_and_display_current_directory(pad, fileNames, displayStrings, &line_count, currPath, false);
                                        else {
                                                currPath=goto_path;
                                                pad = fetch_and_display_current_directory(pad, fileNames, displayStrings, &line_count, currPath, false);
                                        }
                                }
				else if(ch == KEY_DC || ch == KEY_BACKSPACE || ch == 127) {
					command=command.substr(0,size(command)-1);
					wclear(status_bar);
					wprintw(status_bar, "%s", command.c_str());
				}
				else {
                                	command+=ch;
                                	wprintw(status_bar, "%c", ch);
				}
                                wrefresh(status_bar);
                                ch = wgetch(status_bar);
                        }
                        //prefresh(pad, contentsCount, 0, 0, 0, height-2, width-1);
                        //pad = fetch_and_display_current_directory(pad, fileNames, displayStrings, &line_count, currPath);
			wclear(status_bar);
			wprintw(status_bar, "%s", currPath.c_str());
			wrefresh(status_bar);
                }
		wmove(pad, curr_y_pos, curr_x_pos);
		prefresh(pad, contentsCount, 0,0,0, height-2, width-1);
		ch = wgetch(pad);
		//else if ( ch == KEY_RIGHT )
			//printw("right key is pressed\n");
		//else if ( ch == KEY_LEFT )
			//printw("left key is pressed\n");
		//else 
		//	printw("the key pressed is %c\n", ch);
		//refresh();
	}
	noecho();
	endwin();
}
