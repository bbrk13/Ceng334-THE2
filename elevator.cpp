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

class person{
	int person_id{};
	int person_priority{};
	int person_initial_floor{};
	int person_destination_floor{};
	int person_weight{};
	bool is_person_done = false;

public:
	person()= default;

	int get_person_id() const{
		return this->person_id;
	}
	int get_person_priotiy() const{
		return this->person_priority;
	}
	int get_person_initial_floor() const{
		return this->person_initial_floor;
	}
	int get_person_destination_floor() const{
		return this->person_destination_floor;
	}
	int get_person_weight() const{
		return this->person_weight;
	}
	bool get_is_person_done() const{
		return this->is_person_done;
	}
	void set_person_id(int value){
		this->person_id = value;
	}
	void set_person_priority(int value){
		this->person_priority = value;
	}
	void set_person_initial_floor(int value){
		this->person_initial_floor = value;
	}
	void set_person_destination_floor(int value){
		this->person_destination_floor = value;
	}
	void set_person_weight(int value){
		this->person_weight = value;
	}
	void set_is_person_done(bool value){
		this->is_person_done = value;
	}
};

class elevator_monitor: public Monitor{
	Condition cv1, cv2, cv3, cv4;
	//Lock l1,l2,l3,l4;
	vector< person > list_of_person_monitor;
	vector <person> list_of_people_in_the_elevator;
	int current_floor = 0;
	vector<int> destination_floors_list;
	int number_of_people_in_the_elevator = 0;
	int direction_of_elevator = 1; // 1 for up and 0 for down
	int state_of_elevator = 1; // 0 for idle, 1 for moving-up and 2 for moving down
	int carried_weight = 0;


public: elevator_monitor():
	cv1(this), cv2(this), cv3(this), cv4(this)/*,
	l1(this), l2(this), l3(this), l4(this)*/{

	}

	void cv1_notify(){
		cv1.notify();
	}
	void cv1_wait(){
		cv1.wait();
	}
	void cv1_notifyAll(){
		cv1.notifyAll();
	}

	void cv2_notify(){
		cv1.notify();
	}
	void cv2_wait(){
		cv1.wait();
	}
	void cv2_notifyAll(){
		cv1.notifyAll();
	}

