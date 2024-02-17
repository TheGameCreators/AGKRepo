#include "agk.h"

// general

float AGK::EvaluateBezier( float c1, float c2, float t )
{
	float c = 3*c1;
	float b = 3*(c2-c1) - c;
	float a = 1 - c - b;
	return ((a*t + b)*t + c)*t;
}

float AGK::EvaluateBezierDt( float c1, float c2, float t )
{
	float d = 3*c2;
	float c = 3*c1;
	float b = 2*(d-c-c);
	float a = 3 - b - d - c;
	return (a*t + b)*t + c;
}

// ************
// Vector
// ************

void AGKVector::Normalize()
{
	float length = x*x + y*y + z*z;
	if ( length < SMALL_FLOAT ) return;
	if ( length == 1 ) return;

	length = agk::Sqrt( length );
	x = x / length;
	y = y / length;
	z = z / length;
}

float AGKVector::Length() const 
{ 
	return agk::Sqrt(x*x + y*y + z*z); 
}

float AGKVector::GetDist( const AGKVector &v ) const
{ 
	return agk::Sqrt( (v.x-x)*(v.x-x) + (v.y-y)*(v.y-y) + (v.z-z)*(v.z-z) ); 
}

void AGKVector::Interpolate( const AGKVector &from, const AGKVector &to, float t )
{
	x = from.x + t*(to.x - from.x);
	y = from.y + t*(to.y - from.y);
	z = from.z + t*(to.z - from.z);
}

AGKVector& AGKVector::Mult( const AGKMatrix3 &mat )
{
	float resultX = x*mat.mat[0][0] + y*mat.mat[1][0] + z*mat.mat[2][0];
	float resultY = x*mat.mat[0][1] + y*mat.mat[1][1] + z*mat.mat[2][1];
	float resultZ = x*mat.mat[0][2] + y*mat.mat[1][2] + z*mat.mat[2][2];

	x = resultX;
	y = resultY;
	z = resultZ;

	return *this;
}

AGKVector& AGKVector::Mult( const AGKMatrix4 &mat )
{
	float resultX = x*mat.mat[0][0] + y*mat.mat[1][0] + z*mat.mat[2][0] + mat.mat[3][0];
	float resultY = x*mat.mat[0][1] + y*mat.mat[1][1] + z*mat.mat[2][1] + mat.mat[3][1];
	float resultZ = x*mat.mat[0][2] + y*mat.mat[1][2] + z*mat.mat[2][2] + mat.mat[3][2];
	
	x = resultX;
	y = resultY;
	z = resultZ;

	return *this;
}

AGKVector& AGKVector::MultPerspective( const AGKMatrix4 &mat )
{
	float resultX = x*mat.mat[0][0] + y*mat.mat[1][0] + z*mat.mat[2][0] + mat.mat[3][0];
	float resultY = x*mat.mat[0][1] + y*mat.mat[1][1] + z*mat.mat[2][1] + mat.mat[3][1];
	float resultZ = x*mat.mat[0][2] + y*mat.mat[1][2] + z*mat.mat[2][2] + mat.mat[3][2];
	float resultW = x*mat.mat[0][3] + y*mat.mat[1][3] + z*mat.mat[2][3] + mat.mat[3][3];

	x = resultX / resultW;
	y = resultY / resultW;
	z = resultZ / resultW;
	
	return *this;
}

AGKVector& AGKVector::Mult( const AGKQuaternion &rot )
{
	float tx = rot.w*x + rot.y*z - rot.z*y;
	float ty = rot.w*y + rot.z*x - rot.x*z;
	float tz = rot.w*z + rot.x*y - rot.y*x;

	x = x + 2*(rot.y*tz - rot.z*ty);
	y = y + 2*(rot.z*tx - rot.x*tz);
	z = z + 2*(rot.x*ty - rot.y*tx);

	return *this;
}

AGKVector& AGKVector::MultX( const AGKQuaternion &rot )
{
	float ty = rot.z*x;
	float tz = -rot.y*x;

	x = x + 2*(rot.y*tz - rot.z*ty);
	y = 2*(rot.w*ty - rot.x*tz);
	z = 2*(rot.x*ty + rot.w*tz);

	return *this;
}

AGKVector& AGKVector::MultY( const AGKQuaternion &rot )
{
	float tx = -rot.z*y;
	float tz = rot.x*y;

	x = 2*(rot.y*tz + rot.w*tx);
	y = y + 2*(rot.z*tx - rot.x*tz);
	z = 2*(rot.w*tz - rot.y*tx);

	return *this;
}

AGKVector& AGKVector::MultZ( const AGKQuaternion &rot )
{
	float tx = rot.y*z;
	float ty = -rot.x*z;

	x = 2*(rot.w*tx - rot.z*ty);
	y = 2*(rot.z*tx + rot.w*ty);
	z = z + 2*(rot.x*ty - rot.y*tx);

	return *this;
}

AGKVector& AGKVector::Cross( const AGKVector &u, const AGKVector &v )
{
	// right hand rule
	x = u.z*v.y - u.y*v.z;
	y = u.x*v.z - u.z*v.x;
	z = u.y*v.x - u.x*v.y;
	
	return *this;
}

void AGKVector::FlattenToCrossVector( const AGKVector &u, const AGKVector &v )
{
	AGKVector temp;
	temp.Cross( u, v );
	float sqrLength = temp.SqrLength();
	if ( sqrLength > SMALL_FLOAT ) sqrLength = this->Dot(temp)/sqrLength;
	else sqrLength = 0;
	x = temp.x * sqrLength;
	y = temp.y * sqrLength;
	z = temp.z * sqrLength;
}

AGKVector AGKVector::ClosestPoint( const AGKVector &start, const AGKVector &object ) const
{
	AGKVector v2( object.x-start.x, object.y-start.y, object.z-start.z );
    float sqrLen = x*x + y*y + z*z;
    
    float dist;
    if ( sqrLen > SMALL_FLOAT ) dist = (x*v2.x + y*v2.y + z*v2.z)/sqrLen;
    else dist = 0;
    
    AGKVector result;
    
    if ( dist <= 0 ) 
	{
        result = start;
    }
	else
	{
		if ( dist >= 1 ) 
		{
			result.x = start.x + x;
			result.y = start.y + y;
			result.z = start.z + z;
		}
		else
		{
			result.x = start.x + x*dist;
			result.y = start.y + y*dist;
			result.z = start.z + z*dist;
		}
	}
    
    return result;
}

