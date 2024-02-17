#include "agk.h"

using namespace AGK;

cNode::cNode() : m_vChildren(0)
{
	m_pParentNode = 0;
	m_iNodeFlags = 0;
	m_scale.Set(1,1,1);
	m_scaleFinal.Set(1,1,1);
}

cNode::~cNode()
{
	RemoveFromParent();
	for( uint32_t i = 0; i < m_vChildren.NumItems(); i++ )
	{
		m_vChildren[ i ]->m_pParentNode = 0;
		m_vChildren[ i ]->NeedsUpdate();
	}
}

void cNode::AddChild( cNode *pChild )
{
	if ( pChild->m_pParentNode ) pChild->RemoveFromParent();
	m_vChildren.AddItem( pChild );
	pChild->m_pParentNode = this;
	pChild->NeedsUpdate();
}

void cNode::RemoveChild( cNode *pChild )
{
	for( uint32_t i = 0; i < m_vChildren.NumItems(); i++ )
	{
		if ( m_vChildren[ i ] == pChild )
		{
			m_vChildren.RemoveIndex( i );
			pChild->m_pParentNode = 0;
			pChild->NeedsUpdate();
			return;
		}
	}
}

void cNode::RemoveFromParent()
{
	if ( !m_pParentNode ) return;
	m_pParentNode->RemoveChild( this );
	m_pParentNode = 0;
	NeedsUpdate();
}

void cNode::RemoveFromDeletingParent()
{
	m_pParentNode = 0;
	NeedsUpdate();
}

void cNode::TransformChildrenByParent()
{
	for( uint32_t i = 0; i < m_vChildren.NumItems(); i++ )
	{
		m_vChildren[ i ]->m_position = (m_rotation * (m_vChildren[ i ]->m_position*m_scale)) + m_position;
		m_vChildren[ i ]->m_rotation = m_rotation * m_vChildren[ i ]->m_rotation;
		m_vChildren[ i ]->m_scale = m_scale * m_vChildren[ i ]->m_scale;
		m_vChildren[ i ]->NeedsUpdate();
	}
}

float cNode::GetLargestWorldScale()
{
	float largestScale = scaleFinal().x;
	float scale = scaleFinal().y;
	if ( scale > largestScale ) largestScale = scale;
	scale = scaleFinal().z;
	if ( scale > largestScale ) largestScale = scale;
	return largestScale;
}

void cNode::MoveLocalX( float amount )
{
	AGKVector axis(amount,0,0);
	axis.MultX( rot() );
	SetNodePosition( pos().x + axis.x, pos().y + axis.y, pos().z + axis.z );
}

void cNode::MoveLocalY( float amount )
{
	AGKVector axis(0,amount,0);
	axis.MultY( rot() );
	SetNodePosition( pos().x + axis.x, pos().y + axis.y, pos().z + axis.z );
}

void cNode::MoveLocalZ( float amount )
{
	AGKVector axis(0,0,amount);
	axis.MultZ( rot() );
	SetNodePosition( pos().x + axis.x, pos().y + axis.y, pos().z + axis.z );
}

void cNode::RotateLocalX( float amount ) 
{ 
	AGKQuaternion q( rot().w, rot().x, rot().y, rot().z );
	q.AddLocalRotation( 0, amount );
	SetNodeRotation( q.w, q.x, q.y, q.z );
}

void cNode::RotateLocalY( float amount ) 
{ 
	AGKQuaternion q( rot().w, rot().x, rot().y, rot().z );
	q.AddLocalRotation( 1, amount ); 
	SetNodeRotation( q.w, q.x, q.y, q.z );
}

void cNode::RotateLocalZ( float amount ) 
{ 
	AGKQuaternion q( rot().w, rot().x, rot().y, rot().z );
	q.AddLocalRotation( 2, amount ); 
	SetNodeRotation( q.w, q.x, q.y, q.z );
}

void cNode::RotateGlobalX( float amount ) 
{ 
	AGKQuaternion q( rot().w, rot().x, rot().y, rot().z );
	q.AddGlobalRotation( 0, amount ); 
	SetNodeRotation( q.w, q.x, q.y, q.z );
}

void cNode::RotateGlobalY( float amount ) 
{ 
	AGKQuaternion q( rot().w, rot().x, rot().y, rot().z );
	q.AddGlobalRotation( 1, amount ); 
	SetNodeRotation( q.w, q.x, q.y, q.z );
}

void cNode::RotateGlobalZ( float amount ) 
{ 
	AGKQuaternion q( rot().w, rot().x, rot().y, rot().z );
	q.AddGlobalRotation( 2, amount ); 
	SetNodeRotation( q.w, q.x, q.y, q.z );
}

void cNode::LookAt( float x, float y, float z, float roll ) 
{ 
	AGKQuaternion q;
	q.LookAt( x-posFinal().x, y-posFinal().y, z-posFinal().z, roll ); 
	
	if ( m_pParentNode )
	{
		// find location rotation that points to world point
		AGKQuaternion q2(m_pParentNode->rotFinal());
		q2.Invert();
		q = q2 * q;
	}

	SetNodeRotation( q.w, q.x, q.y, q.z );
}

void cNode::OverrideWorldPosition( float x, float y, float z ) 
{ 
	m_position.Set(x,y,z); 
	m_positionFinal.Set(x,y,z); 
	m_iNodeFlags |= AGK_NODE_OVERRIDE_POSITION; 
	NeedsUpdate(); 
}

