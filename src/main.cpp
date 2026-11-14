// Airport Runway & Airplane Landing Simulation -- WinBGIm / C++11.
#include <graphics.h>
// Some Windows headers define these macros; they interfere with standard C++.
#ifdef min
#undef min
#endif
#ifdef max
#undef max
#endif
#include "simulation.h"
#include <chrono>
#include <cstdio>
#include <initializer_list>
#include <string>
#include <thread>
#include <vector>
#include "display.h"

namespace airport {
int rgb(int r, int g, int b) { return COLOR(r, g, b); }
int blend(int a, int b, double t) { return static_cast<int>(lerp(a, b, t)); }
void box(int x1, int y1, int x2, int y2, int color) {
    setfillstyle(SOLID_FILL, color); bar(x1, y1, x2, y2);
}
void stroke(int x1, int y1, int x2, int y2, int color) {
    setcolor(color); line(x1, y1, x2, y2);
}
void disk(int x, int y, int radius, int color) {
    setcolor(color); setfillstyle(SOLID_FILL, color); fillellipse(x,y,radius,radius);
}
void oval(int x, int y, int rx, int ry, int color) {
    setcolor(color); setfillstyle(SOLID_FILL, color); fillellipse(x,y,rx,ry);
}
void poly(std::initializer_list<Point> points, int color, int outline = -1) {
    std::vector<int> vertices;
    for (Point p : points) { vertices.push_back(static_cast<int>(std::lround(p.x))); vertices.push_back(static_cast<int>(std::lround(p.y))); }
    setcolor(outline < 0 ? color : outline); setfillstyle(SOLID_FILL, color);
    fillpoly(static_cast<int>(points.size()), vertices.data());
}
void text(int x, int y, const std::string& value, int color, int size = 1, int background = -1) {
    settextstyle(DEFAULT_FONT, HORIZ_DIR, size); setcolor(color);
    if (background >= 0) setbkcolor(background);
    // Older WinBGIm releases declare outtextxy's text argument as mutable.
    std::vector<char> buffer(value.begin(), value.end()); buffer.push_back('\0');
    outtextxy(x, y, buffer.data());
}

class Renderer {
    const Simulation& sim;
    const int ink = rgb(221,235,241);
    const int muted = rgb(135,160,178);
    const int panel = rgb(14,30,46);
    const int cyan = rgb(79,215,222);
public:
    explicit Renderer(const Simulation& state) : sim(state) {}
    void drawSky() const {
        for (int y = 0; y < 405; ++y) {
            const double t = y / 405.0;
            setcolor(sim.night ? rgb(blend(8,37,t),blend(16,49,t),blend(37,71,t)) :
                                rgb(blend(75,197,t),blend(151,224,t),blend(200,230,t)));
            line(0,y,WIDTH-1,y);
        }
        if (sim.night) {
            for (int i = 0; i < 64; ++i) {
                const int x = (i*173+41) % WIDTH, y = 85+(i*97)%210;
                putpixel(x,y, i%3 ? rgb(128,154,181) : ink);
            }
            disk(1010,144,31,rgb(230,235,216));
            disk(1022,134,29,rgb(18,29,49));
        } else {
            disk(1010,147,44,rgb(239,216,146));
            disk(1010,147,32,rgb(255,238,178));
        }
        for (int i = 0; i < 5; ++i) {
            const double drift = std::fmod(sim.clock*(5+i*0.7)+i*277.0, 1430.0)-115;
            drawCloud(drift, 125+(i%3)*49, 0.65+(i%2)*0.3);
        }
        // Deterministic distant skyline, no random flicker between frames.
        for (int i=0; i<47; ++i) {
            const int x=i*28, h=16+(i*37)%48;
            box(x,396-h,x+20,408,sim.night ? rgb(33,53,65) : rgb(124,166,170));
            if (sim.night && i%3==0) box(x+6,403-h,x+9,408-h,rgb(190,158,90));
        }
    }
    void drawCloud(double x, double y, double s) const {
        const int c=sim.night ? rgb(49,65,86) : rgb(228,242,244);
        oval(static_cast<int>(x),static_cast<int>(y),static_cast<int>(62*s),static_cast<int>(15*s),c);
        oval(static_cast<int>(x-23*s),static_cast<int>(y-10*s),static_cast<int>(25*s),static_cast<int>(19*s),c);
        oval(static_cast<int>(x+10*s),static_cast<int>(y-18*s),static_cast<int>(29*s),static_cast<int>(25*s),c);
    }
    void drawGround() const {
        box(0,402,1199,699,sim.night ? rgb(24,49,46) : rgb(91,133,99));
        box(0,412,1199,448,sim.night ? rgb(40,58,63) : rgb(142,155,148));
        // Taxiway from terminal gate to the stopping area.
        poly({{885,429},{956,429},{1105,617},{989,617}},sim.night ? rgb(52,64,67) : rgb(111,125,124));
        stroke(922,435,1036,600,rgb(215,182,90));
        for (int i=0; i<13; ++i) {
            int x=17+i*98;
            box(x,394,x+4,413,rgb(71,84,66));
            disk(x+2,388,13,sim.night ? rgb(28,65,58) : rgb(62,114,85));
        }
    }
    void drawTerminal() const {
        const int concrete=sim.night ? rgb(71,91,106) : rgb(192,207,207);
        const int glass=sim.night ? rgb(221,193,117) : rgb(67,126,149);
        box(715,332,1078,414,concrete);
        box(705,320,1088,333,rgb(59,84,102));
        for (int x=731; x<1065; x+=30) {
            box(x,346,x+22,376,glass);
            stroke(x,361,x+22,361,sim.night ? rgb(139,133,98) : rgb(133,177,184));
            box(x,388,x+22,405,rgb(51,76,91));
        }
        text(798,325,"AEROPORT  /  TERMINAL 01",ink,1,rgb(59,84,102));
        // Jet bridges and gate labels.
        for (int x=778; x<=988; x+=105) {
            box(x,405,x+17,435,rgb(110,132,142));
            box(x-8,429,x+30,439,rgb(149,167,171));
            text(x,391,"G",ink,1,rgb(51,76,91));
        }
    }
    void drawRadar(double degrees) const {
        const double a=degrees*PI/180.0;
        const int cx=625, cy=251;
        stroke(cx,cy,cx,cy+22,muted);
        const int reach=static_cast<int>(24*std::cos(a));
        stroke(cx-reach,cy-7,cx+reach,cy+7,cyan);
        stroke(cx-reach,cy-7,cx,cy+9,muted);
        stroke(cx+reach,cy+7,cx,cy+9,muted);
        disk(cx,cy,3,ink);
        disk(cx,240,2,std::fmod(sim.clock,1.3)<0.5 ? rgb(255,102,92) : rgb(108,59,62));
    }
    void drawControlTower() const {
        poly({{610,307},{640,307},{649,417},{601,417}},sim.night ? rgb(65,83,99) : rgb(171,190,193));
        box(594,310,655,316,rgb(55,76,92));
        poly({{586,275},{664,275},{656,306},{594,306}},rgb(58,84,106));
        const int glass=sim.night ? rgb(234,204,132) : rgb(98,180,194);
        poly({{594,280},{620,280},{620,300},{599,300}},glass);
        poly({{627,280},{657,280},{652,300},{627,300}},glass);
        box(582,270,668,276,rgb(38,62,82));
        drawRadar(sim.clock*75.0);
    }
    void drawHangar() const {
        box(363,356,529,416,sim.night ? rgb(65,83,94) : rgb(155,180,185));
        poly({{353,356},{447,320},{539,356}},sim.night ? rgb(48,68,82) : rgb(116,148,159));
        box(379,368,514,415,sim.night ? rgb(31,48,58) : rgb(86,116,128));
        for(int x=383;x<514;x+=13) stroke(x,371,x,414,sim.night ? rgb(56,73,84) : rgb(110,138,146));
        text(411,346,"HANGAR 02",ink,1,sim.night ? rgb(48,68,82) : rgb(116,148,159));
    }
    void drawVehicle(int x, int y, bool fuel) const {
        box(x,y-14,x+37,y-3,fuel ? rgb(209,170,78) : rgb(205,215,213));
        box(x+27,y-22,x+43,y-3,rgb(190,206,211));
        box(x+30,y-20,x+40,y-13,rgb(49,95,123));
        disk(x+8,y-2,5,rgb(27,39,48)); disk(x+35,y-2,5,rgb(27,39,48));
        disk(x+8,y-2,2,muted); disk(x+35,y-2,2,muted);
        if (fuel) oval(x+12,y-18,17,9,rgb(218,188,108));
        disk(x+35,y-25,2,rgb(255,183,78));
    }
    void drawRunway() const {
        // A shallow perspective plane preserves a clear side view of wheel contact.
        poly({{-50,486},{1142,467},{1240,589},{-20,614}},rgb(91,104,104));
        poly({{-40,495},{1134,477},{1225,580},{-20,603}},sim.night ? rgb(37,48,60) : rgb(53,67,78));
        stroke(0,500,1130,482,rgb(204,217,215));
        stroke(0,596,1199,574,rgb(204,217,215));
        for(int i=0;i<15;++i) {
            // Nonlinear spacing compresses marks toward the vanishing side.
            double t=i/15.0, t2=(i+0.49)/15.0;
            int a=static_cast<int>(50+1090*(2*t-t*t));
            int b=static_cast<int>(50+1090*(2*t2-t2*t2));
            if (b>a) box(a,541,b,544,rgb(208,218,214));
        }
        for(int i=0;i<5;++i) {
            box(73,505+i*8,126,509+i*8,rgb(221,226,218));
            box(73,551+i*7,126,555+i*7,rgb(221,226,218));
        }
        text(157,555,"27",ink,3,sim.night ? rgb(37,48,60) : rgb(53,67,78));
        box(397,509,445,514,rgb(234,231,208)); box(397,568,445,573,rgb(234,231,208));
        drawRunwayLights();
        text(20,625,"RWY 27  /  2,800 m",ink,1,sim.night ? rgb(24,49,46) : rgb(91,133,99));
        text(835,625,"GATE A4  >",ink,1,sim.night ? rgb(24,49,46) : rgb(91,133,99));
    }
    void drawRunwayLights() const {
        const bool bright=std::fmod(sim.clock,1.8)<1.4;
        for(int i=0;i<=22;++i) {
            const double t=i/23.0;
            int x=static_cast<int>(18+1123*(2*t-t*t));
            int top=static_cast<int>(491-18*t), bottom=static_cast<int>(607-27*t);
            int color=i>19 ? rgb(255,104,89) : rgb(255,230,158);
            for (int y : {top,bottom}) {
                if(sim.night) disk(x,y,5,rgb(98,92,67));
                disk(x,y,bright ? 3 : 2,color);
                stroke(x,y+3,x,y+6,rgb(43,56,59));
            }
            if (i%2==0) disk(x,547,sim.night ? 2 : 1,rgb(203,233,215));
        }
    }
    void drawAirplane(double x, double y, double scale, double angle) const {
        // Every aircraft part shares the SAME local-to-world transformation.
        auto p=[&](double px, double py) { return transform({px,py},{x,y},scale,angle); };
        auto body=[&](std::initializer_list<Point> local, int color) {
            std::vector<int> v;
            for(Point q:local) { Point w=p(q.x,q.y); v.push_back(static_cast<int>(std::lround(w.x))); v.push_back(static_cast<int>(std::lround(w.y))); }
            setcolor(color); setfillstyle(SOLID_FILL,color); fillpoly(static_cast<int>(local.size()),v.data());
        };
        auto segment=[&](Point a,Point b,int c) { Point u=p(a.x,a.y),v=p(b.x,b.y); stroke(static_cast<int>(u.x),static_cast<int>(u.y),static_cast<int>(v.x),static_cast<int>(v.y),c); };
        auto light=[&](double lx,double ly,int c,int radius) { Point q=p(lx,ly); disk(static_cast<int>(q.x),static_cast<int>(q.y),std::max(1,static_cast<int>(radius*scale)),c); };
        // Shadow grows and darkens as the plane approaches the surface.
        if (sim.state!=LandingState::WAITING) {
            double proximity=1.0-std::min(1.0,sim.altitude()/900.0);
            oval(static_cast<int>(x+15),545,static_cast<int>(55+48*proximity),static_cast<int>(3+4*proximity),rgb(31,43,51));
        }
        body({{-23,-7},{-62,-37},{-35,-37},{36,-6}},rgb(147,179,192)); // far wing
        body({{-100,-6},{-119,-55},{-96,-52},{-62,-7}},rgb(35,107,145));
        body({{-111,-3},{-131,-18},{-104,-19},{-73,-1}},rgb(154,187,200));
        if (sim.gear>0.0) drawLandingGear(p,segment,light);
        body({{-116,-4},{-98,-15},{65,-15},{88,-10},{108,1},{101,9},{79,14},{-95,12},{-113,5}},rgb(227,239,240));
        body({{-110,4},{99,4},{92,11},{-94,11}},rgb(169,197,206));
        body({{-103,-1},{89,-1},{97,3},{-103,3}},rgb(38,126,163));
        body({{69,-13},{84,-10},{96,-3},{70,-3}},rgb(38,66,88));
        for(int i=0;i<13;++i) {
            const double wx=-78+i*10;
            body({{wx,-10},{wx+5,-10},{wx+5,-5},{wx,-5}},rgb(40,80,106));
        }
        body({{-20,3},{-65,34},{-35,36},{38,6}},rgb(195,215,223)); // near wing
        body({{-23,9},{4,9},{11,23},{-19,25},{-29,20}},rgb(104,143,164)); // engine
        body({{4,12},{13,15},{13,21},{5,24}},rgb(35,63,82));
        body({{-108,-12},{-115,-39},{-104,-35},{-90,-12}},rgb(39,151,179));
        segment({-105,-32},{-100,-21},rgb(158,231,232));
        if (std::fmod(sim.clock,1.1)<0.65) {
            light(-60,33,rgb(255,82,81),3);
            light(-54,-32,rgb(83,242,150),3);
            light(-116,-3,rgb(250,251,235),3);
        }
        if (std::fmod(sim.clock,0.9)<0.18) light(0,-16,rgb(255,86,75),3);
        // Door follows the aircraft transform too.
        segment({53,-11},{53,9},rgb(120,158,173));
        segment({61,-11},{61,8},rgb(120,158,173));
    }
    template<class P, class S, class L>
    void drawLandingGear(P p, S segment, L light) const {
        (void)p;
        const double extension=sim.gear;
        for(double gx : {-42.0,65.0}) {
            const double gy=9+16*extension;
            segment({gx,8},{gx,gy},rgb(48,68,83));
            segment({gx+2,8},{gx+2,gy},rgb(134,161,175));
            light(gx,gy,rgb(24,32,43),4);
            light(gx,gy,rgb(152,176,187),2);
        }
    }
    void drawSmoke() const {
        if (sim.smokeAge<0 || sim.smokeAge>1.7) return;
        const double age=sim.smokeAge;
        for(int i=0;i<7;++i) {
            const int x=static_cast<int>(TOUCH_X-42-i*7-age*18);
            const int y=static_cast<int>(WHEEL_Y-3-age*(12+i));
            oval(x,y,static_cast<int>(4+age*5+i%3),static_cast<int>(3+age*4),sim.night ? rgb(96,111,122) : rgb(177,190,188));
        }
    }
    void drawInfoPanel() const {
        // Right-side telemetry keeps the approaching aircraft unobstructed.
        box(902,150,1176,328,panel);
        box(902,150,905,328,cyan);
        text(920,165,"FLIGHT  BG-027",ink,2,panel);
        text(920,193,"CLEARED TO LAND  /  RWY 27",cyan,1,panel);
        char value[100];
        std::snprintf(value,sizeof value,"ALTITUDE     %4.0f ft",sim.altitude()); text(920,222,value,ink,1,panel);
        std::snprintf(value,sizeof value,"SPEED        %4.0f kt",sim.speedKnots()); text(920,245,value,ink,1,panel);
        std::snprintf(value,sizeof value,"TO RUNWAY    %4.0f m",sim.distance()); text(920,268,value,ink,1,panel);
        text(920,299,stateName(sim.state),cyan,1,panel);
        // Compact stage timeline: the final landing progress stays visible.
        int active=static_cast<int>(sim.state)-1;
        for(int i=0;i<6;++i) {
            int x=345+i*137;
            stroke(x,101,x+110,101,i<=active ? cyan : rgb(73,99,117));
            disk(x,101,4,i<=active ? cyan : rgb(73,99,117));
            const char* names[]={"APPROACH","DESCENT","FINAL","TOUCHDOWN","BRAKING","LANDED"};
            text(x,113,names[i],i==active ? ink : muted,1,sim.night ? rgb(14,23,44) : rgb(98,166,205));
        }
        if(sim.paused) {
            box(502,181,700,228,panel); text(530,194,"PAUSED",cyan,3,panel);
        }
        if(sim.state==LandingState::LANDED) {
            box(240,175,874,246,panel);
            box(240,175,244,246,cyan);
            text(265,191,"LANDING COMPLETED SUCCESSFULLY",cyan,2,panel);
            text(265,224,"GATE A4  /  PARKING BRAKE SET    -    R TO RESTART",ink,1,panel);
        }
    }
    void drawChrome() const {
        box(0,0,1199,64,panel);
        box(24,20,28,43,cyan);
        text(42,19,"AEROPORT",ink,3,panel);
        text(250,24,"RUNWAY & AIRPLANE LANDING SIMULATION",muted,1,panel);
        char mode[64];
        std::snprintf(mode,sizeof mode,"%s  /  SIM %.2fx",sim.night ? "NIGHT" : "DAY",sim.rate);
        text(946,25,mode,cyan,1,panel);
        box(0,659,1199,699,panel);
        text(24,675,"ENTER START   P PAUSE   R RESTART   +/- SPEED   D DAY   N NIGHT   ESC EXIT",ink,1,panel);
        text(944,675,"F11 FULLSCREEN",muted,1,panel);
    }
    void drawStartScreen() const {
        box(187,154,1013,397,panel);
        box(187,154,193,397,cyan);
        text(219,176,"COMPUTER GRAPHICS LAB PROJECT",cyan,1,panel);
        text(219,208,"AIRPORT RUNWAY &",ink,3,panel);
        text(219,244,"AIRPLANE LANDING SIMULATION",ink,3,panel);
        text(219,293,"Translation  /  Rotation  /  Scaling  /  Animation",muted,1,panel);
        text(219,321,"ENTER  Start landing       P  Pause / Resume",ink,1,panel);
        text(219,342,"R  Restart   +/-  Speed    D / N  Day / Night   ESC  Exit",ink,1,panel);
        text(219,375,"ENTER  Request landing clearance    F11 / F  Fullscreen",cyan,1,panel);
    }
    void drawBackground() const {
        drawSky(); drawGround(); drawHangar(); drawTerminal(); drawControlTower();
        drawVehicle(700+static_cast<int>(std::fmod(sim.clock*7,320.0)),445,false);
        drawVehicle(543,442,true);
        drawRunway();
    }
    void draw() const {
        drawBackground(); drawSmoke();
        drawAirplane(sim.x,sim.y,sim.scale,sim.angle);
        if(sim.state==LandingState::WAITING) drawStartScreen(); else drawInfoPanel();
        drawChrome();
    }
};

bool handleInput(Simulation& sim, Display& display) {
    while(kbhit()) {
        const int key=getch() & 0xff;
        if(key==0 || key==224) {
            // The bundled WinBGIm queues F11 as an extended scan code (133).
            if(kbhit() && (getch() & 0xff)==133) display.toggleFullscreen();
            continue;
        }
        switch(key) {
        case 27: return false;
        case 13: sim.start(); break;
        case 'f': case 'F': display.toggleFullscreen(); break;
        case 'p': case 'P':
            if(sim.state!=LandingState::WAITING) sim.paused=!sim.paused;
            break;
        case 'r': case 'R': sim.restart(); break;
        case '+': case '=': sim.changeRate(1.25); break;
        case '-': case '_': sim.changeRate(0.8); break;
        case 'd': case 'D': sim.night=false; break;
        case 'n': case 'N': sim.night=true; break;
        default: break;
        }
    }
    return true;
}
} // namespace airport

int main() {
    using namespace airport;
    initwindow(WIDTH,HEIGHT,"Airport Runway & Airplane Landing Simulation");
    Display display;
    Simulation simulation;
    Renderer renderer(simulation);
    using Clock=std::chrono::steady_clock;
    auto previous=Clock::now();
    int page=1;
    while(handleInput(simulation,display)) {
        const auto frameStart=Clock::now();
        // Clamp stalls (e.g. dragging the window) to avoid jumping past the landing.
        double elapsed=std::chrono::duration<double>(frameStart-previous).count();
        previous=frameStart;
        simulation.update(std::min(elapsed,0.1));
        setactivepage(page);
        renderer.draw(); // Scene covers the whole canvas; no erase flash.
        setvisualpage(page);
        page=1-page;
        std::this_thread::sleep_until(frameStart+std::chrono::milliseconds(20));
    }
    closegraph();
    return 0;
}