AGKVector AGKVector::ClosestNormalisedPoint( const AGKVector &start, const AGKVector &finish, const AGKVector &object ) const
{
	AGKVector v2( object.x-start.x, object.y-start.y, object.z-start.z );
    float dist = x*v2.x + y*v2.y + z*v2.z;
    AGKVector result;
    
    if ( dist <= 0 ) 
	{
        result = start;
    }
	else
	{
		if ( (dist*dist) >= start.GetSqrDist( finish ) ) {
			result = finish;
		}
		else
		{
			result.x = start.x + x*dist;
			result.y = start.y + y*dist;
			result.z = start.z + z*dist;
		}
	}
    
    return result;
}

AGKVector4& AGKVector4::Mult( const AGKMatrix4 &mat )
{
	float resultX = vec.x*mat.mat[0][0] + vec.y*mat.mat[1][0] + vec.z*mat.mat[2][0] + w*mat.mat[3][0];
	float resultY = vec.x*mat.mat[0][1] + vec.y*mat.mat[1][1] + vec.z*mat.mat[2][1] + w*mat.mat[3][1];
	float resultZ = vec.x*mat.mat[0][2] + vec.y*mat.mat[1][2] + vec.z*mat.mat[2][2] + w*mat.mat[3][2];
	float resultW = vec.x*mat.mat[0][3] + vec.y*mat.mat[1][3] + vec.z*mat.mat[2][3] + w*mat.mat[3][3];

	vec.x = resultX;
	vec.y = resultY;
	vec.z = resultZ;
	w = resultW;

	return *this;
}

// ************
// Quaternion
// ************

void AGKQuaternion::Normalize()
{
	float length = w*w + x*x + y*y + z*z;
	if ( length < SMALL_FLOAT ) return;
	if ( length == 1 ) return;

	length = 1 / agk::Sqrt( length );
	w = w * length;
	x = x * length;
	y = y * length;
	z = z * length;
}

void AGKQuaternion::NormalizeFast() 
{
	float s = x*x + y*y + z*z + w*w;

	// two step Netwon Raphson with initial guess of 1
	//float k = (1 + 6*s + s*s) / (4*(1 + s));

	// three step Newton Raphson with initial guess of 1
	float k = 8*(s+1)*(s*(s+6)+1) / (s*(s*(s*(s + 28) + 70) + 28) + 1);
	
	w *= k;
    x *= k;
    y *= k;
	z *= k;
}

void AGKQuaternion::Mult( const AGKQuaternion &other )
{
	if ( other.IsIdentity() ) return;
	if ( IsIdentity() )
	{
		w = other.w;
		x = other.x;
		y = other.y;
		z = other.z;
		return;
	}

	/*
	// reference version
	float resultW = other.w*w - other.x*x - other.y*y - other.z*z;
	float resultX = other.w*x + other.x*w + other.y*z - other.z*y;
	float resultY = other.w*y - other.x*z + other.y*w + other.z*x;
	float resultZ = other.w*z + other.x*y - other.y*x + other.z*w;
	*/

	// optimized version
	float ww = (other.z + other.x) * (x + y);
    float yy = (other.w - other.y) * (w + z);
    float zz = (other.w + other.y) * (w - z);
    float xx = ww + yy + zz;
    float qq = 0.5f * (xx + (other.z - other.x) * (x - y));

    float resultW = qq - ww + (other.z - other.y) * (y - z);
    float resultX = qq - xx + (other.x + other.w) * (x + w);
    float resultY = qq - yy + (other.w - other.x) * (y + z);
    float resultZ = qq - zz + (other.z + other.y) * (w - x);

	w = resultW;
	x = resultX;
	y = resultY;
	z = resultZ;
}

float AGKQuaternion::Dot( const AGKQuaternion &other )
{
	return w*other.w + x*other.x + y*other.y + z*other.z;
}

void AGKQuaternion::AddLocalRotation( int axis, float ang )
{
	float c = agk::Cos( ang/2.0f );
	float s = agk::Sin( ang/2.0f );

	float resultW, resultX, resultY, resultZ;

	switch( axis )
	{
		case 0:
		{
			resultW = w*c - x*s;
			resultX = w*s + x*c;
			resultY = y*c + z*s;
			resultZ = z*c - y*s;
			break;
		}

		case 1:
		{
			resultW = w*c - y*s;
			resultX = x*c - z*s;
			resultY = w*s + y*c;
			resultZ = x*s + z*c;
			break;
		}

		case 2:
		{
			resultW = w*c - z*s;
			resultX = x*c + y*s;
			resultY = y*c - x*s;
			resultZ = w*s + z*c;
			break;
		}

		default:
		{
			resultW = w;
			resultX = x;
			resultY = y;
			resultZ = z;
		}
	}

	w = resultW;
	x = resultX;
	y = resultY;
	z = resultZ;
}

void AGKQuaternion::AddGlobalRotation( int axis, float ang )
{
	float c = agk::Cos( ang/2.0f );
	float s = agk::Sin( ang/2.0f );

	float resultW, resultX, resultY, resultZ;

	switch( axis )
	{
		case 0:
		{
			resultW = c*w - s*x;
			resultX = c*x + s*w;
			resultY = c*y - s*z;
			resultZ = c*z + s*y;
			break;
		}

		case 1:
		{
			resultW = c*w - s*y;
			resultX = c*x + s*z;
			resultY = c*y + s*w;
			resultZ = c*z - s*x;
			break;
		}

		case 2:
		{
			resultW = c*w - s*z;
			resultX = c*x - s*y;
			resultY = c*y + s*x;
			resultZ = c*z + s*w;
			break;
		}
	}

	w = resultW;
	x = resultX;
	y = resultY;
	z = resultZ;
}

void AGKQuaternion::Slerp( const AGKQuaternion &from, const AGKQuaternion &to, float t )
{
	if ( t < 0 ) t = 0;
	if ( t > 1 ) t = 1;

	float separation = from.w*to.w + from.x*to.x + from.y*to.y + from.z*to.z;
    if( separation >= (1.0-SMALL_FLOAT) || separation <= (-1.0+SMALL_FLOAT) ) 
	{
        w = from.w;
		x = from.x;
		y = from.y;
		z = from.z;
    }
    else
    {
		if ( separation >= 0.9995f ) Lerp( from, to, t );
		else
		{
			float ang = agk::ACosRad( separation>0 ? separation : -separation );
			float ang1 = ang*t;

			w = to.w - from.w*separation;
			x = to.x - from.x*separation;
			y = to.y - from.y*separation;
			z = to.z - from.z*separation;
			Normalize();

			float cosA = agk::CosRad( ang1 );
			float sinA = agk::SinRad( ang1 );
			if ( separation < 0 ) cosA = -cosA;

			w = from.w*cosA + w*sinA;
			x = from.x*cosA + x*sinA;
			y = from.y*cosA + y*sinA;
			z = from.z*cosA + z*sinA;
		}
    }
}

