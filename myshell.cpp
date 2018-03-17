// CS 3377.0W1 Programming Assignment 3
// zxv160130
// Zoraida Vences
/* 
 * This program will take input from the user to run a simple
 * Unix command. It can run several commands separated by ';'.
 * This is done using exec functions and file redirections.
 */

//#include <cstdlib>
#include <iostream>
#include <unistd.h>
#include <vector>
#include <string>
#include <sstream>
#include <iterator>
#include <sys/wait.h>
#include <sys/types.h>

using namespace std;

string askUser();

string addPath(string);

void perfCommand(vector<string>);

void pipeCommand(vector<string>, vector<string>);

void redirectCommand(vector<string>, vector<string>);

/*
 * 
 */
int main(int argc, char** argv) 
{
    string input;
  
    input = askUser();
  
    
    stringstream ss(input);
    
    // separates input by spaces
    istream_iterator<string> begin(ss);
    istream_iterator<string> end; 
    vector <string> sepInput(begin, end);
    
    
    int i;
    for(i =0; i < sepInput.size(); i++)
    {
        if(sepInput[i].find(';') != string::npos) 
        { // finds multiple commands
            int flag = 1;
            vector<string> firstCom;
            vector<string> secondCom;
            vector<string> thirdCom;
            
            for(int u = i+1; u < sepInput.size(); u++)
            { // detects more than one ';'
                if(sepInput[u].find(';') != string::npos)
                {
                    cout << "three commands found" << endl;
                    
                    sepInput[i].pop_back();
                    sepInput[u].pop_back();

                    for (int j =0; j <= i; j++)
                    {
                        firstCom.push_back(sepInput[j]);
                    }

                    for (int j = i+1; j <= u; j++)
                    {
                        secondCom.push_back(sepInput[j]);
                    }
                    
                    for (int j = u+1; j < sepInput.size(); j++)
                    {
                        thirdCom.push_back(sepInput[j]);
                    }

                     // runs each command separately 
                    perfCommand(firstCom);

                    perfCommand(secondCom);
                    
                    perfCommand(thirdCom);
                }
                else if(flag==1)
                { 
                    cout << "two commands found" << endl;
                    sepInput[i].pop_back();

                    for (int j =0; j <= i; j++)
                    {
                        firstCom.push_back(sepInput[j]);
                    }

                    for (int j = ++i; j < sepInput.size(); j++)
                    {
                        secondCom.push_back(sepInput[j]);
                    }

                    // runs each command separately 
                    perfCommand(firstCom);

                    perfCommand(secondCom);
                    
                    flag++;
                }
            }
            
        }
        else if (sepInput[i].find('|') != string::npos)
        { // detects a pipeline
            int flag = 1;
            vector<string> firstCom;
            vector<string> secondCom;
            vector<string> thirdCom;
            
            for(int u = i+1; u < sepInput.size(); u++)
            {
                if(sepInput[u].find('|') != string::npos)
                {
                    cout << "three commands to pipeline" << endl;
                 

                    for (int j =0; j < i; j++)
                    {
                        firstCom.push_back(sepInput[j]);
                    }

                    for (int j = i+1; j < u; j++)
                    {
                        secondCom.push_back(sepInput[j]);
                    }
                    
                    for (int j = u+1; j < sepInput.size(); j++)
                    {
                        thirdCom.push_back(sepInput[j]);
                    }


                    
                }
                else if (flag == 1)
                {
                    cout << "two commands to pipeline" << endl;
                    

                    for (int j =0; j < i; j++)
                    {
                        firstCom.push_back(sepInput[j]);
                    }

                    for (int j = ++i; j < sepInput.size(); j++)
                    {
                        secondCom.push_back(sepInput[j]);
                    }


                    pipeCommand(firstCom, secondCom);
                }
            }
        }
        else if (sepInput[i].find_first_of("<>") != string::npos)
        {
            cout << "redirection found" << endl;
            int flag = 1;
            vector<string> firstCom;
            vector<string> secondCom;
            vector<string> thirdCom;
            
            for(int u = i+1; u < sepInput.size(); u++)
            {
                if(sepInput[u].find_first_of("<>") != string::npos)
                {
                    cout << "three commands to redirect" << endl;
                 

                    for (int j =0; j < i; j++)
                    {
                        firstCom.push_back(sepInput[j]);
                    }

                    for (int j = i+1; j < u; j++)
                    {
                        secondCom.push_back(sepInput[j]);
                    }
                    
                    for (int j = u+1; j < sepInput.size(); j++)
                    {
                        thirdCom.push_back(sepInput[j]);
                    }


                    
                }
                else if (flag == 1)
                {
                    cout << "two commands to redirect" << endl;
                    

                    for (int j =0; j < i; j++)
                    {
                        firstCom.push_back(sepInput[j]);
                    }

                    for (int j = ++i; j < sepInput.size(); j++)
                    {
                        secondCom.push_back(sepInput[j]);
                    }


                    pipeCommand(firstCom, secondCom);
                }
            }
        }
    }
    
    if(i == sepInput.size())
    { // performs on one command 
        perfCommand(sepInput);
    }
    
    cout <<"\n------finished-------"<<endl;
    
    
    return 0;
}

