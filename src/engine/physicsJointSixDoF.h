#ifndef PHYSICSJOINTSIXDOF_H
#define PHYSICSJOINTSIXDOF_H
/**
#include "settingsPhysics.h"
#include "constantsMath.h"
#include "mat3.h"
#include "flags.h"

typedef struct physRigidBody physRigidBody;
typedef struct physJoint physJoint;

typedef struct {

	// Body A constraint reference frame.
	vec3 anchorA;// = RVec3::sZero();
	vec3 xAxisA;// = Vec3::sAxisX();
	vec3 yAxisA;// = Vec3::sAxisY();

	// Body B constraint reference frame.
	vec3 anchorB;// = RVec3::sZero();
	vec3 xAxisB;// = Vec3::sAxisX();
	vec3 yAxisB;// = Vec3::sAxisY();

	// Maximum friction for each constraint axis.
	// We have maximum friction force for the translational axes
	// and maximum friction torque for the rotational axes.
	float linearFriction[3];
	float angularFriction[3];

	// The type of swing constraint that we want to use.
	//ESwingType mSwingType = ESwingType::Cone;

	// Stores the minimum and maximum limits
	// (in that order) for each constraint axis.
	//
	// To allow free movement over an axis, set
	// the minimum to -FLT_MAX and the maximum
	// to FLT_MAX.
	//
	// Conversely, to remove a degree of freedom,
	// set the minimum to FLT_MAX and the maximum
	// to -FLT_MAX.
	//
	// When using rotational limits, x-axis limit
	// should be taken between -pi and pi.
	//
	// For cone limits, the y and z limits should be
	// taken between 0 and pi. These are assumed to
	// be symmetric about 0 (that is, the minimum is
	// assumed to be the negative of the maximum).
	//
	// For pyramid limits, the y and z limits should be
	// taken between -pi and pi. They need not be symmetric.
	float linearLimits[3];
	float angularLimits[3];

	// When enabled, this makes the limits soft. When the constraint exceeds the limits, a spring force will pull it back.
	// Only soft translation limits are supported, soft rotation limits are not currently supported.
	float linearFrequency[3];
	float linearDamping[3];
	float linearStiffness[3];

	// Motor settings for each axis.
	//MotorSettings mMotorSettings[6];

} physJointSixDoF;

typedef struct {

	// Local space constraint positions
	///Vec3 mLocalSpacePosition1;
	///Vec3 mLocalSpacePosition2;
	// Transforms from constraint space to body space
	///Quat mConstraintToBody1;
	///Quat mConstraintToBody2;

	// Local anchor transformations.
	vec3 anchorA;
	vec3 anchorB;
	quat anchorOrientationA;
	quat anchorOrientationB;

	// Limits
	flags_t mFreeAxis = 0;  // Bitmask of free axis (bit 0 = TranslationX)
	flags_t mFixedAxis = 0;  // Bitmask of fixed axis (bit 0 = TranslationX)
	bool mTranslationMotorActive = false;  // If any of the translational frictions / motors are active
	bool mRotationMotorActive = false;  // If any of the rotational frictions / motors are active
	flags_t mRotationPositionMotorActive = 0;  // Bitmask of axis that have position motor active (bit 0 = RotationX)
	bool mHasSpringLimits = false;  // If any of the limit springs have a non-zero frequency/stiffness
	float limit[6];
	float mLimitMax[6];
	SpringSettings mLimitsSpringSettings[3];

	// Friction settings for each axis
	float mMaxFriction[6];

	// Motor controls
	Vec3 mTargetVelocity = Vec3::sZero();
	Vec3 mTargetAngularVelocity = Vec3::sZero();
	Vec3 mTargetPosition = Vec3::sZero();
	Quat mTargetOrientation = Quat::sIdentity();

	// RUN TIME PROPERTIES FOLLOW

	// Constraint space axis in world space
	Vec3 mTranslationAxis[3];
	Vec3 mRotationAxis[3];

	// Translation displacement (valid when translation axis has a range limit)
	float mDisplacement[3];

	// Individual constraint parts for translation, or a combined point constraint part if all axis are fixed
	physConstraintAxis partTranslation[3];
	physConstraintPoint partPoint;

	// Individual constraint parts for rotation or a combined constraint part if rotation is fixed
	physConstraintSwingTwist partSwingTwist;
	physConstraintRotationEuler partRotationEuler;

	// Motor or friction constraints
	physConstraintAxis partTranslationMotor[3];
	physConstraintAngle partRotationMotor[3];

} physJointSixDoF;

void physJointSixDoFInit(
	physJointSixDoF *const __RESTRICT__ joint
);
void physJointSixDoFPresolveConstraints(physJoint *const __RESTRICT__ joint, physRigidBody *const __RESTRICT__ bodyA, physRigidBody *const __RESTRICT__ bodyB, const float dt_s);
void physJointSixDoFSolveVelocityConstraints(physJoint *const __RESTRICT__ joint, physRigidBody *const __RESTRICT__ bodyA, physRigidBody *const __RESTRICT__ bodyB);
return_t physJointSphereSixDoFConfigurationConstraints(physJoint *const __RESTRICT__ joint, physRigidBody *const __RESTRICT__ bodyA, physRigidBody *const __RESTRICT__ bodyB);
**/
#endif
