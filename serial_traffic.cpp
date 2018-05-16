#include <iostream>
#include <stdlib.h>
#include <time.h>
#include <vector>

using std::cout;
using std::endl;
using std::vector;

// ****************************************************************************

// I use periodic boundary conditions for the road.
const int road_length = 40;
int blocks_shown_num = 40;       // number of blocks to show on each lane
const int vmax = 5;              // maximum permitted velocity
int cars_num = 20;               // total number of the cars
double decel_prob = 0.1;         // probability of random decelerations
double change_prob = 0.99;       // probability of lane change when feasible

// ****************************************************************************

typedef struct
{
  int v;            // velocity of the car
  int lane;         // which lane is the car on
  int x;            // position of the car on the corresponding lane
  int d_same_f;     // distance to the car ahead in the same lane
  int d_same_b;     // distance to the car in the back on the same lane
  int d_other_b;    // distance to the car in the back on other lane
  int d_other_f;    // distance to the car in the front on other lane
} car;

typedef struct
{
  int i;            // index of the car
  int x;            // position of the car
} pair;

// ****************************************************************************

int partition (vector<pair>& A, int p, int r);
void quicksort(vector<pair>& A, int p, int r);
void initialize(car* cars);
void find_neighbors(car* cars);
void show_road(car* cars, int blocks_shown);
void single_lane_update(car* cars);
void lane_change(car* cars);
void advance(car* cars);

// ****************************************************************************

int main()
{
  car* cars = (car*) malloc(cars_num * sizeof(car));

  initialize(cars);

  int t_total = 5;

  for (int i = 0; i < t_total; i++)
  {
    cout << "t = " << i << endl;
    lane_change(cars);
    single_lane_update(cars);
    advance(cars);
    show_road(cars, blocks_shown_num);
  }

  free(cars);
}

// ****************************************************************************
// ***************************** SUBROUTINES **********************************
// ****************************************************************************

// Making the cars advance in the same lane for one unit of time.
void advance(car* cars)
{
  for (int i = 0; i < cars_num; i++)
    cars[i].x = ( cars[i].x + cars[i].v ) % road_length;

  find_neighbors(cars);
}

// ****************************************************************************

void show_road(car* cars, int blocks_shown)
{
  vector<pair> lane0;
  vector<pair> lane1;

  for (int i = 0; i < cars_num; i++)
  {
    if (cars[i].lane == 0)
    {
      pair temp;
      temp.i = i;
      temp.x = cars[i].x;
      lane0.push_back(temp);
    }
    else
    {
      pair temp;
      temp.i = i;
      temp.x = cars[i].x;
      lane1.push_back(temp);
    }
  }

  quicksort(lane0, 0, lane0.size()-1);
  quicksort(lane1, 0, lane1.size()-1);

  // works best for cars_num < 100
  for (int i = 0; i < blocks_shown; i++)
  {
    if (i == lane0[0].x)
    {
      if (lane0[0].i < 10)
        cout << "(0" << lane0[0].i << ")";
      else
        cout << "(" << lane0[0].i << ")";

      lane0.erase(lane0.begin());
    }
    else
      cout << "(  )";
  }

  cout << endl;

  // works best for cars_num < 100
  for (int i = 0; i < blocks_shown; i++)
  {
    if (i == lane1[0].x)
    {
      if (lane1[0].i < 10)
        cout << "(0" << lane1[0].i << ")";
      else
        cout << "(" << lane1[0].i << ")";

      lane1.erase(lane1.begin());
    }
    else
      cout << "(  )";
  }

  cout << endl << endl;
}

// ****************************************************************************

void single_lane_update(car* cars)
{
  for (int i = 0; i < cars_num; i++)
  {
    if (cars[i].v < vmax)
      cars[i].v = cars[i].v + 1;
    if (cars[i].v > cars[i].d_same_f)
      cars[i].v = cars[i].d_same_f;

    srand(time(0));
    double random = (rand()%1000)/1000.0;
    if (cars[i].v > 0 && random < decel_prob)
      cars[i].v = cars[i].v - 1;
  }
}

// ****************************************************************************

void lane_change(car* cars)
{
  int l;
  double random;

  for (int i = 0; i < cars_num; i++)
  {
    random = (rand()%1000)/1000.0;
    l = cars[i].v + 1;
    if (cars[i].d_same_f < l && cars[i].d_other_f > l
        && cars[i].d_other_b > vmax && random < change_prob)
    {
      cars[i].lane = 1 - cars[i].lane;
      find_neighbors(cars);
    }
  }
}

// ****************************************************************************

