#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>
//#include <errno.h>
//#include <vector>
//#include <iostream>
//#include <iostream>
//#include <fstream>
//#include <string>
//#include <algorithm>
//#include <functional>


#include "image.h"
#include "opticalflow.h"
#include "io.h"
#include "Utility.h"

using namespace std;

//#include "ImageSource.h"
//using namespace std;
//enum InputDevice {AVI, USB, IMAGES, UNKNOWN};

//using namespace std;


//void usage()
//{
//  printf("usage:\n");
//  printf("./deepflow image1 image2 outputfile [options] \n");
//  printf("Compute the flow between two images and store it into a file\n");
//  printf("Images must be in PPM or JPG format");
//  printf("\n");
//  printf("options:\n");
//  printf("    -h, --help                                                print this message\n");
//  printf("    -a, -alpha              <float>(12.0)                    weight of smoothness terms\n");
//  printf("    -b, -beta               <float>(300.0)                   weight of descriptor matching\n");
//  printf("    -g, -gamma              <float>(3.0)                     weight of gradient constancy assumption\n");
//  printf("    -d, -delta              <float>(2.0)                     weight of color constancy assumption\n");
//  printf("    -s, -sigma              <float>(0.8)                     standard deviation of Gaussian presmoothing kernel\n");
//  printf("    -e, -eta                <float>(0.95)                    ratio factor for coarse-to-fine scheme\n");
//  printf("    -minsize                <interger>(25)                   size of the coarsest level\n");
//  printf("    -inner                  <integer>(5)                     number of inner fixed point iterations\n");
//  printf("    -iter                   <integer>(25)                    number of iterations for the solver\n");
//  printf("    -soromega               <float>(1.6)                     omega parameter of the sor method\n");
//  printf("    -bk                     <float>(0.0)                     use decreasing beta i.e. beta(k) = beta*( k / kmax )^betak, if 0, a last iteration is done with beta=0\n");
//  printf("\n");
//  printf("    -match                                                   '-match filename' reads matches from a file and '-match' from stdin. Matches must be given with the format <int> <int> <int> <int> <float> x1 y1 x2 y2 score at a resolution of 512x256\n");
//  printf("    -matchf                                                  same as -match with images at original resolution\n");
//  printf("\n");
//  printf("    -sintel                                                  set the parameters to the one used in the ICCV paper for MPI-Sintel dataset\n");
//  printf("    -middlebury                                              set the parameters to the one used in the ICCV paper for middlebury dataset\n");
//  printf("    -kitti                                                   set the parameters to the one used in the ICCV paper for KITTI dataset\n");
//  printf("\n");
//}

//void require_argument(const char *arg)
//{
//  fprintf(stderr, "Require an argument after option %s\n", arg);
//  exit(1);
//}

