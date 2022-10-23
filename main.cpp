#include <iostream>
#include <string>
#include <bits/stdc++.h>
#include <vector>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <stack>
#include <set>
#include <algorithm>
using namespace std;

struct minterm {
    int decimal = -1; // the decimal representation whether
    vector<minterm> ms; //if it's a reduced prime implicant (ex: 1--1), this saves the minterms that made it up
    bool paired = false; //in the tabulation method, it helps us determine the prime implicants
    int numof1s; //number of ones in each binary number
    string binary; //binary representation of the minterm
    bool dc = false; //determine whether it's a don't care or not, we only need to determine it for when we creat ethe coverage chart
};

set<string> minimizedBool;
vector<minterm> primes; //the prime implicants genertaed after the tabulation method and to be used in the coverage chart

vector <minterm> mts; //list of all the terms, don't cares and minterms
int bits; //# of bits
vector<int> mt, dc; //these are minterms and don't cares
vector<string> minterms, dontcares; // minterms and don't cares as strings
vector<int> mtOnes, dcOnes; //number of ones in each binary number

vector<int> valid; //an array to use inside the validation function

string inttobin(int n) { //using bitmasking to get the binary representation of each number
    string s = "";
    while (n > 0) {
        if (n & 1) s = '1' + s;
        else s = '0' + s;
        n /= 2;
    }
    return s;
}
void read_input(string file) //funtion to read the txt file
{
    vector<string> lines; // a vector to contain each line
    ifstream mts;
    mts.open(file);
    if (!mts.is_open()) cout << "ERROR OPENING FILE \n"; // check if the file opened or not
    string line;

    while (getline(mts, line)) {
        lines.push_back(line); //reading in each line
    }
    bits = stoi((lines[0])); //assignning the first line to the # of bits
    stringstream s(lines[1]);
    string temp;
    while (s >> temp) { //push back each minterm into a vector
        mt.push_back(stoi(temp));
    }
    if (lines.size() == 3) { //in case we have no don't care terms
        stringstream dcs(lines[2]);
        while (dcs >> temp) {// push back each don't care term into a vector
            dc.push_back(stoi(temp));
        }
    }

    mts.close();
}

bool validation() {
    for (int i = 0; i < mt.size(); i++) valid.push_back(mt[i]);
    for (int i = 0; i < dc.size(); i++) valid.push_back(dc[i]);
    sort(valid.begin(), valid.end());
    for (int i = 0; i < valid.size() - 1; i++) {
        if (valid[i] == valid[i + 1] || valid[i] > pow(2, bits)) return false;
    } return true;
}

void binaryReps() {
    for (int i = 0; i < mt.size(); i++) { //turning the minterms into string to make sure the number of digits equals the number of variables AKA adding zeros on the left of the number 1 to have 3 digits 1 -> 001
        string temp = inttobin(mt[i]); //turning the decimal value of a number into a string representing the binary value
        bool length = 0;
        while (length == false) {
            if (temp.length() == bits) {
                length = true;
            }
            else temp = '0' + temp;
        }
        minterms.push_back(temp);
    }
    // generating binary representation for don't care values and turning them into strings, same procedure we just performed for the minterms
    for (int i = 0; i < dc.size(); i++) {
        string temp = inttobin(dc[i]);
        bool length = 0;
        while (length == false) {
            if (temp.length() == bits) {
                length = true;
            }
            else temp = '0' + temp;
        }
        dontcares.push_back(temp);
    }
}

int generateOnes(string binary) {  //function that counts the number 1's in each number we need this one seperatly because we are going to use it in the middle
    int ones = 0;
    for (int j = 0; j < bits; j++) {
        if (binary[j] == '1') ones++;
    }
    return ones;
}

void generateOnesforminterms() { //a function that counts the ones for each binary value and puts them in vectors for minterms and don't cares
    for (int i = 0; i < minterms.size(); i++) {
        int ones = generateOnes(minterms[i]);
        mtOnes.push_back(ones); //number of ones in each minterm value
    }
    for (int i = 0; i < dontcares.size(); i++) {
        int ones = generateOnes(dontcares[i]);
        dcOnes.push_back(ones); //number of ones in each don't care value
    }
}

