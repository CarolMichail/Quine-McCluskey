#include <iostream>
#include <string>
#include <bits/stdc++.h>
#include <vector>
#include <iomanip>
#include <sstream>
#include <fstream>
using namespace std;

struct minterm{
    int decimal = -1; // the decimal representation whether
    vector<minterm> ms; //if it's a reduced prime implicant (ex: 1--1), this saves the minterms that made it up
    bool paired = false; //in the tabulation method, it helps us determine the prime implicants
    int numof1s; //number of ones in each binary number
    string binary; //binary representation of the minterm
    bool dc; //determine whether it's a don't care or not, we only need to determine it for when we creat ethe coverage chart
};

vector<minterm> primes; //the prime implicants genertaed after the tabulation method and to be used in the coverage chart

vector <minterm> mts; //list of all the terms, don't cares and minterms
int bits; //# of bits
vector<int> mt, dc; //these are minterms and don't cares
vector<string> minterms, dontcares; // minterms and don't cares as strings
vector<int> mtOnes, dcOnes; //number of ones in each binary number

string inttobin(int n){ //using bitmasking to get the binary representation of each number
    string s = "";
    while(n > 0){
        if(n & 1) s = '1' + s;
        else s = '0'+ s;
        n /= 2;
    }
    return s;
}
void read_input(string file) //funtion to read the txt file
{
    vector<string> lines; // a vector to contain each line
    ifstream mts;
    mts.open(file);
    if(!mts.is_open()) cout << "ERROR OPENING FILE \n"; // check if the file opened or not
    string line;

    while(getline(mts,line)){
        lines.push_back(line); //reading in each line
    }
    bits = stoi((lines[0])); //assignning the first line to the # of bits
    stringstream s(lines[1]);
    string temp;
    while( s >> temp){ //push back each minterm into a vector
        mt.push_back(stoi(temp));
    }
    stringstream dcs(lines[2]);
    while(dcs >> temp){// push back each don't care term into a vector
        dc.push_back(stoi(temp));
    }

    mts.close();
}
void binaryReps() {
    for (int i = 0; i < mt.size(); i++) { //turning the minterms into string to make sure the number of digits equals the number of variables AKA adding zeros on the left of the number 1 to have 3 digits 1 -> 001
        string temp = inttobin(mt[i]); //turning the decimal value of a number into a string representing the binary value
        bool length = 0;
        while (length == false) {
            if (temp.length() == bits) {
                length = true;
            } else temp = '0' + temp;
        }
        minterms.push_back(temp);
    }
    // generating binary representation for don't care values and turning them into strings, same procedure we just performed for the minterms
    for(int i = 0; i < dc.size(); i++){
        string temp = inttobin(dc[i]);
        bool length = 0;
        while(length == false){
            if(temp.length() == bits){
                length = true;
            }else temp = '0' + temp;
        }
        dontcares.push_back(temp);
    }
}

int generateOnes(string binary){  //function that counts the number 1's in each number
    int ones = 0;
    for(int j = 0; j < bits; j++){
        if(binary[j] == '1') ones++;
    }
    return ones;
}

void generateOnesforminterms(){ //a function that counts the ones for each binary value and puts them in vectors for minterms and don't cares
    for(int i = 0; i < minterms.size(); i++){
        int ones = generateOnes(minterms[i]);
        mtOnes.push_back(ones); //number of ones in each minterm value
    }
    for(int i = 0; i < dontcares.size(); i++){
        int ones = generateOnes(dontcares[i]);
        dcOnes.push_back(ones); //number of ones in each don't care value
    }
}

void fill_struct(){ //we're filling the vector of structs
    for(int i = 0; i < mt.size(); i++){
        struct minterm obj; //we create an instance and then push into the vector
        obj.decimal = mt[i];
        obj.binary = minterms[i];
        obj.dc = 0;
        obj.numof1s = mtOnes[i];
        mts.push_back(obj);
    }
    for(int i = 0; i < dc.size(); i++){ //push the don't cares into the same vector
        struct minterm obj;
        obj.decimal = dc[i];
        obj.binary = dontcares[i];
        obj.dc = 1;
        obj.numof1s = dcOnes[i];
        mts.push_back(obj);
    }
}
bool compareOnes(minterm one, minterm two){ //define a function to act as the comparator in the sorting function
    if(one.numof1s != two.numof1s) return one.numof1s < two.numof1s;
    if(one.numof1s == two.numof1s) return one.numof1s < two.numof1s;
}


void print(vector<minterm> pi){ //printing to check the vector
    cout << "Properties: Minterms" << setw(30) << "Binary" << setw(30) << "Number of Ones" << setw(30) << endl;
    for(int i = 0; i < pi.size(); i++){
        if(pi[i].decimal != -1) cout << setw(15) << pi[i].decimal << setw(35) << pi[i].binary << setw(28) << pi[i].numof1s << endl;
        else{
            cout << setw(5);
            for(int j = 0; j < pi[i].ms.size()-1; j++){
                cout << "m" << pi[i].ms[j].decimal << ", ";
            }

            cout << "m" << pi[i].ms[pi[i].ms.size()-1].decimal << setw(35) << pi[i].binary << setw(33) << pi[i].numof1s << endl;
        }
    }
}

bool compareMTs(minterm& a, minterm& b, minterm& c){ //function to compare minterms by 1 bit difference
    int diff = 0, index;
    for(int i = 0; i < bits; i++){
        if(a.binary[i] != b.binary[i]) {
            diff++;
            index = i;
        }
    }
    if(diff == 1){
        a.paired = true; b.paired = true; //making sure that these minterms have been paired w smth
        string temp = a.binary; temp[index] = '-';
        //cout << temp;
        c.binary = temp;
        if(a.ms.size() == 0 && b.ms.size() == 0){ //we are checking to see if the values that made the pi up are minterms or other pi's
            //if the size of the minterms that made the pi up is zero, that means it's a minterm, if not, THEN it's a prime implicant
            c.ms.push_back(a); c.ms.push_back(b);
        }else{
            for(int i = 0; i < a.ms.size(); i++){
                c.ms.push_back(a.ms[i]);
                c.ms.push_back(b.ms[i]);
            }
        } c.numof1s = generateOnes(c.binary);
        return true;
    }else return false;
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

void removeDuplicates(){
    set<string> pis; //we use the fact that the set does not accept values that it already has
    for(int i = 0; i < mts.size(); i++){
        int increase = pis.size(); //we check each element, if it is accepted into the set, then it is unique and we insert it
        pis.insert(mts[i].binary);
        if(pis.size() != increase && mts[i].paired == false) primes.push_back(mts[i]); //if it is unique AND has not been paired w another minterm or prime implicant, then it will be used in the coverage chart
    }
}

int main() {
    string file = R"(C:\Users\DELL\Documents\GitHub\DigitalDesignProj1-QuinnMclusky\minterms.txt)";
    read_input(file);
    binaryReps();
    generateOnesforminterms();
    fill_struct();
    sort(mts.begin(), mts.end(), compareOnes);
    tabulationMethod();
    //print(mts);
    removeDuplicates();
    print(primes);

    return 0;
}