	void move_elevator_up(){
		this->current_floor++;
	}
	void move_elevator_down(){

		this->current_floor--;
	}
	void get_person_in(person person_instance){
		this->carried_weight = this->carried_weight + person_instance.get_person_weight();
		this->number_of_people_in_the_elevator++;
		//this->list_of_people_in_the_elevator.push_back(person_instance);
	}
	void get_person_out(person person_instance){
		this->carried_weight = this->carried_weight - person_instance.get_person_weight();
		this->number_of_people_in_the_elevator--;
	}
	void add_to_destination_floor_list(int value){
		// this->destination_floors_list.push_back(value);
		bool is_person_destination_in_the_list;
		if (count(destination_floors_list.begin(), destination_floors_list.end(), value)){
			is_person_destination_in_the_list = true;
			//cout << "is_person_destination_in_the_list = true" << endl;
		} else{
			is_person_destination_in_the_list = false;
			//cout << "is_person_destination_in_the_list = false" << endl;
		}
		if (!is_person_destination_in_the_list){
			destination_floors_list.push_back(value);
		}
	}
	void add_to_list_of_people_in_elevator(person person_instance){
		this->list_of_people_in_the_elevator.push_back(person_instance);
	}
	void remove_from_destination_list(int floor){
		int len = destination_floors_list.size();
		int i;
		bool isFloorInTheList = false;
		for (i = 0; i < len ; i++){
			if (destination_floors_list.at(i) == floor)
			{
				isFloorInTheList = true;
				break;
			}
		}
		//auto it = find(this->list_of_person_monitor.begin(), this->list_of_person_monitor.end(), person_instance);

		if (isFloorInTheList){
			auto it = destination_floors_list.begin();
			this->destination_floors_list.erase(it + i);
		}
		else
		{
			//cout << "floor is not in the destination list" << endl;
		}
	}
	void remove_from_list_of_person_monitor(person person_instance){
		int len = list_of_person_monitor.size();
		int i;
		bool isPersonInTheElevator = false;
		for (i = 0; i < len ; i++){
			if (list_of_person_monitor.at(i).get_person_id() == person_instance.get_person_id())
			{
				isPersonInTheElevator = true;
				break;
			}
		}
		//auto it = find(this->list_of_person_monitor.begin(), this->list_of_person_monitor.end(), person_instance);

		if (isPersonInTheElevator){
			auto it = list_of_person_monitor.begin();
			this->list_of_person_monitor.erase(it + i);
		}
		else
		{
			//cout << "person couldn't be found" << endl;
		}
	}
	void remove_from_people_in_elevator_list(person person_instance){
		int len = list_of_people_in_the_elevator.size();
		int i;
		bool isPersonInTheElevator = false;
		for (i = 0; i < len ; i++){
			if (list_of_people_in_the_elevator.at(i).get_person_id() == person_instance.get_person_id())
			{
				isPersonInTheElevator = true;
				break;
			}
		}
		//auto it = find(this->list_of_person_monitor.begin(), this->list_of_person_monitor.end(), person_instance);

		if (isPersonInTheElevator){
			auto it = list_of_people_in_the_elevator.begin();
			this->list_of_people_in_the_elevator.erase(it + i);
		}
		else
		{
			//cout << "person is not in the elevator" << endl;
		}
	}
	int get_size_of_destination_floor_list(){
		return this->destination_floors_list.size();
	}
	int get_direction_of_elevator() const{
		return this->direction_of_elevator;
	}
	void set_direction_of_elevator(int way){
		this->direction_of_elevator = way;
	}
	int get_state_of_elevator() const{
		return this->state_of_elevator;
	}
	int get_current_floor_of_elevator(){
		return this->current_floor;
	}
	void set_state_of_elevator(int state){
		this->state_of_elevator = state;
	}
	int get_size_of_list_of_person_monitor(){
		return this->list_of_person_monitor.size();
	}
	int get_carried_weight_of_elevator(){
		return this->carried_weight;
	}
	int get_number_of_people_in_the_elevator(){
		return this->number_of_people_in_the_elevator;
	}
	void print_elevator_information(){
		cout << "Elevator (";
		if (state_of_elevator == 0)
			cout << "Idle";
		else if (state_of_elevator == 1)
			cout << "Moving-up";
		else
			cout << "Moving-down";
		cout << ", " ;
		cout << carried_weight;
		cout << ", ";
		cout << number_of_people_in_the_elevator;
		cout << ", ";
		cout << current_floor;
		cout << " -> ";
		for(int i = 0; i < destination_floors_list.size(); i++){
			if (i + 1 == destination_floors_list.size())
				cout << destination_floors_list.at(i) ;
			else
				cout << destination_floors_list.at(i) << ",";
		}
		cout <<")" << endl;

	}
	void sort_destination_floor_list(){
		sort(this->destination_floors_list.begin(), this->destination_floors_list.end());
	}
	vector<int> find_the_next_destination_floor(){
		int number_of_destination_floors = this->destination_floors_list.size();
		int nearest_floor = 9999;
		int distance_of_nearest_floor = 9999;
		int new_direction_of_elevator;
		int index_of_nearest_floor;
		for (int i = 0; i < number_of_destination_floors ; i++)
		{
			int tmp_floor = this->destination_floors_list.at(i);
			int tmp_way = -1;
			if (this->current_floor - tmp_floor < 0)
			{
				tmp_way = 1;
			} else{
				tmp_way = 0;
			}
			// cout << "abs(tmp_floor - this->current_floor) = " << abs(tmp_floor - this->current_floor) << endl;
			if ((tmp_way == this->direction_of_elevator or this->state_of_elevator == 0) and
				abs(tmp_floor - this->current_floor) < distance_of_nearest_floor)
			{
				// cout << "new destination is calculated" << endl;
				index_of_nearest_floor = i;
				nearest_floor = tmp_floor;
				distance_of_nearest_floor = abs(tmp_floor - this->current_floor);
				new_direction_of_elevator = tmp_way;
			}
		}
		vector<int> result;
		result.push_back(nearest_floor);
		result.push_back(new_direction_of_elevator);
		return result;
	}