void AGKQuaternion::Lerp( const AGKQuaternion &from, const AGKQuaternion &to, float t )
{
	if ( t < 0 ) t = 0;
	if ( t > 1 ) t = 1;

	float dot = from.w*to.w + from.x*to.x + from.y*to.y + from.z*to.z;

	if ( dot < 0 )
	{
		w = t*(to.w + from.w) - from.w;
		x = t*(to.x + from.x) - from.x;
		y = t*(to.y + from.y) - from.y;
		z = t*(to.z + from.z) - from.z;
	}
	else
	{
		w = from.w + t*(to.w - from.w);
		x = from.x + t*(to.x - from.x);
		y = from.y + t*(to.y - from.y);
		z = from.z + t*(to.z - from.z);
	}

	NormalizeFast();
}

void AGKQuaternion::ClosestYX()
{
	float det = -w*z - x*y;
    if ( agk::Abs(det) < 0.5f )
	{
		float discr = sqrt(1 - 4*det*det);
		float a = w*x - y*z;
		float b = w*w - x*x + y*y - z*z;
		float c0, s0;
		
		if ( b >= 0 )
		{
			c0 = 0.5f*( discr + b );
			s0 = a;
		}
		else
		{
			c0 = a;
			s0 = 0.5f*( discr - b );
		}
		
		float invLen = 1 / sqrt(c0*c0 + s0*s0);
		c0 *= invLen;
		s0 *= invLen;
		
		float c1 = w*c0 + x*s0;
		float s1 = y*c0 - z*s0;
		invLen = 1 / sqrt(c1*c1 + s1*s1);
		c1 *= invLen;
		s1 *= invLen;
		
		w = c0*c1;
		x = s0*c1;
		y = c0*s1;
		z = -s0*s1;
	}
	else
	{
        float invLen = 1.0f / agk::Sqrt(agk::Abs(det));
		w = w * invLen;
		x = x * invLen;
		y = 0;
		z = 0;
	}	
}

float AGKQuaternion::GetEulerX() const
{
	float value = 2*y*z - 2*w*x;
	float ang1;

	if ( value > 1 ) value = 1;
	if ( value < -1 ) value = -1;

	ang1 = -agk::ASin( value );

	// if we are near the poles (x = +-90) then switch to a more accurate method
	if ( value > 0.999997f || value < -0.999997f )
	{
		float ang2 = 2*agk::ATan2(x,w);
		if ( ang2 > 90 ) ang2= 180-ang2;
		else if ( ang2 < -90 ) ang2= -180-ang2;

		// interpolate between the two methods as there is a sharp transition
		float t = 1.0f - agk::Abs(value);
		t *= 333333.333333f;
		ang1 = ang1*t + ang2*(1-t);
	}
	
	return ang1;
}

float AGKQuaternion::GetEulerY() const
{
	float value = 2*(y*z - w*x);
	float a = 2*(x*z + w*y);
	float b = 1 - 2*(x*x + y*y);
	float ang;

	if ( value > 0.999998f ) 
	{
		if ( a < 0 ) ang = 2*agk::ATan2(z,w);
		else ang = 2*agk::ATan2(z,w)+180;
	}
	else if ( value < -0.999998f ) 
	{
		if ( b > 0 ) ang = -2*agk::ATan2(z,w);
		else ang = -2*agk::ATan2(z,w)+180;
	}
	else
	{
		ang = agk::ATan2( a, b ); 
	}

	if ( ang < 0 ) ang += 360;
	if ( ang > 360 ) ang -= 360;

	return ang;
}

float AGKQuaternion::GetEulerZ() const
{
	float value = 2*(y*z - w*x);
	float a = 2*(x*z + w*y);
	float b = 1 - 2*(x*x + z*z);
	float ang;

	if ( value > 0.999998f ) 
	{
		if ( a < 0 ) ang = 0;
		else ang = 180;
	}
	else if ( value < -0.999998f ) 
	{
		if ( b > 0 ) ang = 0;
		else ang = 180;
	}
	else
	{
		a = 2*(x*y + w*z);
		ang = agk::ATan2( a, b );
	}

	if ( ang < -180 ) ang += 360;
	if ( ang > 180 ) ang -= 360;

	return ang;
}

void AGKQuaternion::LookAt( float vx, float vy, float vz, float roll )
{
	float angY = agk::ATan2( vx, vz );
	float angX;

	if ( agk::Abs(vy) < SMALL_FLOAT )
	{
		angX = 0;
	}
	else
	{
		float vxz = agk::Sqrt( vx*vx + vz*vz );
		if ( agk::Abs(vxz) < SMALL_FLOAT ) angX = vy > 0 ? -90.0f : 90.0f;
		else angX = -agk::ATan( vy / vxz );
	}

	MakeFromEulerYXZ( angX, angY, roll );
}

void AGKQuaternion::LookAtDiscrete( float vx, float vy, float vz, float roll, float limit )
{
	float angY = agk::ATan2( vx, vz );
	if ( limit > 0.0001f ) angY = agk::Round( angY / limit ) * limit;
	float angX;

	if ( agk::Abs(vy) < SMALL_FLOAT )
	{
		angX = 0;
	}
	else
	{
		float vxz = agk::Sqrt( vx*vx + vz*vz );
		if ( agk::Abs(vxz) < SMALL_FLOAT ) angX = vy > 0 ? -90.0f : 90.0f;
		else 
		{
			angX = -agk::ATan( vy / vxz );
			if ( limit > 0.0001f ) angX = agk::Round( angX / limit ) * limit;
		}
	}

	MakeFromEulerYXZ( angX, angY, roll );
}

void AGKQuaternion::MakeFromEulerYX( float xang, float yang )
{
	float sx = agk::Sin( xang/2.0f );
	float cx = agk::Cos( xang/2.0f );

	float sy = agk::Sin( yang/2.0f );
	float cy = agk::Cos( yang/2.0f );

	w = cy*cx;
	x = cy*sx;
	y = sy*cx;
	z = -sy*sx;
}

