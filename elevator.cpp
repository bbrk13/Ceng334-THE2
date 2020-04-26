//
// Created by Berk ARSLAN on 23.04.2020.
//
#include "monitor.h"
//#include <bits/stdc++.h>
#include <stdio.h>
#include <string.h>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <list>
#include <iterator>
#include <vector>
#include <thread>
#include <sys/wait.h>
#include <unistd.h>

using namespace std;

class elevator_monitor: public Monitor{
	Condition cv1, cv2, cv3, cv4;
	vector< vector<int> > list_of_person_monitor;

public: elevator_monitor(): cv1(this), cv2(this), cv3(this), cv4(this){

	}
	void method1(vector<vector<int> > person_list){
		__synchronized__;
		printf("method1 is called and locked\n");
		cv1.wait();
		cout<<"second person weight is "<<person_list.at(1).at(0)<<endl;
		cout<<"method1 is finished"<<endl;
	}
	void method2(){
		__synchronized__;
		printf("method2 is called\n");
		cout<<"method1 is unlocked"<< endl;
		cv1.notify();
		cout<<"method2 is finished"<<endl;
	}
	void cv1_notify(){
		cv1.notify();
	}
};

void dummy_func(){
	cout<<"dummy function is called."<<endl;
}
void ex_func(elevator_monitor em, vector<vector<int> > person_list_ex) {
	cout<< "em.method1() is called"<< endl;
	em.method1(person_list_ex);
	cout<< "em.method1() is ended."<<endl;
}
void ex_func_2(elevator_monitor em, vector<vector<int> > person_list_ex){
	cout<<"ex_func_2 is called"<<endl;
	cout<< "em.method is unlocked"<<endl;
	em.cv1_notify();
}

int main(int argc, char *argv[]) {

	string input_file_path = argv[1];
	string line;
	string word;
	ifstream input_file(argv[1]);

	int index_for_input_line = 0;
	int index_for_input_features = 0;
	int index_for_person = 0;

	int number_of_floors = 0;
	int number_of_people = 0;
	int weight_capacity = 0;
	int person_capacity = 0;
	int travel_time = 0;
	int idle_time = 0;
	int in_out_time = 0;
	vector <int> person_features;
	vector <vector <int> > list_of_persons;

	while (std::getline(input_file, line, '\n')) {
		//cout << line << endl;
		if (index_for_input_line == 0){ // program inputs
			for (istringstream is(line); is >> word;) {
				switch (index_for_input_features) {
					case 0:
						number_of_floors = stoi(word);
						break;
					case 1:
						number_of_people = stoi(word);
						break;
					case 2:
						weight_capacity = stoi(word);
						break;
					case 3:
						person_capacity = stoi(word);
						break;
					case 4:
						travel_time = stoi(word);
						break;
					case 5:
						idle_time = stoi(word);
						break;
					case 6:
						in_out_time = stoi(word);
						break;
					default:
						cout<<"wrong input for the first line" << endl;
				}
				index_for_input_features++;
			}
		}
		else
		{ // people features
			vector <int> tmp_vector;
			for (istringstream is(line); is >> word;){
				tmp_vector.push_back(stoi(word));
			}
			list_of_persons.push_back(tmp_vector);
			//person_features.erase(person_features.begin(), person_features.end());
		}
		index_for_input_line++;
	}
	for (int i = 0; i < number_of_people; i++){
		cout << i + 1 << " people fetures are;" << endl;
		for (int y = 0; y < list_of_persons.at(i).size() ; y++){
			cout << "feature " << y + 1 << " is " << list_of_persons.at(i).at(y) << endl;
		}
	}

	elevator_monitor elevator_monitor_instance;
	thread th1(ex_func, &elevator_monitor_instance, list_of_persons);
	sleep(3);
	thread th2(ex_func_2, &elevator_monitor_instance, list_of_persons);
	th1.join();
	th2.join();




	return 0;
}


