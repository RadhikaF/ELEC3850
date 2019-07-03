/*
    File name:          filefunctions.cpp
    Version:            0.0
    Author:             Tom Bailey
    Date created:       2018-08-19
    Date last modified: 2018-08-19

    Description:

Code Reference: https://stackoverflow.com/questions/612097/how-can-i-get-the-list-of-files-in-a-directory-using-c-or-c
*/
#include <sys/stat.h>

#include "header.hpp"
#include "dirent.h"

using namespace std;

const int dir_num = 4;		// d_type of folders to make sure they're not included

int ListFiles(const char* my_dir, vector <string>* file_list){
	DIR *dir;
	struct dirent *ent;
	struct stat eStat;

	if ((dir = opendir (my_dir)) != NULL) {
	  /* print all the files and directories within directory */
		while ((ent = readdir (dir)) != NULL) {
			if(ent->d_type != dir_num){
				(*file_list).push_back(ent->d_name);	// put the new name in the file list
			}
		}
	  closedir (dir);
	} else {
	  /* could not open directory */
	  perror ("");
	  return EXIT_FAILURE;
	}
}