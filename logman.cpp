//
//  main.cpp
//  logman.cpp
//
//  Created by Ian Perfitt on 11/1/16.
//  Copyright © 2016 logman.cpp. All rights reserved.
//

#include <iostream>
#include <vector>
#include <deque>
#include <algorithm>
#include <fstream>
#include <unordered_map>
#include <string.h>
using namespace std;

//struct for entries
struct entry {
    int id; //entry id
    long t; //timestamp
    string m; //messages
    string c; //category
};

//converts the given string to all lowercase
string convert_to_lower(string &l) {
    locale loc;
    string str = l;
    string lower = "";
    //loop through each string. convert char to lowercase
    for (string::size_type i = 0; i < str.length(); ++i) {
        lower = lower + tolower(str[i], loc);
        }
    return lower;
}

//chops up the string and returns
//vector of all keywords contained
//in string
void chop_string(string str, vector<string> &k) {
    string keyword = "";
    bool added = false;
    string temp = "";
    //loop through string and pick out nonalphanumeric characters
    //to create a keyword
    for (string::iterator it = str.begin(); it != str.end(); it++) {
        added = false;
        temp = *it;
        //push back nonalphnum characters into keyword string
        if (isalnum(*it)) {
            keyword.push_back(static_cast<char>(tolower(*it)));
        }
        else if (keyword != "") {
            //add keyword to vector of keywords if keyword
            //is not an empty string
            for (size_t i = 0; i < k.size(); i++) {
                //prevent from adding copies
                if (k[i] == keyword ) {
                    added = true;
                }
            }
            if (added == false) {
              k.push_back(keyword);
            }
            keyword.clear();
        }
    }
    for (size_t i = 0; i < k.size(); i++) {
        if (k[i] == keyword ) {
            added = true;
        }
    }
    if (keyword != "" && added == false) {
        k.push_back(keyword);
    }
}

void breakup(string c, string m, int index, unordered_map<string, vector<int>> &kmap) {
    vector <string> keys;
    chop_string(c, keys);
    chop_string(m, keys);
    string lower = "";
    string iterate = "";
    for (vector<string>::iterator it = keys.begin(); it != keys.end(); it++) {
        iterate = *it;
        for (string::iterator it2 = iterate.begin(); it2 != iterate.end(); it2++) {
            lower.push_back(static_cast<char>(tolower(*it2)));
        }
        kmap[lower].push_back(index);
        lower = "";
    }
}
    
    
//this function reads in the entries
//and stores them into E
void masterlog(string mlf, vector <entry> &E) {
    entry e;
    ifstream file;
    file.open(mlf);
    string m = "m";
    string d = "d";
    string h = "h";
    string min = "m";
    string s = "s";
    string timestamp = "t";
    int index = 0;
    while (getline(file, m, ':') && m != "\n") {
        if (m == "") {
            return;
        }
        getline(file, d, ':');
        getline(file, h, ':');
        getline(file, min, ':');
        getline(file, s, '|');
        getline(file, e.c, '|');
        getline(file, e.m, '\n');
        timestamp = m + d + h + min + s;
        e.t = stol(timestamp);
        e.id = index;
        E.push_back(e);
        ++index;
    }
    if (E.size() == 0) {
        return;
    }
    cout << E.size() << " entries read" << endl;
}

//if help is read in from command line
//this function prints out a message explaining
//the program
void print_h() {
    cout << "enter t <timestamp1>|<timestamp2> to search by timestamp range search" << endl;
    cout << "enter m <timestamp> to search by matching timestamp" << endl;
    cout << "enter c <string> to search by category" << endl;
    cout << "enter k <string> to search by keyword" << endl;
    cout << "enter a <integer> to append log entry from master log onto excerpt list" << endl;
    cout << "enter r to append search results on to back of excerpt list" << endl;
    cout << "enter d <integer> to delete a log enry from the excerpt list" << endl;
    cout << "enter b <integer> to move excerpt list entry to beginning of excerpt list" << endl;
    cout << "enter e <integer> to move the excerpt list entry to the end of the excerpt list" << endl;
    cout << "enter s to sort the excerpt list" << endl;
    cout << "enter l to clear the excerpt list" << endl;
    cout << "enter g to print most recent search results" << endl;
    cout << "enter p to print the excerpt list" << endl;
    cout << "enter q to quit" << endl;
    cout << "enter # followed by a string to create a comment" << endl;
    exit(0);
}

