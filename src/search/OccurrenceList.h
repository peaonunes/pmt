//
//  OccurrenceList.hpp
//  PCC
//
//  Created by Rafael Nunes G. da Silveira on 24/10/2015.
//  Copyright © 2015 Rafael Nunes. All rights reserved.
//

#ifndef OccurrenceList_hpp
#define OccurrenceList_hpp

#include <string>

class OccurrenceListNode {
public:
    std::string occurrence;
    OccurrenceListNode* next;
    OccurrenceListNode(std::string occurrence);
};

class OccurrenceList {
public:
    OccurrenceListNode *head;
    OccurrenceListNode *tail;

    OccurrenceList();
    void push_front(std::string occurrence);
    void push_back(OccurrenceList *list);
    bool empty();
};

#endif /* OccurrenceList_hpp */
