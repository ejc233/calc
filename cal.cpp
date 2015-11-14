#include "cal.h"
#include <cmath>
#include <tuple>

using std::tuple;
using std::tie;
using std::make_tuple;

//memory
double memory[1024*128];


cal::calc_exp::calc_exp(std::string s):expr(s),result(0),done(false)
{}

int cal::calc_exp::set_exp(std::string s){expr=s;done=false;return 0;}

std::string cal::calc_exp::get_exp(){return expr;}

tuple<double, char*> eval(tuple<double,char*>,int);

tuple<double,char*> num(char* c){
  double r=0;
  switch(c[0]){
  case '(':c++;tie(r,c)=eval(num(c),0);c++;break;
  //P means \pi
  case 'P':c++;r=M_PI;break;
  //E means e
  case 'E':c++;r=M_E;break;
  //e() means exp
  case 'e':c++;tie(r,c)=num(c);r=std::exp(r);break;
  //s() means sin
  case 's':c++;tie(r,c)=num(c);r=std::sin(r);break;
  //l() means log
  case 'l':c++;tie(r,c)=num(c);r=std::log(r);break;
  //c() means cos
  case 'c':c++;tie(r,c)=num(c);r=std::cos(r);break;
  //$() read from memory
  case '$':c++;{tie(r,c)=num(c);
      int addr=(int)r;
      if(0<=addr&&addr<1024*128)
      r=memory[addr];}break;
  //negative sign
  case '-':c++;tie(r,c)=num(c);r*=-1;break;
  default: while(c[0]>='0'&&c[0]<='9'){
      r=r*10+(c[0]-'0');c++;}
  double dec=1;
  if(c[0]=='.'){
    c++;
    while(c[0]>='0'&&c[0]<='9'){
      dec/=10;r+=dec*(c[0]-'0');
      c++;}
  }}
  return make_tuple(r,c);}

inline int oplev(char c){
  switch(c){
  case ';':return 0;
  case ':':return 1;//assignment operator: value:address
  case '>':return 2;
  case '<':return 2;
  case '+':return 3;
  case '-':return 3;
  case '*':return 4;
  case '/':return 4;
  case '%':return 4;
  case '^':return 5;
  default: return -1;}
}

tuple<double,char*> eval(tuple<double,char*> cur,int lev)
{
  char* c;
  double r;
  tie(r,c)=cur;
  while(lev<=oplev(c[0]))
    {char op=c[0];c++;
      double rhs;
	tie(rhs,c)=num(c);
    while(oplev(c[0])>oplev(op))
      tie(rhs,c)=eval(make_tuple(rhs,c),oplev(c[0]));
    switch(op){
    case '+':r+=rhs;break;
    case '-':r-=rhs;break;
    case '*':r*=rhs;break;
    case '/':r/=rhs;break;
    case '^':r=std::pow(r,rhs);break;
    case '%':r=((long)r)%((long)rhs);break;
    case '>':r=(r>rhs);break;
    case '<':r=(r<rhs);break;
    case ':':{int addr=(int)rhs;
	if(addr>=0&&addr<1024*128)memory[addr]=r;} break;
    case ';':r=rhs;break;
    }
}
  return make_tuple(r,c);  
}

double cal::calc_exp::calc(){
  //lazy eval
  if(done)return result;
  done=true;
  double result=std::get<0>(eval(num((char*)expr.c_str()),0));
  return result;
}

 
