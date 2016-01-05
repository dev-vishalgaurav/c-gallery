
/**
	album.c
	name	:	Vishal Gaurav
	email	:	vishal.gaurav.gr@dartmouth.edu
**/

#include <stdio.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#define true 1
#define false 0
#define STRING_LEN  32
#define CAPTION_LEN  64
#define THUMB_NAME_LEN  64
#define ROTATION_LEN 4
#define HTML_BREAK_LIMIT 4


/* file and image description */
char *htmlFileName = "index.html";
char *mediumDir = "medium";
char *thumbDir = "thumb";
char *mediumDirForFile = "medium";
char *thumbDirForFile = "thumb";
char *imageFormat = ".jpg";
char *thumbSize = "25%";
char *mediumSize = "10%";

/*declaration for html syntax to used for generating index.html*/
char *htmlFormatStart =  "<html><title>Digital Directory</title> <h1>Welcome to your digital directory </h1>Please click on a thumbnail to view a medium-size image<h2>%s</h2>";
char *htmlFormatForImages = "%s<a href=\"%s%d%s\"><img src=\"%s%d%s\" border=\"1\">%s</a>";
char *htmlFormatEnd= "</body>";
char *htmlBreakSyntax= "</br>";
/* ends */

char *caption ;
char *rotation ;
char *thumbName;
char rotationBuffer[3];
/* declaration of basic exec commands to be used for calling external applications */
char *displayCommand = "display";
char *displayArgs[] = {"display", "", NULL };
char *convertCommand = "convert";
char *convertArgs[] = {"convert", "","-geometry","","-rotate","","", NULL };
/* ends */
char thumbNameBuffer[17];
char *indent = "-";

/*
* this methid will call "clear command" through exec
*/
int clearScreen(int shouldWaitToFinish){
	int pid, rc, status ;
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	/* child process for clearing screen starts */
	rc  = fork();
	if (0 == rc) {
		pid = getppid();
		char *args[] = {NULL};
		rc = execvp("clear",args);
		exit(11);
	}
	pid = rc ;
	rc = waitpid(pid, &status, 0);
	return rc ;
	/* child process for clear ends */
}

/**
* function to check for existance of a file
*/
int isFileExists(char *fname){
	return (access( fname, F_OK ) != -1 )? true : false ;
}
/**
* function for creating header html
*/
int createHtmlHeader(){
	remove(htmlFileName);
	FILE *fp ;
	fp = fopen(htmlFileName, "a");
	fprintf(fp, htmlFormatStart,caption);
	fclose(fp);
	return 0 ;
}
/**
* function for adding html for each image
*/
int createHtmlForImage(int imageNumber){
	char *breakStrng ;
	FILE *fp ;
	fp = fopen(htmlFileName, "a");
	breakStrng = ((imageNumber % HTML_BREAK_LIMIT) == 0) ? htmlBreakSyntax: "" ;
	fprintf(fp, htmlFormatForImages,breakStrng,thumbDir,imageNumber,imageFormat,mediumDir,imageNumber,imageFormat,thumbName);
	fclose(fp);
	return 0 ;
}
/**
* function for adding html footer.
*/
int createHtmlFooter(){
	FILE *fp ;
	fp = fopen(htmlFileName, "a");
	fprintf(fp, "%s",htmlFormatEnd);
	fclose(fp);
	return 0 ;
}
/*
* This method will generate final html for viewwing the digital directory.
*/
int createHtmlForImages(int numberOfImages){
	int count = 0 ;
	char *breakStrng ;
	remove(htmlFileName);
	remove(thumbDir);
	FILE *fp ;
	fp = fopen(htmlFileName, "a");
	fprintf(fp, htmlFormatStart,caption);
	for (count = 1; count <= numberOfImages; count++) {
		printf("writing thumb%d.jpg \n",count);
		// break html on after defined image threshold
		breakStrng = ((count % HTML_BREAK_LIMIT) == 0) ? htmlBreakSyntax: "" ;
		fprintf(fp, htmlFormatForImages,breakStrng,thumbDir,count,imageFormat,mediumDir,count,imageFormat,thumbName);
	}
	fprintf(fp, "%s",htmlFormatEnd);
	fclose(fp);
	return 0 ;
}