// This function is solely to be used in the quicksort function that follows it.
int partition (vector<pair>& A, int p, int r)
{
  int i = p-1;
  int x = A[r].x;

  for (int j = p; j < r; j++)
  {
    if (A[j].x <= x)
    {
      i = i+1;
      int tempi = A[i].i;
      int tempx = A[i].x;
      A[i].i = A[j].i;
      A[j].i = tempi;
      A[i].x = A[j].x;
      A[j].x = tempx;
    }
  }

  i = i+1;
  int tempi = A[r].i;
  A[r].i = A[i].i;
  A[i].i = tempi;
  int tempx = A[r].x;
  A[r].x = A[i].x;
  A[i].x = tempx;

  return i;
}

void quicksort(vector<pair>& A, int p, int r)
{
  if (p < r)
  {
    int q = partition(A, p, r);
    quicksort(A, p, q-1);
    quicksort(A, q+1, r);
  }
}

//*****************************************************************************

void initialize(car* cars)
{
  // initializing the position of the cars
  vector <int> lane0;
  vector <int> lane1;
  for (int i = 0; i < road_length; i++) lane0.push_back(i);
  for (int i = 0; i < road_length; i++) lane1.push_back(i);

  srand(time(0));
  int random;

  for (int i = 0; i < cars_num/2; i++)
  {
    random = rand() % lane0.size();
    cars[2*i].x  = lane0[random];
    lane0.erase(lane0.begin()+random);
    cars[2*i].lane = 0;

    random = rand() % lane1.size();
    cars[2*i+1].x = lane1[random];
    lane1.erase(lane1.begin()+random);
    cars[2*i+1].lane = 1;
  }

  lane0.clear();
  lane1.clear();

  // initializing the velocity of the cars
  for (int i = 0; i < cars_num; i++)
    cars[i].v = rand() % vmax + 1;

  find_neighbors(cars);
}

//*****************************************************************************

void find_neighbors(car* cars)
{
  // determining the front cars in the same lane
  for (int i = 0; i < cars_num/2; i++)
  {
    int d_same_f = road_length;
    for (int j = 0; j < cars_num/2; j++)
      if (j != i)
      {
        int distance = ( cars[2*j].x - cars[2*i].x + road_length ) % road_length - 1;
        if ( distance < d_same_f )
        {
          cars[2*i].d_same_f = distance;
          d_same_f = distance;
        }
      }

    d_same_f = road_length;
    for (int j = 0; j < cars_num/2; j++)
      if (j != i)
      {
        int distance = ( cars[2*j+1].x - cars[2*i+1].x + road_length ) % road_length - 1;
        if ( distance < d_same_f )
        {
          cars[2*i+1].d_same_f = distance;
          d_same_f = distance;
        }
      }
  }

  // determining the back cars in the same lane
  for (int i = 0; i < cars_num/2; i++)
  {
    int d_same_b = road_length;
    for (int j = 0; j < cars_num/2; j++)
      if (j != i)
      {
        int distance = ( cars[2*i].x - cars[2*j].x + road_length ) % road_length - 1;
        if ( distance < d_same_b )
        {
          cars[2*i].d_same_b = distance;
          d_same_b = distance;
        }
      }

    d_same_b = road_length;
    for (int j = 0; j < cars_num/2; j++)
      if (j != i)
      {
        int distance = ( cars[2*i+1].x - cars[2*j+1].x + road_length ) % road_length - 1;
        if ( distance < d_same_b )
        {
          cars[2*i+1].d_same_b = distance;
          d_same_b = distance;
        }
      }
  }

  // determining the front cars in the other lane
  for (int i = 0; i < cars_num/2; i++)
  {
    int d_other_f = road_length;
    for (int j = 0; j < cars_num/2; j++)
    {
      int distance = ( cars[2*j+1].x - cars[2*i].x + road_length ) % road_length - 1;
      if ( distance < d_other_f )
      {
        cars[2*i].d_other_f = distance;
        d_other_f = distance;
      }
    }

    d_other_f = road_length;
    for (int j = 0; j < cars_num/2; j++)
    {
      int distance = ( cars[2*j].x - cars[2*i+1].x + road_length ) % road_length - 1;
      if ( distance < d_other_f )
      {
        cars[2*i+1].d_other_f = distance;
        d_other_f = distance;
      }
    }
  }

  // determining the back cars in the other lane
  for (int i = 0; i < cars_num/2; i++)
  {
    int d_other_b = road_length;
    for (int j = 0; j < cars_num/2; j++)
    {
      int distance = ( cars[2*i].x - cars[2*j+1].x + road_length ) % road_length - 1;
      if ( distance < d_other_b )
      {
        cars[2*i].d_other_b = distance;
        d_other_b = distance;
      }
    }

    d_other_b = road_length;
    for (int j = 0; j < cars_num/2; j++)
    {
      int distance = ( cars[2*i+1].x - cars[2*j].x + road_length ) % road_length - 1;
      if ( distance < d_other_b )
      {
        cars[2*i+1].d_other_b = distance;
        d_other_b = distance;
      }
    }
  }
}
