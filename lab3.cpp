#include <iostream>
#include <thread>
#include <mutex>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <stdio.h>
#include <fstream>

using namespace std;

//buffer
int buffer1[100] = { 0 }, buffer2[200] = { 0 };
int buffer1Counter = 0, buffer2Counter = 0;
mutex buffer1Lock, buffer2Lock;

//counter
int primeNumberTable[200] = { 0 };
int primeNumberCounter[200] = { 0 };
int primeNumberTableLen = 0;
int randomNumberCounter[100] = { 0 };
int sumNumberCounter[297] = { 0 };
int threadActionCounter[9] = { 0 };

//signal of thread end
int productCounter = 0;
mutex mplock[4], mcplock[3], mclock[2];

//producer
void p(int id){
	srand((unsigned)time(NULL));
	while(1){
		buffer1Lock.lock();
		if(productCounter >= 1000000){
			buffer1Lock.unlock();
			mplock[id].unlock();
			return;
		}
		if(buffer1Counter < 100){
			buffer1[buffer1Counter] = rand() % 100;
			randomNumberCounter[buffer1[buffer1Counter]]++;
			buffer1Counter++;
			productCounter++;
			threadActionCounter[id]++;
		}
		buffer1Lock.unlock();
	}
}

//consumer && producer
void cp(int id){
	while(1){
		buffer1Lock.lock();
		buffer2Lock.lock();
		if(productCounter >= 1000000 && buffer1Counter <= 3){
			buffer1Lock.unlock();
			buffer2Lock.unlock();
			mcplock[id].unlock();
			return;
		}
		if(buffer1Counter >= 3 && buffer2Counter < 100){
			buffer2[buffer2Counter] 
				= buffer1[buffer1Counter - 1] 
				+ buffer1[buffer1Counter - 2]
				+ buffer1[buffer1Counter - 3];
			sumNumberCounter[buffer2[buffer2Counter]]++;
			buffer2Counter++;	
			buffer1Counter -= 3;
			threadActionCounter[4 + id]++;
		}
		buffer1Lock.unlock();
		buffer2Lock.unlock();
	}
}

//consumer
void c(int id){
	while(1){
		buffer2Lock.lock();
		if(productCounter >= 1000000 && buffer1Counter <= 3 && buffer2Counter <= 0){
			buffer2Lock.unlock();
			mclock[id].unlock();
			return;
		}
		if(buffer2Counter >= 1){
			for(int i = 0; i < primeNumberTableLen && buffer2[buffer2Counter] > 1; i++)
				if(buffer2[buffer2Counter] % primeNumberTable[i] == 0){
					primeNumberCounter[i]++;
					buffer2[buffer2Counter] /= primeNumberTable[i];
				}
			buffer2Counter--;
			threadActionCounter[7 + id]++;
		}
		buffer2Lock.unlock();
	}
}

void initPrimeNumberTable(){
	bool b;
	primeNumberTable[0] = 2;
	primeNumberTableLen++;
	for(int i = 3; i <= 297; i++){
		b = true;
		for(int j = 2; j < (int)sqrt(i) + 1; j++)
			if(i % j == 0){
				b = false;
				break;
			}
		if(b)
			primeNumberTable[primeNumberTableLen++] = i;
	}
}

int main(){
	//init
	initPrimeNumberTable();

	//create thread
	thread *producer[4], *consumerAndProducer[3], *consumer[2];
	for(int i = 0; i < 4; i++){
		producer[i] = new thread(p, i);
		producer[i]->detach();
		mplock[i].lock();
		if(i < 3){
			consumerAndProducer[i] = new thread(cp, i);
			consumerAndProducer[i]->detach();
			mcplock[i].lock();
		}
		if(i < 2){
			consumer[i] = new thread(c, i);
			consumer[i]->detach();
			mclock[i].lock();
		}
	}

	//wait for thread end
	mplock[0].lock();
	mplock[1].lock();
	mplock[2].lock();
	mplock[3].lock();
	mcplock[0].lock();
	mcplock[1].lock();
	mcplock[2].lock();
	mclock[0].lock();
	mclock[1].lock();

	//output action counter
	for(int i = 0; i < 4; i++){
		cout << "Producer " << i << " 's action: " << threadActionCounter[i] << "times" <<endl;
	}
	for(int i = 0; i < 3; i++){
		cout << "Producer&&Consumer " << i << " 's action: " << threadActionCounter[4 + i] << "times" <<endl;
	}
	for(int i = 0; i < 2; i++){
		cout << "Consumer " << i << " 's action: " << threadActionCounter[7 + i] << "times" <<endl;
	}

	//save file
	ofstream mfile("result/randomNumber.txt");
	for(int i = 0; i <= 99; i++)
		mfile << i << " " << randomNumberCounter[i] << endl;
	mfile << endl;
	mfile.close();
	mfile.open("result/sumNumber.txt");
	for(int i = 0; i <= 297; i++)
		mfile << i << " " << sumNumberCounter[i] << endl;
	mfile.close();
	mfile.open("result/primeNumber.txt");
	for(int i = 0; i < primeNumberTableLen; i++)
		mfile << primeNumberTable[i] << " " << primeNumberCounter[i] << endl;
	mfile.close();

	//output chart
	FILE* gp;
	gp = popen("gnuplot -persist", "w");
	if(gp == NULL){
		cout<<"cannot open gnuplot"<<endl;
		exit(0);
	}
	fprintf(gp, "set term pngcairo\n");
	fprintf(gp, "set ylabel \"count\"\n");
	//randomNumber
	fprintf(gp, "set output \"result/randomNumberChart.png\"\n");
	fprintf(gp, "set xlabel \"randomNumber\"\n");
	fprintf(gp, "set xrange [0:99]\n");
	fprintf(gp, "set yrange [8000:12000]\n");
	fprintf(gp, "plot \"result/randomNumber.txt\" with lines\n");
	fprintf(gp, "set output\n");

	//sumNumber
	fprintf(gp, "set output \"result/sumNumberChart.png\"\n");
	fprintf(gp, "set xlabel \"sumNumber\"\n");
	fprintf(gp, "set xrange [0:297]\n");
	fprintf(gp, "set yrange [0:3000]\n");
	fprintf(gp, "plot \"result/sumNumber.txt\" with lines\n");
	fprintf(gp, "set output\n");

	//primeNumber
	fprintf(gp, "set output \"result/primeNumberChart.png\"\n");
	fprintf(gp, "set xlabel \"primeNumber\"\n");
	fprintf(gp, "set xrange [0:300]\n");
	fprintf(gp, "set yrange [0:200000]\n");
	fprintf(gp, "plot \"result/primeNumber.txt\" with lines\n");
	fprintf(gp, "set output\n");

	pclose(gp);
	return 0;
}