void AGKQuaternion::MakeFromEulerYXZ( float xang, float yang, float zang )
{
	float sx = agk::Sin( xang/2.0f );
	float sy = agk::Sin( yang/2.0f );
	float sz = agk::Sin( zang/2.0f );

	float cx = agk::Cos( xang/2.0f );
	float cy = agk::Cos( yang/2.0f );
	float cz = agk::Cos( zang/2.0f );

	w = cy*cx*cz + sy*sx*sz;
	x = cy*sx*cz + sy*cx*sz;
	y = sy*cx*cz - cy*sx*sz;
	z = cy*cx*sz - sy*sx*cz;
}

void AGKQuaternion::MakeFromAxisAngle( float vx, float vy, float vz, float ang )
{
	float length = vx*vx + vy*vy + vz*vz;
	if ( length < SMALL_FLOAT )
	{
		Identity();
		return;
	}

	if ( length != 1 )
	{
		length = agk::Sqrt( length );
		vx = vx / length;
		vy = vy / length;
		vz = vz / length;
	}

	float c = agk::Cos( ang/2.0f );
	float s = agk::Sin( ang/2.0f );

	w = c;
	x = vx * s;
	y = vy * s;
	z = vz * s;
}

void AGKQuaternion::MakeFromMatrix( const AGKMatrix3 &rot )
{
	float s = 0;

	float det = 1 + rot.mat[0][0] + rot.mat[1][1] + rot.mat[2][2];
	if ( det > SMALL_FLOAT )
	{
		s = agk::Sqrt( det ) * 2;
		w = 0.25f * s;
		x = ( rot.mat[1][2] - rot.mat[2][1] ) / s;
		y = ( rot.mat[2][0] - rot.mat[0][2] ) / s;
		z = ( rot.mat[0][1] - rot.mat[1][0] ) / s;
	}
	else
	{
		if ( rot.mat[0][0] > rot.mat[1][1] && rot.mat[0][0] > rot.mat[2][2] ) 
		{
			// Column 0: 
			s = agk::Sqrt( 1 + rot.mat[0][0] - rot.mat[1][1] - rot.mat[2][2] ) * 2;
			w = (rot.mat[1][2] - rot.mat[2][1]) / s;
			x = 0.25f * s;
			y = (rot.mat[0][1] + rot.mat[1][0]) / s;
			z = (rot.mat[2][0] + rot.mat[0][2]) / s;
		} 
		else if (rot.mat[1][1] > rot.mat[2][2]) 
		{
			// Column 1: 
			s = agk::Sqrt( 1 + rot.mat[1][1] - rot.mat[0][0] - rot.mat[2][2] ) * 2;
			w = (rot.mat[2][0] - rot.mat[0][2]) / s;
			x = (rot.mat[0][1] + rot.mat[1][0]) / s;
			y = 0.25f * s;
			z = (rot.mat[1][2] + rot.mat[2][1]) / s;
		} 
		else 
		{
			// Column 2:
			s = agk::Sqrt( 1 + rot.mat[2][2] - rot.mat[0][0] - rot.mat[1][1] ) * 2;
			w = (rot.mat[0][1] - rot.mat[1][0]) / s;
			x = (rot.mat[2][0] + rot.mat[0][2]) / s;
			y = (rot.mat[1][2] + rot.mat[2][1]) / s;
			z = 0.25f * s;
		}
	}

	Normalize();
}

// ************
// 3x3 Matrix
// ************

void AGKMatrix3::Identity()
{
	memset( mat, 0, sizeof(float)*9 );
	mat[0][0] = 1;
	mat[1][1] = 1;
	mat[2][2] = 1;
}

bool AGKMatrix3::IsIdentity() const
{
	if ( mat[0][0] != 1 || mat[1][1] != 1 || mat[2][2] != 1 
	  || mat[0][1] != 0 || mat[0][2] != 0 || mat[1][0] != 0 
	  || mat[1][2] != 0 || mat[2][0] != 0 || mat[2][1] != 0 ) return false;
	else return true;
}

AGKMatrix3& AGKMatrix3::Mult( const AGKMatrix3& other )
{
	if ( other.IsIdentity() ) return *this;
	if ( IsIdentity() )
	{
		memcpy( mat, other.mat, 9*sizeof(float) );
		return *this;
	}

	AGKMatrix3 result;
	result.mat[0][0] = mat[0][0]*other.mat[0][0] + mat[0][1]*other.mat[1][0] + mat[0][2]*other.mat[2][0];
	result.mat[1][0] = mat[1][0]*other.mat[0][0] + mat[1][1]*other.mat[1][0] + mat[1][2]*other.mat[2][0];
	result.mat[2][0] = mat[2][0]*other.mat[0][0] + mat[2][1]*other.mat[1][0] + mat[2][2]*other.mat[2][0];

	result.mat[0][1] = mat[0][0]*other.mat[0][1] + mat[0][1]*other.mat[1][1] + mat[0][2]*other.mat[2][1];
	result.mat[1][1] = mat[1][0]*other.mat[0][1] + mat[1][1]*other.mat[1][1] + mat[1][2]*other.mat[2][1];
	result.mat[2][1] = mat[2][0]*other.mat[0][1] + mat[2][1]*other.mat[1][1] + mat[2][2]*other.mat[2][1];

	result.mat[0][2] = mat[0][0]*other.mat[0][2] + mat[0][1]*other.mat[1][2] + mat[0][2]*other.mat[2][2];
	result.mat[1][2] = mat[1][0]*other.mat[0][2] + mat[1][1]*other.mat[1][2] + mat[1][2]*other.mat[2][2];
	result.mat[2][2] = mat[2][0]*other.mat[0][2] + mat[2][1]*other.mat[1][2] + mat[2][2]*other.mat[2][2];

	memcpy( mat, result.mat, 9*sizeof(float) );
	return *this;
}

