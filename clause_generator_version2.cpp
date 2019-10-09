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
void get_combinations(	vector<int> &numbers, int selection_size, int offset, vector<vector<int>> &combinations, vector<int> temp_combination,
			int &min_email_degree, int &max_email_degree, map<int, int> &call_node_degree)
{
	if(selection_size == 0)
	{
		//// check if such a combination is feasible or not
		// getting min and max calling node degrees
//		int min_call_degree = INFINITY, max_call_degree = -INFINITY, email_node_size = temp_combination.size();
//		for(int i=0; i< email_node_size; i++)
//		{
//			auto it = call_node_degree.find(temp_combination[i]);
			// element is there
//			if(it != call_node_degree.end())
//			{
//				if(it->second > max_call_degree)	max_call_degree = it->second;
//				if(it->second < min_call_degree)	min_call_degree = it->second;
//			}
			// element is not there
//			else
//			{
//				min_call_degree = 0;
//				if(max_call_degree < 0)			max_call_degree = 0;
//			}
//		}
		// check redundent clauses
//		if(min_call_degree >= min_email_degree and max_call_degree >= max_email_degree)
			combinations.push_back(temp_combination);
//		else
//		{
//			cout << "it is helping...\n";
//			cout << "email degrees: " << min_email_degree << ", " << max_email_degree << endl;
//			cout << "call degrees: " << min_call_degree << ", " << max_call_degree << endl;
//			for(int i=0; i<email_node_size; i++)	cout << temp_combination[i];
//			cout << endl;
//		}
		return;	
	}
	int digits = numbers.size();
	for(int i=offset; i<=digits-selection_size; i++)
	{
		temp_combination.push_back(numbers[i]);
		get_combinations(numbers, selection_size-1, i+1, combinations, temp_combination, min_email_degree, max_email_degree, call_node_degree);
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
			return r*call_node_size+ (c+1);
//			if(c<r)		return call_node_size*(call_node_size-1) + email_node_size*(email_node_size-1) + r*call_node_size+ (c+1);
//			else		return call_node_size*(call_node_size-1) + email_node_size*(email_node_size-1) + r*call_node_size+ (c+1);
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
			int element_index = key; //- (email_node_size*(email_node_size-1) + call_node_size*(call_node_size-1));
			// zero based indexing
			int r, c;
			c = (element_index-1) % call_node_size;
			r = (element_index-c-1) / call_node_size;
			return make_pair(r+1, c+1);
	}
}

// check if each mapped node is valid or not using degree of corresponding nodes
// i -> p
// 1 based indexing for i and p
bool degree_check(int email_node_index, int call_node_index, map<int, int> &email_node_degree, map<int, int> &call_node_degree)
{
	auto it1 = call_node_degree.find(call_node_index), it2 = email_node_degree.find(email_node_index), end1 = call_node_degree.end(), end2 = email_node_degree.end();
	if(it1 != end1 and it2 != end2 and it1->second < it2->second)		return false;
	else if(it1 != end1 and it2 == end2)					return true;
	else if(it1 == end1 and it2 != end2)					return false;
	else									return true;
}

int main(int argc, char *argv[])
{
	cout << "starting the preprocessing\n";
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
	// storing degree of each node
	map<int, int> email_node_degree, call_node_degree;

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
				// modifing degree of int_node1
				auto it = call_node_degree.find(int_node1);
				// element exists
				if(it != call_node_degree.end())
					it->second = (it->second) +1;
				// element does not exist
				else
					call_node_degree.insert(make_pair(int_node1, 1));
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
			// modifing degree of int_node1
			auto it = email_node_degree.find(int_node1);
			// element exists
			if(it != email_node_degree.end())
				it->second = (it->second) +1;
			// element does not exist
			else
				email_node_degree.insert(make_pair(int_node1, 1));
		}
	}
	input_file.close();

	// finding min and max degrees of email graph
	int min_email_degree = INFINITY, max_email_degree = -INFINITY;
	for(int i=1; i<=email_node_size; i++)
	{
		auto it = email_node_degree.find(i);
		// exists
		if(it != email_node_degree.end())
		{
			if(it->second > max_email_degree)	max_email_degree = it->second;
			if(it->second < min_email_degree)	min_email_degree = it->second;
		}
		// does not exist
		else
		{
			if(0 > max_email_degree)	max_email_degree = 0;
			if(0 < min_email_degree)	min_email_degree = 0;
		}
	}

	cout << "call node degree\n";
	for(auto i=call_node_degree.begin(); i != call_node_degree.end(); i++)
		cout << i->first << ": " << i->second << endl;
	cout << "email nodes degree\n";
	for(auto i=email_node_degree.begin(); i != email_node_degree.end(); i++)
		cout << i->first << ": " << i->second << endl;

	// writing sizes into a file for later use
	fstream size_file;
	size_file.open("size.txt", ios::out);
	if(!size_file)	{ cout << "error while creating file size.txt\n"; return 0;}
	size_file << email_node_size << endl << call_node_size << endl;
	size_file.close();

	//// creating map to refer node id wrt the variable index
	map<int, pair<int, int>> variable_to_node, all_possible_email_edges;
	int offset=1;
	// email edge variables: eij
	for(int i=0; i<email_node_size; i++)
		for(int j=0; j<email_node_size; j++)
			if(i!=j)
			{
				all_possible_email_edges.insert(make_pair(offset, make_pair(i+1, j+1)));
				offset++;
			}
	// call edge variables: cij
