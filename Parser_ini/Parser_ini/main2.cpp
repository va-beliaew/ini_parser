#include <iostream>
#include <map>
#include <fstream>
#include <string>
#include <iomanip>


class parser_ini {
	std::ifstream file;
	std::map<std::string, std::map<std::string, std::string>> data;
	std::pair<std::string, std::string> request;
	std::string::iterator sec_b;
	std::string::iterator sec_e;
	int num_of_lines = 1;

	void read() {
		try {
			std::string line;
			std::map<std::string, std::string> param;
			std::string section;
			bool section_write = false;
			bool section_find = false;
			bool equal = false;
			
			while (!file.eof()) {
				
				section_find = false;
				equal = false;
				getline(file, line);
				section_find = finds(line.begin(), line.end(), equal);
				if (section_find) {
					section = get_sec(sec_b, sec_e, section_write);
				}
				else {

					for (auto it = line.begin(); it < line.end(); ++it) {
						std::string::iterator back;
						if (*it == ';') {
							break;
						}
						else if (*it == ' ') {
							continue;
						}
						else if (section_write && equal) {
							param = get_param(it, line.end(), back);
							it = back;
							break;
						}
						else {

						}

					}
				}
				if (equal) {
				//	data.insert({ section, param });
					ins(section, param);
				}
				++num_of_lines;
			}
		}
		catch (const std::range_error& er) {
			std::cerr << er.what() << " line: " << num_of_lines; 
			exit(1);
		}
		catch (std::string estr) {
			std::cerr << estr << " line: " << num_of_lines;
			exit(1);
		}
	}

	void ins(std::string str, std::map<std::string, std::string> p) {

		if (data.find(str) != data.end()) {
			std::string rev = p.begin()->first;
			auto i = data.find(str);
			if (i->second.find(p.begin()->first) != i->second.end()) {
				i->second.erase(p.begin()->first);
				i->second.insert({ p.begin()->first, p.begin()->second });
			}
			else {
				i->second.insert({ p.begin()->first, p.begin()->second });
			}
		}
		else {
			data.insert({ str, p });
		}

	}

	bool finds(std::string::iterator a, std::string::iterator b, bool& fl) {
		bool s1 = false;
		bool s2 = false;
		for (auto& it = a; it < b; ++it) {
			if (*it == '[') {
				s1 = true;
				sec_b = it + 1;
			}
			else if (*it == ']' && s1) {
				s2 = true;
				sec_e = it;
			}
			else if (!s1 && !s2 && *it == '=') {
				fl = true;
			}
		}
		return s2;
	};

	std::string get_sec(std::string::iterator it, std::string::iterator end, bool& flag) {
		std::string word;
		for (auto& n = it; n < end; ++n) {
			if (*n != ' ' && *n != ';') {
				word.push_back(*n);
			}
			else {
				throw
				std::range_error("Uncorrect name of section!");
				exit(1);
			}
		}
		flag = true;
		return word;
	}

	std::map<std::string, std::string> get_param(std::string::iterator it, std::string::iterator end, std::string::iterator& back) {
		std::string parametr;
		std::string value;
		std::map<std::string, std::string> sm;
		bool wr_param = false;
		bool wr_val = false;
		bool equal = false;
		std::string::iterator v;
		for (auto& n = it; n < end; ++n) {
			if (*n == ' ') {
				continue;
			}
			else if (equal) {
				break;
			}
			else {
				for (auto& m = n; m < end; ++m) {
					if (*m != ' ' && *m != ';' && *m != '=') {
						parametr.push_back(*m);
						wr_param = true;
					}
					else if (*m == ' ') {
						for (auto& i = m; i < end; ++i) {
							if (*i == ' ') {
								continue;
							}
							else if (*i == '=') {
								equal = true;
								v = i + 1;
								back = i + 1;
								break;
							}
							else if(!equal && *i != ' ') {
								throw
									std::range_error("Uncorrect wrote parametr!");
							}
						}
						break;
					}
					else if (*m == '=') {
						equal = true;
						v = m + 1;
						back = m + 1;
						break;
					}
					else {
						throw
							std::range_error("Equal sign not find!");
					}
				}
			}
		}
		for (auto& r = v; r < end; r++) {
			if (*r == ' ') {
				continue;
			}
			else if (*r != ';' && !wr_val) {
				for (auto& m = r; m < end; ++m) {
					r = m;
					back = m;
					if (*m != ' ' && *m != ';') {
						wr_val = true;
						value.push_back(*m);

					}
					else {
						break;
					}

				}
				if (r >= end) {
					break;
				}
			}
			else if (*r == ';') {
				back = r;
				break;
			}
			else if(wr_val && *r != ';' && *r != ' ') {
				throw
					std::string("Parameter has two or more values!");
			}
		}
		sm[parametr] = value;
		return sm;
	}

	std::pair<std::string, std::string> to_pair(std::string s) {
		std::string first;
		std::string second;
		bool flag_point = false;
		for (auto n = s.begin(); n < s.end(); ++n) {
			if (*n == ' ') {
				throw std::invalid_argument("Uncorrect request!");
			}
			else if (*n == '.') {
				for (auto m = n + 1; m < s.end(); ++m) {
					if (*m == ' ') {
						throw std::invalid_argument("Uncorrect request!");
					}
					else {
						flag_point = true;
						second.push_back(*m);
					}
				}
				break;
			}
			else {
				first.push_back(*n);
			}
		}
		if (!flag_point) {
			throw std::invalid_argument("Uncorrect request!");
		}
		return { first, second };
	}

