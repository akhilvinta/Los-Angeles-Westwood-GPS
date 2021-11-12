#include "provided.h"
#include "ExpandableHashMap.h"
#include <list>
#include <queue>
#include <string>
#include <functional>
#include <iostream>

using namespace std;

class PointToPointRouterImpl
{
public:
    PointToPointRouterImpl(const StreetMap* sm);
    ~PointToPointRouterImpl();
    DeliveryResult generatePointToPointRoute(
        const GeoCoord& start,
        const GeoCoord& end,
        list<StreetSegment>& m_route,
        double& totalDistanceTravelled) const;
    
    
private:
    
    const StreetMap* streetMap;
    
};

PointToPointRouterImpl::PointToPointRouterImpl(const StreetMap* sm): streetMap(sm){}
PointToPointRouterImpl::~PointToPointRouterImpl(){}

void printStats(StreetSegment* s){
   std:: cout << "start == (" << s->start.latitudeText << "," << s->start.longitudeText << ") --- end == (" << s->end.latitudeText << "," << s->end.longitudeText << ") --- name = " << s->name << std::endl;
}


DeliveryResult PointToPointRouterImpl::generatePointToPointRoute(const GeoCoord& start, const GeoCoord& end,
        list<StreetSegment>& route, double& totalDistanceTravelled) const {
    
    
    ExapandableHashMap<GeoCoord, bool> didVisit;
    ExapandableHashMap<GeoCoord, GeoCoord> previousCoord; //came to first param from second param
    vector<StreetSegment> getSegs; queue<GeoCoord> queue;
    totalDistanceTravelled = 0;

    if(start == end){
        route.clear();
        return NO_ROUTE;
    }
    if((!streetMap -> getSegmentsThatStartWith(start, getSegs)) || (!streetMap -> getSegmentsThatStartWith(end, getSegs))) return BAD_COORD;
        
    queue.push(start);
    while(!queue.empty()){
        GeoCoord cur = queue.front(); queue.pop();
        didVisit.associate(cur, false);
        
        if(cur == end){
            GeoCoord s = cur;
            vector<StreetSegment> getName; string name_for_seg;
            
            while(s != start){
                
                GeoCoord s_temp = s;
                GeoCoord* geo_ptr = previousCoord.find(s);
                s = *geo_ptr;
                
                totalDistanceTravelled += distanceEarthMiles(s_temp, s);

                this->streetMap->getSegmentsThatStartWith(s, getName);
                for(int i = 0; i < getName.size(); i++)
                    if(getName[i].end == s_temp)
                        name_for_seg = getName[i].name;
                
                route.push_back(StreetSegment(s,s_temp, name_for_seg));
            }

            route.reverse();
            return DELIVERY_SUCCESS;
        }
        
        streetMap -> getSegmentsThatStartWith(cur, getSegs);
        for(int i = 0; i < getSegs.size(); i++){
            bool* is_false_ptr = didVisit.find(getSegs[i].end);
            if(is_false_ptr == nullptr || *is_false_ptr == true){
                previousCoord.associate(getSegs[i].end, cur);
                queue.push(getSegs[i].end);
                didVisit.associate(getSegs[i].end, false);
            }
        }
        
    }
    return NO_ROUTE;
}

//******************** PointToPointRouter functions ***************************

// These functions simply delegate to PointToPointRouterImpl's functions.
// You probably don't want to change any of this code.

PointToPointRouter::PointToPointRouter(const StreetMap* sm)
{
    m_impl = new PointToPointRouterImpl(sm);
}

PointToPointRouter::~PointToPointRouter()
{
    delete m_impl;
}

DeliveryResult PointToPointRouter::generatePointToPointRoute(
        const GeoCoord& start,
        const GeoCoord& end,
        list<StreetSegment>& route,
        double& totalDistanceTravelled) const
{
    return m_impl->generatePointToPointRoute(start, end, route, totalDistanceTravelled);
}


int main(int argc, const char * argv[]) {

    StreetMap s;
    StreetMap* map_ptr = &s;
    list<StreetSegment> route; double totalDistanceTravelled = 5;
    map_ptr -> load("/Users/akhil/Desktop/mapdata copy.txt");
    GeoCoord start("34.0760230", "-118.4679042");
    GeoCoord end("34.0693690","-118.4878280");
    PointToPointRouter p(map_ptr);
    DeliveryResult d = p.generatePointToPointRoute(start, end, route, totalDistanceTravelled);
    if(d != 0) cout << d << endl;
    
    list<StreetSegment>::iterator it;

    for (it = route.begin(); it != route.end(); it++){
        StreetSegment temp = *it;
        printSegStats(&temp);
    }
    cout << totalDistanceTravelled << " miles travelled" << endl;
    
}

