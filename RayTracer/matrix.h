#ifndef __MATRIX_H__
#define __MATRIX_H__
/*
 Class Matrix
 Mat4x4;
*/
//Directx SDK Matrix, LOL

typedef struct _D3DMATRIX {
  union{
    struct {
         float        _11, _12, _13, _14;
         float        _21, _22, _23, _24;
         float        _31, _32, _33, _34;
         float        _41, _42, _43, _44;
    };
    float m[4][4];
  };
}D3DMATRIX;

class Mat4x4 : public D3DMATRIX
{
public:
  Mat4x4();
  Mat4x4( double a00, double a01, double a02, double a03,
	  double a10, double a11, double a12, double a13,
	  double a20, double a21, double a22, double a23,
	  double a30, double a31, double a32, double a33); 
  void SetIdentity(void);
  Mat4x4 GetIdentity(void);
  void Translate( double x, double y, double z, double w);
  void TranslateInv( double x, double y, double z, double w );
  void Scale( double x, double y ,double z );
  void ScaleInv( double x, double y, double z );
  void Rotate( char axis, double angle );
  void Transpose( Mat4x4 *t);
  void MultiplyValue( Mat4x4 *t, double value);
  void MultiplyMat4x4( Mat4x4* t );
  void AddMatrix( Mat4x4 t);
  
  D3DMATRIX GetMatrix(void);
};

Mat4x4::Mat4x4()
{
  for( int x = 0; x < 4; x++){
     for( int y = 0; y < 4; y++ )
     {
       m[x][y] = 0.0f;
     }
  }
} 

void Mat4x4::SetIdentity(void)
{
  m[0][0] = 1.0f;
  m[1][1] = 1.0f;
  m[2][2] = 1.0f;
  m[3][3] = 1.0f;
}

Mat4x4 Mat4x4::GetIdentity(void)
{
   Mat4x4 result;// = new Mat4x4();
   
   result.m[0][0] = 1.0f;
   result.m[1][1] = 1.0f;
   result.m[2][2] = 1.0f;
   result.m[3][3] = 1.0f;
  
  return result;
}

void Mat4x4::Translate( double x, double y, double z, double w )
{
  SetIdentity();
  m[0][3] = x;
  m[1][3] = y;
  m[2][3] = z;
}

void Mat4x4::TranslateInv( double x, double y, double z, double w )
{
  SetIdentity();
  m[0][3] = -x;
  m[1][3] = -y;
  m[2][3] = -z;
}

void Mat4x4::Scale( double x, double y, double z)
{
  if( x != 0 ) m[0][0] *= x;
  else m[0][0] *= 0;  
  if( y != 0 ) m[1][1] *= y;
  else m[1][1] *= 0;
  if( z != 0 ) m[2][2] *= z;
  else m[2][2] *= 0;
}

void Mat4x4::ScaleInv( double x, double y, double z)
{
  SetIdentity();
  m[0][0] /= x;
  m[1][1] /= y;
  m[2][2] /= z;
}

void rotateX(double angle){}
void rotateY(double angle){}
void rotateZ(double angle){}

void Mat4x4::Rotate( char axis, double angle )
{
  if( axis == 'X' )
     rotateX( angle );
  if( axis == 'Y' )
     rotateY( angle );
  if( axis == 'Z' )
      rotateZ( angle );
}

void Mat4x4::Transpose( Mat4x4 *t )
{
  for( int x = 0; x < 4; x++ )
  {
    for( int y =0; y < 4; y++ )
    {
      m[y][x] = (*t).m[x][y];
    }
  }
}

void Mat4x4::MultiplyValue( Mat4x4 *t, double value)
{
  for( int x = 0; x < 4; x++ )
  {
    for ( int y = 0; y < 4; y++ )
    {
      (*t).m[x][y] *= value;
    }
  }
  (*t).m[3][3] = 1;
}

void Mat4x4::MultiplyMat4x4( Mat4x4* t )
{
  Mat4x4 result;
  for( int x; x < 4; x++ )
  {
    for( int y = 0; y < 4; y++ )
    {
      for( int p = 0; p < 4; p++ ){
        result.m[x][y] = (*t).m[x][p] * m[p][y];
      }
    }
  }
  
  for( int x = 0; x < 4; x++)
 {
    for( int y = 0; y < 4; y++ )
   { 
      m[x][y] = result.m[x][y];
   }
 }
}

Mat4x4::Mat4x4( double a00, double a01, double a02, double a03,
	  double a10, double a11, double a12, double a13,
	  double a20, double a21, double a22, double a23,
	  double a30, double a31, double a32, double a33)
{
  m[0][0] = a00;
  m[0][1] = a01;
  m[0][2] = a02;
  m[0][3] = a03;

  m[1][0] = a10;
  m[1][1] = a11;
  m[1][2] = a12;
  m[1][3] = a13;

  m[2][0] = a20;
  m[2][1] = a21;
  m[2][2] = a22;
  m[2][3] = a23;

  m[3][0] = a30;
  m[3][1] = a31;
  m[3][2] = a32;
  m[3][3] = a33;
}  

void Mat4x4::AddMatrix( Mat4x4 t )
{
  for( int x = 0; x < 4; x++ )
  {
      for( int y = 0; y < 4; y++ )
      {
         m[x][y] += t.m[x][y];
      }
  }
}
  
#endif
