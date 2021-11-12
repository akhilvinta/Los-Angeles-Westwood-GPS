#include "provided.h"
#include <vector>

using namespace std;

class DeliveryPlannerImpl
{
public:
    DeliveryPlannerImpl(const StreetMap* sm);
    ~DeliveryPlannerImpl();
    DeliveryResult generateDeliveryPlan(const GeoCoord& depot, const vector<DeliveryRequest>& deliveries, vector<DeliveryCommand>& commands, double& totalDistanceTravelled) const;
    
private:
     const StreetMap* streetMap;
    string convertAngleProceed(double angle) const;
    string convertAngleTurn(double angle) const;
    double SingleDelivery(GeoCoord start, GeoCoord end, vector<DeliveryCommand>& commands, string item) const;
};

DeliveryPlannerImpl::DeliveryPlannerImpl(const StreetMap* sm): streetMap(sm){}

DeliveryPlannerImpl::~DeliveryPlannerImpl(){}

void printSegStats(StreetSegment* s){
   std:: cout << "start == (" << s->start.latitudeText << "," << s->start.longitudeText << ") --- end == (" << s->end.latitudeText << "," << s->end.longitudeText << ") --- name = " << s->name << std::endl;
}

string DeliveryPlannerImpl:: convertAngleProceed(double angle) const{
    if (0 <= angle  && angle < 22.5) return "east";
    if (22.5 <= angle && angle < 67.5) return "northeast";
    if (67.5 <= angle && angle < 112.5) return "north";
    if (112.5 <= angle && angle < 157.5) return "northwest";
    if (157.5 <= angle && angle < 202.5) return "west";
    if (202.5 <= angle && angle < 247.5) return "southwest";
    if (247.5 <= angle && angle < 292.5) return "south";
    if (292.5 <= angle && angle < 337.5) return "southeast";
    if(angle >= 337.5) return "east";
    
    return "";
}

string DeliveryPlannerImpl:: convertAngleTurn(double angle) const{
    
    if(angle < 1 || angle > 359) return "";
    if(angle >= 1 && angle < 180) return "left";
    else return "right";
}


double DeliveryPlannerImpl :: SingleDelivery(GeoCoord start, GeoCoord end, vector<DeliveryCommand>& commands, string item) const{

    double dist; double cur_angle;

      PointToPointRouter router(this->streetMap); list<StreetSegment> route;
      DeliveryResult result = router.generatePointToPointRoute(start,end, route, dist);
      if(result != DELIVERY_SUCCESS)
          return result;
      

      list<StreetSegment>::iterator it = route.begin();
          StreetSegment temp = *it;
    //      printSegStats(&temp);
          cur_angle = angleOfLine(temp);
          string angle_as_string = convertAngleProceed(cur_angle);
          DeliveryCommand first_command;
          first_command.initAsProceedCommand(angle_as_string, temp.name, distanceEarthMiles(temp.start, temp.end));
          commands.push_back(first_command);
          it++;
      for (; it != route.end(); it++){
          
          StreetSegment temp2 = *it;
       //     printSegStats(&temp2);
          
          if(temp2.name == commands.back().streetName()){
                  //temp2 streetname is different from list.back streetname
                  commands.back().increaseDistance(distanceEarthMiles(temp2.start, temp2.end));
          }
          else{
              //temp2 streetname is different from list.back streetname
              cur_angle = angleBetween2Lines(temp, temp2);
              angle_as_string = convertAngleTurn(cur_angle);
              
              if(angle_as_string != ""){
                  DeliveryCommand turn;
                  turn.initAsTurnCommand(angle_as_string, temp2.name);
                  commands.push_back(turn);
              }
              
              cur_angle = angleOfLine(temp2);
              angle_as_string = convertAngleProceed(cur_angle);

              DeliveryCommand proceed;
              proceed.initAsProceedCommand(angle_as_string, temp2.name, distanceEarthMiles(temp2.start, temp2.end));
              commands.push_back(proceed);

          }
          
          temp = temp2;
        }
      /*
      for(int i = 0; i < commands.size(); i++){
         cout << commands[i].description() << endl;
      }*/
    // cout << "total distance travelled: " << totalDistanceTravelled << " miles" << endl;
      
    DeliveryCommand deliver;
    deliver.initAsDeliverCommand(item);
    commands.push_back(deliver);
    
    return dist;
    
}



DeliveryResult DeliveryPlannerImpl::generateDeliveryPlan(const GeoCoord& depot, const vector<DeliveryRequest>& deliveries, vector<DeliveryCommand>& commands, double& totalDistanceTravelled) const {
    
    if(deliveries.empty()) return NO_ROUTE;
    GeoCoord start = depot;

    totalDistanceTravelled = 0;
    GeoCoord finish = deliveries[0].location;
    
    double dist = SingleDelivery(depot, finish, commands, deliveries[0].item); totalDistanceTravelled += dist;
    if(dist == 1) return NO_ROUTE; if(dist == 2) return BAD_COORD;

    for(int i = 1; i < deliveries.size(); i++){
        start = finish;
        finish = deliveries[i].location;
        dist = SingleDelivery(start, finish, commands, deliveries[i].item); totalDistanceTravelled += dist;
        if(dist == 1) return NO_ROUTE; if(dist == 2) return BAD_COORD;
    }
    
    
    for(int i = 0; i < commands.size(); i++){
       cout << commands[i].description() << endl;
    }
    

    cout << "total distance travelled = " << totalDistanceTravelled << " miles" << endl;

    return DELIVERY_SUCCESS;  // Delete this line and implement this function correctly

}

//******************** DeliveryPlanner functions ******************************

// These functions simply delegate to DeliveryPlannerImpl's functions.
// You probably don't want to change any of this code.

DeliveryPlanner::DeliveryPlanner(const StreetMap* sm)
{
    m_impl = new DeliveryPlannerImpl(sm);
}

DeliveryPlanner::~DeliveryPlanner()
{
    delete m_impl;
}

DeliveryResult DeliveryPlanner::generateDeliveryPlan(
    const GeoCoord& depot,
    const vector<DeliveryRequest>& deliveries,
    vector<DeliveryCommand>& commands,
    double& totalDistanceTravelled) const
{
    return m_impl->generateDeliveryPlan(depot, deliveries, commands, totalDistanceTravelled);
}



int main(int argc, const char * argv[]) {

    StreetMap s;
    StreetMap* map_ptr = &s;
    DeliveryPlanner planner(map_ptr);
    map_ptr -> load("/Users/akhil/Desktop/mapdata copy.txt");
    GeoCoord depot("34.0555267", "-118.4796954");
    vector<DeliveryRequest> requests;
    requests.push_back(DeliveryRequest("sardines", GeoCoord("34.0519203", "-118.4780465")));
    requests.push_back(DeliveryRequest("DICK", GeoCoord("34.0560908", "-118.4805176")));

    vector<DeliveryCommand> commands;
    double totalDistanceTravelled;
    cout << planner.generateDeliveryPlan(depot, requests, commands, totalDistanceTravelled) << endl;
    
    
}


