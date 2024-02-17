
#include "stdafx.h"
#include "LMTexNode.h"

LMTexNode::LMTexNode( )
{
	iStartU = 0; iStartV = 0;
	iSizeU = 0; iSizeV = 0;

	bFilled = false;
	bFinalised = false;

	pLeftNode = 0;
	pRightNode = 0;
}

LMTexNode::LMTexNode( int startU, int startV, int sizeU, int sizeV )
{
	iStartU = startU;
	iStartV = startV;
	iSizeU = sizeU;
	iSizeV = sizeV;

	bFilled = false;
	bFinalised = false;

	pLeftNode = 0;
	pRightNode = 0;
}

LMTexNode::~LMTexNode( )
{
	if ( pLeftNode ) delete pLeftNode;
	if ( pRightNode ) delete pRightNode;
}

int LMTexNode::GetStartU( )
{
	return iStartU;
}

int LMTexNode::GetStartV( )
{
	return iStartV;
}

void LMTexNode::Finalise( )
{
	bFinalised = true;

	if ( bFilled )
	{
		pLeftNode->Finalise( );
		pRightNode->Finalise( );
	}
}

bool LMTexNode::IsFinalised( )
{
	return bFinalised;
}

LMTexNode* LMTexNode::AddImage( int width, int height )
{
	if ( bFilled )
	{
		LMTexNode *pFoundNode = pLeftNode->AddImage ( width, height );

		if ( pFoundNode ) return pFoundNode;
		else return pRightNode->AddImage ( width, height );
	}
	else
	{
		if ( iSizeU <= 0 || iSizeV <= 0 ) return 0;

		if ( width > iSizeU || height > iSizeV ) return 0;
		else 
		{
			if ( width/(float)iSizeU > height/(float)iSizeV )
			{
				pLeftNode = new LMTexNode( iStartU + width, iStartV, iSizeU - width, height );
				pRightNode = new LMTexNode( iStartU, iStartV + height, iSizeU, iSizeV - height );
			}
			else
			{
				pLeftNode = new LMTexNode( iStartU, iStartV + height, width, iSizeV - height );
				pRightNode = new LMTexNode( iStartU + width, iStartV, iSizeU - width, iSizeV );
			}

			bFilled = true;

			return this;
		}
	}
}