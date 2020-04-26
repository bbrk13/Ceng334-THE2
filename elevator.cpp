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
#include <utility>
#include <vector>
#include <thread>
#include <sys/wait.h>
#include <unistd.h>

using namespace std;

class elevator_monitor: public Monitor{
	Condition cv1, cv2, cv3, cv4;
	vector< vector<int> > list_of_person_monitor;
	int current_floor = 0;
	vector<int> destination_floors_list;
	int number_of_people_in_the_elevator = 0;
	int direction_of_elevator = 1; // 1 for up and 0 for down
	int state_of_elevator = 0; // 0 for idle, 1 for moving-up and 2 for moving down
	int carried_weight = 0;
	int number_of_people_inside = 0;



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
	void person_make_request_function(vector<int> person_feature_vector){
		string priority;
		bool is_person_destination_in_the_list;
		int person_direction;
		int person_id = person_feature_vector.at(0);
		int person_weight = person_feature_vector.at(1);
		int person_initial_floor = person_feature_vector.at(2);
		int person_destination_floor = person_feature_vector.at(3);
		int person_priority = person_feature_vector.at(4);
		if (person_priority == 1){
			priority.append("hp");
		}
		else{
			priority.append("lp");
		}
		cout << "Person ("<< person_id << ", " << priority << ", " << person_initial_floor << " -> " << person_destination_floor << ", " << person_weight << ") made a request" << endl;

		if (person_initial_floor < person_destination_floor){
			person_direction = 1;
		} else{
			person_direction = 0;
		}
		if (count(destination_floors_list.begin(), destination_floors_list.end(), person_destination_floor)){
			is_person_destination_in_the_list = true;
			//cout << "is_person_destination_in_the_list = true" << endl;
		} else{
			is_person_destination_in_the_list = false;
			//cout << "is_person_destination_in_the_list = false" << endl;
		}
		if (person_direction == direction_of_elevator ){
			if (!is_person_destination_in_the_list){
				destination_floors_list.push_back(person_destination_floor);

			}
			cout << "Elevator (Idle, " ;
			cout << carried_weight;
			cout << ", ";
			cout << number_of_people_inside;
			cout << ", ";
			cout << current_floor;
			cout << " -> ";
			for(int i = 0; i < destination_floors_list.size(); i++){
				cout << destination_floors_list.at(i) << ", ";
			}
			cout <<")" << endl;
		}


	}
};

void person_thread_function(elevator_monitor *em, vector<int> person_feature_list){
	em->person_make_request_function(person_feature_list);
	//cout<<"dummy function is called."<<endl;
}
void ex_func(elevator_monitor *em, vector<vector<int> > person_list_ex) {
	cout<< "em.method1() is called"<< endl;
	em->method1(std::move(person_list_ex));
	cout<< "em.method1() is ended."<<endl;
}
void ex_func_2(elevator_monitor *em, vector<vector<int> > person_list_ex) {
	cout<<"ex_func_2 is called"<<endl;
	cout<< "em.method is unlocked"<<endl;
	em->method2();
	//em.cv1_notify();
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
			tmp_vector.push_back(index_for_input_line - 1);
			for (istringstream is(line); is >> word;){
				tmp_vector.push_back(stoi(word));
			}
			list_of_persons.push_back(tmp_vector);
		}
		index_for_input_line++;
	}

	elevator_monitor elevator_monitor_instance;
	thread person_thread_list[number_of_people];
	for (int z = 0; z < number_of_people ; z++){
		person_thread_list[z] = thread(person_thread_function, &elevator_monitor_instance, list_of_persons.at(z));
		usleep(10);
	}

	for (int z = 0; z < number_of_people ; z++){
		person_thread_list[z].join();
	}

	cout << "all threads are finished" << endl;




	return 0;
}


