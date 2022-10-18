#include <iostream>
#include <string>
#include <bits/stdc++.h>
#include <vector>
#include <fstream>
using namespace std;

struct minterm{
    int decimal; // the decimal representation whether
    vector<minterm> ms; //if it's a reduced prime implicant (ex: 1--1), this saves the minterms that made it up
    bool paired = false; //in the tabulation method, it helps us determine the prime implicants
    int numof1s; //number of ones in each binary number
    string binary; //binary representation of the minterm
    bool dc; //determine whether it's a don't care or not, we only need to determine it for when we creat ethe coverage chart
};

vector <minterm> mts; //list of all the terms, don't cares and minterms
int bits; //# of bits
vector<int> mt, dc; //these are minterms and don't cares
vector<int> binaryMTs, binaryDCs; //binary rep as numbers
vector<string> minterms, dontcares; // minterms and don't cares as strings
vector<int> mtOnes, dcOnes; //number of ones in each binary number

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
    for(int i = 0; i < lines[1].size(); i++){ //reading in the minterms in the 2nd line
        if(lines[1][i] != ' '){
            int temp = int(lines[1][i]) - 48;
            mt.push_back(temp);
        }
    }
    for(int i = 0; i < lines[2].size(); i++){ //reading the don't care terms in the 3rd line and putiing them as ints in a vector
        if(lines[2][i] != ' '){
            int temp = int(lines[2][i]) - 48;
            dc.push_back(temp);
        }
    }
}
void binaryReps() {
    // generating binary representation for minterms values
    for (int i = 0; i < mt.size(); i++) {
        int decimal = mt[i]; //takes in the value of the minterm
        int bin = 0, product = 1;
        while (decimal != 0) { //binary conversion
            int rem = decimal % 2;
            bin += (rem * product);
            decimal /= 2;
            product *= 10;
        }
        binaryMTs.push_back((bin)); //push them back in a vector containing the binary representation of each minterm
    }
    for (int i = 0; i < binaryMTs.size(); i++) { //turning the minterms into string to make sure the number of digits equals the number of variables
        string temp = to_string(binaryMTs[i]); //AKA adding zeros on the left of the number 1 to have 3 digits 1 -> 001
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
        int decimal = dc[i];
        int bin = 0, product = 1;
        while(decimal != 0){
            int rem = decimal%2;
            bin += (rem * product);
            decimal /= 2;
            product *= 10;
        }
        binaryDCs.push_back((bin));
    }

    for(int i = 0; i < binaryDCs.size(); i++){
        string temp = to_string(binaryDCs[i]);
        bool length = 0;
        while(length == false){
            if(temp.length() == bits){
                length = true;
            }else temp = '0' + temp;
        }
        dontcares.push_back(temp);
    }
}
void generateOnes(){ //a function that counts the ones for each binary value and puts them in vectors for minterms and don't cares
    for(int i = 0; i < minterms.size(); i++){
        int ones = 0;
        for(int j = 0; j < bits; j++){
            if(minterms[i][j] == '1') ones++;
        }
        mtOnes.push_back(ones); //number of ones in each minterm value
    }  cout << endl;
    for(int i = 0; i < dontcares.size(); i++){
        int ones = 0;
        for(int j = 0; j < bits; j++){
            if(dontcares[i][j] == '1') ones++;
        }
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
void print(){ //printing to check the vector
    for(int i = 0; i < mts.size(); i++){
        cout << "Properties: " << mts[i].decimal << " " << mts[i].binary << " " << mts[i].numof1s << endl;
    }
}

int main() {
    string file = R"(C:\Users\DELL\Documents\GitHub\DigitalDesignProj1-QuinnMclusky\minterms.txt)";
    read_input(file);
    binaryReps();
    generateOnes();
    fill_struct();
    sort(mts.begin(), mts.end(), compareOnes); 
    //print();

    return 0;
}