	vector<person> check_there_is_person_on_the_floor_to_enter(int current_floor){
		int len_person_list = list_of_person_monitor.size();
		vector<person> result;
		for (int i = 0; i < len_person_list; i++){
			person temp_person = list_of_person_monitor.at(i);
			if (temp_person.get_person_initial_floor() == current_floor){
				result.push_back(temp_person);
			}
		}

		return result;
	}
	vector<person> check_there_is_person_on_the_floor_to_exit(int current_floor){
		int len_person_list = list_of_people_in_the_elevator.size();
		vector<person> result;
		for (int i = 0; i < len_person_list; i++){
			person temp_person = list_of_people_in_the_elevator.at(i);
			if (temp_person.get_person_destination_floor() == current_floor){
				result.push_back(temp_person);
			}
		}

		return result;
	}


	void person_make_request_function(person person_instance, vector<int> elevator_info){
		__synchronized__;
		bool is_person_destination_in_the_list = false;
		bool is_person_in_the_elevator = false;
		while (/*!is_person_destination_in_the_list and */!is_person_in_the_elevator
				/*!(person_instance.get_is_person_done())*/){

			cv1.wait();
			// person info

			string priority;

			int person_direction;

			// elevator info

			int number_of_floors = elevator_info.at(0);
			int number_of_people = elevator_info.at(1);
			int weight_capacity = elevator_info.at(2);
			int person_capacity = elevator_info.at(3);
			int travel_time = elevator_info.at(4);
			int idle_time = elevator_info.at(5);
			int in_out_time = elevator_info.at(6);


			if (person_instance.get_person_priotiy() == 2){
				priority.append("hp");
			}
			else{
				priority.append("lp");
			}

			if (count(destination_floors_list.begin(), destination_floors_list.end(), person_instance.get_person_initial_floor())){
				is_person_destination_in_the_list = true;
				//cout << "is_person_destination_in_the_list = true" << endl;
			} else{
				is_person_destination_in_the_list = false;
				//cout << "is_person_destination_in_the_list = false" << endl;
			}

			for (int y = 0; y < this->number_of_people_in_the_elevator ; y++)
			{
				if (this->list_of_people_in_the_elevator.at(y).get_person_id() == person_instance.get_person_id())
				{
					//cout << "person " << this->list_of_people_in_the_elevator.at(y).get_person_id() << " is in the elevator" << endl;
					is_person_in_the_elevator = true;
					break;
				} else
					is_person_in_the_elevator = false;
			}

			bool is_person_make_request = false;
			for (int z = 0; z < list_of_person_monitor.size() ; z++){
				if (this->list_of_person_monitor.at(z).get_person_id() == person_instance.get_person_id())
				{
					is_person_make_request = true;
					break;
				} else
				{
					is_person_make_request = false;
				}
			}

			if (person_instance.get_person_initial_floor() < person_instance.get_person_destination_floor()){
				person_direction = 1;
			} else{
				person_direction = 0;
			}
			bool is_person_valid_for_request = false;

			if (this->destination_floors_list.empty())
				is_person_valid_for_request = true;

			else if (this->direction_of_elevator == person_direction)
			{
				if (person_direction == 1 and
				this->current_floor < person_instance.get_person_initial_floor() )
				{
					is_person_valid_for_request = true;
				}

				if (person_direction == 0 and
				this->current_floor > person_instance.get_person_initial_floor())
				{
					is_person_valid_for_request = true;
				}

			}





			if (!is_person_in_the_elevator )
			{


				if (!is_person_destination_in_the_list and
					is_person_valid_for_request)
				{
					destination_floors_list.push_back(person_instance.get_person_initial_floor());
					this->sort_destination_floor_list();

				}

				if (!is_person_make_request and
					is_person_valid_for_request)
				{
					list_of_person_monitor.push_back(person_instance);

				}
				if (!is_person_destination_in_the_list or !is_person_make_request)
				{
					cout << "Person (";
					cout << person_instance.get_person_id();
					cout << ", " ;
					cout << priority;
					cout << ", ";
					cout << person_instance.get_person_initial_floor();
					cout << " -> ";
					cout << person_instance.get_person_destination_floor();
					cout << ", ";
					cout << person_instance.get_person_weight();
					cout << ") made a request" << endl;
					this->print_elevator_information();
				}


				// destination_floors_list.push_back(person_destination_floor);
			}
			/*else
			{
				//if (person_instance.get_person_id() == 2 )
				//{
					cout << "-------------- DEBUG ------------------------" << endl;
					cout << "Person ID = " << person_instance.get_person_id() << endl;
					cout << "!is_person_in_the_elevator = " << !is_person_in_the_elevator << endl;
					cout << "this->carried_weight + person_instance.get_person_weight() = " ;
					cout << this->carried_weight + person_instance.get_person_weight() << endl;
					cout << "weight_capacity = " << weight_capacity << endl;
					cout << "this->number_of_people_in_the_elevator + 1 = " << this->number_of_people_in_the_elevator + 1 << endl;
					cout << "person_capacity = " << person_capacity << endl;
					cout << "this->direction_of_elevator = =" << this->direction_of_elevator << endl;
					cout << "person_direction = " << person_direction << endl;
					cout << "this->state_of_elevator = " << this->state_of_elevator << endl;
					cout << "---------------------END-------------------" << endl;
				//}

			}*/
			//sleep(2);
			cv2.notify();
		}
	}