/*
* This method will call a separate process for creating digital directory.
* numberOfImages :- total number of images passed as input
*/
int generateHtmlandWait(int numberOfImages){
	/* child process for html Creation starts */
	int pid, rc, status ;
	rc  = fork();
	if (0 == rc) {
		pid = getppid();
		printf("generating html file \n");
		createHtmlForImages(numberOfImages);
		exit(11);
	}
	/* child process for html Creation ends */
	pid = rc ;
	rc = waitpid(pid, &status, 0);
	printf("html generation ends\n");
	return rc;
}
/**
* add html for a single image on a different process
*/
int generateHtmlForImage(int imageNumber){
	/* child process for html Creation starts */
	int rc ;
	rc  = fork();
	if (0 == rc) {
		getppid();
		createHtmlForImage(imageNumber);
		exit(11);
	}
	return rc;
}
/**
* add html header on a different process
*/
int generateHtmlHeader(){
	/* child process for html Creation starts */
	int rc ;
	rc  = fork();
	if (0 == rc) {
		createHtmlHeader();
		exit(11);
	}
	return rc;
}
/**
* add html footer on a different process and wait for it to finish
*/
int generateHtmlEndAndWait(){
	/* child process for html Creation starts */
	int pid, rc, status ;
	rc  = fork();
	if (0 == rc) {
		pid = getppid();
		printf("generating html file \n");
		createHtmlFooter();
		exit(11);
	}
	pid = rc ;
	rc = waitpid(pid, &status, 0);
	printf("html generation ends\n");
	return rc;
}
/** method for getting input from user
// Credits :- https://ssl.cs.dartmouth.edu/~sws/cs58-f15/proj1/demo.c
// prompt the user with message, and save input at buffer
// (which should have space for at least len bytes)
*/
int input_string(char *message, char *buffer, int len) {
  int rc = 0, fetched, lastchar;
  if (NULL == buffer)
    return -1;
  if (message)
    printf("%s: ", message);
  // get the string.  fgets takes in at most 1 character less than
  // the second parameter, in order to leave room for the terminating null.
  // See the man page for fgets.
  fgets(buffer, len, stdin);
  fetched = strlen(buffer);
  // warn the user if we may have left extra chars
  if ( (fetched + 1) >= len) {
    fprintf(stderr, "warning: might have left extra chars on input\n");
    rc = -1;
  }
  // consume a trailing newline
  if (fetched) {
    lastchar = fetched - 1;
    if ('\n' == buffer[lastchar])
      buffer[lastchar] = '\0';
  }
  return rc;
}

/**
* method to check whether rotatin is changed after user input rotation
*/
int isRotationChanged(){
	return (rotationBuffer[0] == '0') ? false : true ;
}

/**
*method to take care rotation parsing when user enters rotation input
*/
void getRotationFromChar(char* rotation){
	printf("parsing rotation %s \n " , rotation);
	switch (rotation[0]) {
		case 'r':{
			sprintf(rotationBuffer,"%s","90");
		}
		break;
		case 'l':{
			sprintf(rotationBuffer,"%s","270");
		}
		break;
		case 'i':{
			sprintf(rotationBuffer,"%s","180");
		}
		case 'n':{
			sprintf(rotationBuffer,"%s","0");
		}
		default :{
			sprintf(rotationBuffer,"%s","0");
		}
		break;
	}
}

/**
*helper method to ask user to press enter and continue
*/
void callEnterToContinue(void){
	printf("Please press enter to continue : \n");
		while (true){
			int c = getchar();
      if (c == '\n' || c == EOF)
				break;
			}
}


