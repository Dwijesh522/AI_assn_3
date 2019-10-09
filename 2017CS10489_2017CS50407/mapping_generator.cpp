// given two graphs in "test.graphs" and "test.satoutput", generate output mapping in "test.mapping" file.
#include<iostream>
#include<string>
#include<fstream>
#include<cmath>
#include<utility>
#include<vector>
#include<map>
#include<algorithm>
#include<sstream>
using namespace std;

typedef enum {EMAIL, CALL, MAPPING} literal_type;

pair<int, int> key_to_edge(int key, int &email_node_size, int &call_node_size)
{
	// total number of element in mapping matrix
	int element_index = key; //- (email_node_size*(email_node_size-1) + call_node_size*(call_node_size-1));
	// zero based indexing
	int r, c;
	c = (element_index-1) % call_node_size;
	r = (element_index-c-1) / call_node_size;
	return make_pair(r+1, c+1);
}

int main(int argc, char *argv[])
{
	// reading output and mapping file name
	string mapping_filename="", filename="";
	if(argc < 2)	{ cout << "mapping file name not provided as command line argument.\n"; return 0;}
	filename = argv[1];
	mapping_filename = filename + ".mapping";
	
	// opening text file containg graphs sizes
	fstream size_file;
	size_file.open("size.txt");
	if(not size_file.is_open())	{ cout << "error faced while opening size file.\n"; return 0;}

	// getting sizes of two graphs
	string size="";
    int email_node_size, call_node_size;
    getline(size_file, size);
    email_node_size=stoi(size);
    getline(size_file,size);
    call_node_size=stoi(size);
	size_file.close();
    
    // opening file containg variable assignments returned by miniSAT
    int flag=1;//solution exists(1) or not(0)
    fstream input_file;
    input_file.open(filename+".satoutput");
    if(not input_file.is_open())    { cout << "error faced while opening satoutput file.\n"; return 0;}
    // checking whether satisfiable or not
    string str="";
    getline(input_file, str);
    if(str=="UNSAT")
        flag=0;
    else
    {
        getline(input_file,str);
        // Used to split string around spaces.
        istringstream ss(str);
        
        // Traverse through all words
  //      int counter=0;
        ofstream mappingf;
        mappingf.open(mapping_filename);
        if(!mappingf)    { cout << "error while creating mapping file.\n"; return 0;}

	string word;
	
//	cout << str << endl;
	while (ss >> word)
        {
            // Read a word
//            string word;
//            ss >> word;
            // Print the read word
//            if(ss!=0)
//            {
//                counter++;
//                if(counter>=(call_node_size*(call_node_size-1)+email_node_size*(email_node_size-1)+1))
  //              {
                    if((word[0]!='-')&&(word!="0"))
                    {
                        int key=stoi(word);
                        pair<int,int> temp=key_to_edge(key,email_node_size,call_node_size);
                        mappingf << to_string(temp.first) + " " + to_string(temp.second) +"\n";
                    }
//                }
//            }
            // While there is more to read
        }
        mappingf.close();
    }
    input_file.close();
    
    // writing mapping to filename.satoutput
    if(flag==0)
    {
        fstream mappingt;
        mappingt.open(mapping_filename, ios::out);
        if(!mappingt)    { cout << "error while creating mapping file.\n"; return 0;}
        mappingt<<"0\n";
        mappingt.close();
    }
	return 0;
}
