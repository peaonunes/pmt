//
//  aho-corasick.cpp
//  PCC
//
//  Created by Rafael Nunes G. da Silveira on 23/10/2015.
//  Copyright © 2015 Rafael Nunes. All rights reserved.
//

#include "AhoCorasick.h"
#include "../input/FileReader.h"
#include "Occurrence.h"
#include <string>
#include <vector>
#include <queue>
#include <iostream>

using namespace std;

AhoCorasick::AhoCorasick() {
}

AhoCorasick::~AhoCorasick() {
}

Node::Node(){
}

Node* AhoCorasick::build_goto(vector<string> patterns){
  Node *firstNode = new Node();
  Node *currentNode; // sail with it

  int j; // index
  int m; // pattern length
  char currentChar;// current char of the pattern

  unordered_map<char, Node*>::const_iterator iterator; // auxiliar iterator
  std::pair<char, Node*> newTransition; // new transition

  for (int k = 0; k < patterns.size(); k++) {
    currentNode = firstNode;
    j = 0;
    m = patterns[k].size();

    currentChar = patterns[k].at(0);

    while ((j < m) && ((iterator = currentNode->transitions.find(currentChar)) != currentNode->transitions.end())) {
      currentChar = patterns[k].at(j);
      currentNode = iterator->second;
      ++j;
    }

    while (j < m) {
      currentChar = patterns[k].at(j);
      newTransition = make_pair(currentChar, new Node()); // create the new transition
      currentNode->transitions.insert(newTransition); // add the transition to the currentNode map of transitions
      currentNode = newTransition.second; // update the currentNode
      j++;
    }

    currentNode->ocurrencies.push_front(patterns[k]); // Add to this node its ocurrencies
  }

  // Adding empty transitions for all chars that are not in the patterns
  for (int i = 0 ; i < 256; i++) {
    if ((iterator = firstNode->transitions.find((char)i)) == firstNode->transitions.end()) {
      newTransition = make_pair((char)i, firstNode);
      firstNode->transitions.insert(newTransition);
    }
  }

  return firstNode;
}


Node* AhoCorasick::build_fail(Node* firstNode){
  queue<Node*> auxQueue;

  unordered_map<char, Node*>::const_iterator iterator; // auxiliar iterator
  std::pair<char, Node*> newTransition; // new transition

  for (int i = 0; i < 256; i++) {
    if ((iterator = firstNode->transitions.find((char)i)) != firstNode->transitions.end()) {
      if (firstNode != iterator->second) { // If its not the firstNode we enqueue
        auxQueue.push(iterator->second);
        iterator->second->fail = firstNode;
      }
    }
  }

  Node *currentNode;
  Node *nextNode;
  Node *failNode;

  while (!auxQueue.empty()) {
    currentNode = auxQueue.front();
    auxQueue.pop();

    for (int i = 0; i < 256; i++) {
      if ((iterator = currentNode->transitions.find((char)i)) != currentNode->transitions.end()){
        nextNode = iterator->second;
        auxQueue.push(nextNode);

        failNode = currentNode->fail;

        while ((iterator = failNode->transitions.find((char)i)) == failNode->transitions.end()) {
          failNode = failNode->fail;
        }

        nextNode->fail = failNode->transitions.at((char)i);

        nextNode->ocurrencies.push_back(&nextNode->fail->ocurrencies);
      }
    }
  }

  return firstNode;
}

vector<OccurrenceMultiplePatterns> AhoCorasick::search(vector<string> patterns, char *inputFile) {
  FileReader fr(inputFile);
  //Build goto and fail transitions for the automata
  Node *firstNode = build_goto(patterns);
  build_fail(firstNode);
  // auxiliar iterator for map operations
  unordered_map<char, Node*>::const_iterator iterator;
  Node *currentNode = firstNode;
  vector<OccurrenceMultiplePatterns> result;
  string buffer;

  while (fr.hasContent()) {
    buffer = fr.readAsString();

    if (fr.bufferSize) {
      for (int i = 0; i < buffer.size(); i++) {
        while ((iterator = currentNode->transitions.find(buffer[i])) == currentNode->transitions.end()) {
          currentNode = currentNode->fail;
        }

        currentNode = iterator->second;

        OccurrenceListNode *currentOcc = currentNode->ocurrencies.head;

        while (currentOcc!=NULL) {
          result.push_back(OccurrenceMultiplePatterns(
            fr.lineCount,
            (i - currentOcc->occurrence.size() + 1),
            currentOcc->occurrence
          ));
          currentOcc = currentOcc->next;
        }
      }
    }
  }

  delete firstNode;

  return result;
}
