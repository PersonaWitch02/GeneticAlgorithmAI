#include <iostream>
#include "dataParsing.h"

using namespace std;

int main() {


    vector<Classes> classesVector = parseClassesCSV("classes_demand.csv");

    for (auto classObject : classesVector) {
        cout<<"Class ID: "<<classObject.classId << " Endrollment: "<<classObject.enrollment
        <<" Days: "<<classObject.days<<" StartSlot: "<<classObject.startSlot<<" Length: "<<
            classObject.length<<endl;
    }

    vector <Rooms> roomsVector = parseRoomsCSV("rooms_pool.csv");

    for (auto room : roomsVector) {
        cout<<"Room ID: "<< room.roomId<<", Capacity: "<<room.capacity<<", Building: "<<room.building
        <<", X: "<<room.x<<", Y: "<<room.y<<endl;

        }
}