AGKMatrix3& AGKMatrix3::Mult( const AGKMatrix4& other )
{
	if ( other.IsIdentity() ) return *this;
	if ( IsIdentity() )
	{
		mat[0][0] = other.mat[0][0];
		mat[0][1] = other.mat[0][1];
		mat[0][2] = other.mat[0][2];

		mat[1][0] = other.mat[1][0];
		mat[1][1] = other.mat[1][1];
		mat[1][2] = other.mat[1][2];

		mat[2][0] = other.mat[2][0];
		mat[2][1] = other.mat[2][1];
		mat[2][2] = other.mat[2][2];
		return *this;
	}

	AGKMatrix3 result;
	result.mat[0][0] = mat[0][0]*other.mat[0][0] + mat[0][1]*other.mat[1][0] + mat[0][2]*other.mat[2][0];
	result.mat[1][0] = mat[1][0]*other.mat[0][0] + mat[1][1]*other.mat[1][0] + mat[1][2]*other.mat[2][0];
	result.mat[2][0] = mat[2][0]*other.mat[0][0] + mat[2][1]*other.mat[1][0] + mat[2][2]*other.mat[2][0];

	result.mat[0][1] = mat[0][0]*other.mat[0][1] + mat[0][1]*other.mat[1][1] + mat[0][2]*other.mat[2][1];
	result.mat[1][1] = mat[1][0]*other.mat[0][1] + mat[1][1]*other.mat[1][1] + mat[1][2]*other.mat[2][1];
	result.mat[2][1] = mat[2][0]*other.mat[0][1] + mat[2][1]*other.mat[1][1] + mat[2][2]*other.mat[2][1];

	result.mat[0][2] = mat[0][0]*other.mat[0][2] + mat[0][1]*other.mat[1][2] + mat[0][2]*other.mat[2][2];
	result.mat[1][2] = mat[1][0]*other.mat[0][2] + mat[1][1]*other.mat[1][2] + mat[1][2]*other.mat[2][2];
	result.mat[2][2] = mat[2][0]*other.mat[0][2] + mat[2][1]*other.mat[1][2] + mat[2][2]*other.mat[2][2];

	memcpy( mat, result.mat, 9*sizeof(float) );
	return *this;
}

AGKMatrix3& AGKMatrix3::Transpose()
{
	float temp = mat[0][1];
	mat[0][1] = mat[1][0];
	mat[1][0] = temp;

	temp = mat[0][2];
	mat[0][2] = mat[2][0];
	mat[2][0] = temp;

	temp = mat[1][2];
	mat[1][2] = mat[2][1];
	mat[2][1] = temp;

	return *this;
}

float AGKMatrix3::GetEulerX()
{
	return -agk::ASin( mat[2][1] );
}

float AGKMatrix3::GetEulerY()
{
	float angY = agk::ATan2( mat[2][0], mat[2][2] ); 
	return angY;
}

float AGKMatrix3::GetEulerZ()
{
	float angZ = agk::ATan2( mat[0][1], mat[1][1] );
	return angZ;
}


void AGKMatrix3::MakeRotationX( float ang )
{
	float c = agk::Cos( ang );
	float s = agk::Sin( ang );

	Identity();
	mat[1][1] = c;
	mat[2][1] = -s;
	mat[1][2] = s;
	mat[2][2] = c;
}

void AGKMatrix3::MakeRotationY( float ang )
{
	float c = agk::Cos( ang );
	float s = agk::Sin( ang );

	Identity();
	mat[0][0] = c;
	mat[2][0] = s;
	mat[0][2] = -s;
	mat[2][2] = c;
}

void AGKMatrix3::MakeRotationZ( float ang )
{
	float c = agk::Cos( ang );
	float s = agk::Sin( ang );

	Identity();
	mat[0][0] = c;
	mat[1][0] = -s;
	mat[0][1] = s;
	mat[1][1] = c;
}

void AGKMatrix3::MakeFromEulerYXZ( float xang, float yang, float zang )
{
	float sx = agk::Sin( xang );
	float sy = agk::Sin( yang );
	float sz = agk::Sin( zang );

	float cx = agk::Cos( xang );
	float cy = agk::Cos( yang );
	float cz = agk::Cos( zang );

	mat[0][0] = cz*cy + sx*sy*sz;
	mat[1][0] = sy*cz*sx - sz*cy;
	mat[2][0] = sy*cx;
	
	mat[0][1] = sz*cx;
	mat[1][1] = cx*cz;
	mat[2][1] = -sx;

	mat[0][2] = sz*sx*cy - cz*sy;
	mat[1][2] = sz*sy + cy*cz*sx;
	mat[2][2] = cx*cy;

	/*
	AGKMatrix3 temp;

	if ( zang != 0 ) MakeRotationZ( zang );
	else Identity();

	if ( xang != 0 )
	{
		temp.MakeRotationX( xang );
		Mult( temp );
	}

	if ( yang != 0 )
	{
		temp.MakeRotationY( yang );
		Mult( temp );
	}
	*/
}

void AGKMatrix3::MakeFromAxisAngle( float x, float y, float z, float ang )
{
	float c = agk::Cos( ang );
	float s = agk::Sin( ang );

	float length = x*x + y*y + z*z;
	if ( length == 0 ) return;
	if ( length != 1 )
	{
		length = agk::Sqrt( length );
		x = x / length;
		y = y / length;
		z = z / length;
	}

	mat[0][0] = c + x*x*(1-c);
	mat[1][0] = x*y*(1-c) - z*s;
	mat[2][0] = x*z*(1-c) + y*s;

	mat[0][1] = x*y*(1-c) + z*s;
	mat[1][1] = c + y*y*(1-c);
	mat[2][1] = y*z*(1-c) - x*s;

	mat[0][2] = x*z*(1-c) - y*s;
	mat[1][2] = z*y*(1-c) + x*s;
	mat[2][2] = c + z*z*(1-c);
}

void AGKMatrix3::MakeFromQuaternion( float w, float x, float y, float z )
{
	float xx = x*x;
	float yy = y*y;
	float zz = z*z;
	float xy = x*y;
	float wz = w*z;
	float xz = x*z;
	float wy = w*y;
	float yz = y*z;
	float wx = w*x;

	mat[0][0] = 1 - 2*(yy + zz);	// X Tangent
	mat[1][0] = 2*(xy - wz);		// X Normal
	mat[2][0] = 2*(xz + wy);		// X Binormal
	
	mat[0][1] = 2*(xy + wz);		// Y Tangent
	mat[1][1] = 1 - 2*(xx + zz);	// Y Normal
	mat[2][1] = 2*(yz - wx);		// Y Binormal

	mat[0][2] = 2*(xz - wy);		// Z Tangent
	mat[1][2] = 2*(yz + wx);		// Z Normal
	mat[2][2] = 1 - 2*(xx + yy);	// Z Binormal
}

