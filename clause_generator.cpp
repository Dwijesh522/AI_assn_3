// given two graphs in "test.graphs", generate minisat input clauses in cnf format in "test.satinput" file.
#include<iostream>
#include<fstream>
#include<cmath>
#include<utility>
#include<vector>
#include<map>
#include<algorithm>
using namespace std;

// calculate npk
int permutation(int n, int k)
{
	int p=1;
	for(int i=0; i<k; i++)	p*= (n-i);
	return p;
}

// sort wrt starting node of the edge and if there is any tie then wrt destination node
bool edge_comparator(pair<int, int> edge1, pair<int, int> edge2)
{
	if(edge1.first < edge2.first)	return true;
	if(edge1.first > edge2.first)	return false;
	return edge1.second < edge2.second;
}

// true: if query edge is present in edges
// false: if query edge is not present in edges
// search type: binary search, edges are sorted.
bool edge_search(vector<pair<int, int>> &edges, pair<int, int> &query_edge, int start, int end)
{
	if(start > end)	return false;

	int query_source, query_destination, curr_source, curr_destination;
	query_source = query_edge.first;
	query_destination = query_edge.second;
	
	if(start == end)
		// single element !
		if(edges[start].first == query_source and edges[start].second == query_destination)		return true;
		else												return false;
	
	int middle = (start+end)/2;
	curr_source = edges[middle].first;
	curr_destination = edges[middle].second;

	if(curr_source < query_source)			return edge_search(edges, query_edge, middle+1, end);
	if(curr_source > query_source)			return edge_search(edges, query_edge, start, middle-1);
	// sources are same.
	if(curr_destination < query_destination)	return edge_search(edges, query_edge, middle+1, end);
	if(curr_destination > query_destination)	return edge_search(edges, query_edge, start, middle-1);
	// destinations are also the same
	return true;
}

int main(int argc, char *argv[])
{
	// reading input file name
	string graphs_filename="", filename="";
	if(argc < 2)	{ cout << "input file name not provided as command line argument.\n"; return 0;}
	filename = argv[1];
	graphs_filename = filename + ".graphs";
	
	// opening input file
	fstream input_file;
	input_file.open(graphs_filename);
	if(not input_file.is_open())	{ cout << "error faced while opening input file.\n"; return 0;}

	// getting sizes of two graphs, storing edges as pair of nodes.
	string edge="";
	int email_node_size = -INFINITY, call_node_size = -INFINITY;
	bool zero_crossed=false;
	vector<pair<int, int>> email_edges, call_edges;

	while(getline(input_file, edge))
	{
		int edge_string_size=edge.length();
		string node1="", node2="";
		// call edges
		if(not zero_crossed)
		{
			// reading edge nodes as strings
			int i=0;
			for(; edge[i]!=' '; i++)		node1 += edge[i];
			i++;
			for(; i<edge_string_size; i++)		node2 += edge[i];
			// modifing node sizes
			int int_node1 = stoi(node1), int_node2 = stoi(node2);
			// checking for zero crossing
			if(int_node1==0 and int_node2 == 0)	zero_crossed=true;
			else
			{
				if(int_node1 > call_node_size)		call_node_size = int_node1;
				if(int_node2 > call_node_size)		call_node_size = int_node2;
				// adding edge to call_edges for further use
				call_edges.push_back(make_pair(int_node1, int_node2));
			}
		}
		// email edges
		else
		{
			// reading edge nodes as strings
			int i=0;
			for(; edge[i]!=' '; i++)		node1 += edge[i];
			i++;
			for(; i<edge_string_size; i++)		node2 += edge[i];
			// modifing node sizes
			int int_node1 = stoi(node1), int_node2 = stoi(node2);
			if(int_node1 > email_node_size)		email_node_size = int_node1;
			if(int_node2 > email_node_size)		email_node_size = int_node2;
			// adding edge to call_edges for further use
			email_edges.push_back(make_pair(int_node1, int_node2));
		}
	}
	input_file.close();

	//// creating map to refer node id wrt the variable index
	map<int, pair<int, int>> variable_to_node;
	int offset=1;
	// email edge variables: eij
	for(int i=0; i<email_node_size; i++)
		for(int j=0; j<email_node_size; j++)
			if(i!=j)
			{
				variable_to_node.insert(make_pair(offset, make_pair(i+1, j+1)));
				offset++;
			}
	// call edge variables: cij
	for(int i=0; i<call_node_size; i++)
		for(int j=0; j<call_node_size; j++)
			if(i!=j)
			{
				variable_to_node.insert(make_pair(offset, make_pair(i+1, j+1)));
				offset++;
			}
	// mapping variables: mij
	for(int i=0; i<email_node_size; i++)
		for(int j=0; j<call_node_size; j++)
		{
			variable_to_node.insert(make_pair(offset, make_pair(i+1, j+1)));
			offset++;
		}

	// printing key and value
//	for(auto i=variable_to_node.begin(); i!=variable_to_node.end(); i++)
//		cout << i->first << " -> " << i->second.first << ", " << i->second.second << endl;

	// sorting email edges and call edges to make search efficient
	sort(email_edges.begin(), email_edges.end(), edge_comparator);
	sort(call_edges.begin(), call_edges.end(), edge_comparator);

	// printing sorted edges
//	for(int i=0; i<email_edges.size(); i++)
//		cout << email_edges[i].first << " -> " << email_edges[i].second << endl;
//	for(int i=0; i<call_edges.size(); i++)
//		cout << call_edges[i].first << " -> " << call_edges[i].second << endl;
	//// writing clauses to filename.satinput
	fstream satinput;
	satinput.open(filename+".satinput", ios::out);
	if(!satinput)	{ cout << "error while creating satinput file.\n"; return 0;}
	
	int total_variables = 	(email_node_size*email_node_size - email_node_size) +
				(call_node_size*call_node_size - call_node_size) +
				email_node_size*call_node_size,
	    total_clauses   = 	(email_node_size*(email_node_size-1))+		// fact clauses
		   		(call_node_size*(call_node_size-1))+		// fact clauses
				permutation(call_node_size, email_node_size)+	// constraints
				email_node_size;				// existance
	string string_to_file="";
	// writing basic info
	string_to_file = "p cnf " + to_string(total_variables) + " " + to_string(total_clauses);
	satinput << string_to_file << endl;

	//// writing fact clauses
	// writing email and call clauses
	for(auto i=variable_to_node.begin(); i!=variable_to_node.end(); i++)
	{
		// we need to add +key or -key depending upon whether corresponding edge exist or not
		int key = i->first;
		// fact indexing: email edges(eij)
		if(key <= email_node_size*email_node_size - email_node_size)
		{
			// checking if edge in the graph
			if(edge_search(email_edges, i->second, 0, email_edges.size()-1))		satinput << to_string(key) + " 0\n";
			else										satinput << to_string((-1)*key) + " 0\n";
		}
		// fact indexing: call edges(cij)
		else if(key <= (email_node_size*email_node_size - email_node_size) + (call_node_size*call_node_size - call_node_size))
		{
			// checking if edge in the graph
			if(edge_search(call_edges, i->second, 0, call_edges.size()-1))		satinput << to_string(key) + " 0\n";
			else									satinput << to_string((-1)*key) + " 0\n";
		}
		// constraint edges: mappings(mij)
		else	break;
	}
	//// writing constraints clauses.
	

	satinput.close();
	return 0;
}