void fill_struct() { //we're filling vector of structs
    for (int i = 0; i < mt.size(); i++) {
        struct minterm obj; //we create an instance and then push into the vector
        obj.decimal = mt[i];
        obj.binary = minterms[i];
        obj.dc = 0;
        obj.numof1s = mtOnes[i];
        mts.push_back(obj);
    }
    for (int i = 0; i < dc.size(); i++) { //push the don't cares into the same vector
        struct minterm obj;
        obj.decimal = dc[i];
        obj.binary = dontcares[i];
        obj.dc = 1;
        obj.numof1s = dcOnes[i];
        mts.push_back(obj);
    }
}
bool compareOnes(minterm one, minterm two) { //define a function to act as the comparator in the sorting function
    if (one.numof1s != two.numof1s) return one.numof1s < two.numof1s;
    if (one.numof1s == two.numof1s) return one.numof1s < two.numof1s;
}


void print(vector<minterm> pi) { //printing to check the vector
    cout << "Properties: Minterms" << setw(30) << "Binary" << setw(30) << "Number of Ones" << setw(30) << endl;
    for (int i = 0; i < pi.size(); i++) {
        if (pi[i].decimal != -1) cout << setw(15) << pi[i].decimal << setw(35) << pi[i].binary << setw(28) << pi[i].numof1s << endl;
        else {
            cout << setw(5);
            for (int j = 0; j < pi[i].ms.size() - 1; j++) {
                cout << "m" << pi[i].ms[j].decimal << ", ";
            }

            cout << "m" << pi[i].ms[pi[i].ms.size() - 1].decimal << setw(35) << pi[i].binary << setw(33) << pi[i].numof1s << endl;
        }
    }
}

bool compareMTs(minterm& a, minterm& b, minterm& c) { //function to compare minterms by 1 bit difference
    int diff = 0, index;
    for (int i = 0; i < bits; i++) {
        if (a.binary[i] != b.binary[i]) {
            diff++;
            index = i;
        }
    }
    if (diff == 1 ) { // the prime implicant is not made up of two don't care terms
        a.paired = true;
        b.paired = true; //making sure that these minterms have been paired w smth
        string temp = a.binary; temp[index] = '-';
        c.binary = temp;
        if (a.ms.size() == 0 && b.ms.size() == 0) { //we are checking to see if the values that made the pi up are minterms or other pi's
            //if the size of the minterms that made the pi up is zero, that means it's a minterm, if not, THEN it's a prime implicant
            c.ms.push_back(a); c.ms.push_back(b);
        }
        else {
            for (int i = 0; i < a.ms.size(); i++) {
                c.ms.push_back(a.ms[i]);
                c.ms.push_back(b.ms[i]);
            }
        } c.numof1s = generateOnes(c.binary);
        return true;
    }
    else return false;
}

void tabulationMethod() {
    int begin = 0, end = mts.size();
    bool addition = true;

    while (addition) { //checks if anymore columns have been added
        int added = 0;
        while (begin != end - 1) {//if we have reached the end of the current column
            int counter = begin + 1;
            while (mts[counter].numof1s < (mts[begin].numof1s + 2) && counter < end) { //check every element with the next group of ones and stop once we finish the group
                if (mts[counter].numof1s == mts[begin].numof1s) counter++;
                else {//skip the number if it's in the same group of 1's as you.
                    minterm obj;
                    if (compareMTs(mts[begin], mts[counter], obj)) {
                        mts.push_back(obj); //if a new object is created, add it
                        added++; //this determines whether we have merged new pi's to create new columns or not
                    }
                    counter++;
                }
            }
            begin++;
        }
        if (added == 0) addition = false;
        begin = end;
        end += added;
    }
}

void removeDuplicates() {
    set<string> pis; //we use the fact that the set does not accept values that it already has
    for (int i = 0; i < mts.size(); i++) {
        int increase = pis.size(); //we check each element, if it is accepted into the set, then it is unique and we insert it
        pis.insert(mts[i].binary);
        bool mtp = true;
        if (pis.size() != increase){
            if(mts[i].ms.size() != 0){
                mtp = false;//to check if all minterms creating it are dc
                for(auto x : mts[i].ms){
                    if(x.dc == false) mtp = true;
                }}
            if(!mts[i].paired && mtp) primes.push_back(mts[i]); //if it is unique AND has not been paired w another minterm or prime implicant, then it will be used in the coverage chart
        }
    }
}

