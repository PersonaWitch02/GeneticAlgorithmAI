#include "CSVParser.h"
#include <fstream>
#include <sstream>

std::vector<Class> loadClasses(const std::string& filename) {
    std::vector<Class> classes;
    std::ifstream file(filename);
    std::string line;
    std::getline(file, line); // skip header
    while (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string item;
        Class c;
        std::getline(ss, item, ','); c.id = std::stoi(item);
        std::getline(ss, item, ','); c.enrollment = std::stoi(item);
        std::getline(ss, c.days, ',');
        std::getline(ss, item, ','); c.startSlot = std::stoi(item);
        std::getline(ss, item, ','); c.length = std::stoi(item);
        classes.push_back(c);
    }
    return classes;
}

std::vector<Room> loadRooms(const std::string& filename) {
    std::vector<Room> rooms;
    std::ifstream file(filename);
    std::string line;
    std::getline(file, line); // skip header
    while (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string item;
        Room r;
        std::getline(ss, item, ','); r.id = std::stoi(item);
        std::getline(ss, item, ','); r.capacity = std::stoi(item);
        std::getline(ss, item, '"'); // handle the building string with quotes
        std::getline(ss, item, '"');
        std::getline(ss, item, ','); // skip comma
        std::getline(ss, item, ','); r.x = std::stoi(item);
        std::getline(ss, item, ','); r.y = std::stoi(item);
        rooms.push_back(r);
    }
    return rooms;
}