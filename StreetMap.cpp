#include "provided.h"
#include <string>
#include <vector>
#include <functional>
#include "ExpandableHashMap.h"
#include <iostream>
#include <fstream>
#include <sstream>  // needed in addition to <iostream> for string stream I/O

using namespace std;

unsigned int hasher(const GeoCoord& g)
{
    return hash<string>()(g.latitudeText + g.longitudeText);
}

class StreetMapImpl
{
public:
    StreetMapImpl();
    ~StreetMapImpl();
    bool load(string mapFile);
    bool getSegmentsThatStartWith(const GeoCoord& gc, vector<StreetSegment>& segs) const;
    
private:
    ExapandableHashMap<GeoCoord,vector<StreetSegment>> mapData;
    
};

StreetMapImpl::StreetMapImpl(){}

StreetMapImpl::~StreetMapImpl(){}

bool StreetMapImpl::load(string mapFile)
{
    ifstream infile(mapFile);
      if (! infile )
          return false;
      

      int size = 0, num_segments;
      string s, segment_1, segment_2, segment_3, segment_4;
      string name;
      while (!infile.eof()){
          getline(infile, s);
          name = s;
          getline(infile, s);
          stringstream geek(s);
          geek >> num_segments;

          for(int i = 0; i < num_segments; i++){
              
              infile >> segment_1 >> segment_2 >> segment_3 >> segment_4;
              GeoCoord start(segment_1,segment_2);
              GeoCoord end(segment_3,segment_4);
              StreetSegment begin(start, end, name);
              StreetSegment last(end, start, name);
              
              vector<StreetSegment>* ptr_start = mapData.find(start);
              vector<StreetSegment>* ptr_end = mapData.find(end);

              if(ptr_start != nullptr)
                  ptr_start -> push_back(begin);
              else{
                  size++;
                  vector<StreetSegment> segments;
                  segments.push_back(begin);
                  mapData.associate(start, segments);
              }
              if(ptr_end != nullptr)
                  ptr_end -> push_back(last);
              else{
                  size++;
                  vector<StreetSegment> segments_reverse;
                  segments_reverse.push_back(last);
                  mapData.associate(end, segments_reverse);
              }

              getline(infile,s);
          }
          
      }
   // cout << "size = " << size << endl;
  //  mapData.printHashTable();
    return true;
}


bool StreetMapImpl::getSegmentsThatStartWith(const GeoCoord& gc, vector<StreetSegment>& segs) const
{
    
    const vector<StreetSegment>* ptr = mapData.find(gc);
    if(ptr == nullptr)
        return false;
    
    segs.clear();
    segs = *ptr;

    return true;
}

//******************** StreetMap functions ************************************

// These functions simply delegate to StreetMapImpl's functions.
// You probably don't want to change any of this code.

StreetMap::StreetMap()
{
    m_impl = new StreetMapImpl;
}

StreetMap::~StreetMap()
{
    delete m_impl;
}

bool StreetMap::load(string mapFile)
{
    return m_impl->load(mapFile);
}

bool StreetMap::getSegmentsThatStartWith(const GeoCoord& gc, vector<StreetSegment>& segs) const
{
   return m_impl->getSegmentsThatStartWith(gc, segs);
}