//	for(int i=0; i<call_node_size; i++)
//		for(int j=0; j<call_node_size; j++)
//			if(i!=j)
//			{
//				variable_to_node.insert(make_pair(offset, make_pair(i+1, j+1)));
//				offset++;
//			}
	// mapping variables: mij
	offset = 1;
	for(int i=0; i<email_node_size; i++)
		for(int j=0; j<call_node_size; j++)
		{
			variable_to_node.insert(make_pair(offset, make_pair(i+1, j+1)));
			offset++;
		}


	// printing key and value
//	cout << "key to edge mappings: \n";
//	for(auto i=variable_to_node.begin(); i!=variable_to_node.end(); i++)
//		cout << i->first << " -> " << i->second.first << ", " << i->second.second << endl;
	// printing edge to key mapping
//	cout << "edge to key mappings: \n";
//	for(int i=0; i<email_node_size; i++)
//		for(int j=0; j<call_node_size; j++)
//			cout << i+1 << " -> " << j+1 << ": " << edge_to_key(i, j, email_node_size, call_node_size, MAPPING) << endl;

	// sorting email edges and call edges to make search efficient
	sort(email_edges.begin(), email_edges.end(), edge_comparator);
	sort(call_edges.begin(), call_edges.end(), edge_comparator);
	
	cout << "perprocessing done\n";
	cout << "writing clauses into file\n";

	// printing sorted edges
//	for(int i=0; i<email_edges.size(); i++)
//		cout << email_edges[i].first << " -> " << email_edges[i].second << endl;
//	for(int i=0; i<call_edges.size(); i++)
//		cout << call_edges[i].first << " -> " << call_edges[i].second << endl;
	//// writing clauses to filename.satinput
	fstream satinput;
	satinput.open(filename+".temp_satinput", std::fstream::out);
	if(!satinput)	{ cout << "error while creating satinput file.\n"; return 0;}
	
	int total_variables =
//				(email_node_size*email_node_size - email_node_size) +
//				(call_node_size*call_node_size - call_node_size) +
				email_node_size*call_node_size,
	    total_clauses   = 	
//		    		(email_node_size*(email_node_size-1))+										// fact clauses
//		   		(call_node_size*(call_node_size-1))+										// fact clauses
				(email_node_size*call_node_size*(call_node_size-1)) +								// one to many check
				(call_node_size*email_node_size*(email_node_size-1)) +								// many to one check
//				((2*email_node_size*email_node_size - 2*email_node_size)*permutation(call_node_size, email_node_size))+		// constraints
				email_node_size;												// existance
	
//	string string_to_file="";
	// writing basic info
//	string_to_file = "p cnf " + to_string(total_variables) + " " + to_string(total_clauses);
//	satinput << string_to_file << endl;

	//// writing fact clauses
	// writing email and call clauses
//	for(auto i=variable_to_node.begin(); i!=variable_to_node.end(); i++)
//	{
		// we need to add +key or -key depending upon whether corresponding edge exist or not
//		int key = i->first;
		// fact indexing: email edges(eij)
//		if(key <= email_node_size*email_node_size - email_node_size)
//		{
			// checking if edge in the graph
//			if(edge_search(email_edges, i->second, 0, email_edges.size()-1))		satinput << to_string(key) + " 0\n";
//			else										satinput << to_string((-1)*key) + " 0\n";
//		}
		// fact indexing: call edges(cij)