void coverageChart() {
    vector<vector<char>> chart(primes.size(), vector<char>(mt.size(),'-')); //initialize the 2d matrix s.t. the rows are the pi's and the cols are the minterms
    vector<pair<int, vector<int>>> xcount(mt.size()); //this vector saves the number of pi's that cover each minterm and the prime implicant that cover the minterms location

    for (int i = 0; i < primes.size(); i++) { // for every prime implicant we check the minterms it covers by overwriting the '-' with an 'x'
        if (!(int) primes[i].ms.size()) { //if it's a minterms, just cross out its minterm
            for (int j = 0; j < (int) mt.size(); j++) {
                if (mt[j] == primes[i].decimal) { //if it covers the same minterm as the column we are currently at, put an x
                    chart[i][j] = 'x';
                    xcount[j].first++; //the number of x's under each minterm/column
                    xcount[j].second.push_back(i); //the pi's coverig it, we save its row
                }
            }
        } else { //if it's a prime implicant. then cross out the minterms it covers
            for (int j = 0; j < (int) primes[i].ms.size(); j++) {
                for (int x = 0; x < mt.size(); x++) {
                    if (primes[i].ms[j].decimal == mt[x]) {
                        chart[i][x] = 'x';
                        xcount[x].first++;
                        xcount[x].second.push_back(i);
                    }
                }
            }
        }
    }

    for(int i = 0; i < primes.size(); i++){ //printing the coverage chart
        if(primes[i].ms.size() == 0) cout << primes[i].decimal << ": ";
        else cout << primes[i].binary << ": ";
        for(int j = 0; j < mt.size(); j++){
            cout << chart[i][j] << " ";
        }cout << endl;
    }

    // find single x and change it to d, store row and col index in vector
    vector<pair<int, int>> changed; //saves the indices of the changed x's to d
    for (int i = 0; i < mt.size(); i++) {
        if (xcount[i].first == 1) {
            for (int j = 0; j < mt.size(); j++) {
                int row = xcount[i].second[0]; //we set the row as fixed
                if (chart[row][j] == 'x') {  //then we iterate over that row and mark all the x's as d's
                    chart[row][j] = 'd';
                    changed.push_back({row, j}); //save their locations so that we can go and cross out these prime implicant's vertical to the epi columns
                }
            }
        }
    }
    for(auto x: changed) minimizedBool.insert(primes[x.first].binary); // add primes that were first chosen

    // for each prime implicant, change the x in the cols in vector
    for(auto x: changed) { //the indices obtained are used to cross out the columns by fixing the columns and jumping between rows
        for (int row = 0; row < primes.size(); row++) {
            if (chart[row][x.second] == 'x') {
                chart[row][x.second] = 'd';
            }
        }
    }

    set<int> dom_cols;
    for (int col = 0; col < mt.size(); col++) { //remove dominating col
        for (int k = col + 1; k < mt.size(); k++) {
            int c1 = 0, c2 = 0; //c1 counts the x's in the same column, c2 counts the x's in the 2nd column
            int c = 0; //this c counts the x's that share the same column
            for (int row = 0; row < primes.size(); row++) {
                if (chart[row][col] == 'x') c1++;
                if (chart[row][k] == 'x') c2++;
                if (chart[row][col] == 'x' && chart[row][k] == 'x') c++;
            }
            if((c == c1 || c == c2) && c) { // check if one column dominates another
                int choice;
                if (c1 > c2) choice = col;
                else choice = k;
                dom_cols.insert(choice);
            }
        }
    }

//    cout << "\nDominating columns: ";
    for (auto choice : dom_cols) {
        for (int row = 0; row < primes.size(); row++) {
            if (chart[row][choice] == 'x') chart[row][choice] = 'd'; // set dominating column's x's to 'd'
        }
        //cout << mt[choice] << " ";
    }

    // find dominated rows
    set<int> dom_rows;
    for (int row = 0; row < primes.size(); row++) {
        for (int k = row+1; k < primes.size(); k++) {
            int r1 = 0, r2 = 0; //checks the x's in each row
            int r = 0; //checks for common x's
            for (int col = 0; col < mt.size(); col++) {
                if (chart[row][col] == 'x') r1++;
                if (chart[k][col] == 'x') r2++;
                if (chart[row][col] == 'x' && chart[k][col] == 'x') r++;
            }
            if ((r == r1 || r == r2) && r) { // check if one row dominates another
                int choice;
                if (r1 < r2) choice = row; // whichever row has less x's is the dominated AKA the one we need to cross out
                else choice = k;
//                cout << "row: " << row << ", k: " << k << endl;
//                cout << primes[row].binary << " dominates " << primes[k].binary << endl;
                dom_rows.insert(choice);
            }
        }
    }
//    cout << "\nDominated rows: ";
//    for (auto choice : dom_rows) {
//        cout << primes[choice].binary << " ";
//    }
    for (auto choice : dom_rows) {
        for (int col = 0; col < mt.size(); col++) {
            if (chart[choice][col] == 'x') chart[choice][col] = 'd'; // set dominated to 'd'
        }
    }

    for (int row = 0; row < primes.size(); row++) { //after removing the dominating columns and dominated rows, we start taking the prime implicants left and including them in the minimized boolean expression
        int cx = 0; //number of x's
        for (int col = 0; col < mt.size(); col++) {
            if (chart[row][col] == 'x') cx++;
        }
        if (cx > 0) minimizedBool.insert(primes[row].binary);
    }

    cout << endl << endl;
    for(int i = 0; i < primes.size(); i++){ //printing the coverage chart
        if(primes[i].ms.size() == 0) cout << primes[i].decimal << ": ";
        else cout << primes[i].binary << ": ";
        for(int j = 0; j < mt.size(); j++){
            cout << chart[i][j] << " ";
        }cout << endl;
    }

    cout << endl << endl << "Minimized boolean expression = ";
    int i = 0;
    for (auto x : minimizedBool) {
        if (i < minimizedBool.size() - 1)
            cout << x << " + ";
        else cout << x << "\n";
        i++;
    }
}