bool mcomp(entry a, entry b) {
    return a.t < b.t;
}


//searches for all log entries
//with timestamps matching the
//given timestamp
void m_search(vector<entry> &E, string &command, pair<vector<entry>::iterator, vector<entry>::iterator> &range) {
    command = "m";
    vector<string> t(5);
    string search = "";
    //reads in the month,day etc.
    //of the search time as string
    getline(cin, t[0], ':'); //t1 month
    getline(cin, t[1], ':'); //t1 day etc.
    getline(cin, t[2], ':');
    getline(cin, t[3], ':');
    getline(cin, t[4], '\n');
    search = t[0] + t[1] + t[2] + t[3] + t[4];
    if (search.size() != 11) {
        cerr << "incorrect timestamp format";
        return;
    }
    //convert to a long
    long s = stol(search);
    entry temp;
    temp.t = s;
    range = equal_range(E.begin(), E.end(), temp, mcomp);
    size_t begin = 0;
    size_t end = 0;
    begin = range.first - E.begin();
    end = range.second - E.begin();
    cout << "Timestamp search: " << end - begin << " entries found" << endl;
}
//
//searches for all log entries with categories matching
//the provided string
void c_search(unordered_map<std::string, vector<int>> &cmap, string &command, string &category) {
    command = "c";
    string c = ""; //string to compare with
    string cat = ""; //category
    getline(cin, c);
    string search = c.substr(1, c.size() - 1);
    category = convert_to_lower(search);
   cout << "Category search: " << cmap[category].size() << " entries found" << endl;
}

//move the excerpt list entry at given position
//to the end of the excerpt list
void end(deque<int> &excerpt) {
    size_t i = 0;
    cin >> i;
    if (i >= excerpt.size()) {
        return;
    }
    excerpt.push_back(excerpt[i]);
    excerpt.erase(excerpt.begin() + i);
    cout << "Moved excerpt list entry " << i << endl;
}

void beginning(deque <int> &excerpt) {
    int i = 0;
    cin >> i;
    if (((size_t) i) >= excerpt.size()) {
        return;
    }
    excerpt.push_front(excerpt[i]);
    excerpt.erase(excerpt.begin() + i + 1);
    cout << "Moved excerpt list entry " << i << endl;
}

void delete_log_entry(deque<int> &excerpt ) {
    int i = 0;
    cin >> i;
    if ((size_t) i >= excerpt.size() ) {
        return;
    }
    excerpt.erase(excerpt.begin() + i);
    cout << "Deleted excerpt list entry " << i << endl;
}

//sorts the excerpt list by timestamp, with
//ties broken by category, and further ties
//broken by entry ID
void sort_excerpt(deque<int> &excerpt) {
    sort(excerpt.begin(), excerpt.end());
    cout << "excerpt list sorted" << endl;
}

