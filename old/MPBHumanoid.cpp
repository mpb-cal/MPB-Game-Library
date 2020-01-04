
#include "MPBHumanoid.h"
#include "MPBShapeBox.h"
#include "MPBShapeBone.h"
#include "MPBShapeTube.h"
#include "MPBAnimation.h"
#include "mmgr.h"



MPBHumanoid::MPBHumanoid( MPBVector radiusVector ) :
	MPBCreature( radiusVector ),
	m_chest( NULL )
{
return;

/*

chest |
	neck |
		left head -
		right head -
	waist -
		pelvis |
			leftHip -
				leftLeg |
					leftKnee -
						leftLowerLeg |
							leftAnkle -
								leftFoot |
			rightHip
				rightLeg
	leftShoulder -
		leftArm |
			leftElbow -
				leftForearm |
					left wrist -
						leftHand |
	rightShoulder
		rightArm
*/

	MPBFLOAT headWidth = (radiusVector.y * 2.0) * 1.0/7.0;
	MPBAnimation* a;
	MPBVector translation;
	const char TEXTURE[MAX_PATH] = "metal07";


	// chest
	m_chest = new MPBShapeBone( headWidth * 1.5 );
	m_chest->setLit( false );
	addShape( m_chest );
	m_chest->addChild( new MPBShapeTube( headWidth * 1.5 * .3, headWidth * 1.5 * .4, headWidth * 1.5, 6, TEXTURE, false ) );

	a = new MPBAnimation;
	m_chest->addAnimation( a );

	translation.set( 0, 0, 0 );
	a->addKeyFrame( new MPBKeyFrame( 0, MPBVector( 0, headWidth * .5, 0 ), MPBVector( 80, 0, 0 ) ) );
	a->addKeyFrame( new MPBKeyFrame( 1, MPBVector( 0, headWidth * .5 + headWidth * .4, 0 ), MPBVector( 80, 0, 0 ) ) );
	a->addKeyFrame( new MPBKeyFrame( 2, MPBVector( 0, headWidth * .5, 0 ), MPBVector( 80, 0, 0 ) ) );
	a->addKeyFrame( new MPBKeyFrame( 3, MPBVector( 0, headWidth * .5 + headWidth * .4, 0 ), MPBVector( 80, 0, 0 ) ) );
	a->addKeyFrame( new MPBKeyFrame( 4, MPBVector( 0, headWidth * .5, 0 ), MPBVector( 80, 0, 0 ) ) );


	// neck
	MPBShapeBone* neck = new MPBShapeBone( m_chest, headWidth * 1, TEXTURE );

	a = new MPBAnimation;
	neck->addAnimation( a );

	translation.set( 0, 0, -headWidth * 1.5 );
	a->addKeyFrame( new MPBKeyFrame( 0, translation, MPBVector( 0, 0, 0 ) ) );
	a->addKeyFrame( new MPBKeyFrame( 1, translation, MPBVector( 0, 0, 0 ) ) );


	// left head
	MPBShapeBone* leftHead = new MPBShapeBone( neck, headWidth * .5, TEXTURE );

	a = new MPBAnimation;
	leftHead->addAnimation( a );

	translation.set( 0, 0, -headWidth );
	a->addKeyFrame( new MPBKeyFrame( 0, translation, MPBVector( 0, 90, 0 ) ) );
	a->addKeyFrame( new MPBKeyFrame( 1, translation, MPBVector( 0, 90, 20 ) ) );
	a->addKeyFrame( new MPBKeyFrame( 2, translation, MPBVector( 0, 90, 0 ) ) );
	a->addKeyFrame( new MPBKeyFrame( 3, translation, MPBVector( 0, 90, -20 ) ) );
	a->addKeyFrame( new MPBKeyFrame( 4, translation, MPBVector( 0, 90, 0 ) ) );


	// right head
	MPBShapeBone* rightHead = new MPBShapeBone( neck, headWidth * .5, TEXTURE );

	a = new MPBAnimation;
	rightHead->addAnimation( a );

	translation.set( 0, 0, -headWidth );
	a->addKeyFrame( new MPBKeyFrame( 0, translation, MPBVector( 0, -90, 0 ) ) );
	a->addKeyFrame( new MPBKeyFrame( 1, translation, MPBVector( 0, -90, 20 ) ) );
	a->addKeyFrame( new MPBKeyFrame( 2, translation, MPBVector( 0, -90, 0 ) ) );
	a->addKeyFrame( new MPBKeyFrame( 3, translation, MPBVector( 0, -90, -20 ) ) );
	a->addKeyFrame( new MPBKeyFrame( 4, translation, MPBVector( 0, -90, 0 ) ) );


	// waist
	MPBShapeBone* waist = new MPBShapeBone( m_chest, headWidth * .1, TEXTURE );

	a = new MPBAnimation;
	waist->addAnimation( a );

	translation.set( 0, 0, 0 );
	a->addKeyFrame( new MPBKeyFrame( 0, translation, MPBVector( 0, 90, 0 ) ) );
	a->addKeyFrame( new MPBKeyFrame( 1, translation, MPBVector( 0, 90, 0 ) ) );


	// pelvis
	MPBShapeBone* pelvis = new MPBShapeBone( waist, headWidth * .8, headWidth * .6, TEXTURE );

	a = new MPBAnimation;
	pelvis->addAnimation( a );

	translation.set( 0, 0, 0 );
	a->addKeyFrame( new MPBKeyFrame( 0, translation, MPBVector( 0, 90, -10 ) ) );
	a->addKeyFrame( new MPBKeyFrame( 1, translation, MPBVector( 0, 90, -10 ) ) );
	a->addKeyFrame( new MPBKeyFrame( 2, translation, MPBVector( 0, 90, -10 ) ) );
	a->addKeyFrame( new MPBKeyFrame( 3, translation, MPBVector( 0, 90, -10 ) ) );
	a->addKeyFrame( new MPBKeyFrame( 4, translation, MPBVector( 0, 90, -10 ) ) );



	// left shoulder
	MPBShapeBone* leftShoulder = new MPBShapeBone( m_chest, headWidth * 1, TEXTURE );

	a = new MPBAnimation;
	leftShoulder->addAnimation( a );

	translation.set( 0, 0, -headWidth * 1.5 );
	a->addKeyFrame( new MPBKeyFrame( 0, translation, MPBVector( 0, 90, 0 ) ) );
	a->addKeyFrame( new MPBKeyFrame( 1, translation, MPBVector( 0, 90, 0 ) ) );


	// right shoulder
	MPBShapeBone* rightShoulder = new MPBShapeBone( m_chest, headWidth * 1, TEXTURE );

	a = new MPBAnimation;
	rightShoulder->addAnimation( a );

	translation.set( 0, 0, -headWidth * 1.5 );
	a->addKeyFrame( new MPBKeyFrame( 0, translation, MPBVector( 0, -90, 0 ) ) );
	a->addKeyFrame( new MPBKeyFrame( 1, translation, MPBVector( 0, -90, 0 ) ) );


	// left upper arm
	MPBShapeBone* leftArm = new MPBShapeBone( leftShoulder, headWidth * 1, TEXTURE );

	a = new MPBAnimation;
	leftArm->addAnimation( a );

	translation.set( 0, 0, -headWidth * .75 );
	a->addKeyFrame( new MPBKeyFrame( 0, translation, MPBVector( 0, 90, 0 ) ) );
	a->addKeyFrame( new MPBKeyFrame( 1, translation, MPBVector( 0, 90, 20 ) ) );
	a->addKeyFrame( new MPBKeyFrame( 2, translation, MPBVector( 0, 90, 0 ) ) );
	a->addKeyFrame( new MPBKeyFrame( 3, translation, MPBVector( 0, 90, -20 ) ) );
	a->addKeyFrame( new MPBKeyFrame( 4, translation, MPBVector( 0, 90, 0 ) ) );


	// right upper arm
	MPBShapeBone* rightArm = new MPBShapeBone( rightShoulder, headWidth * 1, TEXTURE );

	a = new MPBAnimation;
	rightArm->addAnimation( a );

	translation.set( 0, 0, -headWidth * .75 );
	a->addKeyFrame( new MPBKeyFrame( 0, translation, MPBVector( 0, -90, 0 ) ) );
	a->addKeyFrame( new MPBKeyFrame( 1, translation, MPBVector( 0, -90, 20 ) ) );
	a->addKeyFrame( new MPBKeyFrame( 2, translation, MPBVector( 0, -90, 0 ) ) );
	a->addKeyFrame( new MPBKeyFrame( 3, translation, MPBVector( 0, -90, -20 ) ) );
	a->addKeyFrame( new MPBKeyFrame( 4, translation, MPBVector( 0, -90, 0 ) ) );


	// left elbow
	MPBShapeBone* leftElbow = new MPBShapeBone( leftArm, headWidth * .1, TEXTURE );

	a = new MPBAnimation;
	leftElbow->addAnimation( a );

	translation.set( 0, 0, -headWidth * 1 );
	a->addKeyFrame( new MPBKeyFrame( 0, translation, MPBVector( 0, 90, 0 ) ) );
	a->addKeyFrame( new MPBKeyFrame( 1, translation, MPBVector( 0, 90, 0 ) ) );


	// right elbow
	MPBShapeBone* rightElbow = new MPBShapeBone( rightArm, headWidth * .1, TEXTURE );

	a = new MPBAnimation;
	rightElbow->addAnimation( a );

	translation.set( 0, 0, -headWidth * 1 );
	a->addKeyFrame( new MPBKeyFrame( 0, translation, MPBVector( 0, 90, 0 ) ) );
	a->addKeyFrame( new MPBKeyFrame( 1, translation, MPBVector( 0, 90, 0 ) ) );


	// left forearm
	MPBShapeBone* leftForearm = new MPBShapeBone( leftElbow, headWidth * 1, TEXTURE );

	a = new MPBAnimation;
	leftForearm->addAnimation( a );

	translation.set( 0, 0, 0 );
	a->addKeyFrame( new MPBKeyFrame( 0, translation, MPBVector( 0, -90, 20 ) ) );
	a->addKeyFrame( new MPBKeyFrame( 1, translation, MPBVector( 0, -90, 0 ) ) );
	a->addKeyFrame( new MPBKeyFrame( 2, translation, MPBVector( 0, -90, 20 ) ) );
	a->addKeyFrame( new MPBKeyFrame( 3, translation, MPBVector( 0, -90, 40 ) ) );
	a->addKeyFrame( new MPBKeyFrame( 4, translation, MPBVector( 0, -90, 20 ) ) );


	// right forearm
	MPBShapeBone* rightForearm = new MPBShapeBone( rightElbow, headWidth * 1, TEXTURE );

	a = new MPBAnimation;
	rightForearm->addAnimation( a );

	translation.set( 0, 0, 0 );
	a->addKeyFrame( new MPBKeyFrame( 0, translation, MPBVector( 0, -90, 20 ) ) );
	a->addKeyFrame( new MPBKeyFrame( 1, translation, MPBVector( 0, -90, 40 ) ) );
	a->addKeyFrame( new MPBKeyFrame( 2, translation, MPBVector( 0, -90, 20 ) ) );
	a->addKeyFrame( new MPBKeyFrame( 3, translation, MPBVector( 0, -90, 0 ) ) );
	a->addKeyFrame( new MPBKeyFrame( 4, translation, MPBVector( 0, -90, 20 ) ) );


	// left hip
	MPBShapeBone* leftHip = new MPBShapeBone(	pelvis, headWidth * .5, TEXTURE );

	// hip up and down, back and forth
	a = new MPBAnimation;
	leftHip->addAnimation( a );

	translation.set( 0, 0, -headWidth / 2 );
	a->addKeyFrame( new MPBKeyFrame( 0, translation, MPBVector( 0, -90, 0 ) ) );
	a->addKeyFrame( new MPBKeyFrame( 1, translation, MPBVector( 0, -95, -10 ) ) );
	a->addKeyFrame( new MPBKeyFrame( 2, translation, MPBVector( 0, -90, 0 ) ) );
	a->addKeyFrame( new MPBKeyFrame( 3, translation, MPBVector( 0, -85, 10 ) ) );
	a->addKeyFrame( new MPBKeyFrame( 4, translation, MPBVector( 0, -90, 0 ) ) );


	// right hip
	MPBShapeBone* rightHip = new MPBShapeBone( pelvis, headWidth * .5, TEXTURE );

	// hip up and down, back and forth
	a = new MPBAnimation;
	rightHip->addAnimation( a );

	translation.set( 0, 0, -headWidth / 2 );
	a->addKeyFrame( new MPBKeyFrame( 0, translation, MPBVector( 0, 90, 0 ) ) );
	a->addKeyFrame( new MPBKeyFrame( 1, translation, MPBVector( 0, 85, -10 ) ) );
	a->addKeyFrame( new MPBKeyFrame( 2, translation, MPBVector( 0, 90, 0 ) ) );
	a->addKeyFrame( new MPBKeyFrame( 3, translation, MPBVector( 0, 95, 10 ) ) );
	a->addKeyFrame( new MPBKeyFrame( 4, translation, MPBVector( 0, 90, 0 ) ) );



	// left leg
	MPBShapeBone* leftLeg = new MPBShapeBone( leftHip, headWidth * 1.5, TEXTURE );

	a = new MPBAnimation;
	leftLeg->addAnimation( a );

	translation.set( 0, 0, -headWidth / 2.5 );
	a->addKeyFrame( new MPBKeyFrame( 0, translation, MPBVector( 0, 90, 0 ) ) );
	a->addKeyFrame( new MPBKeyFrame( 1, translation, MPBVector( 0, 90, -30 ) ) );
	a->addKeyFrame( new MPBKeyFrame( 2, translation, MPBVector( 0, 90, 0 ) ) );
	a->addKeyFrame( new MPBKeyFrame( 3, translation, MPBVector( 0, 90, 20 ) ) );
	a->addKeyFrame( new MPBKeyFrame( 4, translation, MPBVector( 0, 90, 0 ) ) );


	// right leg
	MPBShapeBone* rightLeg = new MPBShapeBone( rightHip, headWidth * 1.5, TEXTURE );

	a = new MPBAnimation;
	rightLeg->addAnimation( a );

	translation.set( 0, 0, -headWidth / 2.5 );
	a->addKeyFrame( new MPBKeyFrame( 0, translation, MPBVector( 0, -90, 0 ) ) );
	a->addKeyFrame( new MPBKeyFrame( 1, translation, MPBVector( 0, -90, -30 ) ) );
	a->addKeyFrame( new MPBKeyFrame( 2, translation, MPBVector( 0, -90, 0 ) ) );
	a->addKeyFrame( new MPBKeyFrame( 3, translation, MPBVector( 0, -90, 20 ) ) );
	a->addKeyFrame( new MPBKeyFrame( 4, translation, MPBVector( 0, -90, 0 ) ) );


	// left knee
	MPBShapeBone* leftKnee = new MPBShapeBone( leftLeg, headWidth * .1, TEXTURE );

	a = new MPBAnimation;
	leftKnee->addAnimation( a );

	translation.set( 0, 0, -headWidth * 1.5 );
	a->addKeyFrame( new MPBKeyFrame( 0, translation, MPBVector( 0, 90, 0 ) ) );
	a->addKeyFrame( new MPBKeyFrame( 1, translation, MPBVector( 0, 90, 0 ) ) );


	// right knee
	MPBShapeBone* rightKnee = new MPBShapeBone( rightLeg, headWidth * .1, TEXTURE );

	a = new MPBAnimation;
	rightKnee->addAnimation( a );

	translation.set( 0, 0, -headWidth * 1.5 );
	a->addKeyFrame( new MPBKeyFrame( 0, translation, MPBVector( 0, 90, 0 ) ) );
	a->addKeyFrame( new MPBKeyFrame( 1, translation, MPBVector( 0, 90, 0 ) ) );


	// left lower leg
	MPBShapeBone* leftLowerLeg = new MPBShapeBone( leftKnee, headWidth * 2, headWidth * .3, TEXTURE );

	a = new MPBAnimation;
	leftLowerLeg->addAnimation( a );

	translation.set( 0, 0, 0 );
	a->addKeyFrame( new MPBKeyFrame( 0, translation, MPBVector( 0, -90, 0 ) ) );
	a->addKeyFrame( new MPBKeyFrame( 1, translation, MPBVector( 0, -90, -30 ) ) );
	a->addKeyFrame( new MPBKeyFrame( 1.7, translation, MPBVector( 0, -90, 0 ) ) );
	a->addKeyFrame( new MPBKeyFrame( 2.3, translation, MPBVector( 0, -90, 0 ) ) );
	a->addKeyFrame( new MPBKeyFrame( 3, translation, MPBVector( 0, -90, -30 ) ) );
	a->addKeyFrame( new MPBKeyFrame( 4, translation, MPBVector( 0, -90, 0 ) ) );


	// right lower leg
	MPBShapeBone* rightLowerLeg = new MPBShapeBone( rightKnee, headWidth * 2, headWidth * .3, TEXTURE );

	a = new MPBAnimation;
	rightLowerLeg->addAnimation( a );

	translation.set( 0, 0, 0 );
	a->addKeyFrame( new MPBKeyFrame( 0, translation, MPBVector( 0, -90, 0 ) ) );
	a->addKeyFrame( new MPBKeyFrame( 1, translation, MPBVector( 0, -90, -30 ) ) );
	a->addKeyFrame( new MPBKeyFrame( 1.7, translation, MPBVector( 0, -90, 0 ) ) );
	a->addKeyFrame( new MPBKeyFrame( 2.3, translation, MPBVector( 0, -90, 0 ) ) );
	a->addKeyFrame( new MPBKeyFrame( 3, translation, MPBVector( 0, -90, -30 ) ) );
	a->addKeyFrame( new MPBKeyFrame( 4, translation, MPBVector( 0, -90, 0 ) ) );


	// left ankle
	MPBShapeBone* leftAnkle = new MPBShapeBone( leftLowerLeg, headWidth * .1, TEXTURE );

	a = new MPBAnimation;
	leftAnkle->addAnimation( a );

	translation.set( 0, 0, -headWidth * 2 );
	a->addKeyFrame( new MPBKeyFrame( 0, translation, MPBVector( 0, 90, 0 ) ) );
	a->addKeyFrame( new MPBKeyFrame( 1, translation, MPBVector( 0, 90, 0 ) ) );

	// right ankle
	MPBShapeBone* rightAnkle = new MPBShapeBone( rightLowerLeg, headWidth * .1, TEXTURE );

	a = new MPBAnimation;
	rightAnkle->addAnimation( a );

	translation.set( 0, 0, -headWidth * 2 );
	a->addKeyFrame( new MPBKeyFrame( 0, translation, MPBVector( 0, 90, 0 ) ) );
	a->addKeyFrame( new MPBKeyFrame( 1, translation, MPBVector( 0, 90, 0 ) ) );


	// left foot
	MPBShapeBone* leftFoot = new MPBShapeBone( leftAnkle, headWidth, TEXTURE );

	a = new MPBAnimation;
	leftFoot->addAnimation( a );

	translation.set( 0, 0, 0 );
	a->addKeyFrame( new MPBKeyFrame( 0, translation, MPBVector( 0, -90, 90 ) ) );
	a->addKeyFrame( new MPBKeyFrame( 1, translation, MPBVector( 0, -90, 60 ) ) );
	a->addKeyFrame( new MPBKeyFrame( 2, translation, MPBVector( 0, -90, 90 ) ) );
	a->addKeyFrame( new MPBKeyFrame( 3, translation, MPBVector( 0, -90, 120 ) ) );
	a->addKeyFrame( new MPBKeyFrame( 4, translation, MPBVector( 0, -90, 90 ) ) );


	// right foot
	MPBShapeBone* rightFoot = new MPBShapeBone( rightAnkle, headWidth, TEXTURE );

	a = new MPBAnimation;
	rightFoot->addAnimation( a );

	translation.set( 0, 0, 0 );
	a->addKeyFrame( new MPBKeyFrame( 0, translation, MPBVector( 0, -90, 90 ) ) );
	a->addKeyFrame( new MPBKeyFrame( 1, translation, MPBVector( 0, -90, 60 ) ) );
	a->addKeyFrame( new MPBKeyFrame( 2, translation, MPBVector( 0, -90, 90 ) ) );
	a->addKeyFrame( new MPBKeyFrame( 3, translation, MPBVector( 0, -90, 120 ) ) );
	a->addKeyFrame( new MPBKeyFrame( 4, translation, MPBVector( 0, -90, 90 ) ) );



	m_chest->scaleTimeAnimations( .2 );

	// get the body into 1st frame position
	m_chest->startAnimations();
	m_chest->pauseAnimations();
}

MPBHumanoid::~MPBHumanoid()
{
}

void MPBHumanoid::updateFrame()
{
	MPBCreature::updateFrame();

	if (m_chest)
	{
		MPBFLOAT speed = getVelocity().getLength();
		if (speed < 1)
		{
			m_chest->pauseAnimationsWhenDone();
	//		pauseWalkAnimation();
//			m_chest->setLoopAnimations( false );
		}
		else
		{
//			m_chest->setLoopAnimations( true );
			unpauseWalkAnimation();

	//		MPBFLOAT scale = 1.0 / speed;
	//		m_chest->scaleTimeAnimations( scale );
		}
	}
}

void MPBHumanoid::startWalkAnimation()
{
	if (m_chest) m_chest->startAnimations();
}

void MPBHumanoid::pauseWalkAnimation()
{
	if (m_chest) m_chest->pauseAnimations();
}

void MPBHumanoid::unpauseWalkAnimation()
{
	if (m_chest) m_chest->unpauseAnimations();
}

void MPBHumanoid::stopWalkAnimation()
{
	if (m_chest) m_chest->stopAnimations();
}




