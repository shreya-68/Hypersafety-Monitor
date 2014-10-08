#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define NUM_USERS 23 
#define NUM_DAYS 5

struct Location {
    char *date;
    char *time;
    int timestamp;
    float latNum;
    float lotNum;
    char* lat;
    char* lont;
    int box;
};


int main()
{
    char ch, *file_name = malloc(25*sizeof(char));
    char *file_write = malloc(25*sizeof(char));
    FILE *fp, *fp_write;
    struct Location*** points = malloc(sizeof(void)*10);
    int i, j;
    for (i = 3; i <= NUM_USERS; i++)
    {
        char num[4];
        snprintf(num, 3, "%d", i);
        strcpy(file_name, "gps_volunteer_");
        strcat(file_name, num);
        strcat(file_name, ".txt");
        fp = fopen(file_name, "r");
        char *token;
        int index;
        int stamp = 0;
        int float_to_int;
        if(fp != NULL)
        {
            char line[128];

            struct Location *prevPoint = NULL;
            char *prevDate = malloc(15*sizeof(char));
            prevDate = "";
            j = 1;
            int fileNum;
            while(fgets(line, sizeof line, fp) != NULL ) 
            {
                struct Location *newPoint = malloc(sizeof(struct Location));
                token = strtok(line, "\t");
                index = 0;
                while(token != NULL)
                {
                    switch(index)
                    {
                        case 0:
                            newPoint->date = token;
                            if(strcmp(prevDate, "") == 0)
                            {
                                fileNum = (i*100) + j;
                                snprintf(file_write, 5, "%d", fileNum);
                                strcat(file_write, ".txt");
                                fp_write = fopen(file_write, "w");
                            }
                            else if (strcmp(prevDate, newPoint->date) != 0)
                            {
                                fclose(fp_write);
                                j++;
                                fileNum = (i*100) + j;
                                snprintf(file_write, 5, "%d", fileNum);
                                strcat(file_write, ".txt");
                                fp_write = fopen(file_write, "w");
                                prevPoint = NULL;
                                stamp = 0;
                            }
                            prevDate = malloc(15*sizeof(char));
                            int size = strlen(token);
                            strncpy(prevDate, token, size);
                            prevDate[size] = '\0';
                            break;
                        case 1:
                            newPoint->time = token;
                            break;
                        case 2:
                            newPoint->lat = token;
                            newPoint->latNum = atof(newPoint->lat);
                            break;
                        case 3:
                            newPoint->lont = token;
                            newPoint->lotNum = atof(newPoint->lont);
                            break;
                    }
                    index++;
                    token = strtok(NULL, "\t");
                }
                newPoint->timestamp = stamp;
                int box = 0;
                float minLat = 32.47000;
                float minLong = -121.1000;
                float maxLat = 58.7500;
                float maxLong = -123.5000;
                int numCols = (minLong - maxLong - 0.01)*100;
                int x = (int)((newPoint->latNum - minLat)*100);
                int y = (int)((newPoint->lotNum - maxLong)*100);
                newPoint->box = (x*numCols) + y;
                //printf("Box: %d", newPoint->box);
                if(prevPoint == NULL || prevPoint->box != newPoint->box)
                {
                    prevPoint = newPoint;
                    char str[50];
                    snprintf(str, 10, "%d", newPoint->timestamp);
                    strcat(str, ",");
                    char s[10];
                    snprintf(s, 10, "%d", newPoint->box);
                    strcat(str, s);
                    strcat(str, "\n");
                    fputs(str, fp_write);
                    stamp++;
                }
                //*(points[i]+stamp) = newPoint;
                //printf("Date: %s,", (*(points[i]+stamp))->date);
                //printf("Time: %s,", (*(points[i]+stamp))->time);
                //printf("Lat: %s,", (*(points[i]+stamp))->lat);
                //printf("Longitude: %s", (*(points[i]+stamp))->lont);
                //printf("LatNum: %f", (*(points[i]+stamp))->latNum);
                //printf("LontNum: %f", (*(points[i]+stamp))->lotNum);
                //printf("Timestamp: %d", (*(points[i]+stamp))->timestamp);
                //printf("\n");
                //fputs(line, stdout);
            }
            fclose(fp);
            fclose(fp_write);
        }
        else
        {
            perror(file_name);
        }
    }
    return 0;
}