//sorts the previous searches and appends them
//to the back of the excerpt list
 void append_search(deque<int> &excerpt, vector<int> &ps, string &c,
                    std::vector<entry>::iterator &a, std::vector<entry>::iterator &b, vector<entry> &E,  pair<vector<entry>::iterator, vector<entry>::iterator> &range, vector<int> &category, bool &use_ps)  {
     size_t appended = 0;
     size_t begin = 0;
     size_t end = 0;
     if (use_ps == true) {
         appended = ps.size();
         for (size_t i = 0; i < ps.size(); i++) {
             excerpt.push_back(ps[i]);
         }
         cout << appended << " log entries appended" << endl;
         return;
     }
     if (c == "t") {
         begin = a - E.begin();
         end = b - E.begin();
         appended = end - begin;
         if (appended == 0) {
             return;
         }
         while (begin != end) {
             excerpt.push_back((int) begin);
             ++begin;
         }
     }
    if (c == "m") {
         begin = range.first - E.begin();
         end = range.second - E.begin();
         appended = end - begin;
        if (appended == 0) {
            return;
        }
         while (begin != end) {
             excerpt.push_back((int) begin);
             ++begin;
         }
     }
    if (c == "c") {
        appended = category.end() - category.begin();
        if (appended == 0) {
            return;
        }
        for (vector<int>::iterator it = category.begin(); it != category.end(); ++it) {
            excerpt.push_back(*it);
        }
    }
     cout << appended << " log entries appended" << endl;
}


bool tcomp (entry a, entry b) {
    return a.t < b.t;
}


//performs the timestamp search command
void t_search(vector<entry> &E, std::vector<entry>::iterator &a,
              std::vector<entry>::iterator &b, string &command) {
    command = "t";
    vector<string> c(10); //vector with times to compare
    //reads in the month,day etc.
    //of each range as string
    string l = "l";
    string u = "u";
    long lower = 0;
    long upper = 0;
    getline(cin, c[0], ':'); //t1 month
    getline(cin, c[1], ':'); //t1 day etc.
    getline(cin, c[2], ':');
    getline(cin, c[3], ':');
    getline(cin, c[4], '|');
    getline(cin, c[5], ':'); //t2 month
    getline(cin, c[6], ':'); //t2 day etc.
    getline(cin, c[7], ':');
    getline(cin, c[8], ':');
    getline(cin, c[9]);
      //create lower bound tstamp string
    l = c[0] + c[1] + c[2] + c[3] + c[4];
      //create upper bound tstamp string
    u = c[5] + c[6] + c[7] + c[8] + c[9];
    u = u.substr(0, u.size());
      //convert to longs for comparison
    lower = stol(l);
    upper = stol(u);
      //find lower bound in tmap
    entry temp1;
    entry temp2;
    temp1.t = lower;
    temp2.t = upper;
    a = lower_bound(E.begin(), E.end(), temp1, tcomp);
      //find upper bound in tmap
    b = upper_bound(E.begin(), E.end(), temp2, tcomp); //go to less than b
      //loop through map and copy entries into
      //previous search vector
    cout << "Timestamps search: " << b - a << " entries found" << endl;
}

//appends the log entry from given position in
//master log file onto the end of the excerpt list
void append_log(deque<int> &excerpt, long size, vector<int> &indices) {
    long i = 0;
    cin >> i;
    if (i >= size) {
        return;
    }
    excerpt.push_back(indices[i]);
    cout << "log entry " << i << " appended" << endl;
}

//converts the time stamp to a string
//with the appropriate colons in place
string t_str(string &t) {
    if (t.size() == 1) {
        return "00:00:00:00:0" + t;
    }
    if (t.size() == 2) {
        return "00:00:00:00:" + t;
    }
    if (t.size() == 3) {
        return "00:00:00:0" + t.substr(0, 1) + ":" + t.substr(1,2);
    }
    if (t.size() == 4) {
        return "00:00:00:" + t.substr(0,2) + ":" + t.substr(2,2);
    }
    if (t.size() == 5) {
        return "00:00:0" + t.substr(0, 1) + ":" + t.substr(1,2) + ":" + t.substr(3,4);
    }
    if (t.size() == 6) {
        return "00:00:" + t.substr(0,2) + ":" + t.substr(2,2) + ":" + t.substr(4,2);
    }
    if (t.size() == 7) {
        return "00:0" + t.substr(0, 1) + ":" + t.substr(1,2) + ":"
        + t.substr(3,2) + ":" + t.substr(5,2);
    }
    if (t.size() == 8) {
        return "00:" + t.substr(0,2) + ":" + t.substr(2,2) + ":"
        + t.substr(4,2) + ":" + t.substr(6,2) ;
    }
    if (t.size() == 9) {
        return "0" + t.substr(0, 1) + ":" + t.substr(1,2) + ":" +
        t.substr(3,2) + ":" + t.substr(5,2) + ":" + t.substr(7,2);
    }
    return t.substr(0,2) + ":" + t.substr(2,2) + ":"
    + t.substr(4,2) + ":" + t.substr(6,2) + ":" + t.substr(8,2);
}

