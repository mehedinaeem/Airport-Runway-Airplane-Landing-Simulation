#include "simulation.h"
#include <cstdlib>
#include <iostream>

using namespace airport;
void require(bool condition, const char* message) {
    if(!condition) { std::cerr << "FAIL: " << message << '\n'; std::exit(1); }
}
void near(double actual, double expected, const char* message) {
    require(std::abs(actual-expected)<1e-6,message);
}
int main() {
    Point p=transform({2,3},{10,20},2,90);
    near(p.x,4,"rotation, scaling and translation x");
    near(p.y,24,"rotation, scaling and translation y");
    for(double rate : {0.25,1.0,4.0}) {
        Simulation s;
        s.rate=rate;
        s.update(0.5);
        near(s.x,100,"waiting aircraft stays in place");
        s.start();
        s.paused=true;
        double clock=s.clock;
        s.update(1);
        near(s.clock,clock,"pause freezes scene clock");
        near(s.flightTime,0,"pause freezes aircraft");
        s.paused=false;
        bool visited[7]={true,false,false,false,false,false,false};
        double previousX=s.x, previousY=s.y, previousSpeed=1e9;
        int count=0;
        while(s.state!=LandingState::LANDED && count++<20000) {
            s.update(count%2 ? 0.02 : 0.037);
            visited[static_cast<int>(s.state)]=true;
            require(s.x>=previousX-1e-7,"forward movement is monotonic");
            require(s.y>=previousY-1e-7,"descent is monotonic");
            require(s.wheelHeight()<=WHEEL_Y+1e-6,"wheels never penetrate runway");
            if(s.state==LandingState::TOUCHDOWN || s.state==LandingState::BRAKING) {
                near(s.wheelHeight(),WHEEL_Y,"wheel contact maintained on rollout");
                near(s.gear,1,"gear fully extended on runway");
            }
            if(s.state==LandingState::BRAKING) {
                require(s.velocity<=previousSpeed+1e-7,"braking speed decreases");
                previousSpeed=s.velocity;
            }
            previousX=s.x; previousY=s.y;
        }
        for(bool seen:visited) require(seen,"all landing stages visited");
        require(s.state==LandingState::LANDED,"landing completes");
        near(s.x,STOP_X,"exact stopping position");
        near(s.velocity,0,"zero velocity when parked");
        near(s.altitude(),0,"zero altitude when parked");
        s.update(10); near(s.x,STOP_X,"parked plane stays stopped");
        s.night=true; s.paused=true; s.restart();
        require(s.state==LandingState::WAITING && !s.paused,"restart restores waiting and unpauses");
        require(s.night,"restart preserves day/night preference");
        near(s.rate,rate,"restart preserves simulation rate");
    }
    Simulation s;
    for(int i=0;i<100;++i) s.changeRate(1.25);
    near(s.rate,4,"upper rate bound");
    for(int i=0;i<100;++i) s.changeRate(0.8);
    near(s.rate,0.25,"lower rate bound");
    std::cout << "PASS: transforms, stages, contact, braking, pause, restart, rate bounds\n";
}
