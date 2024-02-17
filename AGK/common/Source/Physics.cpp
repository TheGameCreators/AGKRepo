#include "agk.h"

using namespace AGK;

int Point2D::Count( const Point2D *pPoints )
{
	int count = 0;
	const Point2D *pPoint = pPoints;
	while ( pPoint )
	{
		count++;
		pPoint = pPoint->pNext;
	}

	return count;
}

Point2D* Physics::ConvexHull2D( int width, int height, unsigned char* pixels )
{
	Point2D *pPoints = 0;

	int currWidth = width;
	int currHeight = height;
	int x = 0;
	int y = 0;
	int threshold = 1;
	int index = 0;
	int i = 0;
	int j = 0;

	/*
	uString sLine("");
	for ( int k = 0; k < 50; k++ )
	{
		index = k + 87*width;
		sLine.AppendUInt(  pixels[ index ] ).Append( "," );
	}

	agk::Error( sLine );
	*/

	while ( currWidth > x || currHeight > y )
	{
		// move clockwise around the border of the pixel block reducing the border size by one each time
		// top left to top right
		for ( i = x; i < currWidth; i++ )
		{
			index = i + j*width;
			if ( pixels[ index ] >= threshold ) ConvexHullAddPoint( pPoints, (float) i, (float) j );
		}

		// for loop increments after passing boundary
		i--;

		// top right to bottom right
		for ( j = y+1; j < currHeight; j++ )
		{
			index = i + j*width;
			if ( pixels[ index ] >= threshold ) ConvexHullAddPoint( pPoints, (float) i, (float) j );
		}

		j--;

		// bottom right to bottom left
		for ( i = currWidth-2; i >= x; i-- )
		{
			index = i + j*width;
			if ( pixels[ index ] >= threshold ) ConvexHullAddPoint( pPoints, (float) i, (float) j );
		}

		i++;

		// bottom left to top left
		for ( j = currHeight-2; j > y; j-- )
		{
			index = i + j*width;
			if ( pixels[ index ] >= threshold ) ConvexHullAddPoint( pPoints, (float) i, (float) j );
		}

		j++;

		if ( currWidth > x )
		{
			x++;
			currWidth--;
		}

		if ( currHeight > y )
		{
			y++;
			currHeight--;
		}
	}

	if ( !pPoints ) return pPoints; // 0 points
	if ( !pPoints->pNext ) return pPoints; // 1 point
	if ( !pPoints->pNext->pNext ) return pPoints; // 2 points

	// remove any very short line segments
	ConvexHullCleanPoints( pPoints, 1 );

	if ( !pPoints ) return pPoints; // 0 points
	if ( !pPoints->pNext ) return pPoints; // 1 point
	if ( !pPoints->pNext->pNext ) return pPoints; // 2 points

	// reduce to maximum m_iMaxPolygonPoints points for Box2D
	ConvexHullReducePoints( pPoints, agk::m_iMaxPolygonPoints );

	return pPoints;
}

void Physics::ConvexHullCleanPoints( Point2D * &pPoints, float min )
{
	Point2D *pLastLast = pPoints;
	Point2D *pLast = pPoints->pNext;
	Point2D *pCurr = pPoints->pNext->pNext;
	float vx,vy, vx2,vy2, dotp, length, dist;
	while ( pCurr )
	{
		// vector from pLastLast to pCurr rotated 90 degrees anti-clockwise
		vx = pLastLast->y - pCurr->y;
		vy = pCurr->x - pLastLast->x;

		// vector from pLastLast to pLast
		vx2 = pLast->x - pLastLast->x;
		vy2 = pLast->y - pLastLast->y;

		// get the distance of the middle point from an imaginary line joining the two either side of it
		dotp = vx*vx2 + vy*vy2;
		length = agk::Sqrt( vx*vx + vy*vy );
		if ( length > 0.0001f )
		{
			dist = dotp / length;
			if ( agk::Abs(dist) < min )
			{
				// distance is less than threshold, delete pLast
				pLastLast->pNext = pCurr;
				delete pLast;

				pLast = pCurr;
				pCurr = pCurr->pNext;
				continue;
			}
		}
		
		pLastLast = pLast;
		pLast = pCurr;
		pCurr = pCurr->pNext;
	}

	// two more cases required, second to last point, last point, first point. and last, first, second.
	pCurr = pPoints;

	// vector from pLastLast to pCurr rotated 90 degrees anti-clockwise
	vx = pLastLast->y - pCurr->y;
	vy = pCurr->x - pLastLast->x;

	// vector from pLastLast to pLast
	vx2 = pLast->x - pLastLast->x;
	vy2 = pLast->y - pLastLast->y;

	dotp = vx*vx2 + vy*vy2;
	length = agk::Sqrt( vx*vx + vy*vy );
	if ( length > 0.0001f )
	{
		dist = dotp / length;
		if ( agk::Abs(dist) < min )
		{
			// distance is less than threshold, delete pLast
			pLastLast->pNext = 0;
			delete pLast;

			pLast = pCurr;
			pCurr = pCurr->pNext;
		}
		else 
		{
			pLastLast = pLast;
			pLast = pCurr;
			pCurr = pCurr->pNext;
		}
	}
	else 
	{
		pLastLast = pLast;
		pLast = pCurr;
		pCurr = pCurr->pNext;
	}
	
	// final case
	// vector from pLastLast to pCurr rotated 90 degrees anti-clockwise
	vx = pLastLast->y - pCurr->y;
	vy = pCurr->x - pLastLast->x;

	// vector from pLastLast to pLast
	vx2 = pLast->x - pLastLast->x;
	vy2 = pLast->y - pLastLast->y;

	dotp = vx*vx2 + vy*vy2;
	length = agk::Sqrt( vx*vx + vy*vy );
	if ( length > 0.0001f )
	{
		dist = dotp / length;
		if ( agk::Abs(dist) < min )
		{
			// distance is less than threshold, delete pLast (which is now the first point)
			pPoints = pPoints->pNext;
			delete pLast;
		}
	}
}

