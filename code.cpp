#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <queue>
#include <string>
#include <sstream>

using namespace std;

int rows;
int cols;
int vehicles;
int noRides;
int bonus;
int simulationSteps;
int simulationStep;

struct Ride {
    int a; // start row
    int b; // start col
    int x; // end row
    int y; // end col
    int s; // earliest start
    int f; // latest finish
    int d; // distance from start to finish
};

struct Car;

using CarRide = pair<Car*, Ride*>;

struct CmpByRideStart {
    int operator() (const CarRide& a, const CarRide& b);
};

struct Car {
    vector<Ride*> rides;
    priority_queue<CarRide, vector<CarRide>, CmpByRideStart> ridesByStart{CmpByRideStart()};
    int f; // finish time
};

int dist(int a, int b, int x, int y) {
    return (abs(a - x) + abs(b - y));
}

int CmpByRideStart::operator() (const CarRide& a, const CarRide& b)
{
    Car* car = a.first;
    Ride* rideA = a.second;
    Ride* rideB = b.second;
    Ride* currentRide = car->rides[car->rides.size() - 1];

    int d1 = dist(currentRide->x, currentRide->y, rideA->a, rideA->b);
    int d2 = dist(currentRide->x, currentRide->y, rideB->a, rideB->b);

    return d1 > d2;
};

Ride rides[10000];
Car cars[1000];

auto cmpByStart = [] (const Ride* a, const Ride* b) {
    return a->s > b->s;
};
priority_queue<Ride*, vector<Ride*>, decltype(cmpByStart)> ridesByStart(cmpByStart);

auto cmpByFinish = [] (const Car* a, const Car* b) {
    return a->f > b->f;
};
priority_queue<Car*, vector<Car*>, decltype(cmpByFinish)> carsByFinish(cmpByFinish);


bool addRideToCar(Car* car, Ride* ride, bool simulate)
{
    int a = 0, b = 0;

    if (car->rides.size() > 0) {
        Ride *r = car->rides[car->rides.size() - 1];
        a = r->x;
        b = r->y;
    }

    int f = simulationStep + ride->d + dist(a, b, ride->a, ride->b);
    if (f > simulationSteps) {
        return false;
    }

    if (simulate) {
        return true;
    }

    car->f = f;
    car->rides.push_back(ride);

    carsByFinish.push(car);

    return true;
}

string rideToString(Ride* ride)
{
    stringstream s;
    s << "Start row " << ride->a <<
        " start column " << ride->b <<
        " end row " << ride->x <<
        " end column " << ride->y <<
        " earliest start " << ride->s <<
        " latest finish " << ride->f;
    return s.str();
}

void solve()
{
    for (int i = 0; i < vehicles; ++i) {
        Ride* ride = ridesByStart.top();
        ridesByStart.pop();

        addRideToCar(&cars[i], ride, false);
    }

    while (!carsByFinish.empty()) {
        Car* car = carsByFinish.top();
        carsByFinish.pop();

        simulationStep = car->f;

        Ride *ride;
        vector<Ride*> skipped;
        while (car->ridesByStart.size() < 5 &&
               !ridesByStart.empty()) {
            Ride *ride = ridesByStart.top();
            ridesByStart.pop();

            if (!addRideToCar(car, ride, true)) {
                skipped.push_back(ride);
            } else {
                car->ridesByStart.push(make_pair(car, ride));
            }
        }

        for (Ride* r: skipped) {
            ridesByStart.push(r);
        }

        if (car->ridesByStart.empty()) {
            continue;
        }

        CarRide carRide = car->ridesByStart.top();
        car->ridesByStart.pop();

        ride = carRide.second;

        while (!car->ridesByStart.empty()) {
            CarRide carRide = car->ridesByStart.top();
            car->ridesByStart.pop();
            ridesByStart.push(carRide.second);
        }

        assert(addRideToCar(car, ride, false));
    }
}

int main(int argc, char *argv[])
{
    scanf("%d %d %d %d %d %d",
          &rows, &cols, &vehicles,
          &noRides, &bonus, &simulationSteps);

    for (int i = 0; i < noRides; ++i) {
        scanf("%d %d %d %d %d %d",
              &rides[i].a, &rides[i].b,
              &rides[i].x, &rides[i].y,
              &rides[i].s, &rides[i].f);

        rides[i].d = dist(rides[i].a, rides[i].b,
                          rides[i].x, rides[i].y);

        ridesByStart.push(&rides[i]);
    }

    solve();

    for (int i = 0; i < vehicles; ++i) {
        printf("%d ", cars[i].rides.size());
        for (int j = 0; j < cars[i].rides.size(); ++j) {
            printf("%d ", cars[i].rides[j] - rides);
        }
        putchar('\n');
    }
    return 0;
}
