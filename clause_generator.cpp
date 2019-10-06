// given two graphs in "test.graphs", generate minisat input clauses in cnf format in "test.satinput" file.
#include<iostream>
#include<fstream>
#include<cmath>
#include<utility>
#include<vector>
#include<map>
#include<algorithm>
using namespace std;

typedef enum {EMAIL, CALL, MAPPING} literal_type;
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

// returns all possible combination of numbers of selection size starting from index "offset"
void get_combinations(vector<int> &numbers, int selection_size, int offset, vector<vector<int>> &combinations, vector<int> temp_combination)
{
	if(selection_size == 0)
	{
		combinations.push_back(temp_combination);
		return;	
	}
	int digits = numbers.size();
	for(int i=offset; i<=digits-selection_size; i++)
	{
		temp_combination.push_back(numbers[i]);
		get_combinations(numbers, selection_size-1, i+1, combinations, temp_combination);
		temp_combination.pop_back();
	}
}

// returns corresponding key value for a given edge, graph sizes, literal type
int edge_to_key(int r, int c, int &email_node_size, int &call_node_size, literal_type l_type)
{
	switch(l_type)
	{
		case EMAIL:
			if(c<r)		return r*email_node_size+ (c+1) - r;
			else		return r*email_node_size+ (c+1) - r - 1;
			break;
		case CALL:
			if(c<r)		return email_node_size*(email_node_size-1) + r*call_node_size+ (c+1) - r;
			else		return email_node_size*(email_node_size-1) + r*call_node_size+ (c+1) - r - 1;
			break;
		case MAPPING:
			if(c<r)		return call_node_size*(call_node_size-1) + email_node_size*(email_node_size-1) + r*call_node_size+ (c+1);
			else		return call_node_size*(call_node_size-1) + email_node_size*(email_node_size-1) + r*call_node_size+ (c+1);
			break;
	}
}

pair<int, int> key_to_edge(int key, int &email_node_size, int &call_node_size, literal_type l_type)
{
	switch(l_type)
	{
		case EMAIL:
			cout << "*********** not implemented ************\n";
			return make_pair(-1, -1);
		case CALL:
			cout << "*********** not implemented ************\n";
			return make_pair(-1, -1);
		case MAPPING:
			// total number of element in mapping matrix
			int element_index = key - (email_node_size*(email_node_size-1) + call_node_size*(call_node_size-1));
			// zero based indexing
			int r, c;
			c = (element_index-1) % call_node_size;
			r = (element_index-c-1) / call_node_size;
			return make_pair(r+1, c+1);
	}
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
	cout << "key to edge mappings: \n";
	for(auto i=variable_to_node.begin(); i!=variable_to_node.end(); i++)
		cout << i->first << " -> " << i->second.first << ", " << i->second.second << endl;
	// printing edge to key mapping
//	cout << "edge to key mappings: \n";
//	for(int i=0; i<email_node_size; i++)
//		for(int j=0; j<call_node_size; j++)
//			cout << i+1 << " -> " << j+1 << ": " << edge_to_key(i, j, email_node_size, call_node_size, MAPPING) << endl;

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
	    total_clauses   = 	(email_node_size*(email_node_size-1))+										// fact clauses
		   		(call_node_size*(call_node_size-1))+										// fact clauses
				((2*email_node_size*email_node_size - 2*email_node_size)*permutation(call_node_size, email_node_size))+		// constraints
				email_node_size;												// existance
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
	//adding existence clause to satinput
	int temp=(email_node_size*email_node_size - email_node_size) + (call_node_size*call_node_size - call_node_size), count=0;
	for(int i=temp;i<variable_to_node.size();i++)
	{
		if(count<call_node_size)
		{
			satinput<<to_string(i+1)+" ";
			count++;
		}
		else
		{
			count=0;
			satinput<<"0\n";
			i--;
		}
	}
	satinput<<"0\n";	
	//// writing constraints clauses.
	// creating vector of int: node id of graphs
	// both vectors are already in sorted order: increasing node ids
	// also all elements are unique
	vector<int> email_nodes, call_nodes;
	for(int i=0; i<email_node_size; i++)	email_nodes.push_back(i+1);
	for(int i=0; i<call_node_size; i++)	call_nodes.push_back(i+1);
	
	// selecting n elements out of m elements. n < m. mcn combinations.
	vector<vector<int>> valid_mappings;
	vector<int> temp_combination;
	get_combinations(call_nodes, email_node_size, 0, valid_mappings, temp_combination);
	int valid_mappings_size = valid_mappings.size();
	for(int i=0; i<valid_mappings_size; i++)
	{
		// extracting a permuatation
		vector<int> a_permutation = valid_mappings[i];
		// getting subset of mapping
		do
		{
			string base_clause="";
			// constraint clause for a mapping under consideration
			for(int j=0; j<email_node_size; j++)
				base_clause += to_string((-1)*edge_to_key(j, a_permutation[j]-1, email_node_size, call_node_size, MAPPING)) + " ";
			// temp_counter := number of all possible valid email edges
			int temp_counter = email_node_size*(email_node_size-1);
			for(int j=1; j<=temp_counter; j++)
			{
				int map_i = variable_to_node.at(j).first, map_j = variable_to_node.at(j).second;
				int mapped_key = edge_to_key( a_permutation[map_i-1]-1, a_permutation[map_j-1]-1, email_node_size, call_node_size, CALL);
				satinput << base_clause + to_string((-1)*j) + " " + to_string(mapped_key) + " 0\n";
				satinput << base_clause + to_string(j) + " " + to_string((-1)*mapped_key) + " 0\n";
			}
		}
		while(next_permutation(a_permutation.begin(), a_permutation.end()));
	}

	satinput.close();
	return 0;
}
