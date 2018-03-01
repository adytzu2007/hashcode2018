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

struct Car {
    vector<Ride*> rides;
    int f; // finish time
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

int dist(int a, int b, int x, int y) {
    return (abs(a - x) + abs(b - y));
}

bool addRideToCar(Car* car, Ride* ride)
{
    int a = 0, b = 0;

    if (car->rides.size() > 0) {
        Ride *r = car->rides[car->rides.size() - 1];
        a = r->x;
        b = r->y;
    }

    car->f = simulationStep + ride->d + dist(a, b, ride->a, ride->b);
    if (car->f > simulationSteps) {
        return false;
    }

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

        fprintf(stderr, "%s\n", rideToString(ride).c_str());

        addRideToCar(&cars[i], ride);
    }

    while (!carsByFinish.empty()) {
        Car* car = carsByFinish.top();
        carsByFinish.pop();

        simulationStep = car->f;

        Ride *ride;
        while (!ridesByStart.empty()) {
            ride = ridesByStart.top();
            ridesByStart.pop();

            if (addRideToCar(car, ride)) {
                break;
            } else {
                ridesByStart.push(ride);
                break;
            }
        }
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

        fprintf(stderr, "%s\n", rideToString(&rides[i]).c_str());

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