//		else if(key <= (email_node_size*email_node_size - email_node_size) + (call_node_size*call_node_size - call_node_size))
//		{
			// checking if edge in the graph
//			if(edge_search(call_edges, i->second, 0, call_edges.size()-1))		satinput << to_string(key) + " 0\n";
//			else									satinput << to_string((-1)*key) + " 0\n";
//		}
		// constraint edges: mappings(mij)
//		else	break;
//	}
	//adding existence clause to satinput
//	int temp=(email_node_size*email_node_size - email_node_size) + (call_node_size*call_node_size - call_node_size), count=0;
	

	int count=0;
	for(int i=0; i<variable_to_node.size();i++)
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
	
	cout << "existance clauses done\n";

	//// writing one-one check clauses
	// if i is mapped to p then i can not map to anything else than p
	for(int i=0; i< email_node_size; i++)
	{
		for(int p=0; p< call_node_size; p++)
		{
			string base_clause = to_string((-1)*edge_to_key(i, p, email_node_size, call_node_size, MAPPING)) + " ";
			// one to many check
			for(int q=0; q<call_node_size; q++)
				if(q != p)
					satinput << base_clause + to_string((-1)*edge_to_key(i, q, email_node_size, call_node_size, MAPPING)) + " 0\n";
			// many to one check
			for(int q=0; q<email_node_size; q++)
				if(q != i)
					satinput << base_clause + to_string((-1)*edge_to_key(q, p, email_node_size, call_node_size, MAPPING)) + " 0\n";
		}
	}

	cout << "writing one one check clauses done\n";

	// writing optimized constraint clauses
	for(int i=0; i<email_node_size; i++)
	{
		for(int p=0; p<call_node_size; p++)
		{
			// degree heuristics: degree(p) >= degree(i) else ignore all such clauses
			if(not degree_check(i+1, p+1, email_node_degree, call_node_degree))	
			{
				cout << i+1 << " can not be mapped with " << p+1 << endl; 
				string base_clause = to_string((-1)*edge_to_key(i, p, email_node_size, call_node_size, MAPPING)) + " 0\n";
				satinput << base_clause;
				total_clauses++;
				continue;
			}

			for(int j=i+1; j<email_node_size; j++)
			{
				for(int q=0; q<call_node_size; q++)
				{
					if(q != p)
					{
						
						// degree heuristics: degree(p) >= degree(i) else ignore all such clauses
						if(not degree_check(j+1, q+1, email_node_degree, call_node_degree)) 
						{ 
							string base_clause = to_string((-1)*edge_to_key(j, q, email_node_size, call_node_size, MAPPING)) + " 0\n";
							satinput << base_clause;
							total_clauses++;
							cout << j+1 << " can't be mapped with " << q+1 << endl; 
							continue;
						}
						
						// mip ^ mjq -> (eij  <--> cpq) ^ (eji <--> cqp)
						// i -> p and j -> q
						int email_edges_size = email_edges.size(), call_edges_size = call_edges.size();
						bool eij, cpq, eji, cqp;
						pair<int, int> email_edge_f = make_pair(i+1, j+1), call_edge_f = make_pair(p+1, q+1);
						pair<int, int> email_edge_b = make_pair(j+1, i+1), call_edge_b = make_pair(q+1, p+1);
						eij = edge_search(email_edges, email_edge_f, 0, email_edges_size-1);
						cpq = edge_search(call_edges, call_edge_f, 0, call_edges_size-1);
						eji = edge_search(email_edges, email_edge_b, 0, email_edges_size-1);
						cqp = edge_search(call_edges, call_edge_b, 0, call_edges_size-1);
						//// checking consistancy: (eij  <--> cpq) ^ (eji <--> cqp) 
						// consistant: ignore
						if( ((eij and cpq) or ((not eij) and (not cpq))) and ((eji and cqp) or ((not eji) and (not cqp))) ){}
						// not consistant
						else
						{
							total_clauses++;
							string base_clause = 	to_string((-1)*edge_to_key(i, p, email_node_size, call_node_size, MAPPING)) + " " +
										to_string((-1)*edge_to_key(j, q, email_node_size, call_node_size, MAPPING)) + " 0\n";
							satinput << base_clause;
						}
					}
				}
			}
		}
	}


	//// writing constraints clauses.
	// creating vector of int: node id of graphs
	// both vectors are already in sorted order: increasing node ids
	// also all elements are unique
