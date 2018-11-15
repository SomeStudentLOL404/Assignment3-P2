#include <iostream>
#include <fstream>
#include <string.h>
#include <vector>
#include "tokens.h"
#include "parse.h"
#include "parsetree.h"
#include "tokens.h"

using namespace std;

int main(int argc, char* argv[])
{
    ifstream infile1;
    istream *in = &cin;
    int linenum = 0;
    for(int i = 1; i < argc; i++)
    {
        string arg(argv[i]);
        if(in != &cin)
        {
            cout << "TOO MANY FILENAMES" << endl;
            return -1;
        }
        else {
            infile1.open(arg);
            if (infile1.is_open() == false)
            {
                cout << "COULD NOT OPEN " << arg << endl;
                return -1;
            }
            in = &infile1;
        }
    }
    ParseTree *prog = Prog(in, &linenum);
    if( prog == 0 )
    {
        return 0; // quit on error
    }
    //DO NOT IMPLEMENT COUNTER IN MAIN - keep it here as professor said
    cout << "LEAF COUNT: " << prog->LeafCount() << endl;
    cout << "STRING COUNT: " << prog->StringCount() << endl;
}