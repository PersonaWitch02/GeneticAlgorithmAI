//
// Created by Rusty on 3/28/2026.
//

#include "dataParsing.h"

#include <fstream>
#include <sstream>

using namespace std;

//written to work with CSV with fields: ClassID,Enrollment,Days,StartSlot,Length
vector<Classes> parseClassesCSV(string fileName) {
    ifstream openedFile (fileName);

    if (!openedFile.is_open()) {
        cerr<<"FILE FAILED TO OPEN "<<fileName<<"\n";
    }

    string input;

    //getting rid of the column headers
    getline(openedFile, input);
    cout<<input<<endl;


    vector <Classes> classesVector;

    while (getline(openedFile, input)) {
        stringstream buffer(input);
        string field;
        vector<string> temp;

        //parsing the line taken from the file into field split by commas.
        while (getline(buffer, field, ',')) {
                temp.push_back(field);
        }

        //adding to vector
        classesVector.push_back(
            Classes(stoi(temp[0]),
                stoi(temp[1]),
                stoi(temp[2]),
                stoi(temp[3]),
                stoi(temp[4]))
        );
    }

    return classesVector;
}

//works with fields: RoomID,Capacity,Building,X,Y
//Building must be "x,y" to parse correctly
std::vector<Rooms> parseRoomsCSV(string fileName) {
    ifstream openedFile (fileName);

    if (!openedFile.is_open()) {
        cerr<<"FILE FAILED TO OPEN "<<fileName<<" \n";
    }

    string input;

    //getting rid of the column headers
    getline(openedFile, input);
    cout<<input<<endl;

    vector<Rooms> roomsVector;

    while (getline(openedFile, input)) {
        stringstream buffer(input);
        string field;
        vector<string> temp;

        //parsing the line taken from the file into field split by commas.
        while (getline(buffer, field, ',')) {
            temp.push_back(field);
        }

        roomsVector.push_back(
            Rooms(
                stoi(temp[0]),
                stoi(temp[1]),
                temp[2]+","+temp[3],
                stoi(temp[4]),
                stoi(temp[5]))
        );
    }

    return roomsVector;
}