//	vector<int> email_nodes, call_nodes;
//	for(int i=0; i<email_node_size; i++)	email_nodes.push_back(i+1);
//	for(int i=0; i<call_node_size; i++)	call_nodes.push_back(i+1);

	

	// selecting n elements out of m elements. n < m. mcn combinations.
//	vector<vector<int>> valid_mappings;
//	vector<int> temp_combination;
//	cout << "getting combinations...\n";
//	get_combinations(call_nodes, email_node_size, 0, valid_mappings, temp_combination, min_email_degree, max_email_degree, call_node_degree);
//	cout << "got the combinations\n";
//	cout << "started writing constraint clauses\n";
//	int valid_mappings_size = valid_mappings.size();
//	for(int i=0; i<valid_mappings_size; i++)
//	{
//		// extracting a permuatation
//		vector<int> a_permutation = valid_mappings[i];
//		// getting subset of mapping
//		do
//		{
//			if(degree_check(a_permutation, email_node_degree, call_node_degree))
//			{
//				string base_clause="";
//				// constraint clause for a mapping under consideration
//				for(int j=0; j<email_node_size; j++)
//					base_clause += to_string((-1)*edge_to_key(j, a_permutation[j]-1, email_node_size, call_node_size, MAPPING)) + " ";
//				base_clause += "0\n";
//				// temp_counter := number of all possible valid email edges
//				int temp_counter = email_node_size*(email_node_size-1);
//				for(int j=1; j<=temp_counter; j++)
//				{
//					// map_i, map_j: edges of email graph
//					int map_i = all_possible_email_edges.at(j).first, map_j = all_possible_email_edges.at(j).second;
//					// a_permutation[map_i-1], a_permutation[map_j-1]: edges of call grpahs
//					pair<int, int> 	call_edge = make_pair(a_permutation[map_i-1], a_permutation[map_j-1]);
//					bool is_email_edge = edge_search(email_edges, all_possible_email_edges.at(j), 0, email_node_size-1),
//					     is_call_edge = edge_search(call_edges, call_edge, 0, call_node_size-1);
//					
//					// consistant
//					if(is_email_edge and is_call_edge){}
//					// consistant
//					else if((not is_email_edge) and (not is_call_edge)){}
//					// in-consistant
//					else
//					{
//						satinput << base_clause;
//						total_clauses++;
//					}
//
//					//int mapped_key = edge_to_key( a_permutation[map_i-1]-1, a_permutation[map_j-1]-1, email_node_size, call_node_size, CALL);
////					satinput << base_clause + to_string((-1)*j) + " " + to_string(mapped_key) + " 0\n";
////					satinput << base_clause + to_string(j) + " " + to_string((-1)*mapped_key) + " 0\n";
////					total_clauses += 2;
//				}
//			}
//			else
//			{
//				
//				string base_clause="";
//				// constraint clause for a mapping under consideration
//				for(int j=0; j<email_node_size; j++)
//					base_clause += to_string((-1)*edge_to_key(j, a_permutation[j]-1, email_node_size, call_node_size, MAPPING)) + " ";
//				satinput << base_clause + "0\n";
//				total_clauses ++;
//
////				cout << "it helped...\n";
////				for(int i=0; i<email_node_size; i++)	cout << a_permutation[i];
////				cout << endl;
//			}
//		}
//		while(next_permutation(a_permutation.begin(), a_permutation.end()));
//	}
//	
//	cout << "writing constraint clauses done...\n";


	satinput.close();
	
	cout << "started coping one file to another...\n";

	// coping all clauses to original file that has it's first line a basic info
	fstream satinput_original;
	satinput_original.open(filename+".satinput", ios::out);
	satinput.open(filename+".temp_satinput", ios::in);

	if(!satinput_original or !satinput)	{ cout <<"can not create " << filename << ".satinput file.\n"; return 0;}
	string string_to_file="";
	// writing basic info
	satinput_original << "p cnf " + to_string(total_variables) + " " + to_string(total_clauses) << "\n";
	satinput_original << satinput.rdbuf();
	
	satinput.close();
	satinput_original.close();

	cout << "coping of file done\n";
	cout << "total variables: " << total_variables << endl;
	cout << "total clauses: " << total_clauses << endl;

	return 0;
}
