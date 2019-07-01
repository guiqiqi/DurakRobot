#pragma once

#include "header.h"

class iCard {
private:
	std::string _suit;
	std::string _rank;

public:
	iCard(std::string, std::string);
	std::string suit(void) const;
	std::string rank(void) const;
	static int compare(const iCard*, const iCard*);
	bool operator>(const iCard*&);
	bool operator<(const iCard*&);
	bool operator>=(const iCard*&);
	bool operator<=(const iCard*&);
	bool operator==(const iCard*&);
	friend std::ostream& operator<<(std::ostream&, const iCard&);
};

// Specialized hash support for pair<string, string>
namespace std {
	template<>
	class hash<std::pair<std::string, std::string>> {
	public:
		std::size_t operator()(const std::pair<std::string, std::string>& obj) const {
			std::hash<std::string> first;
			std::hash<std::string> second;
			return first(obj.first) + second(obj.second);
		}
	};
}

class CardManager {
private:
	std::vector<iCard*> all;
	std::unordered_map<std::pair<std::string, std::string>, iCard*> library;

public:
	CardManager(void);
	~CardManager(void);
	std::vector<iCard*> getall(void);
	iCard* get(std::string&, std::string&);
	iCard* get(std::pair<std::string, std::string>&);
};