/**
	loopVariable = file number of the argv
	thumbSize = resize %
	rotation = rotation in degrees ex "90"
*/
int convertImage(int loopVariable,char *argv[],char *thumbSize, char *rotation,char *imagePath, int isWaitToFinish){
	int pid, rc, status ;
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	/* child process for convert to 25% starts */
	rc  = fork();
	if (0 == rc) {
		pid = getppid();
		convertArgs[1] = argv[loopVariable] ;
		convertArgs[3] = thumbSize;
		convertArgs[5] = rotation;
		sprintf(thumbNameBuffer,"%s%d%s",imagePath,loopVariable,imageFormat);
		convertArgs[6] = thumbNameBuffer ;
		rc = execvp(convertCommand,convertArgs);
		exit(11);
	}
	/* child process for display ends */
	pid = rc ;
	if(isWaitToFinish){
		printf("--- wait requested while converting image --- \n");
		rc = waitpid(pid, &status, 0);
	}
	return rc;
}
/**
*method to display images and wait till user closes ImageMagick.
*/
int displayImageAndWait(int loopVariable,char *argv[], char *imagePath){
	int pid, rc, status ;
	pid = getpid();
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	/* child process for display starts */
	rc  = fork();
	if (0 == rc) {
		pid = getppid();
		printf("working on file %s \n", argv[loopVariable]);
		sprintf(thumbNameBuffer,"%s%d%s",imagePath,loopVariable,imageFormat);
		displayArgs[1] = thumbNameBuffer ;
		rc = execvp(displayCommand,displayArgs);
		exit(11);
	}
	/* child process for display ends */
	pid = rc ;
	rc = waitpid(pid, &status, 0);
	return rc;
}
/**
*start creating thumbnails concurrently
*/
void startCreatingThumbnails(int imagesCount, char *argv[]){
	int count ;
	for (count = 1; count < imagesCount; count++) {
		printf("creating thumbs %s%d%s \n",thumbDirForFile,count,imageFormat);
		convertImage(count,argv,thumbSize,"0",thumbDirForFile,false); // rotation is default
	}
}
/**
* allocate memory
*/
void allocMemory(){
	caption = (char *)malloc(CAPTION_LEN);
	rotation = (char *)malloc(ROTATION_LEN);
	thumbName = (char *)malloc(THUMB_NAME_LEN);
}
/**
* free memory
*/
void deAllocMemory(){
	free(caption);
	free(rotation);
	free(thumbName);
}
/**
*program starts folder argument in mandatory
*/
int main(int argc, char *argv[]){
	//char *filePath = argv[1];
	int loopVariable = 0 ;
	allocMemory();
	clearScreen(true);
	if(argc > 1){
		// start creating thumbnail files before user input
		startCreatingThumbnails(argc,argv);
		input_string("Enter the name for your album (max 60 chars ) \n",caption,CAPTION_LEN);
		generateHtmlHeader();
		for(loopVariable = 1 ; loopVariable < argc ; loopVariable++){
			printf(" working on image %d : \n",loopVariable);
			sprintf(thumbNameBuffer,"%s%d%s",thumbDirForFile,loopVariable,imageFormat); // generating filename again
			if(isFileExists(thumbNameBuffer) == false){ // only call convert if file
				printf("%s file doesnt exists\n", thumbNameBuffer);
				convertImage(loopVariable,argv,thumbSize,"0",thumbDirForFile,true); // rotation is default
			}
			displayImageAndWait(loopVariable,argv,thumbDirForFile);
			input_string("\t\t  Enter image rotation \n\t n for Normal \n\t r for Right \n\t l for Left \n\t i for Inverted \n\t\n !! Note any other input will count as normal \n",rotation,ROTATION_LEN);
			getRotationFromChar(rotation);
			if(isRotationChanged()){
				// no need to rotate if its normal
				printf("reconverting after new rotation \n" );
				convertImage(loopVariable,argv,thumbSize,rotationBuffer,thumbDirForFile,false);
			}
			convertImage(loopVariable,argv,mediumSize,rotationBuffer,mediumDirForFile,false);
			input_string("\n\t Enter Caption for this Image : \n",thumbName,THUMB_NAME_LEN);
			generateHtmlForImage(loopVariable);
		}
		generateHtmlEndAndWait();
	}else{
		printf("a valid directory as an argument is needed !! \n");
	}
	deAllocMemory();
	printf("\n\n !! Your digital album is completed !! \n");
	callEnterToContinue();
	return 0 ;
}
