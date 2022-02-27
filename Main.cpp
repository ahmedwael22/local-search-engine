#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <algorithm>
#include <sstream>
#include <math.h>
#include "WebpageClass.h"
using namespace std;

void SearchScreen(vector<Webpage>& Webpages, string search); //definition only

void PromptSearchScreen(vector<Webpage>& Webpages); //definition only

int retrieveWebpageIndex(const vector<Webpage>& Webpages, string searchKey) {
	int n = Webpages.size();
	for (int i = 0; i < n; i++)
		if (Webpages[i].get_name() == searchKey)
			//the same as return i, but this is more scalable (in case someone reuses the code but sorts the vector differently)
			return Webpages[i].get_index();
	return -1; //not found - error flag

}

bool setupWebpages(vector<Webpage>& Webpages) {
	string current_line;
	string current_word;
	int line_index;
	int column_index;


	line_index = 0;
	column_index = 0;

	fstream fileKeywords("Keyword_File.csv");
	if (fileKeywords.is_open()) {
		while (getline(fileKeywords, current_line)) {

			Webpage* temp = new Webpage;
			stringstream line_reader(current_line);
			column_index = 0;

			while (getline(line_reader, current_word, ',')) {

				if (column_index == 0) { //set website details
					temp->set_name(current_word);
					temp->set_index(line_index);
				}
				else { //push each keyword
					temp->push_keyword(current_word);
				}

				column_index++;
			}

			Webpages.push_back(*temp);
			delete temp;

			line_index++;
		}


	}


	current_line = "";
	current_word = "";
	line_index = 0;
	column_index = 0;

	fstream fileNumofimpressions("Numofimpressions_File.csv");
	if (fileNumofimpressions.is_open()) {
		while (getline(fileNumofimpressions, current_line)) {

			stringstream line_reader(current_line);
			column_index = 0;

			while (getline(line_reader, current_word, ',')) {
				if (column_index == 1) //num of impressions
					Webpages[line_index].set_impressions(stoi(current_word));
				else if (column_index == 2) //num of clicks
					Webpages[line_index].set_clicks(stoi(current_word));
				//else column_index == 0 (we already set the name)

				column_index++;
			}

			line_index++;
		}
	}

	current_line = "";
	current_word = "";
	line_index = 0;
	column_index = 0;
	string source;
	string destination;
	int sourceIndex = -2; //not 0 because we use 0-indexing and not -1 because we use -1 as a flag
	int destinationIndex = -2;

	fstream fileWebgraphs("Webgraph_File.csv");
	if (fileWebgraphs.is_open()) {
		while (getline(fileWebgraphs, current_line)) {

			stringstream line_reader(current_line);
			column_index = 0;

			while (getline(line_reader, current_word, ',')) {

				if (column_index == 0) { //source
					sourceIndex = retrieveWebpageIndex(Webpages, current_word);
					if (sourceIndex == -1) {
						cout << "Error. Source " << current_word << " is not recgonized as a webpage.";
						return false;
					}
				}
				else { //destination
					destinationIndex = retrieveWebpageIndex(Webpages, current_word);
					if (destinationIndex == -1) {
						cout << "Error. Desintation " << current_word << " is not recgonized as a webpage.";
						return false;
					}
				}

				column_index++;
			}

			if (sourceIndex == destinationIndex) { //consider to printing an error. Instead do nothing (to not affect PR)
				cout << "Error. The webpage " << current_word << " cannot have a hyperlink to itself.";
				return false;
			}
			else { //valid edge between two pages
				Webpages[sourceIndex].push_outgoing(destinationIndex);
				Webpages[destinationIndex].push_incoming(sourceIndex);
			}

			line_index++;
		}
	}

	fileNumofimpressions.close();
	fileWebgraphs.close();
	fileKeywords.close();

	return true;
}

