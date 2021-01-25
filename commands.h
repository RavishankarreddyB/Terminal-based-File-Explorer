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

string remove_spaces_in_path(string &filePath) {
	string command="";
	for(int i=0; i<(int)filePath.length();i++) {
		if( filePath[i] == '/' || filePath[i] == '_' || filePath[i] == '.' || isalnum(filePath[i]) )
			command+=filePath[i];
		else if(filePath[i] == ' ' || filePath[i] == '(' || filePath[i] == ')')
			command=command+'\\'+filePath[i];
	}
	return command;
}

void split_string(string command, vector<string> &vec) {
	int start=0;
	for(int i=start;i < (int)command.length(); i++) {
		if(isspace(command[i])) {
			vec.push_back(command.substr(start, i-start));
			start=i+1;
		}
		else if(i == (int)command.length()-1)
			vec.push_back(command.substr(start, i-start+1));
	}
}

string execute_command(WINDOW* status_bar, string command) {
	string currentPath = filesystem::current_path();
	vector<string> vec;
	int returnValue;
	split_string(command, vec);
	string cmd="";
	
	if(vec[0] == "copy" || vec[0] == "move") {
		if(vec[0] == "copy")
			cmd+="cp -r ";
		else 
			cmd+="mv ";
		int j;
		for(j=1;j<(int)vec.size()-1;j++) {
			if(vec[j].substr(0,2) == "./") {
                                string path=currentPath;
                                path+=vec[j].substr(1);
                                path=remove_spaces_in_path(path);
                                cmd=cmd+path+" ";
                        }
			else if(vec[j].find("~/") != string::npos) {
				string path=currentPath;
				path+=vec[j].substr(1);
				path=remove_spaces_in_path(path);
				cmd=cmd+path+" ";
			}
			else
				cmd=cmd+vec[j]+" ";
		}
		if(vec[j].substr(0,1) == ".") {
			string path=currentPath;
			path=remove_spaces_in_path(path);
			cmd+=path;
		}
		else if(vec[j].find("~/") != string::npos) {
			string path=currentPath;
			path+=vec[j].substr(1);
			path=remove_spaces_in_path(path);
			cmd+=path;
		}
		else {
			string path=currentPath;
			if(vec[j][0] != '/')
				path+='/';
			else
				path="";
			path+=vec[j];
			path=remove_spaces_in_path(path);
			cmd+=path;
		}
		returnValue = system(cmd.c_str());
		wclear(status_bar);
	}

	else if(vec[0] == "rename") {
		if(vec.size() == 3) {
			cmd+="mv ";
			if(vec[1].substr(0,2) == "./") {
				string path=currentPath;
				path+=vec[1].substr(1);
				path=remove_spaces_in_path(path);
				cmd=cmd+path+" ";
			}
			else if(vec[1].find("~/") != string::npos) {
                                string path=currentPath;
                                path+=vec[1].substr(1);
                                path=remove_spaces_in_path(path);
                                cmd=cmd+path+" ";
                        }
			else 
				cmd=cmd+vec[1]+" ";
			
			if(vec[2].substr(0,2) == "./") {
                                string path=currentPath;
                                path+=vec[2].substr(1);
                                path=remove_spaces_in_path(path);
                                cmd=cmd+path;
                        }
			else if(vec[2].find("~/") != string::npos) {
                                string path=currentPath;
                                path+=vec[2].substr(1);
                                path=remove_spaces_in_path(path);
                                cmd=cmd+path;
                        }
                        else
                                cmd=cmd+vec[2];
			returnValue = system(cmd.c_str());
			wclear(status_bar);
		}
		
	}
	else if(vec[0] == "create_file" || vec[1] == "create_dir") {
		if(vec.size() == 3) {
			if(vec[0] == "create_file")
				cmd+="touch ";
			else
				cmd+="mkdir -p ";
			cmd=cmd+vec[1]+" ";
			
			if(vec[2].substr(0,2) == "./") {
                                string path=currentPath;
                                path+=vec[2].substr(1);
                                path=remove_spaces_in_path(path);
                                cmd=cmd+path;
                        }
                        else if(vec[2].find("~/") != string::npos) {
                                string path=currentPath;
                                path+=vec[2].substr(1);
                                path=remove_spaces_in_path(path);
                                cmd=cmd+path;
                        }
                        else
                                cmd=cmd+vec[2];
			
			returnValue = system(cmd.c_str());
                        wclear(status_bar);
		}
	}
	else if(vec[0] == "delete_file" || vec[1] == "delete_dir") {
		if(vec.size() == 2) {
                        if(vec[0] == "delete_file")
                                cmd+="rm -f ";
                        else
                                cmd+="rm -rf ";

                        if(vec[1].substr(0,2) == "./") {
                                string path=currentPath;
                                path+=vec[1].substr(1);
                                path=remove_spaces_in_path(path);
                                cmd=cmd+path;
                        }
                        else if(vec[1].find("~/") != string::npos) {
                                string path=currentPath;
                                path+=vec[1].substr(1);
                                path=remove_spaces_in_path(path);
                                cmd=cmd+path;
                        }
                        else
                                cmd=cmd+vec[1];

			returnValue = system(cmd.c_str());
                        wclear(status_bar);
                }
	}
	else if(vec[0] == "goto") {
		if(vec.size() == 2) {
			cmd+="cd ";
			string path=currentPath, beforeRemovingSpaces="";
			if(vec[1].substr(0,2) == "./") {
                                path+=vec[1].substr(1);
				beforeRemovingSpaces=path;
                                path=remove_spaces_in_path(path);
                                cmd=cmd+path;
                        }
                        else if(vec[1].find("~/") != string::npos) {
                                path+=vec[1].substr(1);
				beforeRemovingSpaces=path;
                                path=remove_spaces_in_path(path);
                                cmd=cmd+path;
                        }
                        else {
				beforeRemovingSpaces=vec[1];
                                cmd=cmd+vec[1];
			}
			returnValue = system(cmd.c_str());
                        wclear(status_bar);
			return beforeRemovingSpaces;
		}
	}
	else if(vec[0] == "search") {
		if(vec.size() == 2) {
			cmd+="find . -name "+vec[1];
			returnValue = system(cmd.c_str());
			wclear(status_bar);
			if(returnValue == -1 || WEXITSTATUS(returnValue) != 0)
				wprintw(status_bar, "file/dir not found");
			else
				wprintw(status_bar, "file/dir found");
			wrefresh(status_bar);
		}
	}
	return "";
}
