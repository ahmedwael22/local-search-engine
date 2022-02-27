#ifndef WebpageClass
#define WebpageClass

#include <string>
#include <vector>
#include <iostream>
using namespace std;
#pragma once


class Webpage {

private:
	string name;
	int index;

	int clicks;
	int impressions;

	double CTR;
	double PageRank;
	double NormalizedPageRank;
	double score;

	double temp_fraction;

	vector<string> keywords;
	int num_of_keywords;

	vector<int> OutgoingHyperlinks;
	int num_of_outgoing;

	vector<int> IncomingHyperlinks;
	int num_of_incoming;

public:
	//Constructor
	Webpage() {
		num_of_keywords = 0;
		num_of_outgoing = 0;
		num_of_incoming = 0;
	}


	//Printing
	void print_webpage() {
		cout << index << " " << name << " ";
		for (int i = 0; i < num_of_keywords; i++)
			cout << keywords[i] << " ";
		cout << impressions << " " << clicks << " ";
		cout << endl << "        ";
		cout << "Outgoing to: ";
		for (int i = 0; i < num_of_outgoing; i++)
			cout << OutgoingHyperlinks[i] << " ";
		cout << endl << "        ";
		cout << "Incoming from: ";
		for (int i = 0; i < num_of_incoming; i++)
			cout << IncomingHyperlinks[i] << " ";
		cout << endl;
	}


	//Setters and Pushers(to vectors)
	void set_name(string n) {
		name = n;
	}

	void set_index(int ind) {
		index = ind;
	}

	void set_impressions(int i) {
		impressions = i;
	}

	void set_clicks(int c) {
		clicks = c;
	}

	void set_PageRank(double pr) {
		PageRank = pr;
	}

	void set_NormalizedPageRank(double NPR) {
		NormalizedPageRank = NPR;
	}

	void push_keyword(string word) {
		keywords.push_back(word);
		num_of_keywords++;
	}

	void push_outgoing(int out) {
		OutgoingHyperlinks.push_back(out);
		num_of_outgoing++;
	}
	
	void push_incoming(int in) {
		IncomingHyperlinks.push_back(in);
		num_of_incoming++;
	}

	


	//Getters
	int get_index() const {
		return index;
	}
	
	string get_name() const {
		return name;
	}

	int get_num_of_keywords() const {
		return num_of_keywords;
	}

	vector<string> get_keywords() const {
		return keywords;
	}

	int get_num_of_incoming () const {
		return num_of_incoming;
	}

	int get_num_of_outgoing() const {
		return num_of_outgoing;
	}

	vector<int> get_IncomingHyperlinks() const {
		return IncomingHyperlinks;
	}

	double get_PageRank() const {
		return PageRank;
	}

	double get_NormalizedPageRank() const {
		return NormalizedPageRank;
	}

	double get_score() const {
		return score;
	}

	int get_impressions() const {
		return impressions;
	}

	int get_clicks() const {
		return clicks;
	}


	//Computation and Updating
	void compute_CTR() {
		CTR = clicks / impressions;
	}

	void compute_score() {
		temp_fraction = (0.1 * impressions) / (1 + 01.1 * impressions);
		score = 0.4 * NormalizedPageRank + ((1 - temp_fraction) * NormalizedPageRank + temp_fraction * CTR) * 0.6;
		//score = 0.4 * PageRank + ((1 - temp_fraction) * PageRank + temp_fraction * CTR) * 0.6;
	}

	void increment_impressions() {
		impressions++;
	}

	void increment_clicks() {
		clicks++;
	}

	void update_values() {
		compute_CTR();
		compute_score();
	}
};



#endif // !"WebpageClass"