void expression(){
    string let[20] = { "A","B", "C", "D", "E", "F", "G", "H", "I","J", "K", "L", "M", "N", "O", "P", "Q","R", "S", "T"};
    int i = 0;
    for(auto pi : minimizedBool){
        string w = "";
        for(int i = 0; i < pi.size(); i++){
            if(pi[i] == '1'){
                w += let[i];
            }else if(pi[i] == '0') w+= let[i]+'`';
        } if(i < minimizedBool.size() - 1)
            cout << w << " + ";
        else cout << w << "\n";
        i++;
    }
}

int main() {
    string file;
    int in;
    cout<<"Please enter the file  number from 1-10 you want to test";
    cin>> in;

    if ( in==1) file = R"(C:/Users/CG/Documents/GitHub/DigitalDesignProj1-QuinnMclusky/minterms2.txt)";
    if ( in==2)
        file = R"(C:/Users/CG/Documents/GitHub/DigitalDesignProj1-QuinnMclusky/minterms3.txt)";
    if ( in==3)
        file = R"(C:/Users/CG/Documents/GitHub/DigitalDesignProj1-QuinnMclusky/minterms4.txt)";
    if ( in==4)
        file = R"(C:/Users/CG/Documents/GitHub/DigitalDesignProj1-QuinnMclusky/minterms5.txt)";
    if ( in==5)
        file = R"(C:/Users/CG/Documents/GitHub/DigitalDesignProj1-QuinnMclusky/minterms6.txt)";

    if ( in==6)
        file = R"(C:/Users/CG/Documents/GitHub/DigitalDesignProj1-QuinnMclusky/minterms7.txt)";
    if ( in==7)
        file = R"(C:/Users/CG/Documents/GitHub/DigitalDesignProj1-QuinnMclusky/minterms8.txt)";
    if ( in==8)
        file = R"(C:/Users/CG/Documents/GitHub/DigitalDesignProj1-QuinnMclusky/minterms9.txt)";
    if ( in==9)
        file = R"(C:/Users/CG/Documents/GitHub/DigitalDesignProj1-QuinnMclusky/minterms10.txt)";
    if ( in==10)
    {  file = R"(C:/Users/CG/Documents/GitHub/DigitalDesignProj1-QuinnMclusky/minterms.txt)";}

    //put the name/directory of the txt file
    read_input(file);
    if (!validation()) {
        cout << "INVALID INPUT" << endl;
        return 0;
    };
    binaryReps();
    generateOnesforminterms();
    fill_struct();
    sort(mts.begin(), mts.end(), compareOnes);
    tabulationMethod();
    //print(mts);
    //cout << "\t\t\tAFTER REMOVING DUPLICATES\n";
    removeDuplicates();
    print(primes);
    coverageChart();
    cout << endl;
    expression();

    return 0;
}