	std::string find() {
		if (data.find(request.first) == data.end()) {
			throw
				std::invalid_argument("Request section not find in this file!");
		}
		else {
			auto i_find = data.find(request.first);
			if (i_find->second.find(request.second) == i_find->second.end()) {
				all_parameter(i_find);
				throw std::exception(" ");
			
			}
			else {
				auto i_value = i_find->second.find(request.second);

				return i_value->second;
			}
		}
	}

	void all_parameter(std::map<std::string, std::map<std::string, std::string>>::iterator iter) {
		auto n = iter->second.begin();
		std::cout << "Такого параметра не найдено! Возможно, вы искали эти параметры:\n";
		while (n != iter->second.end()) {
			std::cout << n->first << std::endl;
			++n;
		}
	}


	int numeric(const std::string str) const {
		int type = 0;
		bool point = false;
		bool one_point = true;
		bool num = false;
		bool alf = false;
		bool sign = false;
		int one_symbol = 0;
		for (auto v = str.begin(); v < str.end(); ++v) {
			if (*v == '.' || *v == ',') {
				if ((*v == '.' || *v == ',') && point) {
					one_point = false;
				}
				point = true;
			}
			else if (sw(*v)) {
				num = true;
			}
			else if ((*v == '+' || *v == '-') && v == str.begin()) {
				sign = true;
			}
			else {
				alf = true;
			}
			++one_symbol;
		}
		if (num && !point && !alf && one_point) {
			if (sign) {
				type = 2; //int with sign
			}
			else {
				type = 1; //int
			}
		}
		else if (num && point && one_point && !alf) {
			if (sign) {
				type = 4; // double with sign
			}
			else {
				type = 3; // double
			}
		}
		else if (one_symbol == 1 && (alf || point || sign)) {
			type = 5; // char
		}
		else {
			type = 6; // string
		}
		return type;
	}

	bool sw(const char& a) const {
		bool num = false;
		switch (a) {
		case '0':num = true; break;
		case '1':num = true; break;
		case '2':num = true; break;
		case '3':num = true; break;
		case '4':num = true; break;
		case '5':num = true; break;
		case '6':num = true; break;
		case '7':num = true; break;
		case '8':num = true; break;
		case '9': num = true; break;
		default: num = false; break;
		}
		return num;
	}

	void print() {
		auto i = data.begin();
		while (i != data.end()) {
			std::cout << i->first << std::endl;
			auto j = i->second.begin();
			while (j != i->second.end()) {
				std::cout << j->first << '=' << j->second << std::endl;
				++j;
			}
			++i;
		}
		std::cout << std::endl;
	}


public:
	parser_ini(std::string adress) {
		try {
			file.open(adress);
			if (file.is_open()) {
				read();
				print(); // печать для проверки правильности чтения данных (отладочная функция)
			}
			else {
				throw
					std::exception("can't open this file!");
			}
		}
		catch (const std::exception& er) {
			std::cerr << er.what();
			exit(1);
		}
	}

	~parser_ini() {
		file.close();
	}

	template <typename T>
	T get_value(std::string request_) {
		try {
			request = to_pair(request_);
			std::string result = find();
			int type = numeric(result);
			int res_i = 0;
			static double res_d = 0;
			char res_c = ' ';
			std::string res;
			int change = 0;
			if (type == 1 || type == 2) {
				res_i = stoi(result);
				change = 1;
			}
			else if (type == 3 || type == 4) {
				res_d = stod(result);
				change = 2;
			}
			else if (type == 5) {
				res_c = result[0];
				change = 3;
			}
			else {
				res = result;
				change = 4;
			}

			if constexpr (std::is_same<std::string, T>::value) {
				if (change != 4) {
					throw
						std::out_of_range("Request type not provided this function!");
				}
				else {
					return res;
				}
			}
			else if constexpr (std::is_same<int, T>::value) {
				if (change != 1) {
					throw
						std::out_of_range("Request type not provided this function!");
				}
				else {
					return res_i;
				}
			}
			else if constexpr (std::is_same<double, T>::value) {
				if (change != 2) {
					throw
						std::out_of_range("Request type not provided this function!");
				}
				else {
					return res_d;
				}
			}
			else {
				if (change != 3) {
					throw
						std::out_of_range("Request type not provided for this function!");
				}
				else {
					return res_c;
				}
			}
		}
		catch (const std::exception& er) {
			std::cerr << er.what();
			exit(1);
		}
		catch (const std::out_of_range& er) {
			std::cerr << er.what();
		}
		catch (const std::invalid_argument& er) {
			std::cerr << er.what();
			exit(1);
		}
}
	
};


int main() {
	setlocale(LC_ALL, "rus");
	parser_ini parser { "data.ini" };
	auto value = parser.get_value<double> ("Section1.var1");
	std::cout << "value = " << std::fixed << std::setprecision(2) << value;

	return 0;
}