	void response_to_requests_function(vector<int> person_feature_vector, vector<int> elevator_info){
		__synchronized__;
		// person info
		//cv2.wait();

		string priority;
		bool is_person_destination_in_the_list;
		int person_direction;
		int person_id = person_feature_vector.at(0);
		int person_weight = person_feature_vector.at(1);
		int person_initial_floor = person_feature_vector.at(2);
		int person_destination_floor = person_feature_vector.at(3);
		int person_priority = person_feature_vector.at(4);

		// elevator info

		int number_of_floors = elevator_info.at(0);
		int number_of_people = elevator_info.at(1);
		int weight_capacity = elevator_info.at(2);
		int person_capacity = elevator_info.at(3);
		int travel_time = elevator_info.at(4);
		int idle_time = elevator_info.at(5);
		int in_out_time = elevator_info.at(6);

		if (person_priority == 1){
			priority.append("hp");
		}
		else{
			priority.append("lp");
		}
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

		bool isRequestResponded = false;
		//while (!isRequestResponded){
		if (person_direction == direction_of_elevator and
			this->number_of_people_in_the_elevator + 1 < person_capacity and
			this->carried_weight + person_weight < weight_capacity)
			{
				if (!is_person_destination_in_the_list){
					destination_floors_list.push_back(person_initial_floor);
					destination_floors_list.push_back(person_destination_floor);
				}

				cout << "Elevator (";
				cout << this->state_of_elevator;
				cout << ", " ;
				cout << carried_weight;
				cout << ", ";
				cout << number_of_people_in_the_elevator;
				cout << ", ";
				cout << current_floor;
				cout << " -> ";
				for(int i = 0; i < destination_floors_list.size(); i++){
					cout << destination_floors_list.at(i) << ", ";
				}
				cout <<")" << endl;
			}
		cv1.notify();
		//}
	}

	person get_people_from_request_que(){
		//vector<int> result;
		if (this->list_of_person_monitor.empty() == true)
		{
			cv1.notify();
			sleep(1);
			//return result;
		}
		return this->list_of_person_monitor.at(0);
	}

};



void person_thread_function(elevator_monitor *em, person person_instance, vector<int> elevator_info){
	//em->cv1_wait();
	em->person_make_request_function(person_instance, elevator_info);
	//em->cv2_notify();
	//cout<<"dummy function is called."<<endl;
}

void print_new_dest_floor(vector<int> new_dest){
	cout << "new destination floor = " << new_dest.at(0) << endl;
	cout << "new direction is " ;
	if (new_dest.at(1) == 1)
		cout << "up" << endl;
	else
		cout << " down" << endl;
}

void print_person_entered(person person_instance){
	string priority;
	if (person_instance.get_person_priotiy() == 1)
		priority.append("lp");
	else
		priority.append("hp");
	cout << "Person (";
	cout << person_instance.get_person_id();
	cout << ", ";
	cout << priority;
	cout << ", ";
	cout << person_instance.get_person_initial_floor();
	cout << " -> ";
	cout << person_instance.get_person_destination_floor();
	cout << ", ";
	cout << person_instance.get_person_weight();
	cout << ") entered the elevator" << endl;

}

void print_person_left(person person_instance){
	string priority;
	if (person_instance.get_person_priotiy() == 1)
		priority.append("lp");
	else
		priority.append("hp");
	cout << "Person (";
	cout << person_instance.get_person_id();
	cout << ", ";
	cout << priority;
	cout << ", ";
	cout << person_instance.get_person_initial_floor();
	cout << " -> ";
	cout << person_instance.get_person_destination_floor();
	cout << ", ";
	cout << person_instance.get_person_weight();
	cout << ") has left the elevator" << endl;

}

