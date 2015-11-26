#include <iostream>
#include <string>
#include <string.h>
using namespace std;


struct Data{
	char* 	name;
	int 	id;
};

int main()
{
	
	Data *data = new Data;
	data->name = new char[1024];
	memcpy(data->name, "Hello", 1024);
	data->id = 300;
	cout << data->name << " " << data->id << endl;




	



	return 0;
}