void cNode::OverrideWorldRotation( float w, float x, float y, float z ) 
{ 
	m_rotation.Set(w,x,y,z); 
	m_rotationFinal.Set(w,x,y,z); 
	m_iNodeFlags |= AGK_NODE_OVERRIDE_ROTATION; 
	NeedsUpdate(); 
}

void cNode::OverrideWorldScale( float x, float y, float z ) 
{ 
	m_scale.Set(x,y,z); 
	m_scaleFinal.Set(x,y,z); 
	m_iNodeFlags |= AGK_NODE_OVERRIDE_SCALE; 
	NeedsUpdate(); 
}

void cNode::RemoveWorldOverride()
{
	// set relative transforms so the object stays in its current position during the next update
	if ( !m_pParentNode )
	{
		m_position = m_positionFinal;
		m_rotation = m_rotationFinal;
		m_scale = m_scaleFinal;
	}
	else
	{
		// calculate relative transform from parent, transform absolute points by inverse parent transform
		m_pParentNode->UpdateNode();
		AGKQuaternion q(m_pParentNode->m_rotationFinal);
		q.Invert();
		m_rotation = q * m_rotationFinal;

		AGKVector v(m_positionFinal - m_pParentNode->m_positionFinal);
		m_position = (q * v) / m_pParentNode->m_scaleFinal;

		m_scale = m_scaleFinal / m_pParentNode->m_scaleFinal;
	}

	m_iNodeFlags &= ~(AGK_NODE_OVERRIDE_POSITION | AGK_NODE_OVERRIDE_ROTATION | AGK_NODE_OVERRIDE_SCALE);
	NeedsUpdate();
}

void cNode::SetNodePosition( float x, float y, float z ) 
{ 
	if ( m_iNodeFlags & AGK_NODE_OVERRIDE_POSITION ) return;
	if ( m_position.x == x && m_position.y == y && m_position.z == z ) return; 
	m_position.Set(x,y,z); 
	NeedsUpdate(); 
}

void cNode::SetNodeRotation( float w, float x, float y, float z ) 
{ 
	if ( m_iNodeFlags & AGK_NODE_OVERRIDE_ROTATION ) return;
	if ( m_rotation.w == w && m_rotation.x == x && m_rotation.y == y && m_rotation.z == z ) return; 
	m_rotation.Set(w,x,y,z); 
	NeedsUpdate(); 
}

void cNode::SetNodeScale( float x, float y, float z ) 
{ 
	if ( m_iNodeFlags & AGK_NODE_OVERRIDE_SCALE ) return;
	if ( m_scale.x == x && m_scale.y == y && m_scale.z == z ) return; 
	m_scale.Set(x,y,z); 
	NeedsUpdate(); 
}

const AGKVector& cNode::posFinal() 
{ 
	if ( !m_pParentNode ) return m_position;
	
	UpdateNode(); 
	return m_positionFinal; 
}

const AGKQuaternion& cNode::rotFinal() 
{ 
	if ( !m_pParentNode ) return m_rotation;

	UpdateNode(); 
	return m_rotationFinal; 
}

const AGKVector& cNode::scaleFinal() 
{ 
	if ( !m_pParentNode ) return m_scale;

	UpdateNode(); 
	return m_scaleFinal; 
}

void cNode::NeedsUpdate()
{
	// notify sub objects that something changed, used to flag that world and view matrices needs updates
	// don't push this below the flag check as they could update at different rates
	NeedsUpdateCallback(); 

	if ( (m_iNodeFlags & AGK_NODE_NEEDS_UPDATE) != 0 ) return;
	m_iNodeFlags |= AGK_NODE_NEEDS_UPDATE;
	
	for( uint32_t i = 0; i < m_vChildren.NumItems(); i++ )
	{
		m_vChildren[ i ]->NeedsUpdate();
	}
}

void cNode::UpdateNode()
{
	if ( (m_iNodeFlags & AGK_NODE_NEEDS_UPDATE) == 0 ) return;
	m_iNodeFlags &= ~AGK_NODE_NEEDS_UPDATE;

	// if all values are overridden then nothing to do
	uint32_t mask = AGK_NODE_OVERRIDE_POSITION | AGK_NODE_OVERRIDE_ROTATION | AGK_NODE_OVERRIDE_SCALE;
	if ( (m_iNodeFlags & mask) == mask ) return;
	
	if ( m_pParentNode )
	{
		m_pParentNode->UpdateNode();
		if ( (m_iNodeFlags & AGK_NODE_OVERRIDE_POSITION) == 0 ) m_positionFinal = (m_pParentNode->m_rotationFinal * (m_position*m_pParentNode->m_scaleFinal)) + m_pParentNode->m_positionFinal;
		if ( (m_iNodeFlags & AGK_NODE_OVERRIDE_ROTATION) == 0 ) m_rotationFinal = m_pParentNode->m_rotationFinal * m_rotation; // not affected by scale
		if ( (m_iNodeFlags & AGK_NODE_OVERRIDE_SCALE) == 0 ) m_scaleFinal = m_pParentNode->m_scaleFinal * m_scale; // no shear, non-uniform scales might not propagate as expected
	}
	else
	{
		if ( (m_iNodeFlags & AGK_NODE_OVERRIDE_POSITION) == 0 ) m_positionFinal = m_position;
		if ( (m_iNodeFlags & AGK_NODE_OVERRIDE_ROTATION) == 0 ) m_rotationFinal = m_rotation;
		if ( (m_iNodeFlags & AGK_NODE_OVERRIDE_SCALE) == 0 ) m_scaleFinal = m_scale;
	}
}