//int readConfigFile(char* configFile, InputDevice& input, char* resultDir, char* source, Rect& initBB = NULL) {
//	printf ("parsing config file %s\n", configFile);
//
//	fstream f;
//	char cstring[1000];
//	int readS=0;
//	f.open(configFile, fstream::in);
//	if (!f.eof()) {
//		//version
//		//skip first line
//		f.getline(cstring, sizeof(cstring));
//		f.getline(cstring, sizeof(cstring));
//		char param1[200]; strcpy(param1,"");
//		char param2[200]; strcpy(param2,"");
//		char param3[200]; strcpy(param3,"");
//
//		readS=sscanf (cstring, "%s %s", param1,param2);
//		char match[100];
//		strcpy(match,"0.1");
//		if (param2[2]!=match[2]) {
//			printf("ERROR: unsupported version of config file!\n");
//			return -1;
//		}
//		printf("  %s %s\n", param1, param2);
//
//		//source
//		f.getline(cstring, sizeof(cstring));
//		f.getline(cstring, sizeof(cstring));
//		f.getline(cstring, sizeof(cstring));
//		readS=sscanf (cstring, "  %s %s %s", param1,param2, param3);
//		strcpy(match,"IMAGES");
//
//        if (param3[0]==match[0])
//            input= InputDevice::IMAGES;
//        else
//            return -1;
//
//		printf("  %s %s %s\n", param1, param2, param3);
//		f.getline(cstring, sizeof(cstring));
//		f.getline(cstring, sizeof(cstring));
//		//directory
//		if (input!=InputDevice::SB) {
//			readS=sscanf (cstring, "%s %s %s", param1,param2, param3);
//			strcpy( source, param3 );
//
//			printf("  %s %s %s\n", param1, param2, param3);
//
//		}
//
//		f.getline(cstring, sizeof(cstring));
//		f.getline(cstring, sizeof(cstring));
//		f.getline(cstring, sizeof(cstring));
//		//debug information
//		readS=sscanf (cstring, "  %s %s %s", param1,param2, param3);
//		f.getline(cstring, sizeof(cstring));
//		strcpy(match,"true");
//		if (param3[0]==match[0]) {
//
//			readS=sscanf (cstring, "  %s %s %s", param1,param2, param3);
//			printf("  %s %s %s\n", param1, param2, param3);
//
//			//check if result dir exists
//			if ( access( param3, 0 ) == 0 )
//			{
//				struct stat status;
//				stat( param3, &status );
//
//				if ( status.st_mode & S_IFDIR )
//					strcpy(resultDir, param3);
//				else
//					resultDir = NULL;
//			}
//			else resultDir = NULL;
//			if (resultDir == NULL)
//				printf ("    ERROR: resulDir does not exist - switch off debug\n");
//		} else {
//			resultDir=NULL;
//		}
//
//		f.getline(cstring, sizeof(cstring));
//		f.getline(cstring, sizeof(cstring));
//		f.getline(cstring, sizeof(cstring));
//		//boosting parameters
//		readS=sscanf (cstring, "%s %s %d", param1,param2, &numBaseClassifiers);
//		printf("  %s %s %i\n", param1, param2, numBaseClassifiers);
//
//		f.getline(cstring, sizeof(cstring));
//		f.getline(cstring, sizeof(cstring));
//		f.getline(cstring, sizeof(cstring));
//		// search region
//		readS=sscanf (cstring, "%s %s %f", param1,param2, &overlap);
//		printf("  %s %s %5.3f\n", param1, param2, overlap);
//		f.getline(cstring, sizeof(cstring));
//		readS=sscanf (cstring, "%s %s %f", param1,param2, &searchFactor);
//		printf("  %s %s %5.3f\n", param1, param2, searchFactor);
//
//		f.getline(cstring, sizeof(cstring));
//		f.getline(cstring, sizeof(cstring));
//		f.getline(cstring, sizeof(cstring));
//		// initialization bounding box: MOUSE or COORDINATES
//		readS=sscanf (cstring, "  %s %s %s", param1,param2, param3);
//		strcpy(match,"MOUSE");
//		if (param3[0]==match[0])
//			initBB=Rect(0,0,0,0);
//		else {
//			strcpy(match,"COORDINATES");
//			if (param3[0]==match[0]) {
//				f.getline(cstring, sizeof(cstring));
//				f.getline(cstring, sizeof(cstring));
//				f.getline(cstring, sizeof(cstring));
//				readS=sscanf (cstring, "%s %s %i %i %i %i", param1,param2, &initBB.left, &initBB.upper, &initBB.width, &initBB.height);
//				printf("  %s %s %i %i %i %i\n", param1, param2, initBB.left, initBB.upper, initBB.width, initBB.height);
//			}
//		}
//	} else
//		return -1;
//
//	f.close();
//	printf ("parsing done\n\n");
//
//
//	return 0;
//}
//int getdir (string dir, vector<string> &files)
//{
//    DIR *dp;
//    struct dirent *dirp;
//    if((dp  = opendir(dir.c_str())) == NULL) {
//        //cout << "Error(" << errno << ") opening " << dir << endl;
//
//        cout << "Error: opening " << dir << endl;
//
//        return -1;
//    }
//
//    while ((dirp = readdir(dp)) != NULL) {
//        //files.push_back(string(dirp->d_name));
//        cout << "File: " << dirp->d_name << endl;
//
//    }
//    closedir(dp);
//    return 0;
//}