void calculatePageRank(vector<Webpage>& Webpages) {
	
	//Some of the loops in this function can be merged together but would ruin readability and would not affect complexity (non-dominating)

	int rows = Webpages.size();

	int maximumNumberOfIterations = 10;
	double** Table = new double*[rows];

	double mean = 1.0 / rows; //used for Normalization and initializing column 0

	for (int i = 0; i < rows; i++) {
		Table[i] = new double[maximumNumberOfIterations];
		Table[i][0] = mean; //initialize the 0th column for each row
		for (int j=1; j<maximumNumberOfIterations; j++)
			Table[i][j] = 0; //initialization of the other columns for each row --> to make summation easy
	}


	int currentIndex = 1; //initial value does not matter
	double numerator = 1; //initial value does not matter
	double denominator = 1; //initial value does not matter
	double fraction = 1; //initial value does not matter
	

	

	for (int j = 1; j < maximumNumberOfIterations; j++) {

		for (int i = 0; i < rows; i++) {

			for (int k = 0; k < Webpages[i].get_num_of_incoming(); k++) {

				currentIndex = Webpages[i].get_IncomingHyperlinks()[k]; //

				numerator = Table[currentIndex][j - 1];
				denominator = Webpages[currentIndex].get_num_of_outgoing();
				fraction = numerator / denominator;

				Table[i][j] += fraction;


			}

		}
	}

	//compute Standard Deviation for Normalization
	double sd = 0;
	for (int i = 0; i < rows; i++)
		sd += pow((Table[i][maximumNumberOfIterations-1] - mean), 2);
	sd /= double(rows); //Some mathematicians use N for standard deviation and some use N-1. The report explains my choice of N.
	sd = sqrt(sd);


	for (int i = 0; i < rows; i++) {
		Webpages[i].set_PageRank(Table[i][maximumNumberOfIterations - 1]);
		Webpages[i].set_NormalizedPageRank((Table[i][maximumNumberOfIterations - 1] - mean) / sd);
	}


	for (int i = 0; i < rows; i++)
		delete[] Table[i];
	delete[] Table;

	return;
	////debugging
	//for (int i = 0; i < rows; i++) {
	//	for (int j = 0; j < maximumNumberOfIterations; j++)
	//		cout << Table[i][j] << " ";
	//	Webpages[i].set_PageRank(Table[i][maximumNumberOfIterations - 1]);
	//	cout << endl;
	//}
	//cout << endl;



	////Prposed Alternative approach (with no limit on iterations, they only terminate based on accuracy)
	////consider a do-while loop ("do" to ensure iteration 0 occurs) until the threshhold is good
	//
	////2D array of size rows*2
	////the dynamic programming only needs the directly-previously computed values (the older ones can be discarded)
	////so we can discard the values of an iteration after 2 more iterations
	////Here's how it essentially works:
	///*
	//	The 0th iteration is computed in the 0th column
	//	Compute iteration 1 in the last column
	//	Copy iteration 1 into the 0th column (Delete 0th iteration)
	//	Compute iteration 2 in the last column
	//	Copy iteration 2 into the 0th column (Delete 1st iteration)
	//	Compute iteration 3 in the last column
	//	Etc.
	//	Stop iterating when you reach a threshhold (the threshold being negligible change between successive iterations)
	//	It can even be done without the overhead of copying the last column into the 0th column,
	//		but it will make the code uglier and less readbale and won't cut complexity costs
	//*/
	//double** Grid = new double* [rows];
	//for (int i = 0; i < rows; i++) {
	//	Grid[i] = new double[2];
	//	Grid[i][0] = 1.0 / rows; //initialize the 0th column for each row
	//}

	//do {

	//} while ();
	//for (int i = 0; i < rows; i++)
	//	delete[] Grid[i];
	//delete[] Grid;
	//return;
	

}

vector<Webpage> AND_UtilitySearch(vector<Webpage>& List, string key) {

	vector<Webpage> NewList;

	for (int i = 0; i < List.size(); i++) {
		for (int j = 0; j < List[i].get_num_of_keywords(); j++) {
			if (List[i].get_keywords()[j] == key) {
				NewList.push_back(List[i]);
				break;
			}
		}
	}

	return NewList;

}

vector<Webpage> EXACT_UtilitySearch(vector<Webpage>& List, string key) {
	
	vector<Webpage> NewList;
	string mergedkeywords;

	for (int i = 0; i < List.size(); i++) {
		mergedkeywords = "";
		for (int j = 0; j < List[i].get_num_of_keywords(); j++) {
			mergedkeywords += List[i].get_keywords()[j];
		}
		if (mergedkeywords.find(key) != string::npos)
			NewList.push_back(List[i]);
	}

	return NewList;
}

vector<Webpage> Search(vector<Webpage>& Webpages, string search) {

	for (int i = 0; i < Webpages.size(); i++)
		Webpages[i].update_values();
	
	vector<Webpage> List;
	
	int ANDfound;
	int ORfound;
	int location;
	string key;

	if (search[0] == '\"') { //search starts and ends with " "
		List = Webpages;
		key = search.erase(0, 1);
		key = key.erase(key.length() - 1, 1);
		location = search.find(" ");
		while (location != string::npos) {
			key = key.erase(location, 1);
			location = key.find(" ");
		}
		List = EXACT_UtilitySearch(List, key);
	}
	else {
		location = search.find("AND");
		if (location != string::npos) { //search contains AND
			List = Webpages;
			while (1) {
				location = search.find("AND");
				if (location != string::npos) {
					key = search.substr(0, location - 1);
					List = AND_UtilitySearch(List, key);
					key = search.erase(0, location + 4);
				}
				else {
					List = AND_UtilitySearch(List, key);
					break;
				}
			}
		}
		else {
			ORfound = search.find("OR");
			if (ORfound != string::npos) { //search contains OR

			}
			else { //search contains only spaces (treated as OR)

			}
		}
	}

	return List;

}

bool isHigherScore(Webpage W1, Webpage W2) {
	return (W1.get_score() > W2.get_score());
}

