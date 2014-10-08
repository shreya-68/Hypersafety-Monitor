
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <vector>
#include <sys/time.h>
#include <sys/resource.h>


#define MAX_MONITOR 20

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
    int location;
    int step;
    State *next;
    int decision;
};

struct Monitor {
    State *start;
};

static std::vector<Monitor*> PetriNet;
static int final_decision = 0;

int compare(State a, State b)
{
    if(a.location == b.location && a.step == b.step)
        return 1;
    return 0;
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
    int fileNum = 11;
    while(read)
    {
        cout<<"Enter the name of the file containing trace"<<endl;
        gets(new_trace);
        trace_fp = fopen(new_trace, "r");
        if(trace_fp != NULL)
        {
            char line[12];
            char *token;
            std::vector<State*> Test;
            for(std::vector<Monitor*>::iterator itr = PetriNet.begin(); itr != PetriNet.end(); itr++)
            {
                Test.push_back((*itr)->start);
            }
            int found = 1;
            int init = 1;
            Monitor *newMonitor = new Monitor;
            newMonitor->start = NULL;
            State *prevState;
            while(fgets(line, sizeof line, trace_fp) != NULL ) 
            {
                token = strtok(line, ",");
                State *newState = new State;
                newState->step = atoi(token);
                token = strtok(NULL, ",");
                newState->location = atoi(token);
                newState->decision = 0;
                for(std::vector<State*>::iterator itr = Test.begin(); itr != Test.end(); )
                {
                    if(!init)
                    {
                        (*itr) = (*itr)->next;
                    }
                    if(compare(**itr, *newState))
                    {
                        itr++;
                    }
                    else
                    {
                        itr = Test.erase(itr);
                    }

                }
                if(init)
                    init = 0;
                if(newMonitor->start == NULL)
                {
                    newMonitor->start = newState;
                    prevState = newMonitor->start;
                }
                else
                {
                    prevState->next = newState;
                    prevState = newState;
                }
                if(Test.size() == 0)
                {
                    found = 0;
                    break;
                }
            }
            if(found == 0)
            {
                while(fgets(line, sizeof line, trace_fp) != NULL ) 
                {
                    token = strtok(line, ",");
                    State *newState = new State;
                    newState->step = atoi(token);
                    token = strtok(NULL, ",");
                    newState->location = atoi(token);
                    newState->decision = 0;
                    if(newMonitor->start == NULL)
                    {
                        newMonitor->start = newState;
                        prevState = newMonitor->start;
                    }
                    else
                    {
                        prevState->next = newState;
                        prevState = newState;
                    }
                }
                prevState->decision = 1;
                PetriNet.push_back(newMonitor);
            }
            else
            {
                for(std::vector<State*>::iterator itr = Test.begin(); itr != Test.end(); itr++)
                {
                    printf("Decision in this state at step %d, location %d, is %d", (*itr)->step, (*itr)->location, (*itr)->decision);
                    if((*itr)->decision == 1)
                    {
                        final_decision = 1;
                        cout<<"PROPERTY VIOLATED!"<<endl;
                        break;
                    }
                }
            }
            fclose(trace_fp);
            if(final_decision == 1)
                break;

        }
        fileNum++;

    }
    getrusage(RUSAGE_SELF, &usage);
    end = usage.ru_utime;
    printf("Ended at: %ld.%lds\n", end.tv_sec, end.tv_usec);
    cout<<"Number of monitors is "<<PetriNet.size()<<endl;
    return 0;
}