int main(int argc, char ** argv)
{

//load setting files


    //parse settings (including checking conditions)


    //repeat to read each pair image until end of the sequence


    //execute



    // load images


    image_t *match_x = NULL, *match_y = NULL, *match_z = NULL;

    char source[1024] = "IMAGES";
    //char* direction = "./data/Antoine 8";
    //char* direction = "/home/tuan/Dropbox/Project/DeepFlow_Adaptation/data/Antoine 8";
    //char* direction = "/home/tuan/data/Antoine 8";
    char direction[1024] = "D:\MyProjects\Research\MathLab\data\130618 Axiostar ht Col-0 bent to left_Modified";
    char imageTypeLoad[1024] = ".png";
    //char* saveDir = "./data/Antoine 8/result";
    char saveDir[1024] = "/home/tuan/Dropbox/Project/DeepFlow_Adaptation/data/Antoine 8/result";
    char fileSavedType[1024] = ".flo";
    char prefixInput[1024] = "Image_SEG_";//"Image_";
    char prefixOutput[1024] = "root_SEG_";//"root_";
    char readType[1024] = "SEQUENCE";
    //int startSeq = 1; //1->81, 81->154
    //int endSeq = 2;//3;//500;
    //int startSeq = atoi(argv[1]); //1->81, 81->154
    //int endSeq = atoi(argv[2]);//3;//500;
    int startSeq = 0; //1->81, 81->154
    int endSeq = 1;//3;//500;

    // load images
    if(argc < 3)
    {
        char line[1024];

        FILE * file = NULL;

        //string line;
        //ifstream myfile;
        //cout << "Total argc: " << argc << "\n";
        if (argc == 2) //assume this is the config file
        {
            //fprintf(stderr, argv[1]);
            file = fopen(argv[1], "r");
            //cout << "argv[1] " << argv[1] << "\n";
            //myfile.open(argv[1]);
        }
        else  if (argc == 1)
        {
            //fprintf(stderr, "ok here");
            file = fopen("config.txt", "r");
            //myfile.open("config.txt");
        }

        if (file != NULL)
        {
            //% deep flow
            //fscanf(file, "%s", line);
            //fscanf(file, "%s", line);
            fgets(line, sizeof(line), file);
            fgets(line, sizeof(line), file);

            //getline (myfile,line);
            //getline (myfile,line);
             //fprintf(stderr,line);
             // fprintf(stderr,"\n");
            //% source options: USB, AVI, IMAGES (only support IMAGES at this version)
            //fscanf(file, "%s", line);
            //fscanf(file, "%s", line);
             fgets(line, sizeof(line), file);
            fgets(line, sizeof(line), file);
            //getline (myfile,line);
            //getline (myfile,line);
            //source=trimwhitespace(line);
            //source=line;
            chomp(line);
            strcpy(source, line);
            //source=const_cast<char*>(trim(line).c_str());
            //cout<<"Source: " << source << "\n";
             //fprintf(stderr, pos);
             //                             fprintf(stderr, source);
             //fprintf(stderr, line);
             //fprintf(stderr,"\n");
             //printf("%d", sizeof(source));



            //% only if source is AVI OR IMAGES, director
            //fscanf(file, "%s", line);
            //fscanf(file, "%s", line);
            fgets(line, sizeof(line), file);
            fgets(line, sizeof(line), file);
            //getline (myfile,line);
            //cout<<"Test: " << line << "\n";
            //getline (myfile,line);
            //direction = const_cast<char*>(trim(line).c_str());//trimwhitespace(line.c_str());
            //direction =line;// trimwhitespace(line);
             chomp(line);
             strcpy(direction, line);
            //fprintf(stderr,direction);
            //  fprintf(stderr,"\n");
            //% only if source is AVI OR IMAGES, imageTypeLoad
            //fscanf(file, "%s", line);
            //fscanf(file, "%s", line);
            fgets(line, sizeof(line), file);
            fgets(line, sizeof(line), file);
            //getline (myfile,line);
            //getline (myfile,line);
            //imageTypeLoad =line;// trimwhitespace(line);
            chomp(line);
            strcpy(imageTypeLoad, line);
            //fprintf(stderr,imageTypeLoad);
            //fprintf(stderr,"\n");
            //% write debug information, saveDir
            //fscanf(file, "%s", line);
            //fscanf(file, "%s", line);
            fgets(line, sizeof(line), file);
            fgets(line, sizeof(line), file);
            //getline (myfile,line);
            //getline (myfile,line);
            //saveDir = line;//trimwhitespace(line);
           chomp(line);
            strcpy(saveDir, line);
            //fprintf(stderr,saveDir);
             //fprintf(stderr,"\n");
            //% prefix of each image, prefixInput
            //fscanf(file, "%s", line);
            //fscanf(file, "%s", line);
             fgets(line, sizeof(line), file);
            fgets(line, sizeof(line), file);
            //getline (myfile,line);
            //getline (myfile,line);
            //prefixInput = line;//trimwhitespace(line);
            chomp(line);
            strcpy(prefixInput, line);
            //fprintf(stderr,prefixInput);
            //fprintf(stderr,"\n");
            //% specify start and end number
            //fscanf(file, "%s", line);
            //fscanf(file, "%s", line);
             fgets(line, sizeof(line), file);

             //fprintf(stderr,line);
             //fprintf(stderr,"\n");
            //fgets(line, sizeof(line), file);
            fscanf(file, "%d", &startSeq);
            //printf("%d", startSeq);
            //fprintf(stderr,&startSeq);
            //fprintf(stderr,"\n");
            //getline (myfile,line);
            //getline (myfile,line);
            //startSeq = atoi(line);//trimwhitespace(line));
            //fprintf(stderr,&startSeq);
            //printf("%d", startSeq);
             //fprintf(stderr,"\n");
            //fscanf(file, "%s", line);
            //getline (myfile,line);
            //fgets(line, sizeof(line), file);
            //endSeq = atoi(line);//trimwhitespace(line));
            fscanf(file, "%d", &endSeq);
            //printf("%d\n", endSeq);
             //fprintf(stderr,&endSeq);
            //fprintf(stderr,"\n");

            //% prefix of each output, prefixOutput
            //fscanf(file, "%s", line);
            //fscanf(file, "%s", line);
            fgets(line, sizeof(line), file); //need this to skip to the next line
            //fprintf(stderr,"OK\n");
            //fprintf(stderr,line);
            // fprintf(stderr,"\n");
            fgets(line, sizeof(line), file);
             fgets(line, sizeof(line), file);
            //getline (myfile,line);
            //getline (myfile,line);
            //prefixOutput = line;//trimwhitespace(line);
           chomp(line);
            strcpy(prefixOutput, line);
            //fprintf(stderr,prefixOutput);
             // fprintf(stderr,"\n");
            //% read: FOLDER or SEQUENCE, readType
            //fscanf(file, "%s", line);
            //fscanf(file, "%s", line);
             fgets(line, sizeof(line), file);
            fgets(line, sizeof(line), file);
            //getline (myfile,line);
            //getline (myfile,line);
            //readType = line;//trimwhitespace(line); //comment this out
           chomp(line);
            strcpy(readType, line);
           //fprintf(stderr,readType);
            //fprintf(stderr,"\n");

            //myfile.close();
            fclose(file);
            //stop the programm for testing
            //exit(1);
        }
        else
        {
            fprintf(stderr,"Open file error. \n\n");
            exit(1);
        }
    }
    else
    {
        fprintf(stderr,"Wrong command, require 3 arguments, 2 last arguments are start and end frames. ex: ./deepflow 1 3 .\n\n");
        //usage();
        exit(1);
    }


    // set params to default
    optical_flow_params_t* params = (optical_flow_params_t*) malloc(sizeof(optical_flow_params_t));

    if(!params)
    {
        fprintf(stderr,"error deepflow(): not enough memory\n");
        exit(1);
    }

	//get parameters from the file
    //optical_flow_params_default(params);
	getOpticalFlowParameter("opticalflowparams.txt", params);
	

    //ignore parameters for now
 //fprintf(stderr,readType);
    if(strcmp(readType, "SEQUENCE") == 0)
    {
        //fprintf(stderr,direction);
        int index = 0;
        char* fullname1;
        char* name1;
        char* fullname2;
        char* name2;

        char* output;
        char* fulloutput;

        int sizeString = 2024;

        //printf("Begin ");
        fprintf(stderr, "Begin processing:\n");

        fullname1 = (char*)malloc(sizeString);
        fullname2 = (char*)malloc(sizeString);

        name1 = (char*)malloc(256);
        name2 = (char*)malloc(256);

        fulloutput = (char*)malloc(sizeString);
        output = (char*)malloc(256);

        for(index = startSeq; index <= endSeq - 1; index++)
        {
            //fullname1 = strcpy(fullname1, direction);
            //fullname2 = strcpy(fullname2, direction);

            //name1 = strcpy(name1, prefixInput);
            //name2 = strcpy(name2, prefixInput);
            //char* s = "";
            //s = (char*)malloc(4);
            //name1 = strcat(name1, itoa(index, s, 10));
            //name2 = strcat(name2, itoa(index+1, s, 10));

            sprintf(name1, "%s%d%s", prefixInput, index, imageTypeLoad);
            sprintf(name2, "%s%d%s", prefixInput, index + 1, imageTypeLoad);

            //fprintf(stderr, "Begin 3\n");

            //fullname1 = strcat(fullname1, name1);
            //fullname2 = strcat(fullname2, name2);
            sprintf(fullname1, "%s/%s", direction, name1);
            sprintf(fullname2, "%s/%s", direction, name2);

            fprintf(stderr, "Process Images:\n");

            fprintf(stderr, " %s \n", fullname1);
            fprintf(stderr, " %s \n", fullname2);

            //printf("image1: %s \n", fullname1);
            //printf("image2: %s \n", fullname2);

            color_image_t *im1 = color_image_load(fullname1), *im2 = color_image_load(fullname2);

            sprintf(output, "%s%d_%d%s", prefixOutput, index, index+1, fileSavedType);
            sprintf(fulloutput, "%s/%s", saveDir, output);

            //prefixOutput = strcat(prefixOutput, output);

            //printf("output: %s \n", fulloutput);
            fprintf(stderr, "output: %s \n", fulloutput);

            //find
            image_t *wx = image_new(im1->width,im1->height), *wy = image_new(im1->width,im1->height);
            optical_flow(wx, wy, im1, im2, params, match_x, match_y, match_z);

            //fulloutput = strcpy(fulloutput, saveDir);

            writeFlowFile(fulloutput, wx, wy);

            image_delete(wx);
            wx = NULL;
            image_delete(wy);
            wy = NULL;
            image_delete(match_x);
            match_x = NULL;
            image_delete(match_y);
            match_y = NULL;
            image_delete(match_z);
            match_z = NULL;
            color_image_delete(im1);
            im1 = NULL;
            color_image_delete(im2);
            im2 = NULL;

        }

        fprintf(stderr, "Finished!\n");
    }


    free(params);


//   string dir = string(".");
//   vector<string> files = vector<string>();
//
//    getdir(dir,files);
//
//    for (unsigned int i = 0;i < files.size();i++) {
//        cout << files[i] << endl;
//    }



    return 0;
}



