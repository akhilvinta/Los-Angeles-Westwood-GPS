#include "provided.h"
#include <vector>
using namespace std;

class DeliveryOptimizerImpl
{
public:
    DeliveryOptimizerImpl(const StreetMap* sm);
    ~DeliveryOptimizerImpl();
    void optimizeDeliveryOrder(const GeoCoord& depot, vector<DeliveryRequest>& deliveries, double& oldCrowDistance, double& newCrowDistance) const;
    
private:
    const StreetMap* streetMap;
};

DeliveryOptimizerImpl::DeliveryOptimizerImpl(const StreetMap* sm): streetMap(sm){}

DeliveryOptimizerImpl::~DeliveryOptimizerImpl(){}

void DeliveryOptimizerImpl::optimizeDeliveryOrder(const GeoCoord& depot, vector<DeliveryRequest>& deliveries, double& oldCrowDistance, double& newCrowDistance) const
{
    if(deliveries.empty()) return;
    oldCrowDistance = 0; newCrowDistance = 0;
    
    
    GeoCoord start = depot;
    oldCrowDistance += distanceEarthMiles(start, deliveries[0].location);
    start = deliveries[0].location;
   
    for(int i = 1; i < deliveries.size(); i++){
        oldCrowDistance += distanceEarthMiles(start, deliveries[i].location);
        start = deliveries[i].location;
    }
    
    oldCrowDistance += distanceEarthMiles(deliveries[deliveries.size() - 1].location, depot);
    
    start = depot;
    vector<DeliveryRequest> temp;
    while(deliveries.size() != 0){
           std::sort (deliveries.begin(), deliveries.end(), [start](const DeliveryRequest s1, const DeliveryRequest s2){
                 return distanceEarthMiles(s1.location, start) < distanceEarthMiles(s2.location, start);
             });
            
            temp.push_back(deliveries[0]);
            newCrowDistance += distanceEarthMiles(start, deliveries[0].location);

            deliveries.erase(deliveries.begin());

        start = temp.back().location;
    }
    deliveries = temp;
    
    newCrowDistance += distanceEarthMiles(start, depot);

}

DeliveryOptimizer::DeliveryOptimizer(const StreetMap* sm)
{
    m_impl = new DeliveryOptimizerImpl(sm);
}

DeliveryOptimizer::~DeliveryOptimizer()
{
    delete m_impl;
}

void DeliveryOptimizer::optimizeDeliveryOrder(
        const GeoCoord& depot,
        vector<DeliveryRequest>& deliveries,
        double& oldCrowDistance,
        double& newCrowDistance) const
{
    return m_impl->optimizeDeliveryOrder(depot, deliveries, oldCrowDistance, newCrowDistance);
}
