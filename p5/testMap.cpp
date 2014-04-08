#include "MultiMap.h"
#include <iostream>
#include <cassert>
#include "Database.h"

using namespace std;
int main()
{
	MultiMap myMultiMap;
	
	myMultiMap.insert("Mel", 3);
	myMultiMap.insert("Darren", 3);
	myMultiMap.insert("Rick", 3);
	myMultiMap.insert("John", 3);
	myMultiMap.insert("Paulene", 3);
	myMultiMap.insert("Carey", 3);
	myMultiMap.insert("Carey", 3);
	myMultiMap.insert("Arissa", 3);
	myMultiMap.insert("Casey", 3);
	myMultiMap.insert("Dale", 3);
	myMultiMap.insert("Mel", 4);
	myMultiMap.insert("Darren", 4);
	myMultiMap.insert("Rick", 4);
	myMultiMap.insert("John", 4);
	myMultiMap.insert("Paulene", 4);
	myMultiMap.insert("Arissa", 4);
	myMultiMap.insert("Casey", 4);
	myMultiMap.insert("Dale", 4);
	myMultiMap.insert("Mel", 3);
	myMultiMap.insert("Darren", 3);
	myMultiMap.insert("Rick", 3);
	myMultiMap.insert("John", 3);
	myMultiMap.insert("Paulene", 3);
	myMultiMap.insert("Arissa", 3);
	myMultiMap.insert("Casey", 3);
	myMultiMap.insert("Dale", 3);
	myMultiMap.insert("Mel", 4);
	myMultiMap.insert("Darren", 4);
	myMultiMap.insert("Rick", 4);
	myMultiMap.insert("John", 4);
	myMultiMap.insert("Paulene", 4);
	myMultiMap.insert("Arissa", 4);
	myMultiMap.insert("Casey", 4);
	myMultiMap.insert("Dale", 4);
	myMultiMap.insert("Casee", 3);



	// Start at the earliest-occurring association with key "Bill"
	MultiMap::Iterator it = myMultiMap.findEqualOrPredecessor("Dd");
	while (it.valid())
	{
		cout << it.getKey() << " " << it.getValue() << endl;
		it.next(); // advance to the next associaton
	}
	//if(it.valid()) {MultiMap::Iterator p;for ( ; it.valid(); it.prev()) p = it; for ( ; p.valid(); p.next())cout << p.getKey() << " " << p.getValue() << endl;}


	}