void AGKMatrix3::MakeFromAxes( AGKVector &x, AGKVector &y, AGKVector &z )
{
	mat[0][0] = x.x;
	mat[0][1] = x.y;
	mat[0][2] = x.z;

	mat[1][0] = y.x;
	mat[1][1] = y.y;
	mat[1][2] = y.z;

	mat[2][0] = z.x;
	mat[2][1] = z.y;
	mat[2][2] = z.z;
}

void AGKMatrix3::MakeWorld( const AGKQuaternion &rot, const AGKVector &scale )
{
	float xx = rot.x*rot.x;
	float yy = rot.y*rot.y;
	float zz = rot.z*rot.z;
	float xy = rot.x*rot.y;
	float wz = rot.w*rot.z;
	float xz = rot.x*rot.z;
	float wy = rot.w*rot.y;
	float yz = rot.y*rot.z;
	float wx = rot.w*rot.x;

	mat[0][0] = (1 - 2*(yy + zz)) * scale.x;
	mat[1][0] = (2*(xy - wz)) * scale.y;
	mat[2][0] = (2*(xz + wy)) * scale.z;
	
	mat[0][1] = (2*(xy + wz)) * scale.x;
	mat[1][1] = (1 - 2*(xx + zz)) * scale.y;
	mat[2][1] = (2*(yz - wx)) * scale.z;

	mat[0][2] = (2*(xz - wy)) * scale.x;
	mat[1][2] = (2*(yz + wx)) * scale.y;
	mat[2][2] = (1 - 2*(xx + yy)) * scale.z;
}

void AGKMatrix3::MakeInvWorld( const AGKQuaternion &rot, const AGKVector &scale )
{
	float xx = rot.x*rot.x;
	float yy = rot.y*rot.y;
	float zz = rot.z*rot.z;
	float xy = rot.x*rot.y;
	float wz = rot.w*rot.z;
	float xz = rot.x*rot.z;
	float wy = rot.w*rot.y;
	float yz = rot.y*rot.z;
	float wx = rot.w*rot.x;

	mat[0][0] = (1 - 2*(yy + zz)) / scale.x;
	mat[0][1] = (2*(xy - wz)) / scale.y;
	mat[0][2] = (2*(xz + wy)) / scale.z;
	
	mat[1][0] = (2*(xy + wz)) / scale.x;
	mat[1][1] = (1 - 2*(xx + zz)) / scale.y;
	mat[1][2] = (2*(yz - wx)) / scale.z;

	mat[2][0] = (2*(xz - wy)) / scale.x;
	mat[2][1] = (2*(yz + wx)) / scale.y;
	mat[2][2] = (1 - 2*(xx + yy)) / scale.z;
}

// *****************
// 4x4 Matrix
// *****************

AGKMatrix4& AGKMatrix4::operator =( const AGKMatrix3& o ) 
{ 
	memcpy(mat,o.mat,sizeof(float)*3); 
	memcpy(&mat[1][0],&o.mat[1][0],sizeof(float)*3); 
	memcpy(&mat[2][0],&o.mat[2][0],sizeof(float)*3); 
	memset(&mat[3][0],0,sizeof(float)*3);
	mat[0][3] = 0; mat[1][3] = 0; mat[2][3] = 0; mat[3][3] = 1;
	return *this; 
}

void AGKMatrix4::Identity()
{
	memset( mat, 0, sizeof(float)*16 );
	mat[0][0] = 1;
	mat[1][1] = 1;
	mat[2][2] = 1;
	mat[3][3] = 1;
}

bool AGKMatrix4::IsIdentity() const
{
	if ( mat[0][0] != 1 || mat[1][1] != 1 || mat[2][2] != 1 || mat[3][3] != 1 
	  || mat[0][1] != 0 || mat[0][2] != 0 || mat[0][3] != 0 
	  || mat[1][0] != 0 || mat[1][2] != 0 || mat[1][3] != 0 
	  || mat[2][0] != 0 || mat[2][1] != 0 || mat[2][3] != 0 
	  || mat[3][0] != 0 || mat[3][1] != 0 || mat[3][2] != 0 ) return false;
	else return true;
}

AGKMatrix4& AGKMatrix4::Mult( const AGKMatrix4& other )
{
	if ( other.IsIdentity() ) return *this;
	if ( IsIdentity() )
	{
		memcpy( mat, other.mat, 16*sizeof(float) );
		return *this;
	}

	AGKMatrix4 result;
	result.mat[0][0] = mat[0][0]*other.mat[0][0] + mat[0][1]*other.mat[1][0] + mat[0][2]*other.mat[2][0] + mat[0][3]*other.mat[3][0];
	result.mat[1][0] = mat[1][0]*other.mat[0][0] + mat[1][1]*other.mat[1][0] + mat[1][2]*other.mat[2][0] + mat[1][3]*other.mat[3][0];
	result.mat[2][0] = mat[2][0]*other.mat[0][0] + mat[2][1]*other.mat[1][0] + mat[2][2]*other.mat[2][0] + mat[2][3]*other.mat[3][0];
	result.mat[3][0] = mat[3][0]*other.mat[0][0] + mat[3][1]*other.mat[1][0] + mat[3][2]*other.mat[2][0] + mat[3][3]*other.mat[3][0];

	result.mat[0][1] = mat[0][0]*other.mat[0][1] + mat[0][1]*other.mat[1][1] + mat[0][2]*other.mat[2][1] + mat[0][3]*other.mat[3][1];
	result.mat[1][1] = mat[1][0]*other.mat[0][1] + mat[1][1]*other.mat[1][1] + mat[1][2]*other.mat[2][1] + mat[1][3]*other.mat[3][1];
	result.mat[2][1] = mat[2][0]*other.mat[0][1] + mat[2][1]*other.mat[1][1] + mat[2][2]*other.mat[2][1] + mat[2][3]*other.mat[3][1];
	result.mat[3][1] = mat[3][0]*other.mat[0][1] + mat[3][1]*other.mat[1][1] + mat[3][2]*other.mat[2][1] + mat[3][3]*other.mat[3][1];

	result.mat[0][2] = mat[0][0]*other.mat[0][2] + mat[0][1]*other.mat[1][2] + mat[0][2]*other.mat[2][2] + mat[0][3]*other.mat[3][2];
	result.mat[1][2] = mat[1][0]*other.mat[0][2] + mat[1][1]*other.mat[1][2] + mat[1][2]*other.mat[2][2] + mat[1][3]*other.mat[3][2];
	result.mat[2][2] = mat[2][0]*other.mat[0][2] + mat[2][1]*other.mat[1][2] + mat[2][2]*other.mat[2][2] + mat[2][3]*other.mat[3][2];
	result.mat[3][2] = mat[3][0]*other.mat[0][2] + mat[3][1]*other.mat[1][2] + mat[3][2]*other.mat[2][2] + mat[3][3]*other.mat[3][2];

	result.mat[0][3] = mat[0][0]*other.mat[0][3] + mat[0][1]*other.mat[1][3] + mat[0][2]*other.mat[2][3] + mat[0][3]*other.mat[3][3];
	result.mat[1][3] = mat[1][0]*other.mat[0][3] + mat[1][1]*other.mat[1][3] + mat[1][2]*other.mat[2][3] + mat[1][3]*other.mat[3][3];
	result.mat[2][3] = mat[2][0]*other.mat[0][3] + mat[2][1]*other.mat[1][3] + mat[2][2]*other.mat[2][3] + mat[2][3]*other.mat[3][3];
	result.mat[3][3] = mat[3][0]*other.mat[0][3] + mat[3][1]*other.mat[1][3] + mat[3][2]*other.mat[2][3] + mat[3][3]*other.mat[3][3];

	memcpy( mat, result.mat, 16*sizeof(float) );
	return *this;
}

