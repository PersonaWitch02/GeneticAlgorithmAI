#include "CSVParser.h"
#include <fstream>
#include <sstream>
#include <iostream>

std::vector<Class> loadClasses(const std::string &filename) {
    std::vector<Class> classes;
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open " << filename << std::endl;
        return classes;
    }

    std::string line;
    std::getline(file, line); //skip Heder
    while (std::getline(file, line)) {
        if (line.empty()) continue;
        std::stringstream ss(line);
        std::string item;
        Class c;

        std::getline(ss, item, ',');
        c.id = std::stoi(item);
        std::getline(ss, item, ',');
        c.enrollment = std::stoi(item);
        std::getline(ss, c.days, ',');
        std::getline(ss, item, ',');
        c.startSlot = std::stoi(item);
        std::getline(ss, item, ',');
        c.length = std::stoi(item);

        classes.push_back(c);
    }
    return classes;
}

std::vector<Room> loadRooms(const std::string &filename) {
    std::vector<Room> rooms;
    std::ifstream file(filename);
    if (!file.is_open()) return rooms;

    std::string line;
    std::getline(file, line); // skip header
    while (std::getline(file, line)) {
        if (line.empty()) continue; //skip trailing newlines
        std::stringstream ss(line);
        std::string item;
        Room r;

        std::getline(ss, item, ',');
        r.id = std::stoi(item);
        std::getline(ss, item, ',');
        r.capacity = std::stoi(item);

        //Handle the building string with quotes
        if (ss.peek() == '"') {
            ss.get(); // remove first quote
            std::getline(ss, item, '"'); // read until second quote
            std::getline(ss, item, ','); // consume the following comma
        } else {
            std::getline(ss, item, ',');
        }

        std::getline(ss, item, ',');
        r.x = std::stoi(item);
        std::getline(ss, item, ',');
        r.y = std::stoi(item);
        rooms.push_back(r);
    }
    return rooms;
}
