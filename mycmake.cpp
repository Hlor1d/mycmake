#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <map>

struct Data {
	int size;
	std::vector <std::string> targets;
	std::vector <std::string> actions;
	std::vector <bool> realization;
	std::map<int, std::vector <int>> dependencies;
};

std::string error_message_table[] = {
		"", // SUCCESS,
		"cannot open file",
		"wrong input format",
		"no task realisation",
		"tasks hava mutual dependence",
		"no such task"
};

int read_data(std::string filename, Data* data)
{
	data->actions.clear();
	data->dependencies.clear();
	data->targets.clear();
	data->realization.clear();
	data->size = 0;
	std::map<std::string, unsigned> targetsmap;
	std::ifstream input_stream(filename);
	if (input_stream.is_open())
	{
		std::string previousword = "";
		std::string firstword = "";
		for (std::string mystr; std::getline(input_stream, mystr);) {
			firstword = "";
			int l = 0;
			do {
				firstword = mystr.substr(l, mystr.find(' ', l));
				l++;
			} while ((firstword.length() == 0) && (l < 10000));
			if (l >= 10000)
			{
				return 2;
			}
			if (firstword[firstword.length() - 1] == ':') {
				firstword.pop_back();
				previousword = firstword;
				if (targetsmap.count(firstword) == false) {
					data->actions.push_back("");
					data->realization.push_back(true);
					targetsmap[firstword] = data->size;
					data->size++;
				}
				else
				{
					data->realization[targetsmap[firstword]] = true;
				}
				std::vector <int> vect;
				std::stringstream ss(mystr);
				std::string dependencytarget;
				ss >> dependencytarget;
				while (ss >> dependencytarget) {
					if (targetsmap.count(dependencytarget) == false) {
						data->actions.push_back("");
						data->realization.push_back(false);
						targetsmap[dependencytarget] = data->size;
						data->size++;
					}
					vect.push_back(targetsmap[dependencytarget]);

				}
				std::vector <int> vect2 = data->dependencies[targetsmap[firstword]];
				for (auto var : vect)
				{
					vect2.push_back(var);
				}
				data->dependencies[targetsmap[firstword]] = vect2;

			}
			else
			{
				data->actions[targetsmap[previousword]] += mystr;
			}
		}
		data->targets.resize(data->size);
		for (const auto& element : targetsmap)
			data->targets[element.second] = element.first;
	}
	else
	{
		return 1;
	}
	input_stream.close();
	return 0;
}

void check_errors(Data* data, int k, int& rezult)
{
	std::vector <int> check(data->size, 0);
	std::vector <int> stack;
	stack.push_back(k);
	while (!stack.empty())
	{
		k = stack.back();
		if (check[k] == 1)
		{
			check[k] = 2;
			stack.pop_back();
		}
		else
		{
			check[k] = 1;
			if (data->realization[k] == false) {
				rezult = 3;
			}
			for (auto var : data->dependencies[k])
			{
				if ((rezult == 0) && (check[var] == 0)) {
					stack.push_back(var);

				}
				else  if (check[var] == 1)
				{
					rezult = 4;
				}

			}
		}
	}
}

void write_data(Data* data, int k)
{
	std::vector <bool> wrote(data->size, false);
	std::vector <int> check(data->size, 0);
	std::vector <int> stack;
	stack.push_back(k);
	while (!stack.empty())
	{
		k = stack.back();
		if (check[k] == true)
		{
			if (wrote[k] != true)
			{
				std::cout << data->targets[k] + ":\n" + data->actions[k] + '\n';
				wrote[k] = true;
			}
			stack.pop_back();
		}
		else
		{
			check[k] = true;
			for (auto var : data->dependencies[k])
			{
				stack.push_back(var);

			}
		}
	}
}

int main(int argc, char* argv[])
{
	if (argc != 3) {
		std::cerr << "Wrong parameters count" << std::endl;
		return 1;
	}
	Data data;
	std::string mystr = argv[1];
	int rezult = read_data(mystr, &data);
	if (rezult) {
		std::cerr << error_message_table[rezult] << std::endl;
		return rezult;
	}
	int index = -1;
	for (int i = 0; i < data.size; i++)
	{
		if (data.targets[i] == argv[2])
		{
			index = i;
		}
	}
	if (index == -1)
	{
		std::cerr << error_message_table[5] << std::endl;
		return 5;
	}
	check_errors(&data, index, rezult);
	if (rezult) {
		std::cerr << error_message_table[rezult] << std::endl;
	}
	else
	{
		write_data(&data, index);
	}
	return rezult;
}
