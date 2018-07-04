#ifndef __VECTOR_H__
#define __VECTOR_H__

typedef struct _VECTOR{
  double x, y, z, w;
} VECTOR;

typedef struct _RAY
{
  VECTOR start;
  VECTOR dir;
} RAY;

class Vector : public VECTOR, RAY
{
public:
  Vector();
  Vector( double _x, double _y, double _z );
  
  void Subtract( Vector other );
  void Add( Vector other );
  void Mul( double m );
  void Mul( double m, Vector other);
  Vector MulV( double m, Vector other);
  
  void Cross( Vector other );
  double Dot( Vector other );
  double Dot( Vector a, Vector b );
  double Sqr( Vector other );
  
  Vector MakePoint( double _x, double _y, double _z );
  Vector MakeDir( double _x, double _y, double _z );
  
  //Other 
  RAY MakeRay( Vector a, Vector b );
  RAY MakeRay2points( Vector start, Vector end );
private:
  Vector unitVector( Vector a );
};

Vector::Vector()
{
  x = 0; y = 0; z = 0; w = 0;
}
  
Vector Vector::unitVector( Vector a )
{
  double denom;
  denom = sqrt( Dot(a,a));
  
  if( denom == 1 ) return a;
  
  return MulV( 1/denom, a );
}

#endif
