#include "provided.h"
#include "ExpandableHashMap.h"
#include <list>
#include <queue>
#include <string>
#include <functional>
#include <iostream>
#include <limits>

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
    vector<StreetSegment> optimizeRoute(vector<StreetSegment> path, GeoCoord end) const;
};

PointToPointRouterImpl::PointToPointRouterImpl(const StreetMap* sm): streetMap(sm){}
PointToPointRouterImpl::~PointToPointRouterImpl(){}

vector<StreetSegment> PointToPointRouterImpl :: optimizeRoute(vector<StreetSegment> path, GeoCoord end) const{
   
    if(path.size() <= 1) return path;

    std::sort (path.begin(), path.end(), [end](const StreetSegment s1, const StreetSegment s2){
        return distanceEarthMiles(s1.end, end) < distanceEarthMiles(s2.end, end);
    });
    
    return path;
  
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
        
    queue.push(start);  int counter = 1;
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

        getSegs = optimizeRoute(getSegs, end);

        
        for(int i = 0; i < getSegs.size(); i++){
            bool* is_false_ptr = didVisit.find(getSegs[i].end);
            if(is_false_ptr == nullptr || *is_false_ptr == true){
                previousCoord.associate(getSegs[i].end, cur);
                queue.push(getSegs[i].end);
                didVisit.associate(getSegs[i].end, false);
            }
        }
        counter++;
    }
    return NO_ROUTE;
}


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
