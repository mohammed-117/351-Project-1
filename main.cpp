/* Class 351 
Team: Mohammed Hassnain
      Vincent Campbell
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <limits.h>
#include <fcntl.h>
#include <sys/resource.h>
#include <sys/queue.h>
#include <cstring>
#include <climits>
#include <iostream>
#include <fstream>
#include <iomanip>

#define BUFFER_SIZE 4096

using namespace std;

const char *prg;

//You must fill out your name and id below
char * studentName = (char *) "Mohammed Hassnain";
char * studentCWID = (char *) "894088921";

//Do not change this section in your submission
char * usageString =
(char *) "To archive a file: 		fzip -a INPUT_FILE_NAME  OUTPUT_FILE_NAME\n"
"To archive a directory: 	fzip -a INPUT_DIR_NAME   OUTPUT_DIR_NAME\n"
"To extract a file: 		fzip -x INPUT_FILE_NAME  OUTPUT_FILE_NAME\n"
"To extract a directory: 	fzip -x INPUT_DIR_NAME   OUTPUT_DIR_NAME\n";

bool isExtract = false;
void parseArg(int argc, char *argv[], char ** inputName, char ** outputName) {
	if (argc >= 2 && strncmp("-n", argv[1], 2) == 0) {
		printf("Student Name: %s\n", studentName);
		printf("Student CWID: %s\n", studentCWID);
		exit(EXIT_SUCCESS);
	}

	if (argc != 4) {
		fprintf(stderr, "Incorrect arguements\n%s", usageString);
		exit(EXIT_FAILURE);
	}

	*inputName = argv[2];
	*outputName = argv[3];
	if (strncmp("-a", argv[1], 2) == 0) {
		isExtract = false;
	}
	else if (strncmp("-x", argv[1], 2) == 0) {
		isExtract = true;
	}
	else {
		fprintf(stderr, "Incorrect arguements\n%s", usageString);
		exit(EXIT_FAILURE);
	}
}


int is_regular_file(const char *path)
{
	struct stat path_stat;
	stat(path, &path_stat);
	return S_ISREG(path_stat.st_mode);
}

int fSize(char* inputName)
{
	streampos begin, end;
	ifstream myfile(inputName, ios::binary);
	begin = myfile.tellg();
	myfile.seekg(0, ios::end);
	end = myfile.tellg();
	myfile.close();

	return (end - begin);
}

int fType(char* inputName)
{
	return is_regular_file(inputName);
}

int FArchive(FILE* outF, char* inputName)
{
	FILE* currentF;
	char fileName[BUFFER_SIZE];

	int nameLength = strlen(inputName);
	int fileSize = fSize(inputName);
	int fileType = fType(inputName);

	strcpy(fileName, inputName);
	fileType = fType(fileName);

	fprintf(outF, "%d %d %s %d ", fileType, nameLength, fileName, fileSize);

	// regular file
	if (fileType == 1)
	{
		int ch;
		currentF = fopen(fileName, "r");
		do
		{
			ch = fgetc(currentF);
			if (ch != EOF)
				fputc(ch, outF);

		} while (ch != EOF);
		fclose(currentF);
	}
	return 0;
}

int DArchive(FILE* outF, char* outputName, char* inputName, DIR* d, bool root)
{
	DIR* dsub;
	struct dirent* p;
	FILE* currentF;
	char fileName[BUFFER_SIZE];
	int nameLength = strlen(inputName);
	int fileSize = fSize(inputName);
	int fileType = fType(inputName);



	// PRINT ROOT ///////
	if (root)
	{
		fprintf(outF, "%d %d %s %d\n", fileType, nameLength, inputName, fileSize);

	}
	//////////////////////


	root = false;  // MOVING INTO ROOT
	d = opendir(inputName);   // open directory with input name

	while (p = (readdir(d)))   // Point to first entry in directory
	{
		strcpy(fileName, inputName);   // Original Directory Pathname for inside files  //////
		strcat(fileName, "/");
		strcat(fileName, p->d_name); /////////////////////////////////////////////////////////

		nameLength = strlen(p->d_name); //  Name length of file or directory ////
		fileSize = fSize(fileName);
		fileType = fType(fileName);      //////////////////////////////////////

		if (strcmp(p->d_name, ".") != 0) // if they are not the same as current working directory
		{
			if (strcmp(p->d_name, "..") != 0) // if they are not the same as parent directory

			{
				if (p->d_name[nameLength - 1] != '~')  // If not the same as home directory
				{
					fprintf(outF, "%d %d %s %s %d ",
						fileType, nameLength, p->d_name, fileName, fileSize);

					// regular file
					if (fileType == 1)
					{
						int ch;
						currentF = fopen(fileName, "r");
						do
						{
							ch = fgetc(currentF);
							if (ch != EOF)
								fputc(ch, outF);

						} while (ch != EOF);
						fclose(currentF);
					}
					// directory
					else if (fileType == 0)
					{
						fprintf(outF, "\n");
						if (fileName[strlen(fileName) - 1] != '.') // if they are not the same as current working directory
						{
							dsub = opendir(fileName);
							DArchive(outF, outputName, fileName, dsub, root);
						}
					}
				}
			}
		}
	}
	return 0;
}



int DExtract(FILE* fp, char* outputName, char* inputName, DIR* d, bool root)
{
	DIR* dsub;
	struct dirent* p;
	FILE* outF;
	int i = 0;
	char fileName[BUFFER_SIZE];
	char SfileName[BUFFER_SIZE];
	bool SC;

	int nameLength;
	int fileSize;
	int fileType;



	////// ROOT CREATION /////////////
	if (root)
	{
		fscanf(fp, "%d %d %s %d\n", &fileType, &nameLength, inputName, &fileSize);
		if (fileType == 0)
		{
			mkdir(outputName, 0755); // create root directory
		}

	}
	//////////////////////////////////

	root = false;  // MOVING INTO ROOT
	d = opendir(outputName);   // open directory with input name

	while ((p = readdir(d)) && !(feof(fp)))
	{


		//////////////////////////////////////////////////
		if (!feof(fp))
		{
			fscanf(fp, "%d %d %s %s %d ", &fileType, &nameLength, p->d_name, fileName, &fileSize);

			strcpy(SfileName, outputName);
			for (int h = 0; h < strlen(fileName); h++)
			{
				if (fileName[h] == '/')
				{
					SC = true;
				}

				if (SC == true)
				{
					strcat(SfileName, &fileName[h]);
					h = strlen(fileName);

				}

			}
			SC = false;

		}



		//////////////////////////////////////////////////////

		if (strcmp(p->d_name, ".") != 0) // if they are not the same as current working directory
		{
			if (strcmp(p->d_name, "..") != 0) // if they are not the same as parent directory

			{
				if (p->d_name[nameLength - 1] != '~')  // If not the same as home directory
				{

					////////////////////////////////////////////////////////
					if ((fileType == 1) && !(feof(fp)))
					{
						int ch;
						// This is a regular file

						outF = fopen(SfileName, "w+");  // create file
						i = 0;
						do
						{

							ch = fgetc(fp);

							if (feof(fp))
							{
								break;
							}

							if (i <= fileSize)
							{
								fputc(ch, outF);
								i++;

							}

						} while (i < fileSize);
						fclose(outF);

					}
					else if ((fileType == 0))
					{

						// This is a directory

						mkdir(SfileName, 0755);   // create directory

						if (SfileName[strlen(SfileName) - 1] != '.')
						{
							if (feof(fp))
							{
								break;
							}
							dsub = opendir(SfileName);
							cout << "Root status in Dir section: " << root << endl;
							DExtract(fp, outputName, inputName, dsub, root);


						}
					}
				}
			}
		}
	}
	return 0;
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////

int FExtract(FILE* outF, char* inputName)
{
	FILE* inF;
	char fileName[BUFFER_SIZE];

	int nameLength;
	int fileSize;
	int fileType;

	strcpy(fileName, inputName);
	fileType = fType(fileName);

	inF = fopen(inputName, "r");

	fscanf(inF, "%d %d %s %d ", &fileType, &nameLength, fileName, &fileSize);

	// regular file
	if (fileType == 1)
	{
		int ch;
		inF = fopen(fileName, "r");
		do
		{
			ch = fgetc(inF);
			if (ch != EOF)
				fputc(ch, outF);

		} while (ch != EOF);
		fclose(inF);
	}
	return 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


int main(int argc, char *argv[]) {




	/******SAFELIST********/
	int input_fd;
	FILE* outF;
	FILE* fp;
	int FILE = 0;
	int fileCheck;

	int derror = 0;
	DIR *d;
	struct dirent *dir;
	/*******************DUPLICATE NAME TEST******************************/
	  // Check to make sure for duplicate
	d = opendir(".");
	if (d)
	{
		while ((dir = readdir(d)) != NULL)
		{

			if (strcmp(dir->d_name, argv[3]) == 0)
			{
				derror++;
			}
		}
		if (derror > 0)
		{
			cout << "Error: Output name exists in the directory " << endl;
			exit(0);
		}

		closedir(d);
	}
	/*****************************************************************/


	char * inputName, *outputName;
	parseArg(argc, argv, &inputName, &outputName);

	if (isExtract) {
		printf("Extracting %s\n", inputName);
		//TODO: your code to start extracting.
/*************************EXTRACT BEGIN****************************/
		if (argc != 4) {
			printf("Usage: Extract file or directory");
			return EXIT_FAILURE;
		}
		// check for regular file or directory
		// if true then --> regular file
		// if false then --> directory

// returns an integer value, which is used to refer to the file- read only format
		input_fd = open(argv[2], O_RDONLY);

		if (input_fd == -1) {
			perror("open");
			return EXIT_FAILURE;
		}


		outF = fopen(inputName, "r");
		fscanf(outF, "%d", &fileCheck);
		fclose(outF);

		if (fileCheck == 1)
		{  /****************REGULAR FILES **********************/

			outF = fopen(outputName, "w");
			FExtract(outF, inputName);
			fclose(outF);
		}
		else if (fileCheck == 0)
		{
			/************ DIRECTORIES *************************/

			// Todo: directory data here
			fp = fopen(inputName, "r");
			bool root = true;
			DExtract(fp, outputName, inputName, d, root);
			fclose(fp);
		}
		else // for other file types
		{
			printf("Unknown file type.\n");
			return 0;
		}


		printf("%s\n", outputName);


		/*********************EXTRACT END**********************/
	}
	else {
		printf("Archiving %s\n%s\n", inputName, outputName);
		//TODO: your code to start archiving.
/******************START ARCHIVING******************************/
		if (argc != 4) {
			printf("Usage: Archive file or directory");
			return EXIT_FAILURE;
		}


		// returns an integer value, which is used to refer to the file- read only format
		input_fd = open(argv[2], O_RDONLY);

		if (input_fd == -1) {
			perror("open");
			return EXIT_FAILURE;
		}

		FILE = is_regular_file(argv[2]);

		if (FILE == 1)
		{  /****************REGULAR FILES **********************/

			outF = fopen(outputName, "w");
			FArchive(outF, inputName);

		}
		else if (FILE == 0)
		{  /************ DIRECTORIES *************************/

			outF = fopen(outputName, "w");
			bool root = true;
			DArchive(outF, outputName, inputName, d, root);
			fclose(outF);
		}
		else // for other file types
		{
			printf("Unknown file type.\n");
			return 0;
		}
	}
	return EXIT_SUCCESS;
	/********************ARCHIVE END******************************/
}
