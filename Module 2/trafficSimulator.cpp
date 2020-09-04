#include <iostream>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <string.h>
#include <queue>
#include<fstream>
#include <sstream>
#include <iomanip>
#include <algorithm>

using namespace std;

mutex mutex1;

int p_num_threads = 2;
int c_num_threads = 2;

int hour_ind=47;

int ccount = 0;
int con_count=0;
int m=0;


condition_variable producer_cv, consumer_cv;

string ind, t_stamp, tr_light_id, no_of_cars;
vector<int> in;
vector<int> tr_light;
vector<int> no_cars;
vector<string> tstamp;


struct tr_signal
{
    int ind;
    std::string t_stamp;
    int tr_id;
    int num_cars;

};

tr_signal tlSorter[4]={{0,"",1,0},{0,"",2,0},{0,"",3,0},{0,"",4,0}};

queue<tr_signal> tr_sig_queue;
tr_signal sig;

bool sort_method(struct tr_signal first,struct tr_signal second)
{
if(first.num_cars>second.num_cars)
return 1;
return 0;
}
 
void* produce(void* args)
{
    
    while (ccount < m)
    {
        unique_lock<mutex> lk(mutex1);

        if (ccount < m)
        {
            tr_sig_queue.push(tr_signal{in[ccount], tstamp[ccount], tr_light[ccount], no_cars[ccount]});
            consumer_cv.notify_all();
        
            ccount++;
        }

        else
        {
            producer_cv.wait(lk, []{ return ccount < m; }); 
        }


        
        lk.unlock();

        sleep(rand()%3);
    }
}

void* consume(void* args){
    

    while(con_count< m)
    {
        unique_lock<mutex> lk(mutex1);

        if(!tr_sig_queue.empty()) {
            sig = tr_sig_queue.front();

            if(sig.tr_id==1){
                tlSorter[0].num_cars+=sig.num_cars;
            }
            if(sig.tr_id==2){
                tlSorter[1].num_cars+=sig.num_cars;
            }
            if(sig.tr_id==3){
                tlSorter[2].num_cars+=sig.num_cars;
            }
            if(sig.tr_id==4){
                tlSorter[3].num_cars+=sig.num_cars;
            }
            else{}


            tr_sig_queue.pop();
            producer_cv.notify_all();
            con_count++;

        }
        else
        {
            consumer_cv.wait(lk, []{ return !tr_sig_queue.empty(); });
        }

        if(con_count%48==0){
            sort(tlSorter,tlSorter+4,sort_method);
            printf("Traffic lights sorted according to most busy| Time: %s \n",sig.t_stamp.c_str());
            cout << "Traffic Light" << "\t" << "Number of Cars" << endl;
            cout << tlSorter[0].tr_id << "\t" << "\t" << tlSorter[0].num_cars << endl;
            cout << tlSorter[1].tr_id << "\t" << "\t" << tlSorter[1].num_cars << endl;
            cout << tlSorter[2].tr_id << "\t" << "\t" << tlSorter[2].num_cars << endl;
            cout << tlSorter[3].tr_id << "\t" << "\t" << tlSorter[3].num_cars << endl;
 
        }
        
        lk.unlock();

        
        sleep(rand()%3);
    }
}

void get_traff_data(){
    ifstream infile;

    string file;
	cout << "Enter the filename: ";
	cin >> file;

    infile.open(file);

    if (infile.is_open())                                
    {
        std::string line;
		getline(infile, line);

        while (!infile.eof()) 
		{
			
			getline(infile, ind, ',');
            in.push_back(stoi(ind));
			getline(infile, t_stamp, ',');
            tstamp.push_back(t_stamp);
			getline(infile, tr_light_id, ',');
            tr_light.push_back(stoi(tr_light_id));
			getline(infile, no_of_cars, '\n');
            no_cars.push_back(stoi(no_of_cars));
            
			m += 1; 
		}
		infile.close();
		
	}
	else printf("Could not open file, try again.");
}

int main() {

    get_traff_data();
    
 
    pthread_t producers[p_num_threads];
    pthread_t consumers[c_num_threads];

    for(long i=0; i<p_num_threads; i++) pthread_create(&producers[i], NULL, produce, (void*) i);
    for(long i=0; i<c_num_threads; i++) pthread_create(&consumers[i], NULL, consume, (void*) i);

    for(long i=0; i<p_num_threads; i++) pthread_join(producers[i], NULL);
    for(long i=0; i<c_num_threads; i++) pthread_join(consumers[i], NULL);

}