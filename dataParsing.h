//
// Created by Rusty on 3/28/2026.
//

#pragma once

#include <iostream>
#include <string>
#include <vector>

struct Classes {
    int classId;
    int enrollment;
    int days;
    int startSlot;
    int length;

    Classes(const int classId, const int enrollment, const int days, const int startSlot, const int length) :
      classId(classId), enrollment(enrollment), days(days), startSlot(startSlot), length(length) {}

};

struct Rooms {
    int roomId;
    int capacity;
    std::string building;
    int x;
    int y;

    Rooms(const int roomId, const int capacity, const std::string building, int x, int y) : roomId(roomId),
        capacity(capacity), building(building), x(x), y(y) {}
};


std::vector<Classes> parseClassesCSV(std::string fileName);

std::vector<Rooms> parseRoomsCSV(std::string fileName);