void AGKMatrix4::MakeWorld( const AGKMatrix3 &rot, const AGKVector &pos, const AGKVector &scale )
{
	mat[0][0] = scale.x * rot.mat[0][0];
	mat[1][0] = scale.y * rot.mat[1][0];
	mat[2][0] = scale.z * rot.mat[2][0];
	mat[3][0] = pos.x;
	
	mat[0][1] = scale.x * rot.mat[0][1];
	mat[1][1] = scale.y * rot.mat[1][1];
	mat[2][1] = scale.z * rot.mat[2][1];
	mat[3][1] = pos.y;

	mat[0][2] = scale.x * rot.mat[0][2];
	mat[1][2] = scale.y * rot.mat[1][2];
	mat[2][2] = scale.z * rot.mat[2][2];
	mat[3][2] = pos.z;

	mat[0][3] = 0;
	mat[1][3] = 0;
	mat[2][3] = 0;
	mat[3][3] = 1;
}

void AGKMatrix4::MakeWorld( const AGKQuaternion &rot, const AGKVector &pos, const AGKVector &scale )
{
	float xx = rot.x*rot.x;
	float yy = rot.y*rot.y;
	float zz = rot.z*rot.z;
	float xy = rot.x*rot.y;
	float wz = rot.w*rot.z;
	float xz = rot.x*rot.z;
	float wy = rot.w*rot.y;
	float yz = rot.y*rot.z;
	float wx = rot.w*rot.x;

	mat[0][0] = scale.x * (1 - 2*(yy + zz));
	mat[1][0] = scale.y * (2*(xy - wz));
	mat[2][0] = scale.z * (2*(xz + wy));
	mat[3][0] = pos.x;
	
	mat[0][1] = scale.x * (2*(xy + wz));
	mat[1][1] = scale.y * (1 - 2*(xx + zz));
	mat[2][1] = scale.z * (2*(yz - wx));
	mat[3][1] = pos.y;

	mat[0][2] = scale.x * (2*(xz - wy));
	mat[1][2] = scale.y * (2*(yz + wx));
	mat[2][2] = scale.z * (1 - 2*(xx + yy));
	mat[3][2] = pos.z;

	mat[0][3] = 0;
	mat[1][3] = 0;
	mat[2][3] = 0;
	mat[3][3] = 1;
}

void AGKMatrix4::MakeInvWorld( const AGKMatrix3 &rot, const AGKVector &pos, const AGKVector &scale )
{
	mat[0][0] = rot.mat[0][0] / scale.x;
	mat[1][0] = rot.mat[0][1] / scale.x;
	mat[2][0] = rot.mat[0][2] / scale.x;
	mat[3][0] = 0;
	
	mat[0][1] = rot.mat[1][0] / scale.y;
	mat[1][1] = rot.mat[1][1] / scale.y;
	mat[2][1] = rot.mat[1][2] / scale.y;
	mat[3][1] = 0;

	mat[0][2] = rot.mat[2][0] / scale.z;
	mat[1][2] = rot.mat[2][1] / scale.z;
	mat[2][2] = rot.mat[2][2] / scale.z;
	mat[3][2] = 0;

	mat[0][3] = 0;
	mat[1][3] = 0;
	mat[2][3] = 0;
	mat[3][3] = 1;

	AGKVector pos2( -pos.x, -pos.y, -pos.z );
	pos2.Mult( *this );

	mat[3][0] = pos2.x;
	mat[3][1] = pos2.y;
	mat[3][2] = pos2.z;
}

void AGKMatrix4::MakeInvWorld( const AGKQuaternion &rot, const AGKVector &pos, const AGKVector &scale )
{
	float xx = rot.x*rot.x;
	float yy = rot.y*rot.y;
	float zz = rot.z*rot.z;
	float xy = rot.x*rot.y;
	float wz = rot.w*rot.z;
	float xz = rot.x*rot.z;
	float wy = rot.w*rot.y;
	float yz = rot.y*rot.z;
	float wx = rot.w*rot.x;

	mat[0][0] = (1 - 2*(yy + zz)) / scale.x;
	mat[0][1] = (2*(xy - wz)) / scale.y;
	mat[0][2] = (2*(xz + wy)) / scale.z;
	mat[0][3] = 0;
	
	mat[1][0] = (2*(xy + wz)) / scale.x;
	mat[1][1] = (1 - 2*(xx + zz)) / scale.y;
	mat[1][2] = (2*(yz - wx)) / scale.z;
	mat[1][3] = 0;

	mat[2][0] = (2*(xz - wy)) / scale.x;
	mat[2][1] = (2*(yz + wx)) / scale.y;
	mat[2][2] = (1 - 2*(xx + yy)) / scale.z;
	mat[2][3] = 0;

	mat[3][0] = 0;
	mat[3][1] = 0;
	mat[3][2] = 0;
	mat[3][3] = 1;

	AGKVector pos2( -pos.x, -pos.y, -pos.z );
	pos2.Mult( *this );

	mat[3][0] = pos2.x;
	mat[3][1] = pos2.y;
	mat[3][2] = pos2.z;
}