void ViewScreen(vector<Webpage>& Webpages, string name, string search) {

	cout << endl
		<< "You are currently in the webpage:" << endl
		<< name
		<< endl
		<< "What would you like to do?"
		<< endl
		<< "1. Back to search results" << endl
		<< "2. New Search" << endl
		<< "3. Exit" << endl
		<< endl
		<< "Type in your choice: ";
	int choice;
	cin >> choice;

	switch (choice) {
	case 1:
		cout << "In this amazing dynamic search engine, going back to search results will re-sort the entries"
			<< "based on the clicks and impressions you just made!" << endl;
		SearchScreen(Webpages, search);
		break;
	case 2:
		PromptSearchScreen(Webpages);
		break;
	case 3:
		cout << "Thanks for using the search engine. Can't wait to see you again :)" << endl;
		break;
	default:
		cout << "Not a valid choice. Program will exit." << endl;
		break;
	}

	return;

}

void SearchScreen(vector<Webpage>& Webpages, string search) {
	
	vector<Webpage> List = Search(Webpages, search); //also refreshes the values of CTR and score

	sort(List.begin(), List.end(), isHigherScore);
	
	for (int i = 0; i < List.size(); i++) {
		cout << i + 1 << ". " << List[i].get_name() << endl;
		List[i].increment_impressions();
	}

	for (int i = 0; i < Webpages.size(); i++) {
		for (int j = 0; j < List.size(); j++) {
			if (Webpages[i].get_index() == List[j].get_index())
				Webpages[i].set_impressions(List[j].get_impressions());
		}
	}

	cout << endl;

	cout << "What would you like to do?" << endl
		<< "1. Choose a webpage to open" << endl
		<< "2. New search" << endl
		<< "3. Exit" << endl
		<< endl
		<< "Type in your choice: ";

	int choice;
	cin >> choice;

	int choice2;

	switch (choice) {
	case 1:
		cout << "Please choose which webpage you want to view: ";
		cin >> choice2;
		choice2 -= 1; //becuase choice are 1-indexed while vector is 0-indexed

		if (choice2 >= List.size()) {
			cout << "Not a valid choice. Program will exit." << endl;
			break;
		}
		else {
			List[choice2].increment_clicks();
			for (int i = 0; i < Webpages.size(); i++) {
				for (int j = 0; j < List.size(); j++) {
					if (Webpages[i].get_index() == List[j].get_index())
						Webpages[i].set_clicks(List[j].get_clicks());
				}
			}
			ViewScreen(Webpages, List[choice2].get_name(), search);
		}

		break;

	case 2:
		PromptSearchScreen(Webpages);
		break;
	case 3:
		cout << "Thanks for using the search engine. Can't wait to see you again :)" << endl;
		break;
	default:
		cout << "Not a valid choice. Program will exit." << endl;
		break;
	}


}

void PromptSearchScreen(vector<Webpage>& Webpages) {
	cout << "Please type in your search keywords: ";
	string search;
	getline(cin >> ws,  search);
	//cin.getline(search.c_str(),sizeof(search));

	SearchScreen(Webpages, search);


}

void WelcomeScreen(vector<Webpage>& Webpages) {
	cout << "Welcome!" << endl
		<< "What would you like to do?" << endl
		<< "1. New Search" << endl
		<< "2. Exit" << endl
		<< endl
		<< "Type in your choice: ";

	int choice;
	cin >> choice;

	switch (choice) {
	case 1:
		PromptSearchScreen(Webpages);
		break;
	case 2:
		cout << "Thanks for using the search engine. Can't wait to see you again :)" << endl;
		break;
	default:
		cout << "Not a valid choice. Program will exit." << endl;
		break;
	}
	return;
}

void updatefile(vector<Webpage>& Webpages) {

	//trunc because we will erase the entire file and rewrite it
	fstream fileNumofimpressions("Numofimpressions_File.csv", ios::out | ios::trunc);
	if (fileNumofimpressions.is_open())
		for (int i = 0; i < Webpages.size(); i++)
			fileNumofimpressions << Webpages[i].get_name() << ',' << Webpages[i].get_impressions() << ',' << Webpages[i].get_clicks() << endl;
	else cout << endl << "Could not write back to file." << endl;

}

void displayMenu(vector<Webpage>& Webpages) {

	WelcomeScreen(Webpages);

	updatefile(Webpages);

	return;
}

int main() {
	
	vector<Webpage> Webpages;
	
	if (setupWebpages(Webpages) == false)
		return 1;
	
	////Testing setupWebpages
	//cout << endl << "Webpage Information:" << endl;
	//for (int i = 0; i < Webpages.size(); i++)
	//	Webpages[i].print_webpage();




	calculatePageRank(Webpages); //PageRank and NormalizedPageRank

	////Testing calculatePageRank
	//cout << endl << "Page Ranks:" << endl;
	//for (int i = 0; i < Webpages.size(); i++)
	//	cout << Webpages[i].get_index() << " " << Webpages[i].get_name() << " "
	//	<< Webpages[i].get_PageRank() << " " << Webpages[i].get_NormalizedPageRank() << endl;




	cout << endl << endl;
	displayMenu(Webpages); //passed as parameter because the Search function is ultimately called from there - other option is using the vector as a global vector
	cout << endl << endl;
		
	return 0;
}


