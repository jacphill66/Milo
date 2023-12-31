#ifndef _DRT_
#define _DRT_

#include <iostream>
#include <string>
#include <cstdlib>
using namespace std;

class DRT {

 private:
  string data;
  string next;
  string prev;

 public:

  DRT (string d, string n, string p);
  string getdata ();
  string getnext ();
  string getprev ();
  string to_string();

};



#endif