void AGKMatrix4::MakeProj( float fFov, float fAspect, float fNear, float fFar, float fWidth )
{
	//float modifier = -1.0f; // righthand
	float modifier = 1.0f; // lefthand

	Renderer *pRenderer = agk::GetRenderer();

	memset( mat, 0, sizeof(float)*16 );

	if ( fFov == 0 )
	{
		if ( fFar < 0 ) fFar = 1000000000; // can't use infinity
		mat[0][0] = 1 / fWidth;
		mat[1][1] = fAspect / fWidth;
		mat[3][3] = 1;

		if ( !pRenderer->IsClipSpaceZeroOne() )
		{
			// -1 to 1
			mat[2][2] = (2/(fFar - fNear)) * modifier;
			mat[3][2] = -((fFar + fNear)/(fFar - fNear));
		}
		else
		{
			if ( !pRenderer->IsInvertedDepth() )
			{
				// 0 to 1
				mat[2][2] = (1/(fFar - fNear)) * modifier;
				mat[3][2] = -fNear/(fFar - fNear);
			}
			else
			{
				// 1 to 0
				mat[2][2] = (-1/(fFar - fNear)) * modifier;
				mat[3][2] = fFar/(fFar - fNear);
			}
		}
	}
	else
	{
		// horizontal FOV
		float fInvTanFOV = 1.0f / agk::Tan( fFov / 2.0f );
		mat[0][0] = fInvTanFOV;
		mat[1][1] = fAspect * fInvTanFOV;
		mat[2][3] = modifier;

		if ( !pRenderer->IsClipSpaceZeroOne() )
		{
			// -1 to 1
			mat[2][2] = ((fFar + fNear)/(fFar - fNear)) * modifier;
			mat[3][2] = -((2.0f * fFar * fNear)/(fFar - fNear));
		}
		else
		{
			if ( !pRenderer->IsInvertedDepth() )
			{
				// 0 to 1
				mat[2][2] = (fFar/(fFar - fNear)) * modifier;
				mat[3][2] = -(fFar * fNear)/(fFar - fNear);
			}
			else
			{
				if ( fFar < 0 )
				{
					// 1 to 0, far=infinity
					mat[3][2] = fNear;
				}
				else
				{
					// 1 to 0
					mat[2][2] = (fNear/(fNear - fFar)) * modifier;
					mat[3][2] = -(fFar * fNear)/(fNear - fFar);
				}				
			}
		}
	}

	if ( pRenderer->IsTopLeftOrigin() )
	{
		mat[1][1] = -mat[1][1];
		mat[2][1] = -mat[2][1];
	}
}

void AGKMatrix4::MakeProjOffCenter( int iOrtho, float fLeft, float fRight, float fTop, float fBottom, float fNear, float fFar )
{
	//float modifier = -1.0f; // righthand
	float modifier = 1.0f; // lefthand

	Renderer *pRenderer = agk::GetRenderer();

	memset( mat, 0, sizeof(float)*16 );

	if ( iOrtho != 0 )
	{
		mat[0][0] = 2/(fRight-fLeft);
		mat[1][1] = 2/(fTop-fBottom);
		mat[3][0] = -((fRight + fLeft)/(fRight - fLeft));
		mat[3][1] = -((fTop + fBottom)/(fTop - fBottom));
		mat[3][3] = 1;

		if ( !pRenderer->IsClipSpaceZeroOne() )
		{
			// -1 to 1
			mat[2][2] = (2/(fFar - fNear)) * modifier;
			mat[3][2] = -((fFar + fNear)/(fFar - fNear));
		}
		else
		{
			if ( !pRenderer->IsInvertedDepth() )
			{
				// 0 to 1
				mat[2][2] = (1/(fFar - fNear)) * modifier;
				mat[3][2] = -fNear/(fFar - fNear);
			}
			else
			{
				// 1 to 0
				mat[2][2] = (-1/(fFar - fNear)) * modifier;
				mat[3][2] = fFar/(fFar - fNear);
			}
		}
	}
	else
	{
		mat[0][0] = 2*fNear/(fRight-fLeft);
		mat[1][1] = 2*fNear/(fTop-fBottom);
		mat[2][0] = (fRight + fLeft)/(fRight - fLeft) * -modifier; // negative for left hand
		mat[2][1] = (fTop + fBottom)/(fTop-fBottom) * -modifier; // negative for left hand
		mat[2][3] = modifier;

		if ( !pRenderer->IsClipSpaceZeroOne() )
		{
			// -1 to 1
			mat[2][2] = ((fFar + fNear)/(fFar - fNear)) * modifier;
			mat[3][2] = -((2.0f * fFar * fNear)/(fFar - fNear));
		}
		else
		{
			if ( !pRenderer->IsInvertedDepth() )
			{
				// 0 to 1
				mat[2][2] = (fFar/(fFar - fNear)) * modifier;
				mat[3][2] = -(fFar * fNear)/(fFar - fNear);
			}
			else
			{
				if ( fFar < 0 )
				{
					// 1 to 0, far=infinity
					mat[3][2] = fNear;
				}
				else
				{
					// 1 to 0
					mat[2][2] = (fNear/(fNear - fFar)) * modifier;
					mat[3][2] = -(fFar * fNear)/(fNear - fFar);
				}				
			}
		}
	}

	if ( pRenderer->IsTopLeftOrigin() )
	{
		mat[1][1] = -mat[1][1];
		mat[2][1] = -mat[2][1];
	}
}

void AGKMatrix4::MakeOrtho( float l, float r, float b, float t, float n, float f )
{
	Renderer *pRenderer = agk::GetRenderer();

	memset( mat, 0, sizeof(float)*16 );
	mat[0][0] = 2 / (r-l);
	mat[1][1] = 2 / (t-b);
	mat[3][0] = -((r+l)/(r-l));
	mat[3][1] = -((t+b)/(t-b));
	mat[3][3] = 1;

	if ( pRenderer->IsTopLeftOrigin() )
	{
		mat[1][1] = -mat[1][1];
		mat[3][1] = -mat[3][1];
	}

	if ( !pRenderer->IsClipSpaceZeroOne() )
	{
		// -1 to 1
		mat[2][2] = 2 / (f - n);
		mat[3][2] = -(f + n) / (f - n);
	}
	else
	{
		if ( !pRenderer->IsInvertedDepth() )
		{
			// 0 to 1
			mat[2][2] = 1 / (f - n);
			mat[3][2] = -n / (f - n);
		}
		else
		{
			// 1 to 0
			mat[2][2] = -1 / (f - n);
			mat[3][2] = f / (f - n);
		}
	}
}