void print_excerpt(deque<int> &excerpt, vector <entry> &E) {
    string output = "";
    string t = "";
    string time = "";
    for (size_t i = 0; i < excerpt.size(); i++) {
        t = to_string(E[excerpt[i]].t);
        time = t_str(t);
        output = output + to_string(i) + "|" + to_string(E[excerpt[i]].id) + "|" + time + "|"
        + E[excerpt[i]].c + "|" + E[excerpt[i]].m;
        cout << output << endl;
        output = "";
    }
}

//sorts the previous search vector
//then prints it in the configuration
//according to the spec
void print_search(vector<int> &ps, vector<entry> &E, string &command, std::vector<entry>::iterator &a, std::vector<entry>::iterator &b, pair<vector<entry>::iterator, vector<entry>::iterator> &range, vector<int> &category) {
    //timestamp string
    string t = "";
    //string to be printed
    string output = "";
    if (command == "t") {
        while (a != b) {
            t = to_string((*a).t);
            t = t_str(t);
            output = output + to_string((*a).id) +
            "|" + t + "|" + (*a).c + "|" + (*a).m;
            cout << output << endl;
            output = "";
            ++a;
        }
        return;
    }
    if (command == "m") {
        while (range.first != range.second) {
            t = to_string((*range.first).t);
            t = t_str(t);
            output = output + to_string((*range.first).id) +
            "|"+ t + "|" + (*range.first).c + "|" + (*range.first).m;
            cout << output << endl;
            output = "";
            ++range.first;
        }
        return;
    }
    if (command == "c") {
        auto it = category.begin();
        while (it != category.end()) {
            t = to_string(E[*it].t);
            t = t_str(t);
            output = output + to_string(E[*it].id) +
            "|"+ t + "|" + E[*it].c + "|" + E[*it].m;
            cout << output << endl;
            output = "";
            ++it;
        }
        return;
    }
    if (ps.empty()) {
        return;
    }
    //print ps vector
    for (size_t i = 0; i < ps.size(); i++) {
        t = to_string(E[ps[i]].t);
        t = t_str(t);
        output = output + to_string(E[ps[i]].id) +
        "|"+ t + "|" + E[ps[i]].c + "|" + E[ps[i]].m;
        cout << output << endl;
        output = "";
    }
    
}


//performs a keyword search on the
//log categories and log messages
//and displays the number of matching
//entries.
void keysearch(unordered_map<string, vector<int>> &kmap, vector <int> &ps) {
    //string for searching
    string search = "";
    string str = "";
    //read in string of keywords
    getline(cin, str);
    //vector for all keywords
    vector <string> keywords;
    //remove white space
    str = str.substr(1, str.size() - 1);
    //chop up string for individual
    //keywords
    chop_string(str, keywords);
    //loop through the kmap vector
    //with given keyword as key
    vector<int> results;
    results = kmap[keywords[0]];
        for (size_t i = 1; i < keywords.size(); i++) {
            vector<int>::iterator it = set_intersection(results.begin(), results.end(),
                                 kmap[keywords[i]].begin(), kmap[keywords[i]].end(), results.begin());
            results.resize(it - results.begin());
            if (results.empty()) {
                break;
            }
        }
    ps = results;
    cout << "Keyword search: " << ps.size() << " entries found" << endl;
}
    

