
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <vector>
#include <set>
#include <sys/time.h>
#include <sys/resource.h>
#include <unistd.h>


#define MAX_MONITOR 20000
#define INIT_STATES 5
#define FINAL_STATES 5
#define NUM_DAYS 60
#define NUM_USERS 23

using namespace std;

struct Location {
    char *date;
    char *time;
    int timestamp;
    float latNum;
    float lotNum;
    char* lat;
    char* lont;
};


struct State {
    std::set<int> location;
    int step;
    State *next;
    int decision;
};

struct Monitor {
    State *start;
    int user;
};

static std::vector<Monitor*> PetriNet;
static int final_decision = 0;

int compare(State a, State b)
{
    if(a.location == b.location && a.step == b.step)
        return 1;
    return 0;
}

int parseLine(char* line){
        int i = strlen(line);
        while (*line < '0' || *line > '9') line++;
        line[i-3] = '\0';
        i = atoi(line);
        return i;
    }
    

int getValue(){ //Note: this value is in KB!
        FILE* file = fopen("/proc/self/status", "r");
        int result = -1;
        char line[128];
    

        while (fgets(line, 128, file) != NULL){
            if (strncmp(line, "VmSize:", 7) == 0){
                result = parseLine(line);
                break;
            }
        }
        fclose(file);
        return result;
    }

long getVal()
{
long rss = 0L;
	FILE* fp = NULL;
	if ( (fp = fopen( "/proc/self/statm", "r" )) == NULL )
		return (size_t)0L;		// Can't open? */
	if ( fscanf( fp, "%*s%ld", &rss ) != 1 )
	{
		fclose( fp );
		return (size_t)0L;		// Can't read? */
	}
	fclose( fp );
	return (size_t)rss * (size_t)sysconf( _SC_PAGESIZE);
}

int main()
{
    //Read FLTL property
    struct rusage usage;
    struct timeval start, end;
    char prop_file[14] = "prop_file.txt";
    FILE *prop_fp;
    //prop_fp = fopen(prop_file, "r");
    //Read prop_file to get FORALL, variables, relations
    char* relations[10];
    relations[0] = "location";
    int arity[10];
    arity[0] = 2;
    char* rel_file[10];
    //rel_file[0] = "";
    //rel_file[0] = strcat(rel_file[0], relations[0]);
    //rel_file[0] = strcat(rel_file[0], ".txt");
    rel_file[0] = "location.txt";
    //End reading FLTL property and related files

    //Init Set of Monitors
    int last_decision = 0;

    


    char ch, *new_trace = (char*)malloc(25*sizeof(char));
    FILE *trace_fp;
    int read = 1;
    int day;
    int user;
    int fileNum;
    int k;
    int traces = 0;
    int violations = 0;
    bool loop = true;
    for(user = 3; user <= NUM_USERS && loop; user++)
    {
        for(day = 1; day <= NUM_DAYS && loop; day ++)
        {
            fileNum = (user*100) + day;
            snprintf(new_trace, 5, "%d", fileNum);
            strcat(new_trace, ".txt");
            trace_fp = fopen(new_trace, "r");
            if(trace_fp != NULL)
            {
                traces++;
                char line[12];
                char *token;
                std::vector<State*> Test;
                for(std::vector<Monitor*>::iterator itr = PetriNet.begin(); itr != PetriNet.end(); itr++)
                {
                    if ((*itr)->user == user) {
                        Test.push_back((*itr)->start);
                    }
                }
                int found = -1;
                Monitor *newMonitor = new Monitor;
                newMonitor->start = NULL;
                newMonitor->user = user;
                State *prevState;
                std::set<int> visited_locations;
                if(fgets(line, sizeof line, trace_fp) != NULL ) {
                    token = strtok(line, ",");
                    State *newState = new State;
                    newState->step = atoi(token);
                    newState->decision = 0;
                    newState->next = NULL;
                    token = strtok(NULL, ",");
                    if(token != NULL) {
                        (newState->location).insert(atoi(token));
                        visited_locations.insert(atoi(token));
                    }
                    else
                        break;
                    for(std::vector<State*>::iterator itr = Test.begin(); itr != Test.end();) {
                                if(compare(**itr, *newState) == 1){
                                    (*itr) = (*itr)->next;
                                    itr++;
                                }
                                else
                                    itr = Test.erase(itr);
                    }
                    newMonitor->start = newState;
                    prevState = newMonitor->start;
                }

                while(fgets(line, sizeof line, trace_fp) != NULL ) {
                    token = strtok(line, ",");
                    token = strtok(NULL, ",");
                    if(token != NULL)
                        visited_locations.insert(atoi(token));
                    else
                        break;
                }
                State *finalState = new State;
                finalState->step = 1;
                finalState->decision = 1;
                finalState->next = NULL;
                finalState->location = visited_locations;
                prevState->next = finalState;
                if(Test.size() == 0) {
                    PetriNet.push_back(newMonitor);
                }
                else {
                    for(std::vector<State*>::iterator itr = Test.begin(); itr != Test.end();) {
                        if(compare(**itr, *finalState) != 1) {
                            violations++;
                            if(violations == 1)
                            {
                                final_decision = 1;
                                cout<<"PROPERTY VIOLATED!"<<endl;
                                long memy = getVal();
                                printf("Memory : %lu\n", memy);
                                cout<<"Number of monitors is "<<PetriNet.size()<<endl;
                                cout<<"Number of traces is "<<traces<<endl;
                                cout<<"Number of violations is "<<violations<<endl;
                            }
                        }
                    }
                }
                    //loop = false;

                    //for(std::vector<State*>::iterator itr = Test.begin(); itr != Test.end(); itr++)
                    //{
                    //    //printf("Decision in this state at step %d, location %d, is %d", (*itr)->step, (*itr)->location, (*itr)->decision);
                    //    if((*itr)->decision == 1 || k > K_VALUE)
                    //    {
                    //        final_decision = 1;
                    //        cout<<"PROPERTY VIOLATED!"<<endl;
                    //        break;
                    //    }
                    //}
                fclose(trace_fp);
                //if(final_decision == 1)
                //    break;

            }
        }
    }
    getrusage(RUSAGE_SELF, &usage);
    end = usage.ru_utime;
    start = usage.ru_stime;
    long mem = getVal();
    printf("User time : %ld.%lds\n", end.tv_sec, end.tv_usec);
    printf("System time : %ld.%lds\n", start.tv_sec, start.tv_usec);
    printf("Memory : %lu\n", mem);
    cout<<"Number of monitors is "<<PetriNet.size()<<endl;
    cout<<"Number of traces is "<<traces<<endl;
    cout<<"Number of violations is "<<violations<<endl;
    return 0;
}
