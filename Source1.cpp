#include <algorithm>
#include <array>
#include <atomic>
#include <chrono>
#include <cstdio>
#include <iostream>
#include <mutex>
#include <random>
#include <string>
#include <thread>
//#include <ncurses.h>

using namespace std;
mutex cout_mutex;

struct Fork 
{
	mutex mutex;
};

struct Dinner 
{
	atomic<bool> readines{ false };
	array<Fork, 5> Forks;
	~Dinner() 
	 { 
	   cout << "Dinner end's \n"; 
	   system("PAUSE");
	 }
}; 

class Philosopher
{
	mt19937 Range
	{ 
		random_device{}() 
	};

	const string Philosopher_name;
	const Dinner& dinner;
	Fork& Left_Fork;
	Fork& Right_Fork;
	thread worker;

	void Live();
	void Eat();
	void Think();

public:
	Philosopher(string Name, const Dinner& Dinner, Fork& left_fork, Fork& right_fork) : Philosopher_name(move(Name)), dinner(Dinner), Left_Fork(left_fork), Right_Fork(right_fork), worker(&Philosopher::Live, this)
	{ 

	}
	~Philosopher()
	{
		worker.join();
		lock_guard<mutex>  cout_lock(cout_mutex);
		cout << Philosopher_name << " go to bed!" << endl;
	}
};

void Philosopher::Live()
{
	while (not dinner.readines);
	do  
	 {
		lock(Left_Fork.mutex, Right_Fork.mutex);
		Eat();
		if (not dinner.readines)
		 {
			break;
		 }
		Think();
	 }
	while (dinner.readines);
}

void Philosopher::Eat()
{
	lock_guard<mutex> Fork_Left_Lock(Left_Fork.mutex, adopt_lock);
	lock_guard<mutex> Fork_Right_Lock(Right_Fork.mutex, adopt_lock);

	thread_local array<const char*, 6> ForewordToReaction 
	 { 
		 { 
			 " thinks that ",		//Myslie
			 " states that ",		//Ztwierdza
			 " claims that ",	    //Ztwierdza
			 " affirms that ",	    //Zapewnia
			 " assuring that ",		//Zapewnia
			 " convincing that "    //Zapewnia
		 }
	 };
	thread_local array<const char*, 6> Reaction
	 { 
		 {
			 "%s today is better than yesterday",
			 "this %s is good.", 
			 "this %s is gorgeous.",
			 "this %s is delicious.",
			 "this %s is horrible.",
			 "this %s have terrible taste."
		 } 
	 };
	thread_local array<const char*, 6> Food
	 { 
		 { 
			 "Spaghetti",
			 "Apple",
			 "Wine", 
			 "Juice",
			 "Meat",
			 "Porridge" //Kasza
		 } 
	 };
	thread_local uniform_int_distribution<> sleepForRange(1, 6);

	shuffle(ForewordToReaction.begin(), ForewordToReaction.end(), Range);
	shuffle(Reaction.begin(), Reaction.end(), Range);
	shuffle(Food.begin(), Food.end(), Range);

	//random_shuffle(Food.begin(), Food.end());
	//random_shuffle(Reaction.begin(), Reaction.end());
	//random_shuffle(ForewordToReaction.begin(), ForewordToReaction.end());

	constexpr size_t TextBufferSize = 1000;
	char Buffer[TextBufferSize];

	int NumberOfDishes = 5;//Numer of Dishes that evrey Philospher will eat per meal, can't be higher than quantity of arrays(ForewordToReaction,Reaction,Food)
						   //Liczba dan, ktore kazdy z filozofow zje za posilek, nie moze byc wiekszy od rozmiaru tablicy
	int BufforAdding;


	if (not dinner.readines) return;
	 {
		lock_guard<mutex>  cout_lock(cout_mutex);
		cout << Philosopher_name << " starts eating" << endl;
	 }


	for (int i = 0; i < NumberOfDishes; ++i)
	 {
		this_thread::sleep_for(chrono::milliseconds(sleepForRange(Range)) * 50);

		BufforAdding = snprintf(Buffer, TextBufferSize, ForewordToReaction[i], "");
		snprintf(Buffer + BufforAdding, TextBufferSize + BufforAdding, Reaction[i], Food[i]);

		lock_guard<mutex>  cout_lock(cout_mutex);
		cout << Philosopher_name << Buffer << endl;
	 }

	this_thread::sleep_for(chrono::milliseconds(sleepForRange(Range)) * 50);
	lock_guard<mutex>  cout_lock(cout_mutex);
	cout << Philosopher_name << " finished eating and left." << endl;
}

void Philosopher::Think()
{
	static constexpr array<const char*, 3> part1
	 { 
		 { 
			 "is thinking about ",
			 " tries to understand ",
			 " wondering about " 
		 } 
	 };
	static constexpr array<const char*, 6> part2
	 {
		 {
			 "politics.",
			 "art.", 
			 "meaning of life.", 
			 "morality.", 
			 "ethical side of human cloning.",
			 ": 'What is primary, spirit or matter?'" 
		 } 
	 };
	thread_local uniform_int_distribution<> sleepForRange(1, 10);
	thread_local uniform_int_distribution<> SizeOfPart1(0, part1.size() - 1);
	thread_local uniform_int_distribution<> SizeOfPart2(0, part2.size() - 1);

	while ((SizeOfPart1(Range) > 0) && SizeOfPart2(Range) > 0)
	 {
		this_thread::sleep_for(chrono::milliseconds(sleepForRange(Range) * 150));

		lock_guard<mutex>  cout_lock(cout_mutex);
		cout << Philosopher_name << part1[SizeOfPart1(Range)] << part2[SizeOfPart2(Range)] << endl;

		if (not dinner.readines)
		 {
			return;
		 }
	 }

	this_thread::sleep_for(chrono::milliseconds(sleepForRange(Range) * 150));
	lock_guard<mutex>  cout_lock(cout_mutex);
	cout << Philosopher_name << " want to eat " << endl;
}

int main()
{
	Dinner dinner;
	array<Philosopher, 5> philosophers
	 { 
		{
		  { "Friedrich Nietzsche", dinner, dinner.Forks[0], dinner.Forks[1] },
		  { "Immanuel Kant", dinner, dinner.Forks[1], dinner.Forks[2] },
		  { "Leo Tolstoy", dinner, dinner.Forks[2], dinner.Forks[3] },
		  { "Adam Smith", dinner, dinner.Forks[3], dinner.Forks[4] },
		  { "Confucius", dinner, dinner.Forks[4], dinner.Forks[0] }
		} 
	 };

	cout << "Dinner starts" << endl;

	dinner.readines = true;
	this_thread::sleep_for(chrono::seconds(5));
	dinner.readines = false;

	lock_guard<mutex>  cout_lock(cout_mutex);
	cout << "Evening is approaching" << endl;
}
