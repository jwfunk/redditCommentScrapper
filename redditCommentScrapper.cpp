//
//redditCommentScrapper.cpp
//
//Reddit's default api has some limitations so this program will be created to make some of these limitations less terrible

#include <fstream>
#include <iostream>
#include <ctime>
#include <chrono>
#include <string>
#include <vector>
#include <algorithm>

void readPost(std::string post){
	std::string url = post + ".json?limit=500";
	std::string command = "curl -s -A \"A reddit Scrapper\" \"" + url + "\" | jq '.' > .temp.txt";
	system(command.c_str());
	std::fstream fin;
	fin.open(".temp.txt", std::ios::in);
	std::string temp;
	std::string linkID = "";
	std::vector<std::string> moreLists;
	while(fin >> temp){
		if(temp == "\"body\":"){
			std::getline(fin, temp);
			std::cout << temp.substr(2, temp.length() - 4) << "\n";
		}
		if(temp == "\"link_id\":"){
			fin >> temp;
			temp.erase(std::remove(temp.begin(), temp.end(), '\"'), temp.end());
			temp.erase(std::remove(temp.begin(), temp.end(), ','), temp.end());
			linkID = temp;
		}
		if(temp == "\"more\","){
			for(int i = 0;i < 15;++i){
				fin >> temp;
			}
			temp.erase(std::remove(temp.begin(), temp.end(), '\"'), temp.end());
			temp.erase(std::remove(temp.begin(), temp.end(), ','), temp.end());
			std::string list = temp;
			fin >> temp;
			int length = 1;
			while(temp != "]"){
				temp.erase(std::remove(temp.begin(), temp.end(), '\"'), temp.end());
				temp.erase(std::remove(temp.begin(), temp.end(), ','), temp.end());
				if(length == 1){
					moreLists.push_back(list);
					list = temp;
					length = 0;
				}
				else
					list += "%2C" + temp;
				fin >> temp;
				++length;
			}
			moreLists.push_back(list);
		}
	}
	fin.close();
	for(auto it = moreLists.begin(); it != moreLists.end(); ++it){
		command = "curl -s -A \"reddit scrapper\" \"https://www.reddit.com/api/morechildren?api_type=json&children=" + *it + "&link_id=" + linkID + "\" | jq . | grep 'contentText' > .temp.txt";
		system(command.c_str());
		fin.open(".temp.txt", std::ios::in);
		while(std::getline(fin, temp)){
			std::string extra = "            \"contentText\": ";
			std::string::size_type i = temp.find(extra);
			temp.erase(i, extra.length());
			std::cout << temp.substr(1, temp.length() - 3) << "\n";
		}
		fin.close();
	}
}

int main(int argc, char *argv[]){
	if(argc < 2){
		std::string post;
		while(std::cin >> post){
			readPost(post);
		}
	}
	else{
		std::string post(argv[1]);
		readPost(post);
	}
//curl -s -A "reddit scrapper" "https://www.reddit.com/api/morechildren?api_type=json&children=gnnp5xw&link_id=t3_lkun4x" | jq .
	system("rm .temp.txt");
}