void Physics::ConvexHullReducePoints( Point2D * &pPoints, int max )
{
	// count points
	int iNumPoints = 0;
	Point2D *pPoint = pPoints;
	while ( pPoint )
	{
		iNumPoints++;
		pPoint = pPoint->pNext;
	}

	if ( iNumPoints <= max ) return;
	int iDelPoints = iNumPoints - max; // need to delete this many points

	while ( iDelPoints > 0 )
	{
		float fMinLength = 1000000.0f; // some suitably large number

		Point2D *pDel = 0;
		Point2D *pLastDel = 0;

		Point2D *pLastLast = pPoints;
		Point2D *pLast = pPoints->pNext;
		Point2D *pCurr = pPoints->pNext->pNext;
		float vx,vy, vx2,vy2, dotp, length, dist;
		while ( pCurr )
		{
			// vector from pLastLast to pCurr rotated 90 degrees anti-clockwise
			vx = pLastLast->y - pCurr->y;
			vy = pCurr->x - pLastLast->x;

			// vector from pLastLast to pLast
			vx2 = pLast->x - pLastLast->x;
			vy2 = pLast->y - pLastLast->y;

			// get the distance of the middle point from an imaginary line joining the two either side of it
			dotp = vx*vx2 + vy*vy2;
			length = agk::Sqrt( vx*vx + vy*vy );
			if ( length > 0.0001f )
			{
				dist = dotp / length;
				if ( agk::Abs(dist) < fMinLength ) 
				{
					fMinLength = agk::Abs(dist);
					pDel = pLast;
					pLastDel = pLastLast;
				}
			}
			
			pLastLast = pLast;
			pLast = pCurr;
			pCurr = pCurr->pNext;
		}

		// two more cases required, second to last point, last point, first point. and last, first, second.
		pCurr = pPoints;

		// vector from pLastLast to pCurr rotated 90 degrees anti-clockwise
		vx = pLastLast->y - pCurr->y;
		vy = pCurr->x - pLastLast->x;

		// vector from pLastLast to pLast
		vx2 = pLast->x - pLastLast->x;
		vy2 = pLast->y - pLastLast->y;

		dotp = vx*vx2 + vy*vy2;
		length = agk::Sqrt( vx*vx + vy*vy );
		if ( length > 0.0001f )
		{
			dist = dotp / length;
			if ( agk::Abs(dist) < fMinLength )
			{
				fMinLength = agk::Abs(dist);
				pDel = pLast;
				pLastDel = pLastLast;
			}
		}
		
		pLastLast = pLast;
		pLast = pCurr;
		pCurr = pCurr->pNext;
			
		// final case
		// vector from pLastLast to pCurr rotated 90 degrees anti-clockwise
		vx = pLastLast->y - pCurr->y;
		vy = pCurr->x - pLastLast->x;

		// vector from pLastLast to pLast
		vx2 = pLast->x - pLastLast->x;
		vy2 = pLast->y - pLastLast->y;

		dotp = vx*vx2 + vy*vy2;
		length = agk::Sqrt( vx*vx + vy*vy );
		if ( length > 0.0001f )
		{
			dist = dotp / length;
			if ( agk::Abs(dist) < fMinLength )
			{
				// distance is less than threshold, delete pLast (which is now the first point)
				fMinLength = agk::Abs(dist);
				pDel = pLast;
				pLastDel = 0;
			}
		}

		if ( pDel == 0 )
		{
#ifdef _AGK_ERROR_CHECK
			agk::Error( "Failed to reduce the number of polygon points to less than 12" );
#endif
			return;
		}

		if ( pLastDel ) pLastDel->pNext = pDel->pNext;
		else pPoints = pDel->pNext;
		delete pDel;
		iDelPoints--;
	}
}

