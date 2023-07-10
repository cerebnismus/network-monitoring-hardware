/**
 * @file Test.cpp
 * @description Main Function
 * @author oguzhan.ince@protonmail.com
 */

#include <iostream>
#include <fstream>
#include <String.h>
#include "test.hpp"

using namespace std;
int main()
{
    string line;              // line value from file
    ifstream file;            // for open file
    file.open("iplist.txt");  // open file
    getline(file, line);
    DoublyLinkedList *listone = new DoublyLinkedList();
    DoublyLinkedList *listwo = new DoublyLinkedList();
    if (line[line.length()] < 16) // max length of ip
    {
      // send icmp packet to ip in 'line'
    }
}