void elevator_controller_func(int number_of_people_to_serve, int idle_time, elevator_monitor *em, vector<int> elevator_information){
	//cout<<"elevator controller is started"<<endl;
	int nanosec = idle_time * 1000;
	int in_out_time = elevator_information.at(6);
	in_out_time = in_out_time * 1000;
	int travel_time = elevator_information.at(4);
	travel_time = travel_time * 1000;
	while (number_of_people_to_serve > 0){
		//em->remove_from_destination_list(em->get_current_floor_of_elevator());

		vector<person> person_at_floor_vector_to_enter = em->check_there_is_person_on_the_floor_to_enter(em->get_current_floor_of_elevator());
		vector<person> person_at_floor_vector_to_exit = em->check_there_is_person_on_the_floor_to_exit(em->get_current_floor_of_elevator());

		if (person_at_floor_vector_to_enter.size() + person_at_floor_vector_to_exit.size() > 0){ // there is a person to enter or exit
			if (!(person_at_floor_vector_to_exit.empty())){ // there is a person to exit
				int tmp_len = person_at_floor_vector_to_exit.size();
				//cout << "There are " << tmp_len << " people to exit" << endl;
				for (int i = tmp_len - 1 ; 0 <= i ;i-- ){
					person tmp_person = person_at_floor_vector_to_exit.at(i);
					em->get_person_out(tmp_person);
					em->remove_from_people_in_elevator_list(tmp_person);
					tmp_person.set_is_person_done(true);
					em->sort_destination_floor_list();
					usleep(in_out_time);
					print_person_left(tmp_person);
					em->print_elevator_information();
					number_of_people_to_serve--;
				}
			}
			if (!(person_at_floor_vector_to_enter.empty())){ //there is a person to enter
				int tmp_len = person_at_floor_vector_to_enter.size();
				vector <person> high_priotiry_people_vector;
				vector<person> low_priority_people_vector;
				for (int i = 0; i < tmp_len; i++){
					person temporary_person = person_at_floor_vector_to_enter.at(i);
					if (temporary_person.get_person_priotiy() == 2)
						high_priotiry_people_vector.push_back(temporary_person);
					else
						low_priority_people_vector.push_back(temporary_person);
				}

				//cout << "-----There are " << high_priotiry_people_vector.size() << " high priority people to enter-----" << endl;
				//cout << "-----There are " << low_priority_people_vector.size() << " low priority people to enter-----" << endl;

				for (int i = 0; i < high_priotiry_people_vector.size() ;i++ ){
					person tmp_person = high_priotiry_people_vector.at(i);
					int person_direction;
					if (tmp_person.get_person_initial_floor() < tmp_person.get_person_destination_floor())
						person_direction = 1;
					else
						person_direction = 0;

					if (em->get_carried_weight_of_elevator() + tmp_person.get_person_weight() <= elevator_information.at(2) and
						em->get_number_of_people_in_the_elevator() + 1 <= elevator_information.at(3) and
						(em->get_direction_of_elevator() == person_direction or em->get_state_of_elevator() == 0))
					{
						em->get_person_in(tmp_person);
						em->sort_destination_floor_list();
						em->add_to_list_of_people_in_elevator(tmp_person);
						em->add_to_destination_floor_list(tmp_person.get_person_destination_floor());
						em->remove_from_list_of_person_monitor(tmp_person);
						usleep(in_out_time);
						print_person_entered(tmp_person);
						em->print_elevator_information();
					}
				}
				for (int i = 0; i < low_priority_people_vector.size() ;i++ ){
					person tmp_person = low_priority_people_vector.at(i);
					int person_direction;
					if (tmp_person.get_person_initial_floor() < tmp_person.get_person_destination_floor())
						person_direction = 1;
					else
						person_direction = 0;

					if (em->get_carried_weight_of_elevator() + tmp_person.get_person_weight() <= elevator_information.at(2) and
						em->get_number_of_people_in_the_elevator() + 1 <= elevator_information.at(3) and
						(em->get_direction_of_elevator() == person_direction or em->get_state_of_elevator() == 0))
					{
						em->get_person_in(tmp_person);
						em->sort_destination_floor_list();
						em->add_to_list_of_people_in_elevator(tmp_person);
						em->add_to_destination_floor_list(tmp_person.get_person_destination_floor());
						em->remove_from_list_of_person_monitor(tmp_person);
						usleep(in_out_time);
						print_person_entered(tmp_person);
						em->print_elevator_information();
					}
				}

				// original
				/*for (int i = 0; i < tmp_len ;i++ ){
					person tmp_person = person_at_floor_vector_to_enter.at(i);
					int person_direction;
					if (tmp_person.get_person_initial_floor() < tmp_person.get_person_destination_floor())
						person_direction = 1;
					else
						person_direction = 0;

					if (em->get_carried_weight_of_elevator() + tmp_person.get_person_weight() <= elevator_information.at(2) and
						em->get_number_of_people_in_the_elevator() + 1 <= elevator_information.at(3) and
						(em->get_direction_of_elevator() == person_direction or em->get_direction_of_elevator() == 0))
					{
						em->get_person_in(tmp_person);
						em->sort_destination_floor_list();
						em->add_to_list_of_people_in_elevator(tmp_person);
						em->add_to_destination_floor_list(tmp_person.get_person_destination_floor());
						em->remove_from_list_of_person_monitor(tmp_person);
						usleep(in_out_time);
						print_person_entered(tmp_person);
						em->print_elevator_information();
					}
				}*/
			}
		}
		em->remove_from_destination_list(em->get_current_floor_of_elevator());
		vector<int> destination = em->find_the_next_destination_floor();
		// print_new_dest_floor(destination);

		if (destination.at(0) == 9999){
			em->set_state_of_elevator(0);
			if (em->get_size_of_destination_floor_list() < 0)
				em->print_elevator_information();
			em->cv1_notify();
			usleep(nanosec);
		}
		else
		{
			if (destination.at(1) == 1){
				//em->print_elevator_information();
				em->set_state_of_elevator(1);
				//cout << "cv1_notify" << endl;
				em->cv1_notify();
				usleep(elevator_information.at(4) * 1000);
				em->move_elevator_up();
				em->print_elevator_information();

			}
			else {
				//em->print_elevator_information();
				em->set_state_of_elevator(2);
				//cout << "cv1_notify" << endl;
				em->cv1_notify();
				usleep(elevator_information.at(4) * 1000);
				em->move_elevator_down();
				em->print_elevator_information();
			}

		}
		em->sort_destination_floor_list();
		//em->print_elevator_information();
		//cout << "----- number of people in monitor list = " << em->get_size_of_list_of_person_monitor()<< "------" << endl;
	}
	em->print_elevator_information();



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
	vector <person> list_of_persons;
	vector<int> elevator_info;

	// parsing the input
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
						break;
						//cout<<"wrong input for the first line" << endl;
				}
				index_for_input_features++;
			}
		}
		else
		{ // people features
			vector<int > tmp_vector;
			person tmp_person;
			tmp_person.set_person_id(index_for_input_line - 1);
			tmp_vector.push_back(index_for_input_line - 1);
			for (istringstream is(line); is >> word;){
				tmp_vector.push_back(stoi(word));
			}
			tmp_person.set_person_priority(tmp_vector.at(4));
			tmp_person.set_person_initial_floor(tmp_vector.at(2));
			tmp_person.set_person_destination_floor(tmp_vector.at(3));
			tmp_person.set_person_weight(tmp_vector.at(1));
			list_of_persons.push_back(tmp_person);
		}
		index_for_input_line++;
	}

	elevator_info.push_back(number_of_floors);
	elevator_info.push_back( number_of_people);
	elevator_info.push_back(weight_capacity);
	elevator_info.push_back(person_capacity);
	elevator_info.push_back(travel_time);
	elevator_info.push_back(idle_time);
	elevator_info.push_back(in_out_time);


	elevator_monitor elevator_monitor_instance;
	thread person_thread_list[number_of_people];
	thread elevator_controller_thread;
	elevator_controller_thread= thread(elevator_controller_func, number_of_people, idle_time, &elevator_monitor_instance, elevator_info);
	sleep(1);
	for (int z = 0; z < number_of_people ; z++){
		person_thread_list[z] = thread(person_thread_function, &elevator_monitor_instance, list_of_persons.at(z), elevator_info);
		usleep(10);
	}

	for (int z = 0; z < number_of_people ; z++){
		person_thread_list[z].join();
	}
	elevator_controller_thread.join();

	//cout << "all threads are finished" << endl;







	return 0;
}