//reads in the searching commands
void read_command(vector<int> &indices, unordered_map<string,
                  vector<int>> &cmap, vector <entry> &E, unordered_map<string, vector<int>> &kmap) {
    //vector of previous searches
    vector <int> ps;
    //excerpt list implemented as a vector
    deque <int> excerpt;
    std::vector<entry>::iterator a;
    std::vector<entry>::iterator b;
    pair<vector<entry>::iterator, vector<entry>::iterator> range;
    string category = "";
    string command = "";
    string c = "";
    bool use_ps = false;
    while (c != "q" ) {
        cout << "% ";
        cin >> c;
        if (c == "") {
            return;
        }
        if (c == "t") {
            ps.clear();
            use_ps = false;
            t_search(E, a, b, command);
            continue;
        }
        if (c == "m") {
            ps.clear();
            use_ps = false;
            m_search(E, command, range);
            continue;
        }
        if (c == "c") {
            ps.clear();
            use_ps = false;
            c_search(cmap, command, category);
            continue;
        }
        if (c == "p") {
            print_excerpt(excerpt, E);
            continue;
        }
        if (c == "l") {
            excerpt.clear();
            cout << "excerpt list cleared" << endl;
            continue;
        }
        if (c == "e") {
            end(excerpt);
            continue;
        }
        if (c == "g") {
            print_search(ps, E ,command, a, b, range, cmap[category]);
            continue;
        }
        if (c == "b") {
            beginning(excerpt);
            continue;
        }
        if (c == "d") {
            delete_log_entry(excerpt);
            continue;
        }
        if (c == "r") {
            if (!ps.empty() || (command == "t" || command == "m" || command =="c")) {
             append_search(excerpt, ps, command, a, b, E, range, cmap[category], use_ps);
            }
            continue;
        }
        if (c == "a") {
            append_log(excerpt, E.size(), indices);
            continue;
        }
        if (c == "s") {
            sort_excerpt(excerpt);
            continue;
        }
        if (c == "k") {
            use_ps = true;
            ps.clear();
            keysearch(kmap, ps);
            continue;
        }
        if (c == "#") {
            string comment = "comment";
            getline(cin, comment);
            continue;
        }
    }
}
//helper function for sorting
//the E vector
bool sortE(entry f, entry s) {
    if (f.t > s.t) {
        return false;
    }
    if (f.t == s.t) {
        if (strcasecmp(f.c.c_str(), s.c.c_str()) > 0) {
            return false;
        }
        if (strcasecmp(f.c.c_str(), s.c.c_str()) < 0) {
            return true;
        }
        if (strcasecmp(f.c.c_str(), s.c.c_str()) == 0) {
            if (f.id < s.id) {
                return true;
            }
            if (f.id > s.id) {
                return false;
            }
        }
    }
    return true;
}


void prepare_indices(vector<entry> &E, vector<int> &indices, unordered_map<string, vector<int>> &cmap,
                   unordered_map<string, vector<int>> &kmap) {
    string l = "";
    //sort the log vector
    stable_sort(E.begin(), E.end(), sortE);
    //store the original indices of the entries
    //as they were read in
    int index = 0;
    for (size_t i = 0; i < E.size(); i++) {
        indices[E[i].id] = (int) i;
        l = convert_to_lower(E[i].c);
        index = (int) i;
        cmap[l].push_back(index);
        breakup(E[i].c, E[i].m, (int)i, kmap);
    }
}



int main(int argc, const char * argv[]) {
    ios_base::sync_with_stdio(false);
    //contains the unsorted entries from master log file
    vector <entry> E;
    //unordered map for category searches
    unordered_map<string, vector<int>> cmap;
    //unordered map for keywordsearches
    unordered_map<string, vector<int>> kmap;
    char help[] = "help";
    char h[] = "-h";
    if (argc > 1) {
    if ((strcmp(argv[1] , help) == 0) || (strcmp(argv[1] , h) == 0)) {
        print_h();
    }
    else {
        masterlog(argv[1], E);
    }
        //sorted vector of entries
        vector<int> indices(E.size());
        prepare_indices(E, indices, cmap, kmap);
        read_command(indices, cmap, E, kmap);
}
    return 0;
}