string askUser()
{ // prompts user
    string input;
    cout << "Enter a string" << endl;
    // cout << "Or enter QUIT to exit" << endl;

    // get input
    getline(cin, input);
    
    return input;
}

string addPath(string first)
{ // adds path to command
    string newPath = "/bin/";
    newPath.append(first);
    return newPath;
}

void perfCommand(vector<string> str)
{ // performs commands
    char * args[] = {NULL, NULL, NULL, NULL, NULL};
    int i;
    
    // turns string into char *
    for (i = 0; i < str.size(); i++)
    {
        char * token = new char[str[i].size()+1];
        copy(str[i].begin(), str[i].end(), token);
        token[str[i].size()] = '\0';
        
        args[i] = token;
    }
   
    
    // creates new path
    string path;
    path = addPath(str[0]);
    
    const char * pathArg = path.c_str();
    
    pid_t pid = fork();
    int status;
    
    
    if (pid == -1)
    {
        cout << "error, did not fork()" << endl;
    }
    else if (pid == 0)
    {
        execv(pathArg, args);
    }
    else{
        if (waitpid(pid, &status, 0) > 0)
        {
            //cout << "waitpid worked" << endl;
        }
        else
        {
            cout << "waitpid failed " << endl;
        }
    }
    
}

void pipeCommand(vector<string> str1, vector<string> str2)
{ // pipelines command
    char * args1[]={NULL,NULL,NULL,NULL,NULL};
    char * args2[]={NULL,NULL,NULL,NULL,NULL};
    int piFileDes[2];
    pid_t pid1, pid2;
    int status;
    
    for (int i = 0; i < str1.size(); i++)
    {
        char * token = new char[str1[i].size()+1];
        copy(str1[i].begin(), str1[i].end(), token);
        token[str1[i].size()] = '\0';
        
        args1[i] = token;
    }
    for (int i = 0; i < str2.size(); i++)
    {
        char * token = new char[str2[i].size()+1];
        copy(str2[i].begin(), str2[i].end(), token);
        token[str2[i].size()] = '\0';
        
        args2[i] = token;
    }
    string path1;
    string path2;
    
    path1 = addPath(str1[0]);
    const char * pathArg1 = path1.c_str();
    
    path2 = addPath(str2[0]);
    const char * pathArg2 = path2.c_str();
    
    pipe(piFileDes);
    
    pid1 = fork();
    
    if(pid1 == 0)
    {
        
        dup2(piFileDes[0], 0);
        
        close(piFileDes[1]);
        
        execv(pathArg2, args2);
        
        if (waitpid(pid1, &status, 0) > 0)
        {
            //cout << "waitpid worked" << endl;
        }
        else
        {
            cout << "waitpid failed " << endl;
        }
        
        perror("child failed");
        
    }
    else
    {
        
        dup2(piFileDes[1], 1);
        
        close(piFileDes[0]);
        
        execv(pathArg1, args1);
    }
    
    close(piFileDes[0]);
    close(piFileDes[1]);
    
    
    cout << "done" << endl;
    
    
}
