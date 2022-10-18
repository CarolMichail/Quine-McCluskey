#include <iostream>
#include <string>
#include <bits/stdc++.h>
#include <vector>
#include <fstream>
using namespace std;

struct minterm{
    int decimal;
    vector<minterm> ms;
    bool paired = false;
    int numof1s;
    string binary;
    bool dc;
};

vector <minterm> mts;
int bits;
vector<int> mt, dc; //these are minterms and don't cares
vector<int> binaryMTs, binaryDCs; //binary rep as numbers
vector<string> minterms, dontcares; // minterms and don't cares as strings
vector<int> mtOnes, dcOnes; //number of ones in each binary number

void read_input(string file)
{
    vector<string> lines;

    ifstream mts;
    mts.open(file);
    if(!mts.is_open()) cout << "ERROR OPENING FILE \n";
    string line;
    while(getline(mts,line)){
        lines.push_back(line);
    }
    bits = stoi((lines[0]));
    for(int i = 0; i < lines[1].size(); i++){
        if(lines[1][i] != ' '){
            int temp = int(lines[1][i]) - 48;
            mt.push_back(temp);
        }
    }
    for(int i = 0; i < lines[2].size(); i++){
        if(lines[2][i] != ' '){
            int temp = int(lines[2][i]) - 48;
            dc.push_back(temp);
        }
    }
}
void binaryReps() {
    // generating binary representation for minterms values
    for (int i = 0; i < mt.size(); i++) {
        int decimal = mt[i];
        int bin = 0, product = 1;

        while (decimal != 0) {
            int rem = decimal % 2;
            bin += (rem * product);
            decimal /= 2;
            product *= 10;
        }
        binaryMTs.push_back((bin));
    }
    for (int i = 0; i < binaryMTs.size(); i++) {
        string temp = to_string(binaryMTs[i]);
        bool length = 0;
        while (length == false) {
            if (temp.length() == bits) {
                length = true;
            } else temp = '0' + temp;
        }
        minterms.push_back(temp);
    }
    // generating binary representation for don't care values
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
void generateOnes(){
    for(int i = 0; i < minterms.size(); i++){
        int ones = 0;
        for(int j = 0; j < bits; j++){
            if(minterms[i][j] == '1') ones++;
        }
        mtOnes.push_back(ones);
    }  cout << endl;
    for(int i = 0; i < dontcares.size(); i++){
        int ones = 0;
        for(int j = 0; j < bits; j++){
            if(dontcares[i][j] == '1') ones++;
        }
        dcOnes.push_back(ones);
    }
}
void fill_struct(){
    for(int i = 0; i < mt.size(); i++){
        struct minterm obj;
        obj.decimal = mt[i];
        obj.binary = minterms[i];
        obj.dc = 0;
        obj.numof1s = mtOnes[i];
        mts.push_back(obj);
    }
    for(int i = 0; i < dc.size(); i++){
        struct minterm obj;
        obj.decimal = dc[i];
        obj.binary = dontcares[i];
        obj.dc = 1;
        obj.numof1s = dcOnes[i];
        mts.push_back(obj);
    }
}
bool compareOnes(minterm one, minterm two){
    if(one.numof1s != two.numof1s) return one.numof1s < two.numof1s;
    if(one.numof1s == two.numof1s) return one.numof1s < two.numof1s;
}
void print(){
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