void Physics::ConvexHullAddPoint( Point2D * &pPoints, float x, float y )
{
	// no current points
	if ( !pPoints )
	{
		pPoints = new Point2D();
		pPoints->x = x;
		pPoints->y = y;
		return;
	}

	// one current point
	if ( !pPoints->pNext )
	{
		pPoints->pNext = new Point2D();
		pPoints->pNext->x = x;
		pPoints->pNext->y = y;
		return;
	}

	// find the line segments that the point is outside, delete them and replace them with line segments to the new point
	Point2D* pLastLast = 0;
	Point2D* pLast = pPoints;
	Point2D* pCurr = pPoints->pNext;
	Point2D* pFound = 0;
	bool bAdded = false;
	while( pCurr )
	{
		// vector from pLast to pCurr rotated 90 degrees anti-clockwise
		float vx = pLast->y - pCurr->y;
		float vy = pCurr->x - pLast->x;

		// vector from pLast to the point in question
		float vx2 = x - pLast->x;
		float vy2 = y - pLast->y;

		// if the dot product between the two vectors is positive the point is inside the hull relative to this line segment.
		// zero would be on the line (counts as outside), negative would be outside.
		float dotp = vx*vx2 + vy*vy2;
		float dotp2 = vy*vx2 - vx*vy2;
		// coplaner points are only outside if they are longer than the existing line
		if ( dotp < 0.000001f && (dotp < -0.000001f || dotp2<0 || dotp2>(vx*vx+vy*vy)) ) 
		{
			// outside
			if ( !pFound ) pFound = pLast;
			else
			{
				//delete point as both its sides are covered by the new point
				if ( pLastLast ) pLastLast->pNext = pCurr;
				else pPoints = pCurr;
				delete pLast;
				pLast = pCurr;
				pCurr = pCurr->pNext;
				continue;
			}
		}
		else
		{
			// inside
			if ( pFound )
			{
				// end of deletion, insert new point between pFound and pLast
				Point2D *pNew = new Point2D();
				pNew->x = x;
				pNew->y = y;
				pNew->pNext = pLast;
				pFound->pNext = pNew;
				pFound = 0;
				bAdded = true;
			}
		}

		pLastLast = pLast;
		pLast = pCurr;
		pCurr = pCurr->pNext;
	}

	//last segment from end point back to first point
	pCurr = pPoints;

	// vector from pLast to pCurr rotated 90 degrees anti-clockwise
	float vx = pLast->y - pCurr->y;
	float vy = pCurr->x - pLast->x;

	// vector from pLast to the point in question
	float vx2 = x - pLast->x;
	float vy2 = y - pLast->y;

	// if the dot product between the two vectors is positive the point is inside the hull relative to this line segment.
	// zero would be on the line (counts as inside), negative would be outside.
	float dotp = vx*vx2 + vy*vy2;
	float dotp2 = vy*vx2 - vx*vy2;
	if ( dotp < 0.000001f && (dotp < -0.000001f || dotp2<0 || dotp2>(vx*vx+vy*vy)) ) 
	{
		// outside
		// only add if the point hasn't already been added
		if ( !bAdded )
		{
			if ( !pFound ) 
			{
				// insert new point at end
				Point2D *pNew = new Point2D();
				pNew->x = x;
				pNew->y = y;
				pNew->pNext = 0;
				pLast->pNext = pNew;
			}
			else
			{
				// replace last point with new point as both sides covered by new point
				pLast->x = x;
				pLast->y = y;
			}
		}
	}
	else
	{
		// inside
		if ( pFound )
		{
			if ( bAdded )
			{
				// point already added, delete last point
				pLastLast->pNext = 0;
				delete pLast;
			}
			else
			{
				// end of deletion, insert new point between pFound and pLast
				Point2D *pNew = new Point2D();
				pNew->x = x;
				pNew->y = y;
				pNew->pNext = pLast;
				pFound->pNext = pNew;
			}
		}
	}
}

bool Physics::IsPointInConvexHull( Point2D* pPoints, float x, float y )
{
	// if 0 or 1 points return false
	if ( !pPoints || !pPoints->pNext ) return false;

	// to be inside the hull the point must be inside relative to every line segment
	Point2D* pLast = pPoints;
	Point2D* pCurr = pPoints->pNext;
	while( pCurr )
	{
		// vector from pLast to pCurr rotated 90 degrees anti-clockwise
		float vx = pLast->y - pCurr->y;
		float vy = pCurr->x - pLast->x;

		// vector from pLast to the point in question
		float vx2 = x - pLast->x;
		float vy2 = y - pLast->y;

		// if the dot product between the two vectors is positive the point is inside the hull relative to this line segment.
		// zero would be on the line (counts as inside), negative would be outside.
		float dotp = vx*vx2 + vy*vy2;
		if ( dotp < -0.000001f ) return false;

		pLast = pCurr;
		pCurr = pCurr->pNext;
	}

	// last segment is end point back to first point
	pCurr = pPoints;

	// vector from pLast to pCurr rotated 90 degrees anti-clockwise
	float vx = pLast->y - pCurr->y;
	float vy = pCurr->x - pLast->x;

	// vector from pLast to the point in question
	float vx2 = x - pLast->x;
	float vy2 = y - pLast->y;

	// if the dot product between the two vectors is positive the point is inside the hull relative to this line segment.
	// zero would be on the line (counts as inside), negative would be outside.
	float dotp = vx*vx2 + vy*vy2;
	if ( dotp < -0.000001f ) return false;

	// passed all checks
	return true;
}
